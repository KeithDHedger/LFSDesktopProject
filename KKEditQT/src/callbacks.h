/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
 * callbacks.h
 *
*/

#ifndef _CALLBACKS_
#define _CALLBACKS_

#include "kkedit-includes.h"


void setSensitive(void);

void copyToClip(Widget* widget,uPtr data);
void cutToClip(Widget* widget,uPtr data);
void pasteFromClip(Widget* widget,uPtr data);

void undo(Widget* widget,uPtr data);
void redo(void);

#ifndef _USEQT5_
void dropUri(GtkWidget *widget,GdkDragContext *context,gint x,gint y,GtkSelectionData *selection_data,guint info,guint32 time,gpointer user_data);
#else
//TODO//
void dropUri(void);
#endif
#ifndef _USEQT5_
void externalTool(GtkWidget* widget,gpointer data);
#else
//TODO//
void externalTool(void);
#endif

void openHelp(Widget* widget,uPtr data);
void printFile(Widget* widget,uPtr data);

#ifndef _USEQT5_
void populatePopupMenu(GtkTextView *entry,GtkMenu *menu,gpointer user_data);
#else
//TODO//
void populatePopupMenu(void);
#endif
#ifndef _USEQT5_
bool tabPopUp(GtkWidget *my_widget, GdkEventButton *event,gpointer user_data);
#else
//TODO//
bool tabPopUp(void);
#endif

//void doShutdown(Widget* widget,uPtr data);

#ifndef _USEQT5_
void setToolOptions(GtkWidget* widget,gpointer data);
#else
//TODO//
void setToolOptions(void);
#endif

void doAbout(Widget* widget,uPtr data);




void closeAllTabs(Widget* widget,uPtr data);

#ifndef _USEQT5_
void recentFileMenu(GtkRecentChooser* chooser,gpointer* data);
#else
//TODO//
void recentFileMenu(void);
#endif
void refreshMainWindow(void);
#ifndef _USEQT5_
gboolean whatPane(GtkWidget *widget,GdkEvent *event,gpointer data);
#else
//TODO//
gboolean whatPane(void);
#endif



#ifndef _USEQT5_
void changeSourceStyle(GtkWidget* widget,gpointer data);
#else
//TODO//
void changeSourceStyle(void);
#endif
int yesNo(char* question,char* file);

void setToobarSensitivex(void);




#ifndef _USEQT5_
void removeAllBookmarks(GtkWidget* widget,GtkTextIter* titer);
#else
//TODO//
void removeAllBookmarks(void);
#endif


#ifndef _USEQT5_
void updateStatusBar(GtkTextBuffer* textbuffer,GtkTextIter* location,GtkTextMark* mark,gpointer data);
#else
//TODO//
void updateStatusBar(void);
#endif


#ifndef _USEQT5_
void closeDocViewer(GtkWidget *widget,GdkEvent *event,gpointer data);
#else
//TODO//
void closeDocViewer(void);
#endif

void getPlugins(Widget* widget,uPtr data);

void showToolOutput(bool immediate);
void hideToolOutput(bool immediate);





#endif
