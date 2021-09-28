#ifndef MAPCROSS_HPP
#define MAPCROSS_HPP

#include "maplocation.hpp"

/*! \brief The CMapCross is defined in terms of a TGeoCoord which specifies the location of the cross.
 *  It is possible to specify the width and the height in pixel of the cross.
 *  but the horizontal and vertical size of the cross are independent on the zoom.
 */
class CMapCross : public CMapLocation
{
public:
  /*! Size of the cross in pixels. */
  enum ECrossSize { SizeX = 3, sizeY = 3};

  /*! Contructor.
   *  \param id: The map shape identifier.
   *  \param width: Width in pixels (e.g. left to center size).
   *  \param height: height in pixels (e.g. top to center size).
   */
  CMapCross (TMapShapeId id = 0, int width = SizeX, int height = sizeY) :
    CMapLocation (Cross, id),  m_width (width), m_height (height) {}

  /*! Contructor.
   *  \param coordinates: The location.
   *  \param id: The map shape identifier.
   *  \param width: Width in pixels (e.g. left to center size).
   *  \param height: height in pixels (e.g. top to center size).
   */
  CMapCross (TGeoCoord const & coordinates, TMapShapeId id = 0, int width = SizeX, int height = sizeY) :
    CMapLocation (coordinates, Cross, id),  m_width (width), m_height (height) {}

  /*! Returns the width. */
  int width () const { return m_width; }

  /*! Sets the width. */
  void setWidth (int width) { m_width = width; }

  /*! Returns the height. */
  int height () const { return m_height; }

  /*! Sets the height. */
  void setHeight (int height) { m_height = height; }

  /*! See the base class. */
  void draw (QPainter* painter, CTileAdapter* tileAdapter, SViewportToWidget const & vt) const override;
  bool isVisible (CAabb const & aabb) const override;
  bool contains (TGeoCoord const & coords, TCoordType dx = 0, TCoordType dy = 0) const override;
  CAabb aabb (TCoordType dx, TCoordType dy) const override;

protected:
  int m_width = SizeX, m_height = sizeY; // Half cross dimensions
};

#endif // MAPCROSS_HPP
