#ifndef TOWN_HPP
#define TOWN_HPP

#include "../tools/aabb.hpp"
#include <QDataStream>
#include <QSharedDataPointer>

/*! TOWN_VERSION is (major << 16) + (minor << 8) + patch. */
#define TOWN_VERSION 0x010000

struct STownData;

/*! Read a vertex. */
inline QDataStream& operator >> (QDataStream& in, TGeoCoord& vertex)
{
  in >> vertex.x ();
  in >> vertex.y ();
  return in;
}

/*! Read a vertex. */
inline QDataStream& operator >> (QDataStream& in, CAabb& aabb)
{
  TGeoCoord v;
  in >> v; aabb.add (v);
  in >> v; aabb.add (v);
  return in;
}

/*! \brief The is the town container.
 *
 *  The town use QSharedDataPointer technology to minimize data copy.
 */
class CTown
{
public:
  using TTownCode   = int;
  using TRegionCode = int;

  /*! The town polygon. */
  using TPath  = QVector<TGeoCoord>;

  /*! The town list of polygons. */
  using TPaths = QVector<TPath>;

  /*! Default constructor. */
  CTown () : m_d (new STownData) {}

  /*! Copy constructor. */
  CTown (CTown const & rhs) : m_d (rhs.m_d) {}

  /*! Default destructor. */
  ~CTown () = default;

  /*! Assignation operator. */
  inline CTown& operator = (CTown const & rhs);

  /*! Equality operator. */
  inline bool operator == (CTown const & rhs) const;

  /*! Returns the town code (insee). */
  TTownCode code () const { return m_d->m_code; }

  /*! Returns the town name. */
  QString const & name () const { return m_d->m_name; }

  /*! Returns the town region code. */
  TRegionCode region () const { return m_d->m_region; }

  /*! Returns the polygons. */
  TPaths& paths () { return m_d->m_paths; }

  /*! Returns the polygons. */
  TPaths const & paths () const { return m_d->m_paths; }

  /*! Returns the bounding box of the polygons. */
  CAabb aabb () const { return m_d->m_aabb; };

  /*! Returns the area in m2. */
  TCoordType area () const { return m_d->m_area; }

  /*! Sets the town code (insee). */
  void setCode (TTownCode code) { m_d->m_code = code; }

  /*! Sets the town name. */
  void setName (QString const & name) { m_d->m_name = name; }

  /*! Sets the town region code. */
  void setRegion (TRegionCode region) { m_d->m_region = region; }

  /*! Sets the polygons. */
  void setPaths (TPaths const & paths, bool boundingBox);

  /*! Sets the bounding box of the polygons. */
  void setAabb (CAabb const & bb) { m_d->m_aabb = bb; };

  /*! Sets the area in m2. */
  void setArea (TCoordType area) { m_d->m_area = area; }

  /*! Returns true if the polygons contain the point (latitude, longitude). */
  bool contains (TCoordType x, TCoordType y) const;

  /*! Returns true if the bounding box contains the point (latitude, longitude). */
  bool aabbContains (TCoordType x, TCoordType y) const { return m_d->m_aabb.contains (x, y); }

  /*! Adds paths. */
  void appendPaths (TPaths const & paths);

  /*! Update the centroid. */
  void updateCentroid ();

  /*! Returns the centroid. */
  TGeoCoord const & centroid () const { return m_d->m_centroid; }

  /*! Converts string town code from file to int. */
  static TTownCode townCode (QString const & code) { return code.toInt (nullptr, 16); }

  /*! Converts string region code from file to int. */
  static TRegionCode regionCode (QString const & code) { return code.toInt (nullptr, 10); }

private:
  struct STownData : public QSharedData
  {
    TTownCode    m_code;
    TRegionCode  m_region;
    QString      m_name;
    TCoordType   m_area = 0;
    TPaths       m_paths;
    CAabb        m_aabb;
    TGeoCoord    m_centroid;
  };

  QSharedDataPointer<STownData> m_d;
};

CTown& CTown::operator = (CTown const & rhs)
{
  if (this != &rhs)
  {
    m_d.operator = (rhs.m_d);
  }

  return *this;
}

bool CTown::operator == (CTown const & rhs) const
{
  return code () == rhs.code ();
}

/*! Reads a path. It is a list of vertexes. */
QDataStream& operator >> (QDataStream& in, CTown::TPath& path);

/*! Reads a paths. It is a list of paths. */
QDataStream& operator >> (QDataStream& in, CTown::TPaths& paths);

/*! \brief Reads town from file.
 *
 *  The file contains:
 *  magicNumber 2
 *  version     0
 *  m_code      Town code converted in int from base 16 string
    m_region    Region code converted in int from base 10 string
    m_name      Town Name
    m_area      Town area
    m_paths     Town paths (contours)
    CAabb       Town axis alignment bouding box
 */
 QDataStream& operator >> (QDataStream& in, CTown& town);

#endif // TOWN_HPP
