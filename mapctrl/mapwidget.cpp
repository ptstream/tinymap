#include "mapwidget.hpp"
#include "mappolygon.hpp"
#include "mappolyline.hpp"
#include "mapcross.hpp"
#include "mapimage.hpp"
#ifdef Q_OS_WASM
#include "maptooltip.hpp"
#endif
#include "mapcircle.hpp"
#include "maptext.hpp"
#include <QPainter>
#include <QMouseEvent>
#include <QShortcut>
#ifndef Q_OS_WASM
#include <QToolTip>
#endif

CMapWidget::CMapWidget (QWidget* parent) : QFrame (parent)
{
}

CMapWidget::~CMapWidget ()
{
#ifdef Q_OS_WASM
  CMapToolTip::destroy ();
#endif
  for (TShapeList::reverse_iterator it = m_shapes.rbegin (), end = m_shapes.rend (); it != end; ++it)
  {
    delete (*it);
  }

  delete m_tileAdapter;
  m_tileAdapter = nullptr;
}

void CMapWidget::setTiteAdapter (CTileAdapter* adapter)
{
  delete m_tileAdapter;
  m_tileAdapter = adapter;
  connect (m_tileAdapter, &CTileAdapter::newTileAvailable, this, QOverload<>::of(&CMapWidget::update));
}

void CMapWidget::setCenter (TGeoCoord center)
{
  m_center = center;
  initTransformations ();
  update ();
}

void CMapWidget::setZoom (int zoom)
{
  if (zoom >= m_tileAdapter->zoomMin () && zoom <= m_tileAdapter->zoomMax ())
  {
    m_zoom = zoom;
    emit zoomChanged (zoom);
    initTransformations ();
    update ();
  }
}

QPixmap CMapWidget::tile (int i, int j) const
{
  QPixmap pixmap;
  QString id = m_tileAdapter->url (i, j, m_zoom);
  if (!m_tileAdapter->contains (id))
  {
    m_tileAdapter->tile (i, j, m_zoom);
  }
  else
  {
#ifdef Q_OS_WASM
    pixmap = m_tileAdapter->value (id);
#else
    pixmap = m_tileAdapter->fromCache (i, j, m_zoom);
#endif
  }

  return pixmap;
}

