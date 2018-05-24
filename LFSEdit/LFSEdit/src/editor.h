/*
 *
 * ©K. D. Hedger. Wed 23 May 14:15:50 BST 2018 kdhedger68713@gmail.com

 * This file (editor.h) is part of LFSEdit.

 * LFSEdit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSEdit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSEdit.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef _EDITOR_
#define _EDITOR_

enum KEY_ACTION
{
	KEY_NULL=0,     /* NULL */
	CTRL_C=3,       /* Ctrl-c */
	CTRL_D=4,       /* Ctrl-d */
	CTRL_F=6,       /* Ctrl-f */
	CTRL_H=8,       /* Ctrl-h */
	TAB=9,          /* Tab */
	CTRL_L=12,      /* Ctrl+l */
	ENTER=13,       /* Enter */
	CTRL_Q=17,      /* Ctrl-q */
	CTRL_S=19,      /* Ctrl-s */
	CTRL_U=21,      /* Ctrl-u */
	ESC=27,         /* Escape */
	BACKSPACE= 127, /* Backspace */
	/* The following are just soft codes,not really reported by the
	 * terminal directly. */
	ARROW_LEFT=1000,
	ARROW_RIGHT,
	ARROW_UP,
	ARROW_DOWN,
	DEL_KEY,
	HOME_KEY,
	END_KEY,
	PAGE_UP,
	PAGE_DOWN
};

struct editorRow
{
	int idx;            /* Row index in the file,zero-based. */
	int size;           /* Size of the row,excluding the null term. */
	int rsize;          /* Size of the rendered row. */
	char *chars;        /* Row content. */
	char *render;       /* Row content "rendered" for screen (for TABs). */
	unsigned char *hl;  /* Syntax highlight type for each character in render.*/
	int hl_oc;          /* Row had open comment at end in last syntax highlight
                           check. */
};

struct editorConfig
{
	int cx,cy;  /* Cursor x and y position in characters */
	int rowoff;     /* Offset of row displayed. */
	int coloff;     /* Offset of column displayed. */
	int screenrows; /* Number of rows that we can show */
	int screencols; /* Number of cols that we can show */
	int numrows;    /* Number of rows */
	int rawmode;    /* Is terminal raw mode enabled? */
	editorRow *row;      /* Rows */
	int dirty;      /* File modified but not saved. */
	char *filename; /* Currently open filename */
	char statusmsg[80];
	time_t statusmsg_time;
	editorSyntax *syntax;    /* Current syntax highlight,or NULL. */
};

void editorAtExit(void);
void editorRefreshScreen(void);
int editorReadKey(int fd);

#endif
