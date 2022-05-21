
/*
	Thanks to Johan for the original code available here:
	http://sourceforge.net/projects/windwm/?source=navbar

	Changes/additions
	©keithhedger Tue 23 Jun 09:56:25 BST 2015 keithdhedger@gmail.com

	Extra code released under GPL3

*/

#ifndef _EWMH_
#define _EWMH_

#define NET_WM_STATE_REMOVE 0
#define NET_WM_STATE_ADD 1
#define NET_WM_STATE_TOGGLE 2

#define DEFAULT_NUMBER_OF_DESKTOPS 12

void ewmh_notifyndesk(unsigned long);
void ewmh_notifycurdesk(unsigned long);
void ewmh_notifyclientdesktop(Window,unsigned long);
void ewmh_notifyframeextents(Window,struct extents);
void ewmh_startwm(void);
void ewmh_stopwm(void);
void ewmh_maprequest(struct client *);
void ewmh_notifyfull(Window,Bool);
void ewmh_manage(struct client *);
void ewmh_unmanage(struct client *);
void ewmh_withdraw(struct client *);
void ewmh_notifyfocus(Window,Window);
void ewmh_notifyrestack(void);
void ewmh_propertynotify(struct client *,XPropertyEvent *);
void ewmh_clientmessage(struct client *,XClientMessageEvent *);
void ewmh_rootclientmessage(XClientMessageEvent *);
void reloadwindowdesktop(struct client *c);
void changestate(Window w,int how,Atom state);
void reloadwindowstate(struct client *c);
void reloadwindowtype(struct client *c);

#endif
