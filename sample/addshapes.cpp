#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "../mapctrl/mappolygon.hpp"
#include "../mapctrl/mappolyline.hpp"
#include "../mapctrl/mapcross.hpp"
#include "../mapctrl/mapimage.hpp"
#include "../mapctrl/mapcircle.hpp"
#include "../mapctrl/maptext.hpp"
#include "../town/towns.hpp"

int const Lyon       = 430371; // Town Lyon identifier
int const stepCount  = 121;    // Number of steps
int const hueMax     = 120;    // 0->red, 120->green
int const saturation = 255;    // Saturation=255 for red to green
int const lightness  = 128;    // Lightness=128 for red to green
int const alpha      = 0x80;   // Semi-transparent
int const maxIcon    = 5;      // Number max of icons in the resources

static TGeoCoord  lyonCentroid;
static TCoordType step;

static void prepareColors (CTowns const * towns)
{
  // color depends of the distance of Lyon to the town (distance of centroids).
  CTown lyon   = towns->value (Lyon);
  lyonCentroid = lyon.centroid ();

  // Maximun of distances.
  TCoordType d = 0;
  for (CTown const & town : qAsConst (*towns))
  {
    d = std::max (d, ::len (lyonCentroid, town.centroid ()));
  }

  step = d / stepCount; // Step size in term of pseudo-distance.
}

static inline int index (TGeoCoord  const & centroid)
{
  return stepCount - ::qRound (::len (lyonCentroid, centroid) / step);
}

static inline QRgb color (TGeoCoord  const & centroid)
{
  QColor color;
  int    hue = index (centroid);
  color.setHsl (hue, saturation, lightness, alpha);
  return color.rgba ();
}

void CMainWindow::addPolygons (bool clear)
{
  if (clear)
  {
    ui->m_map->shapes ().clear ();
  }

  prepareColors (m_towns);
  for (CTown const & town : qAsConst (*m_towns))
  {
    TMapShapeId    id    = town.code ();
    TPaths const & paths = town.paths ();
    auto           p     = new CMapPolygon (paths, town.aabb (), id);
    p->setColor (color (town.centroid ()));
    ui->m_map->addMapShape (p);
  }

  update ();
}

void CMainWindow::addCrosses (bool clear)
{
  if (clear)
  {
    ui->m_map->shapes ().clear ();
  }

  prepareColors (m_towns);
  for (CTown const & town : qAsConst (*m_towns))
  {
    auto cross = new CMapCross (town.centroid (), town.code ());
    cross->setColor (color (town.centroid ()));
    ui->m_map->addMapShape (cross);
  }

  update ();
}

void CMainWindow::addPolylines (bool clear)
{
  if (clear)
  {
    ui->m_map->shapes ().clear ();
  }

  prepareColors (m_towns);
  for (CTown const & town : qAsConst (*m_towns))
  {
    for (TPath path : town.paths ())
    {
      path.append (path.first ()); // Close the polyline.
      auto polyline = new CMapPolyline (path, town.code ());
      polyline->setColor (color (town.centroid ()));
      polyline->setWidth (2);
      ui->m_map->addMapShape (polyline);
    }
  }

  update ();
}

void CMainWindow::addImages (bool clear)
{
  if (clear)
  {
    ui->m_map->shapes ().clear ();
  }

  prepareColors (m_towns);
  for (CTown const & town : qAsConst (*m_towns))
  {
    int        index = ::index (town.centroid ());
    int        icon  = std::min (maxIcon, ::qRound (static_cast<float>(maxIcon) * index / hueMax));
    QPixmap    pxm (QString (":/icons/%1.png").arg (maxIcon - icon));
    QPoint     anchorPoint (-pxm.width () / 2, -pxm.height ()); // Move the image. The foot is on the centroid.
    auto       image = new CMapImage (town.centroid (), anchorPoint, pxm, town.code ());
    TCoordType z     = (index + 1) * 2;
    image->setZ (z);
    ui->m_map->addMapShape (image);

    anchorPoint = QPoint(0, -(pxm.height () / 2 + 4)); // Move the text to the center of rectangle.
    auto text   = new CMapText (town.centroid (), anchorPoint, QString::number (index), town.code ());
    text->setFlags (Qt::AlignCenter);
    text->setZ (z + 1);
    text->setPointSize (11);
    text->setColor (0xFF000000);
    ui->m_map->addMapShape (text);
  }

  update ();
}

void CMainWindow::addCircles (bool clear)
{
  if (clear)
  {
    ui->m_map->shapes ().clear ();
  }

  prepareColors (m_towns);
  for (CTown const & town : qAsConst (*m_towns))
  {
    TCoordType r = 0;
    for (TPath const & path : town.paths ())
    {
      for (TGeoCoord const & p : path)
      {
        r = std::max (r, ::len (town.centroid (), p));
      }
    }

    auto circle = new CMapCircle (town.centroid (), r * metersPerDegree, town.code ());
    circle->setColor (color (town.centroid ()));
    circle->setWidth (2);
    if (circle->id () == Lyon)
    { // Set circle of "Lyon" to top of picking.
      circle->setZ (1);
    }

    ui->m_map->addMapShape (circle);
  }

  update ();
}

void CMainWindow::addTexts (bool clear)
{
  if (clear)
  {
    ui->m_map->shapes ().clear ();
  }

  prepareColors (m_towns);
  for (CTown const & town : qAsConst (*m_towns))
  {
    auto text = new CMapText (town.centroid (), QPoint (), town.name (), town.code ());
    text->setColor (0xFF000000);
    text->setBackgroundColor (color (town.centroid ()));
    ui->m_map->addMapShape (text);
  }

  update ();
}
