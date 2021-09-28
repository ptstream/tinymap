#ifndef MAPTEXT_HPP
#define MAPTEXT_HPP

#include "mapanchoredlocation.hpp"

/*! \brief The text is defined in terms of a TGeoCoord which specifies the location of the text.
 *
 *  The parameters are (see also QFont):
 *  - The text content.
 *  - The font family.
 *  - The font point size.
 *  - The font weight.
 *  - The italic flag.
 *  - The flags defining the position in pixels from the geo-coordinate. The possible values is a combinaison of:
 *    * Qt::AlignLeft
 *    * Qt::AlignRight
 *    * Qt::AlignTop
 *    * Qt::AlignBottom
 *    * Qt::AlignHCenter
 *    * Qt::AlignVCenter
 *    * Qt::AlignCenter
 *
 *    The default value is Qt::AlignLeft | Qt::AlignBottom.
 *    CMapText accepts also anchor position.
 *
 *    \remark Text with flags other than Qt::AlignLeft | Qt::AlignBottom must be slow for a large number of texts.
 */
class CMapText : public CMapAnchoredLocation
{
public:
  /*! Contructor.
   *  \param id: The map shape identifier.
   */
  CMapText (TMapShapeId id = 0) : CMapAnchoredLocation (Text, id) {}

  /*! Contructor.
   *  \param coordinates: The location.
   *  \param text: The unicode text content.
   *  \param id: The map shape identifier.
   */
  CMapText (TGeoCoord const & coordinates, QString const & text, TMapShapeId id = 0) :
    CMapAnchoredLocation (coordinates, Text, id), m_text (text) {}

  /*! Contructor.
   *  \param coordinates: The location.
   *  \param anchorPoint: The pixel position from coordinates.
   *  \param text: The unicode text content.
   *  \param id: The map shape identifier.
   */
  CMapText (TGeoCoord const & coordinates, QPoint const & anchorPoint, QString const & text, TMapShapeId id = 0) :
    CMapAnchoredLocation (coordinates, anchorPoint, Text, id), m_text (text) {}

  /*! Returns the text content. */
  QString const & text () const { return m_text; }

  /*! Returns the font family. */
  QString const & family () const { return m_family; }

  /*! Returns the font point size. */
  int pointSize () const { return m_pointSize; }

  /*! Returns the font weight. */
  int weight () const { return m_weight; }

  /*! Returns the italic flag. */
  int italic () const { return m_italic; }

  /*! Returns the position flags. */
  int flags () const { return m_flags; }

  /*! Returns the background color. */
  QRgb backgroundColor () const { return m_backgroundColor; }

  /*! Returns the text content. */
  void setText (QString const & text) { m_text = text; }

  /*! Sets the font family. */
  void setFamily (QString const & family) { m_family = family; }

  /*! Sets the font point size. */
  void setPointSize (int pointSize) { m_pointSize = pointSize; }

  /*! Sets the font weight. */
  void setWeight (int weight) { m_weight = weight; }

  /*! Sets the italic flag. */
  void setItalic (int italic) { m_italic = italic; }

  /*! Sets the position flags. */
  void setFlags (int flags) { m_flags = flags; }

  /*! Returns the background color. */
  void setBackgroundColor (QRgb backgroundColor) { m_backgroundColor = backgroundColor; }

  /*! See the base class. */
  void draw (QPainter* painter, CTileAdapter* tileAdapter, SViewportToWidget const & vt) const override;
  bool isVisible (CAabb const &) const override;
  bool contains (TGeoCoord const & coords, TCoordType dx = 0, TCoordType dy = 0) const override;
  CAabb aabb (TCoordType dx, TCoordType dy) const override;

protected:
  mutable QSize m_size;
  QString       m_text;
  QString       m_family;
  int           m_flags           = 0;
  int           m_pointSize       = -1;
  int           m_weight          = -1;
  QRgb          m_backgroundColor = 0x00000000;
  bool          m_italic          = false;
};

#endif // MAPTEXT_HPP
