#ifndef MAPPOLYLINE_HPP
#define MAPPOLYLINE_HPP

#include "mapshape.hpp"

/*! \brief The CMapPolyline class defines a geographic polyline.
 *
 *  The polyline is defined by an ordered list of TGeoCoord representing its path.
 *  The polyline is defined by:
 *  - The vertex list.
 *  - The border color.
 *  - The border width in pixels.
 */
class CMapPolyline : public CMapShape
{
public:
  /*! Creates a new polyline.
   *  \param id: The map shape identifier.
   */
  CMapPolyline (TMapShapeId id = 0) : CMapShape (Polyline, id) {}

  /*! Creates a new polyline.
   *  \param path: The list of vertexes.
   *  \param id: The map shape identifier.
   *
   *  \remark The bounding box is automatically computed.
   */
  CMapPolyline (TPath const & path, TMapShapeId id = 0) : CMapShape (Polyline, id), m_path (path) {}

  /*! Returns the list of vertexes. */
  TPath const & path () const { return m_path; }

  /*! Sets the list of vertexes. */
  void setPath (TPath const & path) { m_path = path; }

  /*! Returns the pen width in pixels. */
  int width () const { return m_width; }

  /*! Sets the pen width in pixels. */
  void setWidth (int width) { m_width = width; }

  /*! See the same functions on the base class. */
  void draw (QPainter* painter, CTileAdapter* tileAdapter, SViewportToWidget const & vt) const override;
  bool isVisible (CAabb const &) const override;
  bool contains (TGeoCoord const & coords, TCoordType dx, TCoordType dy) const override;
  CAabb aabb (TCoordType = 0, TCoordType = 0) const override;

  /*! Returns if the polyline intersects a rectangle.
   *  \param x0, y0: the first extremity
   *  \param x1, y1: the second extremity
   *  \param xmin, ymin, xmax, ymax: The rectangle.
   */
  static bool passThrough (TCoordType x0, TCoordType y0, TCoordType x1, TCoordType y1, TCoordType xmin, TCoordType ymin, TCoordType xmax, TCoordType ymax);

protected:
  TPath m_path;
  int   m_width = 1;
};

#endif // MAPPOLYLINE_HPP
