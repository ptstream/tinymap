#include "mapcircle.hpp"
#include "tileadapter.hpp"
#include "../tools/ellipsehelper.hpp"
#include <QPainter>

CAabb CMapCircle::aabb (TCoordType, TCoordType) const
{
  // Calculate aabb of the circle (extract from Qt QGeoCircle).
  TCoordType dr   = m_radius / earthMeanRadius;
  TCoordType dlat = ::rdToDg (dr);
  TCoordType x    = m_coordinates.x ();
  TCoordType y    = m_coordinates.y ();
  TCoordType dlon = ::rdToDg (std::asin (std::sin (dr) / std::cos (::dgToRd (y))));

  TGeoCoord tl (x - dlon, y + dlat);
  TGeoCoord br (x + dlon, y - dlat);
  CAabb     aabb;
  aabb.add (tl);
  aabb.add (br);
  return aabb;
}

void CMapCircle::draw (QPainter* painter, CTileAdapter* tileAdapter, SViewportToWidget const & vt) const
{
  if (CStatus::contains (Visible))
  {
    updatePen (painter, m_color, m_width);
    updateBrush (painter, 0);

    CAabb aabb = this->aabb ();
    QRect rc (tileAdapter->coordinatesToWidget (aabb.tl (), vt), tileAdapter->coordinatesToWidget (aabb.br (), vt));
    painter->drawEllipse (rc);
  }
}

bool CMapCircle::isVisible (CAabb const &) const
{
  return CStatus::contains (Visible);
}

bool CMapCircle::contains (TGeoCoord const & coords, TCoordType dx, TCoordType dy) const
{
  dx                 *= m_pickingSize;
  dy                 *= m_pickingSize;
  TCoordType dp       = std::sqrt (dx * dx + dy * dy);
  bool       contains = CMapShape::isVisible ();
  if (contains)
  {
    CAabb      aabb = this->aabb ();
    TCoordType a    = aabb.width ()  / 2; // Major axis
    TCoordType b    = aabb.height () / 2; // Minor axis
    aabb.enlarge (static_cast<TCoordType>(0.02)); // +2% to be sure to launch the proximity calculation with the ellipse.
    contains = aabb.contains (coords);
    if (contains)
    {
      TGeoCoord   p = coords - m_coordinates;
      TCoordType  d = CEllipseHelper ().distancePointEllipse (a, b, p);
      contains      = d < dp;
    }
  }

  return contains;
}
