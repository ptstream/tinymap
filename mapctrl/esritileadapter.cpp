#include "esritileadapter.hpp"

CEsriTileAdapter::CEsriTileAdapter () : CTileAdapter (QStringList (), "esri", 256, 3, 19, true)
{
  QStringList urls = { QStringLiteral ("https://services.arcgisonline.com/ArcGIS/rest/services/World_Street_Map/MapServer/tile/%1/%2/%3"),
                       QStringLiteral ("http://server.arcgisonline.com/ArcGIS/rest/services/World_Imagery/MapServer/tile/%1/%2/%3"),
                       QStringLiteral ("http://server.arcgisonline.com/ArcGIS/rest/services/World_Terrain_Base/MapServer/tile/%1/%2/%3"),
                       QStringLiteral ("http://server.arcgisonline.com/ArcGIS/rest/services/World_Topo_Map/MapServer/tile/%1/%2/%3"),
                       QStringLiteral ("http://services.arcgisonline.com/ArcGIS/rest/services/USA_Topo_Maps/MapServer/tile/%1/%2/%3"),
                       QStringLiteral ("http://services.arcgisonline.com/ArcGIS/rest/services/NatGeo_World_Map/MapServer/tile/%1/%2/%3"),
                       QStringLiteral ("http://services.arcgisonline.com/ArcGIS/rest/services/Canvas/World_Light_Gray_Base/MapServer/tile/%1/%2/%3"),
                       QStringLiteral ("http://server.arcgisonline.com/ArcGIS/rest/services/World_Physical_Map/MapServer/tile/%1/%2/%3"),
                       QStringLiteral ("http://server.arcgisonline.com/ArcGIS/rest/services/World_Shaded_Relief/MapServer/tile/%1/%2/%3"),
                       QStringLiteral ("http://server.arcgisonline.com/arcgis/rest/services/Ocean/World_Ocean_Base/MapServer/tile/%1/%2/%3"),
                       QStringLiteral ("http://services.arcgisonline.com/ArcGIS/rest/services/Canvas/World_Dark_Gray_Base/MapServer/tile/%1/%2/%3"),
                       QStringLiteral ("http://server.arcgisonline.com/ArcGIS/rest/services/Specialty/DeLorme_World_Base_Map/MapServer/tile/%1/%2/%3"),
                     };

  QStringList indexNames = { QStringLiteral ("World Street Map"),
                             QStringLiteral ("World Imagery"),
                             QStringLiteral ("World Terrain Base"),
                             QStringLiteral ("World Topography Map"),
                             QStringLiteral ("USA Topography Maps"),
                             QStringLiteral ("National Geographic World Map"),
                             QStringLiteral ("World Light Gray Base"),
                             QStringLiteral ("World Physical Map"),
                             QStringLiteral ("World Shaded Relief"),
                             QStringLiteral ("World Ocean Base"),
                             QStringLiteral ("World Dark Gray Base"),
                             QStringLiteral ("DeLorme World Base Map"),
                           };

  QString copyright     = "x00A9 Esri contributor";
  QString copyrightLink = "https://www.arcgis.com/home/index.html";

  setUrls (urls);
  setIndexNames (indexNames);

  auto s = TCopyright (copyright, copyrightLink);
  updateCopyrightSymbol (s.first);
  m_copyrights << s;
}
