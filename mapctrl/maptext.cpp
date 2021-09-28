#include "maptext.hpp"
#include "tileadapter.hpp"
#include <QPainter>

CAabb CMapText::aabb (TCoordType dx, TCoordType dy) const
{
  dx                *= m_size.width ();
  dy                *= m_size.height ();
  TCoordType offsetX = 0;
  TCoordType offsetY = 0;
  if ((m_flags & Qt::AlignRight) != 0)
  {
    offsetX = -dx;
  }
  else if ((m_flags & Qt::AlignHCenter) != 0)
  {
    offsetX = -dx / 2;
  }

  if ((m_flags & Qt::AlignBottom) != 0)
  {
    offsetY = -dy;
  }
  else if ((m_flags & Qt::AlignVCenter) != 0)
  {
    offsetY = -dy / 2;
  }

  CAabb      aabb;
  TCoordType dx1 = offsetX + m_anchorPoint.x () * dx;
  TCoordType dy1 = offsetY - m_anchorPoint.y () * dy;
  aabb.add (m_coordinates + TGeoCoord (dx1, dy1));
  aabb.add (aabb.tl () + TGeoCoord (dx, dy));
  return aabb;
}

void CMapText::draw (QPainter* painter, CTileAdapter* tileAdapter, SViewportToWidget const & vt) const
{
  if (CStatus::contains (Visible))
  {
    updatePen (painter, m_color, 1);
    QFont font       = painter->font ();
    bool  updateFont = false;
    if (!m_family.isEmpty () && font.family () != m_family)
    {
      font.setFamily (family ());
      updateFont = true;
    }

    if (m_pointSize != -1 && font.pixelSize () != m_pointSize)
    {
      font.setPointSize (m_pointSize);
      updateFont = true;
    }

    if (m_weight != -1 && font.weight () != m_weight)
    {
      font.setWeight (m_weight);
      updateFont = true;
    }

    if (font.italic () != m_italic)
    {
      font.setItalic (m_italic);
      updateFont = true;
    }

    if (updateFont)
    {
      painter->setFont (font);
    }

    QPoint       loc = tileAdapter->coordinatesToWidget (m_coordinates, vt);
    int          x   = loc.x ();
    int          y   = loc.y ();
    QFontMetrics fm (painter->font ());
    m_size = fm.boundingRect (m_text).size ();

    painter->setBackgroundMode ((m_backgroundColor & 0xFF000000) != 0 ? Qt::OpaqueMode : Qt::TransparentMode);
    painter->setBackground (QColor (m_backgroundColor));
    if (m_flags == 0)
    {
      painter->drawText (x, y, m_text);
    }
    else
    {
      if ((m_flags & Qt::AlignRight) != 0)
      {
        x -= m_size.width ();
      }
      else if ((m_flags & Qt::AlignHCenter) != 0)
      {
        x -= m_size.width () / 2;
      }

      if ((m_flags & Qt::AlignTop) != 0)
      {
        y -= m_size.height ();
      }
      else if ((m_flags & Qt::AlignVCenter) != 0)
      {
        y -= m_size.height () / 2;
      }

      x += m_anchorPoint.x ();
      y += m_anchorPoint.y ();
      QRect rc (x - 1, y - 1, m_size.width () + 2, m_size.height () + 2);
      painter->drawText (rc, m_flags, m_text);
    }
  }
}

bool CMapText::isVisible (CAabb const &) const
{
  return CStatus::contains (Visible);
}

bool CMapText::contains (TGeoCoord const & coords, TCoordType dx, TCoordType dy) const
{
  CAabb aabb    = this->aabb (dx, dy);
  bool contains = aabb.contains (coords);
  return contains;
}
