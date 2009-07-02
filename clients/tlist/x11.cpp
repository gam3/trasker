#if defined(X11)
#include <QX11Info>
#if defined (Q_WS_X11)
#include <X11/Xlib.h>
#include <X11/Xatom.h>

/*! \namespace x11
 *  \brief This namespace is used for all X11 c code.
 *
 *  There is some QT X11 behavior that is not best. Code is this namespace
 *  adresses those problems.
 */
namespace x11
{

/** wmMessage sends a message to the X server
 *
 */

void wmMessage(Window win, long type, long l0, long l1, long l2, long l3, long l4)
{
    XClientMessageEvent xev;

    xev.type = ClientMessage;
    xev.window = win;
    xev.message_type = type;
    xev.format = 32;
    xev.data.l[0] = l0;
    xev.data.l[1] = l1;
    xev.data.l[2] = l2;
    xev.data.l[3] = l3;
    xev.data.l[4] = l4;

    XSendEvent(QX11Info::display(), QX11Info::appRootWindow(), False,
          (SubstructureNotifyMask | SubstructureRedirectMask),
          (XEvent *)&xev);
}

}
#endif
