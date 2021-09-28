#include "osmtileadapter.hpp"

COsmTileAdapter::COsmTileAdapter (QString const & apiKey) : CTileAdapter (QStringList (), "osm")
{
  QStringList urls;
  urls.reserve (11);
  QStringList indexNames;
  indexNames.reserve (11);
  m_apiKey = apiKey;
  if (apiKey.isEmpty ())
  {
    urls << QStringLiteral ("https://maps.wikimedia.org/osm-intl/%1/%2/%3.png")
         << QStringLiteral ("http://a.tile.thunderforest.com/cycle/%1/%2/%3.png")
         << QStringLiteral ("http://a.tile.thunderforest.com/transport/%1/%2/%3.png")
         << QStringLiteral ("http://a.tile.thunderforest.com/transport-dark/%1/%2/%3.png")
         << QStringLiteral ("http://a.tile.thunderforest.com/landscape/%1/%2/%3.png")
         << QStringLiteral ("http://a.tile.thunderforest.com/outdoors/%1/%2/%3.png");

    indexNames << QStringLiteral ("Street Map")
               << QStringLiteral ("Cycle Map")
               << QStringLiteral ("Transport Map")
               << QStringLiteral ("Transport dark Map")
               << QStringLiteral ("Landscape Map")
               << QStringLiteral ("Hiking Map");
  }
  else
  {
    urls << QStringLiteral ("https://maps.wikimedia.org/osm-intl/%1/%2/%3.png")
         << QStringLiteral ("http://a.tile.thunderforest.com/cycle/%1/%2/%3.png?apikey=%4")
         << QStringLiteral ("http://a.tile.thunderforest.com/transport/%1/%2/%3.png?apikey=%4")
         << QStringLiteral ("http://a.tile.thunderforest.com/transport-dark/%1/%2/%3.png?apikey=%4")
         << QStringLiteral ("http://a.tile.thunderforest.com/landscape/%1/%2/%3.png?apikey=%4")
         << QStringLiteral ("http://a.tile.thunderforest.com/outdoors/%1/%2/%3.png?apikey=%4")
         << QStringLiteral ("https://tile.thunderforest.com/spinal-map/%1/%2/%3.png?apikey=%4")
         << QStringLiteral ("https://tile.thunderforest.com/pioneer/%1/%2/%3.png?apikey=%4")
         << QStringLiteral ("https://tile.thunderforest.com/mobile-atlas/%1/%2/%3.png?apikey=%4")
         << QStringLiteral ("https://tile.thunderforest.com/neighbourhood/%1/%2/%3.png?apikey=%4")
         << QStringLiteral ("https://tile.thunderforest.com/atlas/%1/%2/%3.png?apikey=%4");

    indexNames << QStringLiteral ("Street Map")
               << QStringLiteral ("Cycle Map")
               << QStringLiteral ("Transport Map")
               << QStringLiteral ("Transport dark Map")
               << QStringLiteral ("Landscape Map")
               << QStringLiteral ("Hiking Map")
               << QStringLiteral ("Spinal Map")
               << QStringLiteral ("Pioneer")
               << QStringLiteral ("Mobile Atlas")
               << QStringLiteral ("Neighbourhood")
               << QStringLiteral ("Atlas");
  }

  QString copyrights[] = { "Map x00A9 WikiMedia Foundation",
                           "Data x00A9 www.osm.org/copyright",
                           "Maps x00A9 www.thunderforest.com",
                         };

  QString copyrightLinks[] = { "https://foundation.wikimedia.org/wiki/Terms_of_Use/en",
                               "https://www.openstreetmap.org/copyright",
                               "https://thunderforest.com",
                             };
  setUrls (urls);
  setIndexNames (indexNames);

  unsigned count = sizeof (copyrightLinks) / sizeof (const char *);
  if (apiKey.isEmpty ())
  {
    --count;
  }

  for (unsigned i = 0; i < count; ++i)
  {
    auto s = TCopyright (copyrights[i], copyrightLinks[i]);
    updateCopyrightSymbol (s.first);
    m_copyrights << s;
  }
}
