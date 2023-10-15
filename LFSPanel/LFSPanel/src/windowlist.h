/*
 *
 * ©K. D. Hedger. Mon 12 Oct 16:45:32 BST 2015 keithdhedger@gmail.com

 * This file (windowList.h) is part of LFSPanel.

 * LFSPanel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSPanel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSPanel.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef _WINDOWLIST_
#define _WINDOWLIST_

#define MAXWINDOWSINLIST 512
#include <ctype.h>

typedef struct
{
	int x, y;       /* location of window */
	int width, height;      /* width and height of window */
	int border_width;       /* border width of window */
	int depth;      /* depth of window */
	Visual *visual; /* the associated visual structure */
	Window root;    /* root of screen containing window */
	int classe;      /* InputOutput, InputOnly*///<<<<<<BUGGER THIS!!!
	int bit_gravity;        /* one of the bit gravity values */
	int win_gravity;        /* one of the window gravity values */
	int backing_store;      /* NotUseful, WhenMapped, Always */
	unsigned long backing_planes;   /* planes to be preserved if possible */
	unsigned long backing_pixel;    /* value to be used when restoring planes */
	Bool save_under;        /* boolean, should bits under be saved? */
	Colormap colormap;      /* color map to be associated with window */
	Bool map_installed;     /* boolean, is color map currently installed*/
	int map_state;  /* IsUnmapped, IsUnviewable, IsViewable */
	long all_event_masks;   /* set of events all people have interest in*/
	long your_event_mask;   /* my event mask */
	long do_not_propagate_mask;     /* set of events that should not propagate */
	Bool override_redirect; /* boolean value for override-redirect */
	Screen *screen; /* back pointer to correct screen */
} MyXWindowAttributes;


extern LFSTK_buttonClass		*windowAll;
extern LFSTK_buttonClass		*windowDesk;
extern const char			*possibleError;
extern LFSTK_menuClass		*windowMenu;
extern menuStruct			**windowList;

int addWindowDeskMenu(int x,int y,int grav,bool fromleft);
int addWindowMenu(int x,int y,int grav,bool fromleft);

#endif
