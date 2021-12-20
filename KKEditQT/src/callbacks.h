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





#ifndef _USEQT5_
void setToolOptions(GtkWidget* widget,gpointer data);
#else
//TODO//
void setToolOptions(void);
#endif

void doAbout(Widget* widget,uPtr data);




void getPlugins(Widget* widget,uPtr data);

void showToolOutput(bool immediate);
void hideToolOutput(bool immediate);





#endif
