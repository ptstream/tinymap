#include "mapimage.hpp"
#include "tileadapter.hpp"
#include <QPainter>

CAabb CMapImage::aabb (TCoordType dx, TCoordType dy) const
{
  CAabb aabb;
  TCoordType dx1 =  m_anchorPoint.x () * dx;
  TCoordType dy1 = -m_anchorPoint.y () * dy;
  aabb.add (m_coordinates + TGeoCoord (dx1, dy1));
  aabb.add (aabb.tl () + TGeoCoord (m_pixmap.width () * dx, -m_pixmap.height () * dy));
  return aabb;
}

void CMapImage::draw (QPainter* painter, CTileAdapter* tileAdapter, SViewportToWidget const & vt) const
{
  if (CStatus::contains (Visible))
  {
    QPoint loc = tileAdapter->coordinatesToWidget (m_coordinates, vt);
    int    x   = loc.x () + m_anchorPoint.x ();
    int    y   = loc.y () + m_anchorPoint.y ();
    if (m_color == 0)
    {
      painter->drawPixmap (x, y, m_pixmap);
    }
    else
    {
      QPixmap p (m_pixmap.size ());
      QColor color = QColor::fromRgba (m_color);
      p.fill (color);
      painter->drawPixmap (x, y, p);
    }
  }
}

bool CMapImage::isVisible (CAabb const &) const
{
  return CStatus::contains (Visible);
}

bool CMapImage::contains (TGeoCoord const & coords, TCoordType dx, TCoordType dy) const
{
  CAabb aabb = this->aabb (dx, dy);
  bool  in   = aabb.contains (coords);
  return in;
}
