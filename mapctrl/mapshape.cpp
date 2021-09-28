#include "mapshape.hpp"
#include <QPainter>

int CMapShape::m_pickingSize = 3; // 3 pixels on either side of the search point

void CMapShape::updatePen (QPainter* painter, quint32 color, int width) const
{
  bool changed = false;
  QPen pen     = painter->pen ();
  if (width > 0)
  {
    if (pen == Qt::NoPen || pen.color ().toRgb () != color)
    {
      pen.setColor (QColor::fromRgba (color));
      changed = true;
    }

    if (pen.width () != width)
    {
      pen.setWidth (width);
      changed = true;
    }

    if (changed)
    {
      painter->setPen (pen);
    }
  }
  else if (pen != Qt::NoPen)
  {
    painter->setPen (Qt::NoPen);
  }
}

void CMapShape::updateBrush (QPainter* painter, quint32 color) const
{
  QBrush brush = painter->brush ();
  if (color != 0)
  {
    if (brush.color ().toRgb () != m_color)
    {
      brush.setColor (QColor::fromRgba (m_color));
      painter->setBrush (brush);
    }
  }
  else if (brush != Qt::NoBrush)
  {
     painter->setBrush (Qt::NoBrush);
  }
}
