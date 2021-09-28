#ifndef AABB_HPP
#define AABB_HPP

#include "tglobals.hpp"

/*! \brief The CAabb class provides a simple axis aligment rectangular bounding box in 2d dimension.
 *
 *  This bounding box is very similar than CRetF but it use TCoordType coordinates.
 */
class CAabb
{
public:
  /*! Default constructor. */
  inline CAabb ();

  /*! Contructor with two points. */
  inline CAabb (TGeoCoord const & v0, TGeoCoord const & v1);

  /*! Returns top left corner. */
  inline TGeoCoord const & tl () const;

  /*! Returns bottom right corner. */
  inline TGeoCoord const & br () const;

  /*! Returns bottom left corner. */
  inline TGeoCoord bl () const;

  /*! Returns top right corner. */
  inline TGeoCoord tr () const;

  /*! Returns the width. */
  TCoordType width () const { return m_v1.x () - m_v0.x (); }

  /*! Returns the height. */
  TCoordType height () const { return m_v1.y () - m_v0.y (); }

  /*! Returns true if the box contains the point. */
  inline bool contains (TCoordType x, TCoordType y) const;

  /*! Returns true if the box contains the point. */
  inline bool contains (TGeoCoord const & vertex) const;

  /*! Returns true if the box contains the aabb. */
  inline bool contains (CAabb const & other) const;

  /*! Returns true if the box intersetcs other. */
  bool intersects (CAabb const & other) const;

  /*! Returns the center of the box. */
  inline TGeoCoord center () const;

  /*! Returns the radius of the box. It is the circle centered at center and tangent at the biggest dimension. */
  inline TCoordType radius () const;

  /*! Returns true is width > 0 and height > 0. */
  inline bool isValid () const;

  /*! Returns the diagonal. */
  inline TCoordType diag () const;

  /*! Returns the area. */
  inline TCoordType area () const;

  /*! Initializes the box. The box is invalid. */
  inline void init ();

  /*! Expands the box to contain the point. */
  inline void add (TCoordType x, TCoordType y);

  /*! Expands the box to contain the point. */
  inline void add (TGeoCoord vertex);

  /*! Expands the box to contain another box. */
  inline void add (CAabb const & other);

  /*! Enlarges the box of x precent. */
  inline void enlarge (TCoordType percent);

  /*! Enlarges the box with 4 dimensions. */
  inline void enlarge (TCoordType l, TCoordType t, TCoordType r, TCoordType b);

protected:
  TGeoCoord m_v0, m_v1; //!< Top left and bottom right.
};

CAabb::CAabb ()
{
  init ();
}

CAabb::CAabb (TGeoCoord const & v0, TGeoCoord const & v1)
{
  init ();
  add (v0);
  add (v1);
}

bool CAabb::isValid () const
{
  return m_v0.x () < m_v1.x () && m_v0.y () < m_v1.y ();
}

TCoordType CAabb::diag () const
{
  return len (m_v0, m_v1);
}

void CAabb::init ()
{
  m_v0.x () = std::numeric_limits<TCoordType>::max ();
  m_v0.y () = m_v0.x ();
  m_v1.x () = -m_v0.x ();
  m_v1.y () = m_v1.x ();
}

void CAabb::add (TCoordType x, TCoordType y)
{
  m_v0.x () = std::min (x, m_v0.x ());
  m_v0.y () = std::min (y, m_v0.y ());
  m_v1.x () = std::max (x, m_v1.x ());
  m_v1.y () = std::max (y, m_v1.y ());
}

void CAabb::add (TGeoCoord vertex)
{
  add (vertex.x (), vertex.y ());
}

void CAabb::add (CAabb const & other)
{
  TGeoCoord const & bl = other.bl ();
  TGeoCoord const & tr = other.tr ();
  add (bl);
  add (tr);
}

TGeoCoord const & CAabb::tl () const
{
  return m_v0;
}

TGeoCoord const & CAabb::br () const
{
  return m_v1;
}

TGeoCoord CAabb::bl () const
{
  return { m_v0.x (), m_v1.y () };
}

TGeoCoord CAabb::tr () const
{
  return { m_v1.x (), m_v0.y () };
}

bool CAabb::contains (TCoordType x, TCoordType y) const
{
  return x >= m_v0.x () && x <= m_v1.x () && y >= m_v0.y () && y <= m_v1.y ();
}

bool CAabb::contains (TGeoCoord const & vertex) const
{
  return contains (vertex.x (), vertex.y ());
}

inline bool CAabb::contains (CAabb const & other) const
{
  return contains (other.tl ()) && contains (other.br ());
}

void CAabb::enlarge (TCoordType percent)
{
  if (percent != 0)
  {
    TCoordType dx = this->width ()  * percent;
    TCoordType dy = this->height () * percent;
    m_v0.x ()    -= dx;
    m_v1.x ()    += dx;
    m_v0.y ()    -= dy;
    m_v1.y ()    += dy;
  }
}

void CAabb::enlarge (TCoordType l, TCoordType t, TCoordType r, TCoordType b)
{
  m_v0.x () -= l;
  m_v1.x () += r;
  m_v0.y () -= t;
  m_v1.y () += b;
}

TCoordType CAabb::area () const
{
  return std::fabs (width ()) * std::fabs (height ());
}

TGeoCoord CAabb::center () const
{
  return mid (m_v0, m_v1);
}

TCoordType CAabb::radius () const
{
  TCoordType w = std::fabs (width ());
  TCoordType h = std::fabs (height ());
  return static_cast<TCoordType>(0.5) * (w < h ? h : w);
}

#endif // AABB_HPP
