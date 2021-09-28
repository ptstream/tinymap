#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "mapshapedata.hpp"
#include "../mapctrl/osmtileadapter.hpp"
#include "../mapctrl/esritileadapter.hpp"
#include "../mapctrl/mapboxtileadapter.hpp"
#include "../mapctrl/mapwidget.hpp"
#ifdef Q_OS_WASM
#include "../mapctrl/maptooltip.hpp"
#define DToolTip CMapToolTip
#endif
#include "../town/towns.hpp"
#include <QMouseEvent>
#include <QDesktopServices>
#ifndef Q_OS_WASM
#include <QToolTip>
#define DToolTip QToolTip
#endif

#define START_WITH_MAP_ONLY

CMainWindow::CMainWindow (QWidget* parent) : QMainWindow (parent), ui (new Ui::CMainWindow)
{
  ui->setupUi (this);
  m_windowTitle = windowTitle ();
  auto provider = new QActionGroup (this);
  provider->addAction (ui->actionOSM);
  provider->addAction (ui->actionESRI);

  add (DynamicTooltip);
  ui->m_map->add (CMapWidget::PickingActivated);
  m_towns = new CTowns ();
  m_towns->load (QString (":/config/%1.towns").arg (m_region));
  ui->m_map->shapes ().reserve (m_towns->size ());
  connect (ui->m_map, QOverload<QMouseEvent const *, TGeoCoord>::of(&CMapWidget::mapMouseMouseEvent),
           this, &CMainWindow::mapMouseMove);
  connect (ui->m_map, QOverload<QEnterEvent const *, TGeoCoord>::of(&CMapWidget::mapEnterEvent),
           this, &CMainWindow::mapEnter);
  connect (ui->m_map, &CMapWidget::copyRightClicked, this, &CMainWindow::mapCopyRightClicked);
  connect (ui->m_map, &CMapWidget::mapMouseReleaseEvent, this, &CMainWindow::mapMouseRelease);
  connect (ui->m_map, &CMapWidget::zoomChanged, this, &CMainWindow::zoomChanged);

  on_actionOSM_triggered ();
  ui->m_map->setCenter (m_center0);
  ui->m_map->setZoom (m_zoom);
  ui->m_map->activateKeyboardShortCut ();
#ifndef START_WITH_MAP_ONLY
  addPolygons ();
  update ();
#endif
}

CMainWindow::~CMainWindow ()
{
  delete ui;
}

void CMainWindow::updateSelectedShapes (TGeoCoord coordinates)
{
  for (TSelectedShape const & selectedShape : qAsConst (m_selectedShapes))
  {
    selectedShape.first->setColor (selectedShape.second);
  }

  QList<CMapShape*> shapes = ui->m_map->pick (coordinates);
  m_selectedShapes.reserve (shapes.size ());
  for (CMapShape* shape : qAsConst (shapes))
  {
    m_selectedShapes.append (TSelectedShape (shape, shape->color ()));
    shape->setColor (m_selectionColor);
  }
}

static QString tooltipText (CTown const & town)
{
  QString line (QLatin1String("%1: %2<br/>"));
  QString format =
#ifdef Q_OS_WASM
  QString (QLatin1String("<h3 style=\"text-align:center\">%1</h3>"));
#else
  QString (QLatin1String("<h2 style=\"text-align:center\">%1</h2>"));
#endif
  QString text = format.arg (town.name ());
  text        += line.arg ("Code", QString::number (town.code (), 16));
  text        += line.arg ("Region").arg (town.region ());
  return text;
}

void CMainWindow::dynamicPickingManager (QPoint const & pos, TGeoCoord coordinates)
{
  TMapToolTip::hideText ();
  if (contains (DynamicColor))
  {
    updateSelectedShapes (coordinates);
    update ();
  }

  if (contains (DynamicTooltip))
  {
    QList<CMapShape*> shapes = ui->m_map->pick (coordinates);
    if (!shapes.isEmpty ())
    {
      CMapShape*  shape = shapes.first ();
      TMapShapeId id    = shape->id ();
      CTown       town  = m_towns->value (static_cast<int>(id));
      QString     text  = tooltipText (town);
      TMapToolTip::showText (ui->m_map->mapToGlobal (pos), text);
    }
  }
}

void CMainWindow::mapMouseMove (QMouseEvent const * event, TGeoCoord coordinates)
{
  dynamicPickingManager (event->pos (), coordinates);
}

