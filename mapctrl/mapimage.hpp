#ifndef MAPIMAGE_HPP
#define MAPIMAGE_HPP

#include "mapanchoredlocation.hpp"
#include <QPixmap>

/*! \brief The CMapImage class defines a rectangular area covered by a pixmap.
 *
 *  The image is defined in terms of a TGeoCoord which specifies the location, a pixmap and the anchor point.
 *  The anchor point define the position in pixels from the geographic location (coordinates).
 */
class CMapImage : public CMapAnchoredLocation
{
public:
  /*! Contructor.
   *  \param id: The map shape identifier.
   */
  CMapImage (TMapShapeId id = 0) : CMapAnchoredLocation (Image, id) { m_color = 0x00000000; }

  /*! Contructor.
   *  \param coordinates: The geographic location.
   *  \param image: The pixmap.
   *  \param id: The map shape identifier.
   */
  CMapImage (TGeoCoord const & coordinates, QPixmap const & image, TMapShapeId id = 0) :
    CMapAnchoredLocation (coordinates, Image, id), m_pixmap (image) { m_color = 0x00000000; }

  /*! Contructor.
   *  \param coordinates: The geographic location.
   *  \param anchorPoint: The pixel position from coordinates.
   *  \param image: The pixmap.
   *  \param id: The map shape identifier.
   */
  CMapImage (TGeoCoord const & coordinates, QPoint const & anchorPoint, QPixmap const & image, TMapShapeId id = 0) :
    CMapAnchoredLocation (coordinates, anchorPoint, Image, id), m_pixmap (image) { m_color = 0x00000000; }

  /*! Returns the pixmap. */
  QPixmap const & image () const { return m_pixmap; }

  /*! Sets the pixmap. */
  void setImage (QPixmap const & image) { m_pixmap = image; }

  /*! See the base class. */
  void draw (QPainter* painter, CTileAdapter* tileAdapter, SViewportToWidget const & vt) const override;
  bool isVisible (CAabb const &) const override;
  bool contains (TGeoCoord const & coords, TCoordType dx = 0, TCoordType dy = 0) const override;
  CAabb aabb (TCoordType dx, TCoordType dy) const override;

protected:
  QPixmap m_pixmap;
};

#endif // MAPIMAGE_HPP
