
/*
	Thanks to Johan for the original code available here:
	http://sourceforge.net/projects/windwm/?source=navbar

	Changes/additions
	©keithhedger Tue 23 Jun 09:56:25 BST 2015 keithdhedger@gmail.com

	Extra code released under GPL3

*/

#ifndef _ROOT_
#define _ROOT_

struct keybind
{
	KeySym keysym;
	unsigned modifiers;
	void (*function)(KeySym key,unsigned state,Time t,int arg);
	int arg;
	KeyCode keycode;
};

void initroot(void);

#endif
