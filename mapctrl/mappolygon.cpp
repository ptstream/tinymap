#include "mappolygon.hpp"
#include "tileadapter.hpp"
#include <QPainter>

CMapPolygon::CMapPolygon (TPaths const & paths, TMapShapeId id) : CMapShape (Polygon, id), m_paths (paths)
{
  for (TPath const & path : qAsConst (m_paths))
  {
    for (TGeoCoord const & vertex : path)
    {
      m_aabb.add (vertex);
    }
  }
}

int CMapPolygon::vertexCount () const
{
  int count = 0;
  for (TPath const & path : m_paths)
  {
    count += path.size ();
  }

  return count;
}

CAabb CMapPolygon::aabb (TCoordType, TCoordType) const
{
  return m_aabb;
}

void CMapPolygon::draw (QPainter* painter, CTileAdapter* tileAdapter, SViewportToWidget const & vt) const
{
  if (CStatus::contains (Visible))
  {
    updatePen (painter, m_borderColor, m_borderWidth);
    updateBrush (painter, m_color);

    QPolygonF polygon;
    polygon.reserve (vertexCount ());
    for (TPath const & path : m_paths)
    {
      polygon.clear ();
      for (TGeoCoord const & vertex : path)
      {
        QPoint pi = tileAdapter->coordinatesToWidget (vertex, vt);
        polygon.append (QPointF (pi));
      }

      painter->drawPolygon (polygon.constData (), polygon.size (), Qt::OddEvenFill);
    }
  }
}

bool CMapPolygon::isVisible (CAabb const & aabb) const
{
   return CStatus::contains (Visible) && aabb.intersects (m_aabb);
}

static bool rayCrossesSegment (TGeoCoord const & v, TGeoCoord const & a, TGeoCoord const & b)
{
  TCoordType px = v.y (), py = v.x ();
  TCoordType ax = a.y (), ay = a.x (), bx = b.y (), by = b.x ();
  if (ay > by)
  {
    ax = b.y ();
    ay = b.x ();
    bx = a.y ();
    by = a.x ();
  }

  // alter longitude to cater for 180 degree crossings
  if (px < 0)
  {
    px += 360;
  }
  if (ax < 0)
  {
    ax += 360;
  }

  if (bx < 0)
  {
    bx += 360;
  }

  if (py == ay || py == by)
  {
    py += static_cast<TCoordType>(0.00000001);
  }

  if ((py > by || py < ay) || (px > std::max (ax, bx)))
  {
    return false;
  }

  if (px < std::min (ax, bx))
  {
    return true;
  }

  TCoordType red  = (ax != bx) ? ((by - ay) / (bx - ax)) : std::numeric_limits<TCoordType>::max ();
  TCoordType blue = (ax != px) ? ((py - ay) / (px - ax)) : std::numeric_limits<TCoordType>::max ();
  return (blue >= red);
}

// https://gis.stackexchange.com/questions/42879/check-if-lat-long-point-is-within-a-set-of-polygons-using-google-maps/46720#46720
bool CMapPolygon::contains (TGeoCoord const & coords, TCoordType, TCoordType) const
{
  bool contains = false;
  if (!m_paths.isEmpty () && m_aabb.contains (coords))
  {
    for (TPath const & path : qAsConst (m_paths))
    {
      int crossings = 0;
      for (int i = 0, count = path.size (); i < count; ++i)
      {
        TGeoCoord const & a = path.at (i);
        int               j = i == count - 1 ? 0 : i + 1;
        TGeoCoord const & b = path.at (j);
        if (rayCrossesSegment (coords, a, b))
        {
          ++crossings;
        }
      }

      // odd number of crossings?
      contains = (crossings & 1) == 1;
      if (contains)
      {
        break;
      }
    }
  }

  return contains;
}

