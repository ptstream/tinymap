
/* This sample shows the map of the french region "Auvergne Rhone-Alpes".
 *
 * It use the maps provides by OSM or ESRI provider without API key by default.
 *
 * The menu bar allows to add:
 * - Circles circumscribed to polygons towns and centered on the town's centroid.
 * - Crosses positionned on the town's centroid.
 * - Images positionned on the town's centroid. Each image contains the index of the color.
 * - Polygons of the towns.
 * - Polylines which are the polygon countours.
 * - Texts positionned on the town's centroid. The wording is hue value of the color.
 *
 * Four buttons at left allow to:
 * - Center on town name "Lyon".
 * - Fit in view the entire region.
 * - Zoom in.
 * - Zoom out.
 *
 * The colors reflect the distance from the centroide of "lyon" to the centroides of other towns.
 * Towns near "Lyon" are green, towns far "Lyon" are red.
 *
 * The town polygons are contained in a file attached at a Qt resource (84.qrc).
 * The reason is, for webassembly, the file access is difficult for file on disk.
 */

#include "mainwindow.hpp"
#include <QApplication>

int main (int argc, char* argv[])
{
  QApplication a (argc, argv);
  CMainWindow w;
  w.show ();
  return a.exec ();
}
