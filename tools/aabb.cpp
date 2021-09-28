#include "aabb.hpp"

bool CAabb::intersects (CAabb const & r) const
{
  TCoordType l1 = m_v0.x ();
  TCoordType r1 = m_v1.x ();
  TCoordType l2 = r.m_v0.x ();
  TCoordType r2 = r.m_v1.x ();
  bool  in = l1 < r2 && l2 < r1;
  if (in)
  {
    TCoordType t1 = m_v0.y ();
    TCoordType b1 = m_v1.y ();
    TCoordType t2 = r.m_v0.y ();
    TCoordType b2 = r.m_v1.y ();
    in            = t1 < b2 && t2 < b1;
  }

  return in;
}
