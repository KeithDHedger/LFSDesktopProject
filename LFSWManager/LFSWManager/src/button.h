
/*
	Thanks to Johan for the original code available here:
	http://sourceforge.net/projects/windwm/?source=navbar

	Changes/additions
	©keithhedger Tue 23 Jun 09:56:25 BST 2015 keithdhedger@gmail.com

	Extra code released under GPL3

*/


#ifndef _BUTTON_
#define _BUTTON_

#include "wind.h"

struct button
{
			listener		listen;
			void			(*function)(void *,Time);
			void			*arg;
			struct bitmap	*bitmap;
			Pixmap			pixmap;
			int				width;
			int				height;
			Window			window;
			Bool			pressed;
			Bool			entered;
			int				buttonNumber;
			struct frame	*f;
			int				x;
};

extern bitmap	*deletebitmap;
extern bitmap	*maximizeBitmap;
extern bitmap	*minimizeBitmap;
extern bitmap	*shadeBitmap;

void update(struct button *b);
void buttonpress(struct button *,XButtonEvent *);
void buttonrelease(struct button *,XButtonEvent *);
void enternotify(struct button *,XCrossingEvent *);
void leavenotify(struct button *,XCrossingEvent *);
void expose(struct button *,XExposeEvent *);
void unmapnotify(struct button *,XUnmapEvent *);
void buttonevent(void *,XEvent *);

struct button *bcreate(void (*function)(void *,Time),void *arg,struct bitmap *bitmap,Window parent,int x,int y,int width,int height,int gravity,int buttonnum,struct frame *f);
void bdestroy(struct button *);

#endif
