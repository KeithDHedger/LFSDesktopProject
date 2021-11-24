/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
*/

#ifndef _GUIS_
#define _GUIS_

enum {STOCKMENU=0,IMAGEMENU,PIXMAPMENU,NORMALMENU};


void buildTools(void);

void buildWordCheck(int documentCheck);
int showFunctionEntry(void);



#ifdef _USEQT5_
void cancelPrefs(void);
#endif

typedef void (*menuCallbackVoid)(Widget* widget,long data);
typedef bool (*menuCallbackBool)(Widget* widget,long data);

#ifndef _USEQT5_
GtkWidget*	makeMenuItem(const char* stocklabel,GtkWidget* parent,void* function,char hotkey,const char* name,int setimage,const char* menulabel,void* userdata);
#else
QAction* makeMenuItem(Widget* menu,const char* name,const QKeySequence key,const char* iconname,const char* widgetname,menuCallbackVoid ptrvoid,menuCallbackBool ptrbool,int data);
#endif

void doFindForwardWrap(void);
void doFindBackWrap(void);
void doReplaceWrap(void);



#endif