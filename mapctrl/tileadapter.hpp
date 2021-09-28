#ifndef TILEADAPTER_HPP
#define TILEADAPTER_HPP

#include "mapshape.hpp"
#include <QNetworkAccessManager>
#include <QNetworkReply>

/*! MAPCTRL_VERSION is (major << 16) + (minor << 8) + patch. */
#define MAPCTRL_VERSION 0x010000

// https://api.mapbox.com/v4/mapbox.satellite/z/x/y@2x.png?access_token=YOUR_MAPBOX_ACCESS_TOKEN"

using TCopyright = QPair<QString, QString>;
using TCopyrights = QList<TCopyright>;

/*! \brief The CTileAdapter base class used to manage tiles from tile servers.
 * It is used by COsmTileAdapter and CEsriTileAdapter.
 */
class CTileAdapter : public QNetworkAccessManager,
#ifdef Q_OS_WASM
    public QMap<QString, QPixmap>
#else
    public QMap<QString, QString>
#endif
{
  Q_OBJECT
public:
  /*! Contructor.
   *
   * For all systems excepted web assembly, all tiles are stored in disque (cache) in the standard cache location.
   * Windows: C:/Users/<USER>/AppData/Local/<APPNAME>/cache
   * MacOs: ~/Library/Caches/<APPNAME>
   * Linux: ~/.cache/<APPNAME>
   * WAsm: no cache.
   *
   *  \param url: The url format to retreive one tile from the server.
   *  \param name: The name of the adapter. This name is used to create the folder where tile files ar stored.
   *  \param tileSize: The size of the tiles in pixels.
   *  \param zoomin: The minimum zoom provides by the server.
   *  \param zoomax: The maximum zoom provides by the server.
   *  \param swapCoordinates: false (default) for server accepting %1=z, %2=x, %3=y (Osm).
   *                          true for server accepting %1=z, %2=y, %3=x (Esri).
   *  \param maxCacheSize: The size cache in Mbytes. -1 (default) means 50Mbytes. The minimum cache size is 10MByes.
   *                       For web assembly, this member is not use (no cache).
   */
  CTileAdapter (QString const & name, int tileSize = 256,
                int zoomMin = 3, int zoomMax = 19, bool swapCoordinates = false,
                int maxCacheSize = -1);

  /*! Contructor.
   *
   * For all systems excepted web assembly, all tiles are stored in disque (cache) in the standard cache location.
   * Windows: C:/Users/<USER>/AppData/Local/<APPNAME>/cache
   * MacOs: ~/Library/Caches/<APPNAME>
   * Linux: ~/.cache/<APPNAME>
   * WAsm: no cache.
   *
   *  \param url: The url format to retreive one tile from the server.
   *  \param name: The name of the adapter. This name is used to create the folder where tile files ar stored.
   *  \param tileSize: The size of the tiles in pixels.
   *  \param zoomin: The minimum zoom provides by the server.
   *  \param zoomax: The maximum zoom provides by the server.
   *  \param swapCoordinates: false (default) for server accepting %1=z, %2=x, %3=y (Osm).
   *                          true for server accepting %1=z, %2=y, %3=x (Esri).
   *  \param maxCacheSize: The size cache in Mbytes. -1 (default) means 50Mbytes. The minimum cache size is 10MByes.
   *                       For web asembly, this member is not use (no cache).
   */
  CTileAdapter (QStringList const & urls, QString const & name, int tileSize = 256,
                int zoomMin = 3, int zoomMax = 19, bool swapCoordinates = false,
                int maxCacheSize = -1);

  /*! Destructor.
   *  The cache is cleared.
   */
  ~CTileAdapter () override;

  QStringList const & urls () const { return m_urls; }
  void setUrls (QStringList const & urls);

  QStringList const & indexNames () const { return m_indexNames; }
  void setIndexNames (QStringList const & indexNames) { m_indexNames = indexNames; }

  int urlIndex () const { return m_urlIndex; }
  void setUrlIndex (int index) { m_urlIndex = index; }

  /*! Returns the tile adapter name. */
  QString const & name () const { return m_name; }

  /*! Set the tile adapter name. */
  void setName (QString const & name) { m_name = name; }

  /*! Adds "User-Agent" to request. */
  void addUserAgent (bool set) { m_userAgent = set; }

  /*! Sets the tile image format. */
  void setImageFormat (QByteArray const & format) { m_imageFormat = format; }

  /*! Returns the size of tiles. */
  int tileSize () const { return m_tileSize; }

  /*! Sends the request to download tile. */
  void tile (int x, int y, int z);

  /*! Returns the downloaded image of tile. */
  QPixmap fromCache (int x, int y, int z);

  /*! Returns the url of the tile. */
  inline QString url (int x, int y, int z);

  /*! Returns the point on tile from the coordinates and zoom.
   *
   *  \param coordinates: The point location (longitude, latitude).
   *  \pamam zoom: The current zoom [minZoom, maxZoom].
   *  \return The point on tile [tileSize, tileSize].
   */
  inline QPoint coordinatesToViewport (TGeoCoord const & coordinates, int zoom) const;

  /*! Returns the point on tile from the coordinates and zoom.
   *
   * The code is based on https://github.com/TheDZhon/QMapControl
   *
   *  \param coordinates: The point location (longitude, latitude).
   *  \pamam zoom: The current zoom [minZoom, maxZoom].
   *  \return The point on tile [tileSize, tileSize].
   */
  TGeoCoord coordinatesToViewportF (TGeoCoord const & coordinates, int zoom) const;

   /*! Returns the coodinates from point on tile zoom.
   *
   *  \param point: The point on tile.
   *  \pamam zoom: The current zoom [minZoom, maxZoom]
   *  \return The point location (longitude, latitude).
   */
  inline TGeoCoord viewportToCoordinates (QPoint const & point, int zoom) const;

  /*! Returns the coodinates from point on tile zoom.
   *
   * The code is based on https://github.com/TheDZhon/QMapControl
   *
   *  \param point: The point on tile.
   *  \pamam zoom: The current zoom [minZoom, maxZoom]
   *  \return The point location (longitude, latitude).
   */
  TGeoCoord viewportToCoordinatesF (TGeoCoord const & point, int zoom) const;

  /*! Returns the min of zoom. */
  int zoomMin () const { return m_zoomMin; }

  /*! Returns the max of zoom. */
  int zoomMax () const { return m_zoomMax; }

  /*! Returns the location in widget coordinates from geo-coordinates
   *
   *  \param geoLoc: The location in WGS84 datum.
   *  \param vw: Viewport widget transformation.
   */
  inline QPoint coordinatesToWidget (TGeoCoord const & geoLoc, CMapShape::SViewportToWidget const & vw);

  /*! Returns the location in widget coordinates from geo-coordinates.
   *  This function is the same that above without integer trucature.
   *
   *  \param geoLoc: The location in WGS84 datum.
   *  \param vw: Viewport widget transformation.
   */
  inline TGeoCoord coordinatesToWidgetF (TGeoCoord const & geoLoc,
                                         CMapShape::SViewportToWidget const & vw);

  /*! Sets the copy rights components.
   *
   *  The copyright component is a pair of QString.
   *  - first: The text showed on the map.
   *  - second: The url corresponding.
   *  When the user click on a copyright, the signal copyrightClicked is emitted
   *  with the corresponding url.
   */
  void setCopyrights (TCopyrights const & copyrights) { m_copyrights = copyrights; }

  /*! Returns the actual copyrights as a const refernce. */
  TCopyrights const & copyrights () const { return m_copyrights; }

  /*! Returns the actual copyrights as a refernce. */
  TCopyrights& copyrights () { return m_copyrights; }

  /*! Returns the number of tiles from zoom value. */
  static inline int tileCountOnZoom (int zoom);

  /*! Replaces "x00A9" in s by QChar (0x00A9). */
  static void updateCopyrightSymbol (QString& s);

protected slots:
  /*! Download error. Show a message to the console. */
  void downloadError (QNetworkReply::NetworkError err);

  /*! Download is finished. Store the url in this. */
  void downloadFinished ();

  /*! Read downloaded data. */
  void downloadReadData ();

signals:
  /*! Download is finished. The tile image is ready. */
  void newTileAvailable ();

protected:
  void updatePixmapFormat ();

protected:
  QString     m_name;                    //!< Name.
  int         m_urlIndex = 0;            //!< Actual url index
  QStringList m_urls;                    //!< The urls of tiles e.g. https://server/%1/%2/%3.png
  QStringList m_indexNames;              //!< The urls name of tiles
  QString     m_apiKey;                  //!< Api key.
  QString     m_message;                 //!< The error message
  QByteArray  m_imageFormat = { "PNG" }; //!< The image format e.g. PNG

  int m_tileSize; //!< Size of the tiles (e.g. 256)
  int m_zoomMin = 3;  //!< zoom min
  int m_zoomMax = 19; //!< zoom max

  QMap<QNetworkReply*, QByteArray> m_replies; //!< Map of network replies
  TCopyrights                      m_copyrights;
  bool                             m_userAgent      = false;
  bool                             m_swapCoordinate = false;
};

