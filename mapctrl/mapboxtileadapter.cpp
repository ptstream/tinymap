#include "mapboxtileadapter.hpp"

CMapboxTileAdapter::CMapboxTileAdapter (QString const & apiKey) : CTileAdapter (QStringList (), "mapbox", 512)
{
  QStringList urls;
  urls.reserve (11);
  QStringList indexNames;
  indexNames.reserve (11);
  m_apiKey = apiKey;
  urls << QStringLiteral ("https://api.mapbox.com/styles/v1/mapbox/streets-v9/tiles/%1/%2/%3?access_token=%4");
  indexNames << QStringLiteral ("Street Map");

  QString copyrights[] = { "" };

  QString copyrightLinks[] = { "" };
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
