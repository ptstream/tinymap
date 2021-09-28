#include "ellipsehelper.hpp"

static inline TCoordType robustLen (TCoordType v0, TCoordType v1)
{
  TCoordType len;
  TCoordType av0 = std::fabs (v0);
  TCoordType av1 = std::fabs (v1);
  if (av0 == av1)
  {
    len = av1 == 0 ? 0 : std::sqrt (av0 * av0 + av1 * av1);
  }
  else
  {
    if (av1 > av0)
    {
      std::swap (av0, av1);
    }

    TCoordType r = av1 / av0;
    len          = av0 * std::sqrt (1 + r * r);
  }

  return len;
}

TCoordType CEllipseHelper::getRoot (TCoordType r0 , TCoordType z0 , TCoordType z1 , TCoordType g)
{
  m_iterationsCount = 0;
  TCoordType n0     = r0 * z0;
  TCoordType s0     = z1 - 1;
  TCoordType s1     = g < 0 ? 0 : robustLen (n0, z1) - 1;
  TCoordType s      = 0;
  int        i;
  for (i = 0; i < m_maxIterations; ++i)
  {
    s = (s0 + s1) / 2 ;
    if (s == s0 || s == s1)
    {
      break;
    }

    TCoordType ratio0 = n0 / (s + r0);
    TCoordType ratio1 = z1 / (s + 1);
    g                 = ratio0 * ratio0 + ratio1 * ratio1 - 1;
    if (g > 0)
    {
      s0 = s;
    }
    else if (g < 0)
    {
      s1 = s;
    }
    else
    {
      break;
    }
  }

  m_iterationsCount = i;
  return s;
}

// e0 ≥ e1 > 0, x ≥ 0, and y ≥ 0.
TCoordType CEllipseHelper::distancePointQuarterEllipse (TCoordType e0, TCoordType e1, TGeoCoord& p)
{
  TCoordType xx0 = p.x ();
  TCoordType yy0 = p.y ();
  TCoordType xx1, yy1;
  TCoordType distance;
  if (yy0 > 0)
  {
    if (xx0 > 0)
    {
      TCoordType z0 = xx0 / e0;
      TCoordType z1 = yy0 / e1;
      TCoordType g  = z0 * z0 + z1 * z1 - 1;
      if (g != 0)
      {
        TCoordType r0   = e0 / e1;
        r0             *= r0;
        TCoordType sbar = getRoot (r0 , z0 , z1 , g);
        xx1             = r0 * xx0 / (sbar + r0);
        yy1             = yy0 / (sbar + 1);
        TCoordType dx   = xx1 - xx0;
        TCoordType dy   = yy1 - yy0;
        distance        = std::sqrt (dx * dx + dy * dy);
      }
      else
      {
        xx1      = xx0;
        yy1      = yy0 ;
        distance = 0;
      }
    }
    else // xx0 == 0
    {
      xx1      = 0;
      yy1      = e1;
      distance = std::fabs (yy0 - e1);
    }
  }
  else
  { // yy0 == 0
    TCoordType numer0 = e0 * xx0, denom0 = e0 * e0 - e1 * e1;
    if (numer0 < denom0)
    {
      TCoordType xde0 = numer0 / denom0;
      xx1             = e0 * xde0;
      yy1             = e1 * std::sqrt (1 - xde0 * xde0);
      TCoordType dx   = xx1 - xx0;
      distance        = std::sqrt (dx * dx + yy1 * yy1);
    }
    else
    {
      xx1      = e0;
      yy1      = 0;
      distance = std::fabs (xx0 - e0);
    }
  }

  p.x () = xx1;
  p.y () = yy1;
  return distance;
}

TCoordType CEllipseHelper::distancePointEllipse (TCoordType e0, TCoordType e1, TGeoCoord& p)
{
  enum { XNeg = 1, YNeg = 2, E0LTE1 = 4 };

  unsigned code = 0;
  if (e1 > e0)
  { // Rotate by 90° to have e1 > e0.
    code        |= E0LTE1;
    TCoordType x = p.x ();
    p.x ()       = p.y ();
    p.y ()       = -x;
    std::swap (e0, e1);
  }

  if (p.x () < 0)
  { // Make x >= 0.
    p.x () = -p.x ();
    code  |= XNeg;
  }

  if (p.y () < 0)
  { // Make y >= 0.
    p.y () = -p.y ();
    code  |= YNeg;
 }

  TCoordType d = distancePointQuarterEllipse (e0, e1, p);

  if ((code & XNeg) != 0)
  { // Restore the original coordinate system.
    TCoordType x = p.x ();
    p.x ()       = p.y ();
    p.y ()       = -x;
  }

  if ((code & XNeg) != 0)
  { // Restore x axis direction.
    p.x () = -p.x ();
  }

  if ((code & YNeg) != 0)
  { // Restore y axis direction.
    p.y () = -p.y ();
  }

  return d;
}
