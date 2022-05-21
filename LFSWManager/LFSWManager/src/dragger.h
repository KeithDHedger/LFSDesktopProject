
/*
	Thanks to Johan for the original code available here:
	http://sourceforge.net/projects/windwm/?source=navbar

	Changes/additions
	©keithhedger Tue 23 Jun 09:56:25 BST 2015 keithdhedger@gmail.com

	Extra code released under GPL3

*/

#ifndef _DRAGGER_
#define _DRAGGER_

struct dragger
{
	struct listener listener;
	void (*dragnotify)(void *,int,int,unsigned long,Time);
	void *arg;
	unsigned long counter;
	Window window;
	int x0;
	int y0;
	int x;
	int y;
	int width;
	int height;
	int wadjust;
	int hadjust;
};

struct dragger *dcreate(Window,int,int,int,int,int,Cursor,void (*)(void *,int,int,unsigned long,Time),void *);
void ddestroy(struct dragger *);

#endif
