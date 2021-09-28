#include "mapcross.hpp"

#include "tileadapter.hpp"
#include <QPainter>

CAabb CMapCross::aabb (TCoordType dx, TCoordType dy) const
{
  dx *= m_width;
  dy *= m_height;
  CAabb aabb;
  aabb.add (m_coordinates);
  aabb.enlarge (dx, dy, dx, dy);
  return aabb;
}

void CMapCross::draw (QPainter* painter, CTileAdapter* tileAdapter, SViewportToWidget const & vt) const
{
  if (CStatus::contains (Visible))
  {
    updatePen (painter, m_color, 1);
    QPoint loc = tileAdapter->coordinatesToWidget (m_coordinates, vt);
    int    x   = loc.x ();
    int    y   = loc.y ();
    painter->drawLine (x - m_width, y, x + m_width, y);
    painter->drawLine (x, y - m_height, x , y + m_height);
  }
}

bool CMapCross::isVisible (CAabb const & aabb) const
{
  return CStatus::contains (Visible) && aabb.contains (m_coordinates);
}

bool CMapCross::contains (TGeoCoord const & coords, TCoordType dx, TCoordType dy) const
{
  CAabb aabb     = this->aabb (dx, dy);
  bool  contains = aabb.contains (coords);
  return contains;
}
