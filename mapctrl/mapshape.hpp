#ifndef MAPSHAPE_HPP
#define MAPSHAPE_HPP

#include "../tools/status.hpp"
#include "../tools/aabb.hpp"
#include <QRect>
#include <QRgb>
#include <QVector>

class QPen;
class QPainter;
class CTileAdapter;
class CAabb;
class QBrush;

#if Q_PROCESSOR_WORDSIZE == 8
  using TMapShapeId = quint64; //!< On most systems
#else
  using TMapShapeId = quint32;
#endif

using TPath  = QVector<TGeoCoord>; //!< Set of geo-coordinates defining a polyline or a polygon.
using TPaths = QVector<TPath>;     //!< Set of path defining a polygon.

/*! \brief Base class of all map shapes.
 *
 *  The map shape is defined by
 *  - The type (CMapCircle, CMapText, ...
 *  - The color with the argb format. When the graphics system allows it,
 *    the alpha channel is used to apply the transparency.
 *  - The identifier used by picking (see TMapShapeId comment).
 *  - The z used to sort the order of the display.
 */
class CMapShape : public CStatus<quint32>
{
public:
  //!< Uses to prepare viewport rectangle to tiles rect (widget coordinates to draw CMapShape).
  struct SViewportToWidget
  {
    SViewportToWidget () = default;
    void init (QRect const & viewport, QRect const & tileRect, int zoom)
    {
      m_zoom = zoom;
      m_vx0  = viewport.x ();
      m_vy0  = viewport.y ();
      m_tx0  = tileRect.x ();
      m_ty0  = tileRect.y ();
      m_rx   = static_cast<TCoordType>(tileRect.width ())  / viewport.width ();
      m_ry   = static_cast<TCoordType>(tileRect.height ()) / viewport.height ();
    }

    int        m_zoom;       // Current zoom factor.
    int        m_vx0, m_vy0; // Viewport origin.
    int        m_tx0, m_ty0; // tile origin of viewport in pixel.
    TCoordType m_rx,  m_ry;  // Length ratios.
  };

  /*! Status of map shapde. */
  enum EStatus : quint32 { Visible = 0x00000001, // Visible or not/
                         };

  /*! Type of map shape. */
  enum EType : quint32 { NoType,
                         Cross,
                         Polyline,
                         Polygon,
                         Image,
                         Circle,
                         Text,
                       };

  /*! Constructor. */
  CMapShape (EType type, TMapShapeId id = 0) : CStatus (Visible), m_type (type), m_id (id) {}

  /*! Destructor. */
  virtual ~CMapShape () = default;

  /*! Returns true if the map shape is visible. */
  inline bool isVisible () const;

  /*! Sets the map shape visibimity. */
  inline void setVisible (bool visible);

  /*! Returns the map shape identifier. */
  inline TMapShapeId id () const;

  /*! Sets the map shape identifier. */
  inline void setId (TMapShapeId id);

  /*! Returns the map shape type. */
  inline EType type () const;

  /*! Sets the map shape type. */
  inline void setType (EType type);

  /*! Returns z. */
  inline TCoordType z () const { return m_z; }

  /*! Sets z. */
  inline void setZ (TCoordType z) { m_z = z; }

  /*! Returns the map shape color. */
  inline QRgb color () const;

  /*! Sets the map shape color. */
  inline void setColor (QRgb color);

  /*! Updates the current pen of the QPainter.
   *  \param painter: The current QPainter.
   *  \param color: The pen color. The color use argb format.
   *  \param width: The pen width in pixel. -1 means the pen has set to Qt::NoPen.
  */
  void updatePen (QPainter* painter, quint32 color, int width = -1) const;

  /*! Updates the current brush of the QPainter.
   *  \param painter: The current QPainter. The color use argb format.
   *  \param color: The brush color. The color use argb format. If color=0 (0x00000000) Qt::NoBrush is used.
   */
  void updateBrush (QPainter* painter, quint32 color) const;

  /*! Returns true if the map shape is visible.
   *  The map shape is visible if the flag Visible is set and if all coordinates aare in the bounding box.
   *
   *  \param aabb: The bounding box. Generally the bounding box corresponding at the map widget.
   */
  virtual bool isVisible (CAabb const & aabb) const = 0;

  /*! Draws the map shape.
   *  \param QPainter: the current QPainter.
   *  \param SViewportToWidget: the current SViewportToWidget.
   */
  virtual void draw (QPainter*, CTileAdapter*, SViewportToWidget const &) const = 0;

  /*! Returns true if the map shape is near of a point.
   *  \param TGeoCoord: the test point.
   *  \param TCoordType: The latitude angle corresponding at 1 pixel.
   *  \param TCoordType: The longitude angle corresponding at 1 pixel.
   */
  virtual bool contains (TGeoCoord const &, TCoordType, TCoordType) const = 0;

  /*! Returns the bounding box of the map shape. */
  virtual CAabb aabb (TCoordType = 0, TCoordType = 0) const = 0;

  /*! Returns the picking square in pixel. By default 3 pixels. */
  static int pickingSize () { return m_pickingSize; }

  /*! Sets the picking square in pixel. */
  static void setPickingSize (int size) { m_pickingSize = size; }

protected:
  EType       m_type  = NoType;      //!< Map shape type.
  QRgb        m_color = 0xFF000000;  //!< Map shape color (black).
  TMapShapeId m_id;                  //!< Map shape identifer. 0 by default.
  TCoordType  m_z = 0;               //!< Defines the order of draw.

  static int  m_pickingSize;         //!< Square picking size. Default 3 pixels.
};

bool CMapShape::isVisible () const
{
  return CStatus::contains (Visible);
}

void CMapShape::setVisible (bool visible)
{
  visible ? add (Visible) : remove (visible);
}

TMapShapeId CMapShape::id () const
{
  return m_id;
}

void CMapShape::setId (TMapShapeId id)
{
  m_id = id;
}

CMapShape::EType CMapShape::type () const
{
  return m_type;
}

void CMapShape::setType (EType type)
{
  m_type = type;
}

QRgb CMapShape::color () const
{
  return m_color;
}

void CMapShape::setColor (QRgb color)
{
  m_color = color;
}

#endif // MAPSHAPE_HPP
