#ifndef MAPTOOLTIP_HPP
#define MAPTOOLTIP_HPP

#include <QLabel>
#include <QBasicTimer>

/*! \brief The CMapToolTip class provides tool tips (balloon help) for CMapWidget.
 *
 *  This class is very similar at QToolTip.
 *  This class is used only with WebAssembly project (Qt 5.15.2 has a bug and the toolTip is always black).
 *  It is not as efficient as QToolTip. I have coded that only awaiting bug correction may be in QT 6.
 *  The application creates an uniq instance of CMapToolTip.
 */
class CMapToolTip : public QLabel
{
public:
  /*! \brief Construct an uniq instance of CMapToolTip. */
  CMapToolTip ();

  /*! Removes all data of the global instance. */
  static void destroy ();

  /*! Hides the toolTip. */
  static void hideText ();

  /*! Shows the toolTip.
   *  \param pos: It is the global position on the screen.
   *  \param text: The toolTip content (See QLabel text).
   */
  static void showText (QPoint pos, QString const & text);

  /*! Sets the removal time.  */
  static void setTimerInterval (int interval);

  /*! See QToolTip functions. */
  static bool isVisible () { return !m_instance->isHidden (); }
  static QFont font () { return m_instance->QLabel::font (); }
  static QPalette palette () { return m_instance->QLabel::palette (); }
  static void setFont (QFont const & font) { m_instance->QLabel::setFont (font); }
  static void setPalette (QPalette const & palette) { m_instance->QLabel::setPalette (palette); }
  static QString text () { return m_instance->QLabel::text (); }

protected:
  void paintEvent (QPaintEvent* event) override;
  bool eventFilter (QObject*, QEvent* event) override;
  void resizeEvent (QResizeEvent* event) override;
  void timerEvent (QTimerEvent* event) override;

private:
  static void create ();

protected:
  QBasicTimer m_timer;
  int         m_timerInterval = 500;
  static CMapToolTip* m_instance;
};

#endif // MAPTOOLTIP_HPP
