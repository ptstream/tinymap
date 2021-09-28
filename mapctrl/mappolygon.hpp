#ifndef MAPPOLYGON_HPP
#define MAPPOLYGON_HPP

#include "mapshape.hpp"

class QPainter;
class QBrush;
class CTileAdapter;

/*! \brief The CMapPolygon class defines a geographic polygon.
 *
 *  The polygon is defined by an ordered list of TGeoCoord representing its perimeter.
 *  The polygon can contain several contours, these will be displayed as independent polygons
 *  with the same identifier. Contours other than the first contour do not represent holes.
 *  The polygon is defined by:
 *  - The vertex lists.
 *  - The border color.
 *  - The border width in pixels.
 */
class CMapPolygon : public CMapShape
{
public:
  /*! Creates a new polygon.
   *  \param id: The map shape identifier.
   */
  CMapPolygon (TMapShapeId id = 0) : CMapShape (Polygon, id) {}

  /*! Creates a new polygon.
   *  \param paths: The list of contours.
   *  \param id: The map shape identifier.
   *
   *  \remark The bounding box is automatically computed.
   */
  CMapPolygon (TPaths const & paths, TMapShapeId id = 0);

  /*! Creates a new polygon.
   *  \param paths: The list of contours.
   *  \param aabb: The bounding box.
   *  \param id: The map shape identifier.
   */
  CMapPolygon (TPaths const & paths, CAabb const & aabb, TMapShapeId id = 0) : CMapShape (Polygon, id),
    m_paths (paths), m_aabb (aabb) {}

  /*! Returns the border color */
  QRgb borderColor () const { return m_color; }

  /*! Sets the border color; */
  void setBorderColor (QRgb color) { m_borderColor = color; }

  /*! Returns the border width in pixels */
  int borderWidth () const { return m_borderWidth; }

  /*! Sets the border width in pixels */
  void setBorderWidth (int width) { m_borderWidth = width; }

  /*! Sets the bounding box. */
  void setAabb (CAabb const & aabb) { m_aabb = aabb; }

  /*! Returns the number of vertexes. */
  int vertexCount () const;

  /*! See the same functions on the base class. */
  void draw (QPainter* painter, CTileAdapter* tileAdapter, SViewportToWidget const & vt) const override;
  bool isVisible (CAabb const & aabb) const override;
  bool contains (TGeoCoord const & coords, TCoordType, TCoordType) const override;
  CAabb aabb (TCoordType = 0, TCoordType = 0) const override;

protected:
  QRgb   m_borderColor = 0xFF000000;
  int    m_borderWidth = 1;
  TPaths m_paths;
  CAabb  m_aabb;
};

#endif // MAPPOLYGON_HPP
