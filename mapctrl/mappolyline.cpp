#include "mappolyline.hpp"
#include "tileadapter.hpp"
#include <QPainter>

CAabb CMapPolyline::aabb (TCoordType, TCoordType) const
{
  CAabb aabb;
  for (TGeoCoord const & vertex : m_path)
  {
    aabb.add (vertex);
  }

  return aabb;
}

void CMapPolyline::draw (QPainter* painter, CTileAdapter* tileAdapter, SViewportToWidget const & vt) const
{
  if (CStatus::contains (Visible))
  {
    updatePen (painter, m_color, m_width);
    QPolygonF polygon;
    for (TGeoCoord const & vertex : m_path)
    {
      QPoint pi = tileAdapter->coordinatesToWidget (vertex, vt);
      polygon.append (QPointF (pi));
    }

    painter->drawPolyline (polygon);
  }
}

bool CMapPolyline::isVisible (CAabb const &) const
{
   return CStatus::contains (Visible);
}

enum EPos : quint8 { INSIDE = 0, // 0000
                     LEFT   = 1, // 0001
                     RIGHT  = 2, // 0010
                     BOTTOM = 4, // 0100
                     TOP    = 8, // 1000
                   };

bool CMapPolyline::passThrough (TCoordType x0, TCoordType y0, TCoordType x1, TCoordType y1, TCoordType xmin, TCoordType ymin, TCoordType xmax, TCoordType ymax)
{
  auto code = [xmin, ymin, xmax, ymax] (TCoordType x, TCoordType y) -> quint8
  {
    quint8 c = INSIDE;
    if (x < xmin)
    {
      c |= LEFT;
    }
    else if (x > xmax)
    {
      c |= RIGHT;
    }

    if (y < ymin)
    {
      c |= BOTTOM;
    }
    else if (y > ymax)
    {
      c |= TOP;
    }

    return c;
  };

  quint8 c0        = code (x0, y0);
  quint8 c1        = code (x1, y1);
  bool passThrough = false;
  for (;;)
  {
    if ((c0 | c1) == 0)
    { // Entirely inside the rectangle.
      passThrough = true;
      break;
    }

    if ((c0 & c1) != 0)
    { // Entirely outside the rectangle.
      break;
    }

    // Now find the intersection point.
    // Use formulas:
    //   slope = (y1 - y0) / (x1 - x0)
    //   x = x0 + (1 / slope) * (ym - y0), where ym is ymin or ymax
    //   y = y0 + slope * (xm - x0), where xm is xmin or xmax
    // No need to worry about divide-by-zero because, in each case, the
    // c bit being tested guarantees the denominator is non-zero
    TCoordType    x, y;
    quint8 c = c1 > c0 ? c1 : c0;
    if ((c & TOP) != 0)
    {
      x = x0 + (x1 - x0) * (ymax - y0) / (y1 - y0);
      y = ymax;
    }
    else if ((c & BOTTOM) != 0)
    {
      x = x0 + (x1 - x0) * (ymin - y0) / (y1 - y0);
      y = ymin;
    }
    else if ((c & RIGHT) != 0)
    {
      y = y0 + (y1 - y0) * (xmax - x0) / (x1 - x0);
      x = xmax;
    }
    else
    {
      y = y0 + (y1 - y0) * (xmin - x0) / (x1 - x0);
      x = xmin;
    }

    if (c == c0)
    {
      x0 = x;
      y0 = y;
      c0 = code (x0, y0);
    }
    else
    {
      x1 = x;
      y1 = y;
      c1 = code (x1, y1);
    }
  }

  return passThrough;
}

bool CMapPolyline::contains (TGeoCoord const & coords, TCoordType dx, TCoordType dy) const
{
  dx *= std::max (m_pickingSize, m_width);
  dy *= std::max (m_pickingSize, m_width);

  TCoordType  xmin = (coords.x () - dx);
  TCoordType  ymin = (coords.y () - dy);
  TCoordType  xmax = (coords.x () + dx);
  TCoordType  ymax = (coords.y () + dy);

  bool passThrough = false;
  for (TPath::const_iterator it = m_path.cbegin (), end = m_path.cend () - 1; it != end && !passThrough; ++it)
  {
    TGeoCoord const & v0 = *it;
    TGeoCoord const & v1 = *(it + 1);
    passThrough          = this->passThrough (v0.x (), v0.y (), v1.x (), v1.y (), xmin, ymin, xmax, ymax);
  }

  return passThrough;
}
