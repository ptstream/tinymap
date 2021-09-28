#include "maptooltip.hpp"
#include <QApplication>
#include<QStyle>
#include<QStylePainter>
#include<QStyleOptionFrame>
#include<QKeyEvent>

CMapToolTip* CMapToolTip::m_instance = nullptr;

CMapToolTip::CMapToolTip () : QLabel (nullptr,
          Qt::ToolTip | Qt::BypassGraphicsProxyWidget | Qt::WindowTransparentForInput | Qt::WindowDoesNotAcceptFocus)
{
  setFrameStyle (QFrame::NoFrame);
  ensurePolished ();
  setMargin (2 + style ()->pixelMetric (QStyle::PM_ToolTipLabelFrameWidth, nullptr, this));
  setAlignment (Qt::AlignLeft);
  setIndent (1);
  qApp->installEventFilter (this);
}

void CMapToolTip::create ()
{
  if (m_instance == nullptr)
  {
    m_instance = new CMapToolTip;
  }
}

void CMapToolTip::destroy ()
{
  delete m_instance;
  m_instance = nullptr;
}

void CMapToolTip::hideText ()
{
  if (m_instance != nullptr)
  {
    m_instance->hide ();
  }
}

void CMapToolTip::showText (QPoint pos, QString const & text)
{
  if (m_instance == nullptr)
  {
    create ();
  }

  int const Offset = 10;
  int       margin = m_instance->frameWidth () + Offset;
  pos             += QPoint (margin, margin);
  m_instance->setText (text);
  m_instance->move (pos);
  m_instance->show ();
  m_instance->m_timer.start (m_instance->m_timerInterval, m_instance);
}

void CMapToolTip::setTimerInterval (int interval)
{
  if (m_instance == nullptr)
  {
    create ();
  }

  m_instance->m_timerInterval = interval;
}

void CMapToolTip::paintEvent (QPaintEvent* event)
{
  QStylePainter     p (this);
  QStyleOptionFrame opt;
  opt.init (this);
  p.drawPrimitive (QStyle::PE_PanelTipLabel, opt);
  p.end ();
  QLabel::paintEvent (event);
}

bool CMapToolTip::eventFilter (QObject*, QEvent* event)
{
  QEvent::Type type = event->type ();
  switch (type)
  {
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    {
      int key = static_cast<QKeyEvent*>(event)->key ();
      // Anything except key modifiers or caps-lock, etc.
      if (key < Qt::Key_Shift || key > Qt::Key_ScrollLock)
      {
        hideText ();
      }

      break;
    }

    case QEvent::WindowActivate:
    case QEvent::WindowDeactivate:
    case QEvent::FocusIn:
    case QEvent::FocusOut:
    case QEvent::Close:
      hideText();
      break;

    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::Wheel:
      hideText();
      break;

    default:
      break;
  }

  return false;
}

void CMapToolTip::resizeEvent (QResizeEvent* event)
{
  QStyleHintReturnMask frameMask;
  QStyleOption         option;
  option.init (this);
  if (style()->styleHint (QStyle::SH_ToolTip_Mask, &option, this, &frameMask))
  {
    setMask (frameMask.region);
  }

  QLabel::resizeEvent(event);
}

void CMapToolTip::timerEvent (QTimerEvent* event)
{
  int const delta = 5;
  if (event->timerId () == m_timer.timerId ())
  {
    QPoint loc = mapFromGlobal (QCursor::pos ());
    QRect  rc  = rect ();
    rc.adjust (-delta, -delta, delta, delta);
    if (!rc.contains (loc, true))
    {
      m_timer.stop ();
      hideText ();
    }
  }
}
