#include "tileadapter.hpp"
#include <QNetworkDiskCache>
#include <QStandardPaths>
#include <QPixmap>
#include <QDir>
#include <QDebug>

CTileAdapter::CTileAdapter (QStringList const & urls, QString const & name, int tileSize,
                            int zoomMin, int zoomMax, bool swapCoordinates, int maxCacheSize) :
  QNetworkAccessManager (), m_name (name), m_urls (urls), m_tileSize (tileSize),
  m_zoomMin (zoomMin), m_zoomMax (zoomMax), m_swapCoordinate (swapCoordinates)
{
#ifdef Q_OS_WASM
  maxCacheSize = -1;
#else
  if (maxCacheSize == -1)
  {
    maxCacheSize = 0;
  }

#endif
  if (maxCacheSize >= 0)
  {
    QString folder = QStandardPaths::writableLocation (QStandardPaths::CacheLocation);
    folder        += QLatin1String ("/tiles/") + m_name;
    auto cache     = new QNetworkDiskCache (this);
    if (maxCacheSize > 0)
    {
      cache->setMaximumCacheSize (maxCacheSize * 1024 * 1024);
    }

    cache->setCacheDirectory (folder);
    setCache (cache);
  }

  updatePixmapFormat ();
}

CTileAdapter::~CTileAdapter ()
{
  // Abort all pending network communications.
  for (QMap<QNetworkReply*, QByteArray>::key_iterator it = m_replies.keyBegin (), end = m_replies.keyEnd (); it != end; ++it)
  {
    (*it)->abort ();
  }

  QNetworkDiskCache* cache = findChild<QNetworkDiskCache*> ();
  if (cache != nullptr)
  {
    cache->clear ();
    QDir folder = cache->cacheDirectory ();
    setCache (nullptr);
    if (!folder.isEmpty ())
    {
      folder.removeRecursively ();
    }
  }
}

void CTileAdapter::setUrls (QStringList const & urls)
{
  m_urls = urls;
  updatePixmapFormat ();
}

void CTileAdapter::updatePixmapFormat ()
{
  if (m_urlIndex < m_urls.size ())
  {
    // Try to determine the downloaded image format.
    int index = m_urls[m_urlIndex].lastIndexOf ('.');
    if (index != -1)
    {
      m_imageFormat = m_urls[m_urlIndex].mid (index + 1).toUpper ().toLatin1 ();
    }
  }
}

TCoordType const CPI = static_cast<TCoordType>(M_PI);

TGeoCoord CTileAdapter::coordinatesToViewportF (TGeoCoord const & coordinates, int zoom) const
{
  TCoordType tt = ::powerOf2 (zoom) * m_tileSize;
  TCoordType x  = (coordinates.x () + 180) * tt / 360;
  TCoordType y  = (1 - (std::log (std::tan (CPI / 4 + dgToRd (coordinates.y ()) / 2)) / CPI)) * tt / 2;
  return TGeoCoord (x, y);
}

TGeoCoord CTileAdapter::viewportToCoordinatesF (TGeoCoord const & point, int zoom) const
{
  TCoordType tt  = ::powerOf2 (zoom) * m_tileSize;
  TCoordType lat = (point.x () * (360 / tt)) - 180;
  TCoordType lon = ::rdToDg (std::atan (std::sinh ((1 - point.y () * (2 / tt)) * CPI)));
  return TGeoCoord (lat, lon);
}

void CTileAdapter::tile (int x, int y, int z)
{
  QString surl = this->url (x, y, z);
  QUrl    url (surl);
  QNetworkRequest request (url);
  if (m_userAgent)
  {
    request.setRawHeader ("User-Agent", "Mozilla/5.0 (PC; U; Intel; Linux; en) AppleWebKit/420+ (KHTML, like Gecko)");
  }

  if (cache () != nullptr)
  {
    request.setAttribute (QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
  }

  QNetworkReply* reply = get (request);
  if (reply != nullptr)
  {
    reply->setReadBufferSize (m_tileSize * m_tileSize * sizeof (quint32)); // Max size for uncompressed tile image.
    connect (reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred),
             this, &CTileAdapter::downloadError);
    connect (reply, &QNetworkReply::finished, this, &CTileAdapter::downloadFinished);
    connect (reply, &QIODevice::readyRead, this, &CTileAdapter::downloadReadData);
    m_replies.insert (reply, QByteArray ());
#ifdef Q_OS_WASM
    insert (reply->url ().toString (), QPixmap ());
#else
    insert (reply->url ().toString (), QString ());
#endif
  }
}

QPixmap CTileAdapter::fromCache (int x, int y, int z)
{
  QPixmap    pixmap;
  QIODevice* device = cache ()->data (url (x, y, z));
  if (device != nullptr && device->open (QIODevice::ReadOnly))
  {
    pixmap.loadFromData (device->readAll (), m_imageFormat);
    delete device;
  }

  return pixmap;
}

void CTileAdapter::downloadError (QNetworkReply::NetworkError err)
{
  auto reply = static_cast<QNetworkReply*>(sender ());
  m_message  = reply->errorString ();
  qDebug () << QStringLiteral ("Download error: ") << err << QStringLiteral (" (") << m_message << ')';
}

void CTileAdapter::downloadFinished ()
{
  auto reply = static_cast<QNetworkReply*>(sender ());
  if (reply != nullptr)
  {
    QByteArray& data = m_replies[reply];
    QPixmap     pixmap;
    pixmap.loadFromData (data, m_imageFormat.constData ());
    (*this)[reply->url ().toString ()] =
#ifdef Q_OS_WASM
      pixmap;
#endif
    m_replies.remove (reply);
    reply->deleteLater ();
    emit newTileAvailable ();
  }
}

void CTileAdapter::downloadReadData ()
{
  auto reply = static_cast<QNetworkReply*>(sender ());
  if (reply != nullptr)
  {
    QByteArray& data = m_replies[reply];
    data            += reply->readAll ();
  }
}

void CTileAdapter::updateCopyrightSymbol (QString& s)
{
  QString symbol ("x00A9");
  int     index = s.indexOf (symbol);
  while (index != -1)
  {
    s.remove (index, symbol.length ());
    s.insert (index, QChar (0x00A9));
    index = s.indexOf (symbol);
  }
}

