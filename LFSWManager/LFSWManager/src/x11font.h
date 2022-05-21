
/*
	Thanks to Johan for the original code available here:
	http://sourceforge.net/projects/windwm/?source=navbar

	Changes/additions
	©keithhedger Tue 23 Jun 09:56:25 BST 2015 keithdhedger@gmail.com

	Extra code released under GPL3

*/

#ifndef _X11FONT_
#define _X11FONT_

//#define DEFAULTFONT "sans-serif:size=10"

struct fontcolor *ftloadcolor(const char *,const char *fallback);
void ftfreecolor(struct fontcolor *);
struct font *ftload(const char *);
void ftfree(struct font *);
void ftdrawstring(Drawable,struct font *,struct fontcolor *,int,int,const char *);
void ftdrawstring_utf8(Drawable,struct font *,struct fontcolor *,int,int,const char *);
int fttextwidth(struct font *,const char *);
int fttextwidth_utf8(struct font *,const char *);

#endif