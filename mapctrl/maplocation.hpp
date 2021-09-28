#ifndef MAPLOCATION_HPP
#define MAPLOCATION_HPP

#include "mapshape.hpp"

/*! \brief Base class of all shapes with a single geo-coordinate positionning.
 *  The map location is defined by a CMapShape and one geo-coordinate.
 *  It is the base class of CMapAnchoredLocation, CMapCircle.
 */
class CMapLocation : public CMapShape
{
public:
  /*! Contructor.
   *  \param type: Shape type.
   *  \param id: Identifier.
   */
  CMapLocation (EType type, TMapShapeId id = 0) : CMapShape (type, id) {}

  /*! Contructor.
   *  \param coordinates: geo-location.
   *  \param type: Shape type.
   *  \param id: Identifier.
   */
  CMapLocation (TGeoCoord coordinates, EType type, TMapShapeId id = 0) : CMapShape (type, id),
    m_coordinates (coordinates) {}

  /*! Returns the location. */
  TGeoCoord const & coordinates () const { return m_coordinates; }

  /*! Sets the location. */
  void setCoordinates (TGeoCoord const & coordinates) { m_coordinates = coordinates; }

protected:
  TGeoCoord m_coordinates; //!< The location in WGS84 datum.
};

#endif // MAPLOCATION_HPP