void CMapWidget::initTransformations ()
{
  add (InitTransformations);
  m_centerOnTiles      = m_tileAdapter->coordinatesToViewport (m_center, m_zoom);
  int    tileSize      = m_tileAdapter->tileSize ();
  int    centerOnTileX = m_centerOnTiles.x () % tileSize;
  int    centerOnTileY = m_centerOnTiles.y () % tileSize;

  // Widget center.
  QSize size            = QWidget::size ();
  int   centerOnWidgetX = size.width ()  >> 1; // x>>1 = x/2
  int   centerOnWidgetY = size.height () >> 1;

  // Origin of the tile.
  m_cv.m_x = centerOnWidgetX - centerOnTileX;
  m_cv.m_y = centerOnWidgetY - centerOnTileY;

  // First tile.
  m_cv.m_tileI = m_centerOnTiles.x () / tileSize;
  m_cv.m_tileJ = m_centerOnTiles.y () / tileSize;

  // Left left.
  int spaceLeft    = centerOnWidgetX - centerOnTileX;
  m_cv.m_tilesLeft = spaceLeft / tileSize + (spaceLeft > 0);

  // Tiles above.
  int spaceAbove    = centerOnWidgetY - centerOnTileY;
  m_cv.m_tilesAbove = spaceAbove / tileSize + (spaceAbove > 0);

  // Tiles right.
  int spaceRight    = centerOnWidgetX - (tileSize - centerOnTileX);
  m_cv.m_tilesRight = spaceRight / tileSize + (spaceRight > 0);

  // Tiles below.
  int spaceBottom    = centerOnWidgetY - (tileSize - centerOnTileY);
  m_cv.m_tilesBottom = spaceBottom  / tileSize + (spaceBottom > 0);

  // Viewport rectangle.
  m_cv.m_from = QPoint ((m_cv.m_tileI - m_cv.m_tilesLeft)      * tileSize, (m_cv.m_tileJ - m_cv.m_tilesAbove)      * tileSize);
  m_cv.m_to   = QPoint ((m_cv.m_tileI + m_cv.m_tilesRight + 1) * tileSize, (m_cv.m_tileJ + m_cv.m_tilesBottom + 1) * tileSize);
  QRect viewport (m_cv.m_from, m_cv.m_to);

  // Tile rectangle.
  QPoint p0 (m_cv.m_x - m_cv.m_tilesLeft  * tileSize,       m_cv.m_y - m_cv.m_tilesAbove * tileSize);
  QPoint p1 (m_cv.m_x + (m_cv.m_tilesRight + 1) * tileSize, m_cv.m_y + (m_cv.m_tilesBottom + 1) * tileSize);
  QRect tileRect (p0, p1);

  m_vw.init (viewport, tileRect, m_zoom);

  auto latitudeFromViewport = [this, tileSize] (TCoordType y) -> TCoordType
  {
    TCoordType const CPI = static_cast<TCoordType>(M_PI);
    TCoordType       tt  = ::powerOf2 (m_zoom) * tileSize;
    return ::rdToDg (std::atan (std::sinh ((1 - y * (2 / tt)) * CPI)));
  };

  int const h = 50; // Number of pixels for precision.
  // latitude for y_center_of_widget + h
  TCoordType y0   = (centerOnWidgetY + h - m_vw.m_ty0) / m_vw.m_ry + m_vw.m_vy0;
  TCoordType lat0 = latitudeFromViewport (y0);

  // latitude for y_center_of_widget - h
  TCoordType yh   = (centerOnWidgetY - h - m_vw.m_ty0) / m_vw.m_ry + m_vw.m_vy0;
  TCoordType lath = latitudeFromViewport (yh);

  // For small zoom value [0-4], m_pixelAngleY and m_pixelAngleX are very imprecise due to Mercator projection.
  m_pixelAngleY = std::fabs (lath - lat0) / (2 * h);
  m_pixelAngleX = ::rdToDg (std::asin (std::sin (::dgToRd (m_pixelAngleY)) / std::cos (::dgToRd ((lath + lat0) / 2))));
}

void CMapWidget::resizeEvent (QResizeEvent*)
{
  initTransformations ();
}

void CMapWidget::drawScale (QPainter& painter)
{
  static TCoordType distances[] = { 5000000,
                                    2000000,
                                    1000000,
                                    1000000,
                                    1000000,
                                    100000,
                                    100000,
                                    50000,
                                    50000,
                                    10000,
                                    10000,
                                    10000,
                                    1000,
                                    1000,
                                    500,
                                    200,
                                    100,
                                    50,
                                    50,
                                    25,
                                    25,
                                    10,
                                    10
                                  };

  if (m_zoom >= 0 && m_zoom < static_cast<int>(sizeof (distances) / sizeof (TCoordType)))
  {
    int        line = ::qRound (distances[m_zoom] / std::pow (2, 18 - m_zoom) / static_cast<TCoordType>(0.597164));
    QString    unit;
    TCoordType limit;
    TCoordType coef;
    if (!contains (USSaleUnit))
    {
      limit = 1000;
      if (distances[m_zoom] >= limit)
      {
        unit = " km";
        coef = limit;
      }
      else
      {
        unit = " m";
        coef = 1;
      }
    }
    else
    {
      limit = static_cast<TCoordType>(1609.344);
      if (distances[m_zoom] >= limit)
      {
        unit = " mi";
        coef = limit;
      }
      else
      {
        unit = " yd";
        coef = static_cast<TCoordType>(0.9144);
      }
    }

    QString      distance = QString::number (distances[m_zoom] / coef, 'f', 0) + unit;
    QFont        font     = painter.font ();
    QFontMetrics fm (font);
    int          textWidth = fm.width (distance);

    QPen pen = painter.pen ();
    pen.setColor (m_scaleColor);
    pen.setWidth (0);
    painter.setPen (pen);
    painter.setRenderHints (QPainter::Antialiasing, false);

    int margin = 2 * m_copyrightMargin;
    int y      = height () - margin;
    int x0     = width () - line - margin - textWidth;
    int x1     = x0 + line;
    painter.drawLine (x0, y, x1, y);

    int y0 = y - m_copyrightMargin;
    int y1 = y + m_copyrightMargin;
    painter.drawLine (x0, y0, x0, y1);
    font.setUnderline (false);
    painter.setFont (font);
    painter.drawLine (x1, y0, x1, y1);
    painter.setRenderHints (QPainter::Antialiasing, true);
    painter.drawText (x1 + m_copyrightMargin, y1, distance);
  }
}

