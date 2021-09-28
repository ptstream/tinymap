#ifndef MAPCIRCLE_HPP
#define MAPCIRCLE_HPP

#include "maplocation.hpp"

/*! \brief The CMapCircle class defines a circular geographic curve.
 *
 *  The circle is defined in terms of a TGeoCoord which specifies the center
 *  of the circle and a TCoordType which specifies the radius of the circle in meters.
 *  It is also possible to specify the width in pixel of the pen.
 */
class CMapCircle : public CMapLocation
{
public:
  /*! Contructor.
   *  \param id: The map shape identifier.
   */
  CMapCircle (TMapShapeId id = 0) : CMapLocation (Circle, id) {}

  /*! Contructor.
   *  \param center: The center (CMapLocation::m_coordinates).
   *  \param radius: the radius in meters.
   *  \param id: The map shape identifier.
   */
  CMapCircle (TGeoCoord const & center, TCoordType radius, TMapShapeId id = 0) :
    CMapLocation (center, Circle, id), m_radius (radius) {}

  /*! Returns the pen width in pixels. */
  int width () const { return m_width; }

  /*! Sets the pen width in pixels. */
  void setWidth (int width) { m_width = width; }

  /*! Returns the center. */
  TGeoCoord center () const { return m_coordinates; }

  /*! Sets the center. */
  void setCenter (TGeoCoord const & center) { m_coordinates = center; }

  /*! Returns the radius in meters. */
  TCoordType radius () const { return m_radius; }

  /*! Sets the radius in meters. */
  void setRadius (TCoordType radius) { m_radius = radius; }

  /*! See the same functions on the base class. */
  void draw (QPainter* painter, CTileAdapter* tileAdapter, SViewportToWidget const & vt) const override;
  bool isVisible (CAabb const &) const override;
  bool contains (TGeoCoord const & coords, TCoordType dx, TCoordType dy) const override;
  CAabb aabb (TCoordType = 0, TCoordType = 0) const override;

protected:
  TCoordType m_radius;    //!< The radius in meters.
  int        m_width = 1; //!< The pen size in pixels.
};

#endif // MAPCIRCLE_HPP