void CMainWindow::mapEnter (QEnterEvent const * event, TGeoCoord coordinates)
{
  dynamicPickingManager (event->pos (), coordinates);
}

void CMainWindow::mapMouseRelease (QMouseEvent const *, TGeoCoord coordinates)
{
  QList<CMapShape*> shapes = ui->m_map->pick (coordinates);
  if (!shapes.isEmpty ())
  {
    CMapShape*  shape = shapes.first ();
    TMapShapeId id    = shape->id ();
    CTown       town  = m_towns->value (static_cast<int>(id));
    CMapShapeData msd (town, this);
    msd.exec ();
  }
}

void CMainWindow::on_m_zoomIn_clicked (bool)
{
  ui->m_map->zoomIn ();
}

void CMainWindow::on_m_zoomOut_clicked (bool)
{
  ui->m_map->zoomOut ();
}

void CMainWindow::on_m_center0_clicked (bool)
{
  ui->m_map->setCenter (m_center0);
}

void CMainWindow::on_m_fitInView_clicked (bool)
{
  ui->m_map->fitInView ();
}

void CMainWindow::mapIndex (bool)
{
  auto action = static_cast<QAction*> (sender ());
  int  index  = action->data ().toInt ();
  ui->m_map->tileAdapter ()->setUrlIndex (index);
  update ();
}

void CMainWindow::updateMapTypeNemu ()
{
  auto urlGroup = new QActionGroup (this);
  auto menu     = menuBar ()->findChild<QMenu*> ("menuMapTypes");
  menu->clear ();
  int index = 0;
  for (QString const & indexName : ui->m_map->tileAdapter ()->indexNames ())
  {
    QAction* action = menu->addAction (indexName, this, &CMainWindow::mapIndex);
    action->setData (index);
    action->setCheckable (true);
    urlGroup->addAction (action);
    if (index == 0)
    {
      action->setChecked (true);
    }

    ++index;
  }
}

void CMainWindow::on_actionOSM_triggered (bool)
{
  CTileAdapter* adapter = ui->m_map->tileAdapter ();
  if (adapter == nullptr || adapter->name () != "osm")
  {
    QString apiKey; // Funderforest (OSM map) API key looks like QString apiKey ("f1024a76c381244ad3f121853043f302");
    ui->m_map->setTiteAdapter (new COsmTileAdapter (apiKey));
    updateMapTypeNemu ();
  }

  ui->m_map->add (CMapWidget::ShowScale);
  update ();
}

void CMainWindow::on_actionMapbox_triggered (bool)
{
  CTileAdapter* adapter = ui->m_map->tileAdapter ();
  if (adapter == nullptr || adapter->name () != "mapbox")
  {
    ui->m_map->setTiteAdapter (new CMapboxTileAdapter);
    updateMapTypeNemu ();
  }

  ui->m_map->add (CMapWidget::ShowScale);
  update ();
}

void CMainWindow::on_actionESRI_triggered (bool)
{
  CTileAdapter* adapter = ui->m_map->tileAdapter ();
  if (adapter == nullptr || adapter->name () != "ersi")
  {
    ui->m_map->setTiteAdapter (new CEsriTileAdapter);
    updateMapTypeNemu ();
  }

  update ();
}

void CMainWindow::on_actionCircle_triggered (bool)
{
  addCircles ();
}

void CMainWindow::on_actionCross_triggered (bool)
{
  addCrosses ();
}

void CMainWindow::on_actionImage_triggered (bool)
{
  addImages ();
}

void CMainWindow::on_actionPolygon_triggered (bool)
{
  addPolygons ();
}

void CMainWindow::on_actionPolyline_triggered (bool)
{
  addPolylines ();
}

void CMainWindow::on_actionText_triggered (bool)
{
  addTexts ();
}

void CMainWindow::on_actionQuit_triggered (bool)
{
  close ();
}

void CMainWindow::on_actionClear_triggered (bool)
{
  ui->m_map->remMapShapes ();
}

void CMainWindow::mapCopyRightClicked (QString const & url)
{
  QDesktopServices::openUrl (url);
}

void CMainWindow::zoomChanged (int zoom)
{
  QString title = m_windowTitle + QString (" (zoom: %1)").arg (zoom);
  setWindowTitle (title);
}
