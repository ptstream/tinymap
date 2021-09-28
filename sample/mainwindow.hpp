#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "../mapctrl/mapshape.hpp"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class CMainWindow; }
QT_END_NAMESPACE

class CTowns;
class QEnterEvent;
class CMapShape;
class CMapTooltip;

/*! Selected shapes type.
 *  The shape is describe by a pointer on the shape and the original color.
 */
using TSelectedShape = QPair<CMapShape*, QRgb>;

class CMainWindow : public QMainWindow, CStatus<quint32>
{
  Q_OBJECT
public:
  enum EPickingType : quint32 { DynamicColor   = 0x00000001, // Activate picking color.
                                DynamicTooltip = 0x00000002, // Activate picking tooltip.
                              };

  CMainWindow (QWidget* parent = nullptr);
  ~CMainWindow ();

protected slots:
  void on_m_zoomIn_clicked (bool = false);    // Handler of zoom in.
  void on_m_zoomOut_clicked (bool = false);   // Handler of zoom out.
  void on_m_center0_clicked (bool = false);   // Handler of center on Lyon.
  void on_m_fitInView_clicked (bool = false); // Handler of fit in view.

  void on_actionOSM_triggered (bool = false);      // OMS map providers.
  void on_actionESRI_triggered (bool = false);     // ESRI map providers.
  void on_actionMapbox_triggered (bool = false);   // MAPBOX map providers.

  void on_actionCircle_triggered (bool = false);   // Handler of circles.
  void on_actionCross_triggered (bool = false);    // Handler of crosses.
  void on_actionImage_triggered (bool = false);    // Handler of images.
  void on_actionPolygon_triggered (bool = false);  // Handler of polygons.
  void on_actionPolyline_triggered (bool = false); // Handler of polylines.
  void on_actionText_triggered (bool = false);     // Handler of texts.
  void on_actionQuit_triggered (bool = false);     // Quit the application.
  void on_actionClear_triggered (bool = false);    // Remove all added shapes.

  void mapMouseMove (QMouseEvent const * event, TGeoCoord coordinates); // Handler of mapMouseMoveEvent.
  void mapEnter (QEnterEvent const * event, TGeoCoord coordinates);     // Handler of mapMouseMouseEnter.
  void mapCopyRightClicked (QString const & url);
  void mapMouseRelease (QMouseEvent const *, TGeoCoord coordinates); // Handler of mapMouseReleaseEvent.
  void mapIndex (bool);
  void zoomChanged (int zoom);

private:
  void updateSelectedShapes (TGeoCoord coordinates); // Update selected shapes from the geo_location of the cursor.
  void dynamicPickingManager (QPoint const & pos, TGeoCoord coordinates); // Update the widget from selected shapes.
  void addPolygons (bool clear = true);  // Add polygons of cities.
  void addCrosses (bool clear = true);   // Add crosses positionned on the city's center of gravity.
  void addPolylines (bool clear = true); // Add polylines of cities.
  void addImages (bool clear = true);    // Add image positionned on the city's center of gravity.
  void addCircles (bool clear = true);   // Add circles circumscribed to polygons and centered on the city's center of gravity.
  void addTexts (bool clear = true)   ;  // Add texts positionned on the city's center of gravity.
  void updateMapTypeNemu ();

private:
  Ui::CMainWindow*      ui;
  TGeoCoord             m_center0 = { 4.831918F, 45.768204F }; // geo-location of Lyon
  int                   m_zoom    = 7; // Initial zoom.
  QString               m_region  = "84"; // Auvergne Rhone-Aples
  CTowns*               m_towns; // Hash table of towns.
  QList<TSelectedShape> m_selectedShapes; // Selected shape list.
  QRgb                  m_selectionColor = 0xFF0000FF; // Color of selection.
  QString               m_windowTitle;
};

#endif // MAINWINDOW_HPP
