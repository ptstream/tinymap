#ifndef MAPANCHOREDLOCATION_HPP
#define MAPANCHOREDLOCATION_HPP

#include "maplocation.hpp"

/*! \brief Base class of all shapes with the positionning defined by one geo-coordinate.
 *  The map location is defined by a CMapShape and one geo-coordinate.
 *  It is the base class of CMapText, CMapCross, CMapImage, CMapCircle.
 */
class CMapAnchoredLocation : public CMapLocation
{
public:
  /*! Contructor.
   *  \param type: Shape type.
   *  \param id: identifier.
   */
  CMapAnchoredLocation (EType type, TMapShapeId id = 0) : CMapLocation (type, id) {}

  /*! Contructor.
   *  \param type: Shape type.
   *  \param id: identifier.
   *  \param coordinates: geo-location.
   *
   *  \remark In this case the anchor point is QPoint (0, 0).
   */
  CMapAnchoredLocation (TGeoCoord const & coordinates, EType type, TMapShapeId id = 0) :
    CMapLocation (coordinates, type, id), m_anchorPoint (QPoint ()) {}

   /*! Contructor.
   *  \param type: Shape type.
   *  \param id: identifier.
   *  \param coordinates: geo-location.
   *  \param anchorPoint: anchor point in pixels.
   */
  CMapAnchoredLocation (TGeoCoord const & coordinates, QPoint const & anchorPoint, EType type, TMapShapeId id = 0) :
    CMapLocation (coordinates, type, id), m_anchorPoint (anchorPoint) {}

protected:
  QPoint m_anchorPoint;
};

#endif // MAPANCHOREDLOCATION_HPP
