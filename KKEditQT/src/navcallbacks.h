/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
 * navcallbacks.h
 *
*/

#ifndef _NAVCALLBACKS_
#define _NAVCALLBACKS_

//void goToDefine(functionData* fdata);

void goToDefinition(Widget* widget,uPtr data);
//void findFile(Widget* widget,uPtr data);
void jumpToLine(Widget* widget,uPtr data);
void gotoLine(Widget* widget,uPtr data);

#ifndef _USEQT5_
void jumpToLineFromBar(GtkWidget* widget,gpointer data);
#else
//TODO//
void jumpToLineFromBar(const QString text);
#endif

void functionSearch(Widget* widget,uPtr data);

#ifndef _USEQT5_
void gotoLine(GtkWidget* widget,gpointer data);
#else
//TODO//
void gotoLine(void);
#endif
#ifndef _USEQT5_
void jumpToMark(GtkWidget* widget,gpointer glist);
#else
//TODO//
void jumpToMark(void);
#endif
void rebuildBookMarkMenu(void);
#ifdef _BUILDDOCVIEWER_
#ifndef _USEQT5_
gboolean docLinkTrap(WebKitWebView* web_view,WebKitWebFrame* frame,WebKitNetworkRequest* request,WebKitWebNavigationAction* navigation_action,WebKitWebPolicyDecision* policy_decision, gpointer user_data);
#endif
#endif

#endif