void CMapWidget::paintEvent (QPaintEvent*)
{
  if (!contains (InitTransformations))
  {
    initTransformations ();
  }

  // Bounding box.
  TGeoCoord v0 = m_tileAdapter->viewportToCoordinates (m_cv.m_from, m_zoom);
  TGeoCoord v1 = m_tileAdapter->viewportToCoordinates (m_cv.m_to,   m_zoom);
  CAabb     aabb (v0, v1);

  QPainter painter (this);
  painter.setRenderHints (QPainter::Antialiasing);

  // Pixmap of first tile.
  int     tileSize = m_tileAdapter->tileSize ();
  QSize   pixmapSize (tileSize, tileSize);
  QPixmap pixmap     = tile (m_cv.m_tileI, m_cv.m_tileJ);
  int     x          = m_cv.m_x;
  int     y          = m_cv.m_y;
  if (pixmap.size () == pixmapSize)
  {
    painter.drawPixmap (x, y, pixmap);
  }

  int z = (1 << m_zoom) - 1;
  for (int i = m_cv.m_tileI - m_cv.m_tilesLeft; i <= m_cv.m_tilesRight + m_cv.m_tileI; ++i)
  {
    if (i >= 0 && i <= z)
    {
      x = m_cv.m_x + (i - m_cv.m_tileI) * tileSize;
      for (int  j = m_cv.m_tileJ - m_cv.m_tilesAbove; j <= m_cv.m_tilesBottom + m_cv.m_tileJ; ++j)
      {
        if (j >= 0 && j <= z && (i != m_cv.m_tileI || j != m_cv.m_tileJ))
        {
          pixmap = tile (i, j);
          if (pixmap.size () == pixmapSize)
          {
            y = m_cv.m_y + (j - m_cv.m_tileJ) * tileSize;
            painter.drawPixmap (x, y, pixmap);
          }
        }
      }
    }
  }

  if (!contains (Sorted))
  {
    add (Sorted);
    std::sort (m_shapes.begin (), m_shapes.end (),
               [] (CMapShape const * s1, CMapShape const * s2) -> bool { return s1->z () < s2->z (); });
  }

  // Initialize pen and brush.
  painter.setBrush (QBrush (QColor::fromRgba (0x60000000)));
  painter.setPen (QPen (QColor::fromRgba (0x000000)));
  for (CMapShape* shape : qAsConst (m_shapes))
  {
    if (shape->isVisible (aabb))
    {
      shape->draw (&painter, m_tileAdapter, m_vw);
    }
  }

  if (!(contains (HideCopyrightLink)))
  {
    showCopyRightLinks (painter);
  }

  if ((contains (ShowScale)))
  {
    drawScale (painter);
  }
}

void CMapWidget::mousePressEvent (QMouseEvent* event)
{
  TMapToolTip::hideText ();
  if (event->button () == Qt::LeftButton)
  {
    m_prePanning = event->pos ();
    for (int i = 0, count = m_copyrightRects.count (); i < count; ++i)
    {
      if (m_copyrightRects[i].contains (m_prePanning))
      {
        emit copyRightClicked (m_tileAdapter->copyrights()[i].second);
        return;
      }
    }

    add (MousePressed);
    TGeoCoord c = widgetToCoordinates (m_prePanning);
    emit mapMousePressEvent (event, c);
  }
}

void CMapWidget::mouseReleaseEvent (QMouseEvent* event)
{
  TMapToolTip::hideText ();
  if (event->button () == Qt::LeftButton && !contains (Pan))
  {
    emit mapMouseReleaseEvent (event, widgetToCoordinates (m_prePanning));
  }

  remove (MousePressed);
  remove (Pan);
}

