#ifndef MAPWIDGET_HPP
#define MAPWIDGET_HPP

#include "mapshape.hpp"
#include "tileadapter.hpp"
#include <QFrame>

class CTileAdapter;
class CMapShape;
class QEnterEvent;

using TShapeList = QVector<CMapShape*>;

#ifdef Q_OS_WASM
#define TMapToolTip CMapToolTip
#else
#define TMapToolTip QToolTip
#endif

/*!
 * \brief The CMapWidget class is the graphic widget of the map and its shapes.
 *
 * This class is similar at Qml Map.
 * Advantages:
 * - Extremely lighter in memory espacially for big number of shapes.
 * - Run on WebAssembly (Qml Map generates various errors with browsers).
 * - Do not use Qml and Java script just C++.
 * - Can be build for float or double.
 * - Classical QWidget more simple to use than QWindow.
 *
 * Disavantages
 * - Much less possibilities than Qml Map.
 * - The zoom step is limited at zoom level (e.g. [0,1,2,3,..]).
 * - Noticeably slower (Not use OpenGL to run without restriction on WebAssembly).
 * - Actually limited at OSM and ESRI.
 */
class CMapWidget : public QFrame, public CStatus<quint32>
{
  Q_OBJECT
public:

  /*! Temporary structure to store data to convert geo-coordinates to viewport coordinates.
   *  This structure is updated during paintEevent and picking.
   */
  struct SCoordinateToViewport
  {
    int    m_x, m_y;
    int    m_tileI, m_tileJ;
    int    m_tilesLeft, m_tilesAbove, m_tilesRight, m_tilesBottom;
    QPoint m_from, m_to;
  };

  enum EStatus : quint32 { PickingActivated    = 0x00000001, //!< Activate picking.
                           Sorted              = 0x00000002, //!< Activate sort by z order.
                           HideCopyrightLink   = 0x00000004, //!< Hide the copyright.
                           ShowScale           = 0x00000008, //!< Show scale.
                           USSaleUnit          = 0x00000010, //!< Set scale text with US units.
                           // Status above are transient.
                           InitTransformations = 0x00010000, //!< InitTransformations has been set.
                           Pan                 = 0x00020000, //!< Pan is in progress.
                           MousePressed        = 0x00040000, //!< The mouse as been pressed to prepare panning.
                         };

  explicit CMapWidget (QWidget* parent = nullptr);
  ~CMapWidget () override;

  /*! Returns the list of shapes as a reference. */
  TShapeList& shapes () { return m_shapes; }

  /*! Returns the list of shapes as a const reference. */
  TShapeList const & shapes () const { return m_shapes; }

  /*! Sets the tile adapter. */
  void setTiteAdapter (CTileAdapter* adapter);

  /*! Returns the tile adapter. */
  CTileAdapter* tileAdapter () const { return m_tileAdapter; }

  /*! Sets the centerof widget from a geo-coordinates. */
  void setCenter (TGeoCoord center);

  /*! Sets the current zoom. */
  void setZoom (int zoom);

  /*! Returns the current zoom. */
  int zoom () const { return m_zoom; };

  /*! Adds a shape to the map.
   *  After last shape added, update () must be called to see the result.
   */
  void addMapShape (CMapShape* shape);

  /*! Removes all shapes. */
  void remMapShapes ();

  /*! Converts the widget coordinates to geo-coordiantes. */
  TGeoCoord widgetToCoordinates (QPoint const & p) const;

  /*! Converts the widget coordinates to geo-coordiantes. */
  TGeoCoord widgetToCoordinatesF (TGeoCoord const & p) const;

  /*! Converts geo-coordinates to widget coordinates. */
  inline QPoint coordinatesToWidget (TGeoCoord const & v) const;

  /*! Converts geo-coordinates to widget coordinates. */
  inline TGeoCoord coordinatesToWidgetF (TGeoCoord const & v) const;

  /*! \brief Returns the list of shapes nearest coordinates.
   *  \param coordinates: The geo-coordinates of the cursor.
   *  \return The list of shapes sorted by z. The first element have the greater z.
   */
  QList<CMapShape*> pick (TGeoCoord const & coordinates) const;

