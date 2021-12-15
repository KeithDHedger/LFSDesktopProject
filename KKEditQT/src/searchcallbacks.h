/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
 * functioncallbacks.h
 *
*/

#ifndef _FUNCCALLBACKS_
#define _FUNCCALLBACKS_

#ifdef _BUILDDOCVIEWER_
#ifdef _USEQT5_
void webKitGoBack(void);
#else
void webKitGoBack(Widget* widget,uPtr data);
#endif

#ifdef _USEQT5_
void webKitGoForward(void);
#else
void webKitGoForward(Widget* widget,uPtr data);
#endif

#ifdef _USEQT5_
void webKitGoHome(void);
#else
void webKitGoHome(Widget* widget,uPtr data);
#endif

void docSearchInPageFoward(Widget* widget,uPtr data);
void docSearchInPageBack(Widget* widget,uPtr data);
#endif

#ifdef _USEQT5_
void webKitGoBack(void);
#endif

#ifndef _USEQT5_
void doFindReplace(GtkDialog *dialog,gint response_id,gpointer user_data);
#else
void doFindReplace(int response_id);
#endif

void find(Widget* widget,uPtr data);
//void doSearchPrefs(int state);


#ifndef _USEQT5_
void doLiveSearch(GtkWidget* widget,GdkEvent *event,gpointer data);
#else
//TODO//
void doLiveSearch(void);
#endif


void showDocView(int howtodisplay,char* text,const char* title);
void pasteFRClip(Widget* widget,uPtr data);
void doDoxy(Widget* widget,uPtr data);
void doxyDocs(Widget* widget,uPtr data);

#endif