void CMapWidget::mouseMoveEvent (QMouseEvent* event)
{
  TMapToolTip::hideText ();
  QPoint newPosition = event->pos ();
  if (contains (MousePressed))
  { // Panning
    add (Pan);
    QPoint  offset = m_prePanning - newPosition;
    m_center       = m_tileAdapter->viewportToCoordinates (m_centerOnTiles + offset, m_zoom);
    m_prePanning   = newPosition;
    initTransformations ();
    update ();
  }
  else if (contains (PickingActivated))
  {
    TGeoCoord c = widgetToCoordinates (newPosition);
    emit mapMouseMouseEvent (event, c);
  }
}

void CMapWidget::enterEvent (QEvent* event)
{
  TMapToolTip::hideText ();
  if (contains (PickingActivated))
  {
    auto      ev  = static_cast<QEnterEvent*>(event);
    QPoint    pos = ev->pos ();
    TGeoCoord c   = widgetToCoordinates (pos);
    emit mapEnterEvent (ev, c);
  }
}

void CMapWidget::wheelEvent (QWheelEvent* event)
{
  TMapToolTip::hideText ();
  int delta = event->angleDelta ().y ();
  if (delta != 0)
  {
    // Position on widget of the cursor.
    QPoint position = event->position ().toPoint ();

    // Widget center.
    QSize  size            = QWidget::size ();
    int    centerOnWidgetX = size.width ()  >> 1;
    int    centerOnWidgetY = size.height () >> 1;
    QPoint centerOnWidget (centerOnWidgetX, centerOnWidgetY);

    // Move center to set the cursor position at the center of widget.
    QPoint offset = position - centerOnWidget;
    m_center      = m_tileAdapter->viewportToCoordinates (m_centerOnTiles + offset, m_zoom);

    // Apply new zoom.
    if (delta > 0)
    {
      if (m_zoom < m_tileAdapter->zoomMax ())
      {
        ++m_zoom;
      }
    }
    else
    {
      if (m_zoom > m_tileAdapter->zoomMin ())
      {
        --m_zoom;
      }
    }

    // New center on tiles.
    m_centerOnTiles = m_tileAdapter->coordinatesToViewport (m_center, m_zoom);

    // Move center by reserve translation vector.
    m_center = m_tileAdapter->viewportToCoordinates (m_centerOnTiles - offset, m_zoom);
    initTransformations ();
    update ();
    event->accept ();
    emit zoomChanged (m_zoom);
  }
}

void CMapWidget::zoomIn ()
{
  setZoom (m_zoom + 1);
}

void CMapWidget::zoomOut ()
{
  setZoom (m_zoom - 1);
}

void CMapWidget::fitInView ()
{
  if (m_shapes.isEmpty ())
  {
    return;
  }

  // The bounding box of the shapes
  CAabb aabb;
  for (CMapShape* shape : qAsConst (m_shapes))
  {
    if (shape->isVisible ())
    {
      switch (shape->type ())
      {
        case CMapShape::Cross:
        case CMapShape::Image:
        case CMapShape::Text:
          aabb.add (static_cast<CMapLocation*>(shape)->coordinates ());
          break;

        default:
          aabb.add (static_cast<CMapPolygon*>(shape)->aabb ());
          break;
      }
    }
  }

  // Sets the origin at the bounding box center.
  m_center = aabb.center ();
  initTransformations ();

  // Evaluate the better zoom factor.
  QSize     size = QWidget::size ();
  TGeoCoord c0   = widgetToCoordinates (QPoint (0, 0));
  TGeoCoord c1   = widgetToCoordinates (QPoint (size.width (), size.height ()));
  CAabb     widgetAabb;
  widgetAabb.add (c0);
  widgetAabb.add (c1);
  if (!widgetAabb.contains (aabb))
  {
    int zoomMin = m_tileAdapter->zoomMin ();
    while (!widgetAabb.contains (aabb) && m_zoom > zoomMin)
    {
      --m_zoom;
      initTransformations ();
      c0 = widgetToCoordinates (QPoint (0, 0));
      c1 = widgetToCoordinates (QPoint (size.width (), size.height ()));
      widgetAabb.init ();
      widgetAabb.add (c0);
      widgetAabb.add (c1);
    }
  }

  int zoomMax = m_tileAdapter->zoomMax ();
  int zoom    = m_zoom;
  while (widgetAabb.contains (aabb) && m_zoom <= zoomMax)
  {
    zoom = m_zoom;
    ++m_zoom;
    initTransformations ();
    c0 = widgetToCoordinates (QPoint (0, 0));
    c1 = widgetToCoordinates (QPoint (size.width (), size.height ()));
    widgetAabb.init ();
    widgetAabb.add (c0);
    widgetAabb.add (c1);
  }

  setZoom (zoom);
}

