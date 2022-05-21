
/*
	Thanks to Johan for the original code available here:
	http://sourceforge.net/projects/windwm/?source=navbar

	Changes/additions
	©keithhedger Tue 23 Jun 09:56:25 BST 2015 keithdhedger@gmail.com

	Extra code released under GPL3

*/

#ifndef _LIB_
#define _LIB_

void *xmalloc(size_t);
void *xrealloc(const void *,size_t);
char *xstrdup(const char *);
void grabkey(int,unsigned,Window,Bool,int,int);
void ungrabkey(int,unsigned,Window);
void grabbutton(unsigned,unsigned,Window,Bool,unsigned,int,int,Window,Cursor);
void ungrabbutton(unsigned,unsigned,Window);
long getwmstate(Window);
void setwmstate(Window,long);
Bool ismapped(Window);
char *decodetextproperty(XTextProperty *);
void setprop(Window,Atom,Atom,int,void *,int);
void *getprop(Window,Atom,Atom,int,unsigned long *);
void drawbitmap(Drawable,GC,struct bitmap *,int,int);
unsigned long getpixel(const char *);
int fileExists(char *name);

#endif