int CTileAdapter::tileCountOnZoom (int zoom)
{
  return ::powerOf2 (zoom);
}

QString CTileAdapter::url (int x, int y, int z)
{
  if (m_swapCoordinate)
  {
    std::swap (x, y);
  }

  bool apiKey = m_urls[m_urlIndex].endsWith ("%4");
  return !apiKey ? m_urls[m_urlIndex].arg (z).arg (x).arg (y)
                : m_urls[m_urlIndex].arg (z).arg (x).arg (y).arg (m_apiKey);
}

QPoint CTileAdapter::coordinatesToWidget (TGeoCoord const & geoLoc, CMapShape::SViewportToWidget const & vw)
{
  QPoint loc = coordinatesToViewport (geoLoc, vw.m_zoom);
  int    x   = ::qRound ((loc.x () - vw.m_vx0) * vw.m_rx + vw.m_tx0);
  int    y   = ::qRound ((loc.y () - vw.m_vy0) * vw.m_ry + vw.m_ty0);
  return QPoint (x, y);
}

TGeoCoord CTileAdapter::coordinatesToWidgetF (TGeoCoord const & geoLoc, CMapShape::SViewportToWidget const & vw)
{
  TGeoCoord  loc = coordinatesToViewportF (geoLoc, vw.m_zoom);
  TCoordType x   = ::qRound ((loc.x () - vw.m_vx0) * vw.m_rx + vw.m_tx0);
  TCoordType y   = ::qRound ((loc.y () - vw.m_vy0) * vw.m_ry + vw.m_ty0);
  return TGeoCoord (x, y);
}

QPoint CTileAdapter::coordinatesToViewport (TGeoCoord const & coordinates, int zoom) const
{
  TGeoCoord p = coordinatesToViewportF (coordinates, zoom);
  return QPoint (::qRound (p.x ()), ::qRound (p.y ()));
}

TGeoCoord CTileAdapter::viewportToCoordinates (QPoint const & point, int zoom) const
{
  TGeoCoord p (point.x (), point.y ());
  return viewportToCoordinatesF (p, zoom);
}

#endif // TILEADAPTER_HPP