void CMapWidget::addMapShape (CMapShape* shape)
{
  remove (Sorted);
  m_shapes.append (shape);
}

void CMapWidget::remMapShapes ()
{
  m_shapes.clear ();
  update ();
}

TGeoCoord CMapWidget::widgetToCoordinates (QPoint const & p) const
{
  int       x = (p.x () - m_vw.m_tx0) / m_vw.m_rx + m_vw.m_vx0;
  int       y = (p.y () - m_vw.m_ty0) / m_vw.m_ry + m_vw.m_vy0;
  TGeoCoord v = m_tileAdapter->viewportToCoordinates ({ x, y}, m_zoom);
  return v;
}

TGeoCoord CMapWidget::widgetToCoordinatesF (TGeoCoord const & p) const
{
  TCoordType x = (p.x () - m_vw.m_tx0) / m_vw.m_rx + m_vw.m_vx0;
  TCoordType y = (p.y () - m_vw.m_ty0) / m_vw.m_ry + m_vw.m_vy0;
  TGeoCoord  v = m_tileAdapter->viewportToCoordinatesF (TGeoCoord (x, y), m_zoom);
  return v;
}

QList<CMapShape*> CMapWidget::pick (TGeoCoord const & coordinates) const
{
  QList<CMapShape*> shapes;
  TGeoCoord v0 = m_tileAdapter->viewportToCoordinates (m_cv.m_from, m_zoom);
  TGeoCoord v1 = m_tileAdapter->viewportToCoordinates (m_cv.m_to,   m_zoom);
  CAabb     aabb (v0, v1);
  for (CMapShape* shape : qAsConst (m_shapes))
  {
    if (shape->id () != 0 && shape->isVisible (aabb) && shape->contains (coordinates, m_pixelAngleX, m_pixelAngleY))
    {
      shapes.append (shape);
    }
  }

  std::sort (shapes.begin (), shapes.end (),
             [] (CMapShape const * s1, CMapShape const * s2) -> bool { return s1->z () > s2->z (); });
  return shapes;
}

void CMapWidget::showCopyRightLinks (QPainter& painter)
{
  m_copyrightRects.clear ();
  TCopyrights const &         copyrights = m_tileAdapter->copyrights ();
  TCopyrights::const_iterator begin      = copyrights.cbegin ();
  TCopyrights::const_iterator end        = copyrights.cend ();
  QString                     text;
  QRect                       boundingRect, rect = this->rect ();
  rect.setLeft (m_copyrightMargin);
  rect.setHeight (rect.height () - m_copyrightMargin);
  QFont font = painter.font ();
  QPen  pen  = painter.pen ();
  painter.setBackgroundMode (Qt::TransparentMode);
  for (TCopyrights::const_iterator it = begin; it != end; ++it)
  {
    if (it != begin)
    {
      text = " | ";
      painter.setPen (pen);
      font.setUnderline (false);
      painter.setFont (font);
      painter.drawText (rect, Qt::AlignBottom | Qt::AlignLeft, text, &boundingRect);
      rect.setLeft (rect.left () + boundingRect.width ());
    }

    text = (*it).first;
    painter.setPen (Qt::blue);
    font.setUnderline (true);
    painter.setFont (font);
    painter.drawText (rect, Qt::AlignBottom | Qt::AlignLeft, text, &boundingRect);
    m_copyrightRects << boundingRect;
    rect.setLeft (rect.left () + boundingRect.width ());
  }
}

void CMapWidget::activateKeyboardShortCut ()
{
  connect (new QShortcut (QKeySequence("Ctrl++"), this), &QShortcut::activated, this, &CMapWidget::zoomIn);
  connect (new QShortcut (QKeySequence("Ctrl+-"), this), &QShortcut::activated, this, &CMapWidget::zoomOut);
  connect (new QShortcut (QKeySequence("Ctrl+A"), this), &QShortcut::activated, this, &CMapWidget::fitInView);
}