  /*! Initializes all transformations before drawing, picking... */
  void initTransformations ();

  /*! Returns the longitude for one x pixel. */
  TCoordType pixelAngleX () const { return m_pixelAngleX; }

  /*! Returns the approximate latitude for one y pixel. */
  TCoordType pixelAngleY () const { return m_pixelAngleY; }

  /*! Activates the keyboard shortcuts.
   *  Ctrl+ for zoom in.
   *  Ctrl- for zoom out.
   *  CtrlA for fit in view.
   */
  void activateKeyboardShortCut ();

public slots:
  /*! Zoom in. */
  void zoomIn ();

  /*! Zoom out. */
  void zoomOut ();

  /*! Ajust the zoom to see all shapes. */
  void fitInView ();

protected:
  /*! Classical handlers. */
  void resizeEvent (QResizeEvent*) override;
  void paintEvent (QPaintEvent*) override;
  void mousePressEvent (QMouseEvent* event) override;
  void mouseReleaseEvent (QMouseEvent*event) override;
  void mouseMoveEvent (QMouseEvent* event) override;
  void wheelEvent (QWheelEvent* event) override;
  void enterEvent (QEvent *event) override;

signals:
  /*!
   * This signals allows to receive mouse move events within the MapWidget together with coordinates.
   * \param event: the QMouseEvent that occured
   * \param coordinates: the WGS84 coordinates
   */
  void mapMouseMouseEvent (QMouseEvent const * event, TGeoCoord coordinate);

  /*!
   * This signals allows to receive mouse move events within the MapWidget together with coordinates.
   * \param event: the QMouseEvent that occured
   * \param coordinates: the WGS84 coordinates
   */
  void mapEnterEvent (QEnterEvent const * event, TGeoCoord coordinate);

  /*!
   * This signals allows to receive mouse press events within the MapWidget together with coordinates.
   * \param event: the QMouseEvent that occured
   * \param coordinates: the WGS84 coordinates
   */
  void mapMousePressEvent (QMouseEvent const * event, TGeoCoord coordinate);

  /*!
   * This signals allows to receive mouse release events within the MapWidget together with coordinates.
   * \param event: the QMouseEvent that occured
   * \param coordinates: the WGS84 coordinates
   */
  void mapMouseReleaseEvent (QMouseEvent const * event, TGeoCoord coordinate);

  /*! Signal is emitted when mouse left button is clicked on copyrights. */
  void copyRightClicked (QString const & url);

  /*! Zoom have changed. */
  void zoomChanged (int zoom);

protected:
  CTileAdapter* m_tileAdapter = nullptr; // Actual tile adapter.
  TGeoCoord     m_center;                // Actual widget center in geo-coordiantes
  int           m_zoom = 0;              // Actual zoom.
  TShapeList    m_shapes;                // List of shapes.

private:
  QPixmap tile (int i, int j) const;
  void showCopyRightLinks (QPainter& painter);
  void drawScale (QPainter& painter);

private:
  QPoint               m_prePanning;          //!< Pointer under the cursor at button click.
  int                  m_copyrightMargin = 4; //!< Left and top margin in pixels.
  QColor               m_scaleColor      = Qt::black;
  mutable QPoint       m_centerOnTiles;       //!< Actual center on tile space.
  mutable TCoordType   m_pixelAngleX;         //!< Longitude variation of one pixel.
  mutable TCoordType   m_pixelAngleY;         //!< Latitude variation of one pixel.
  mutable QList<QRect> m_copyrightRects;      //!< Copyright title and url.
  mutable SCoordinateToViewport        m_cv;  //!< Internal structure.
  mutable CMapShape::SViewportToWidget m_vw;  //!< Internal structure.
};

QPoint CMapWidget::coordinatesToWidget (TGeoCoord const & v) const
{
  return m_tileAdapter->coordinatesToWidget (v, m_vw);
}

TGeoCoord CMapWidget::coordinatesToWidgetF (TGeoCoord const & v) const
{
  return m_tileAdapter->coordinatesToWidgetF (v, m_vw);
}

#endif // MAPWIDGET_HPP
