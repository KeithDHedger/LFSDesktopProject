/*
 *
 * ©K. D. Hedger. Wed 23 May 14:16:10 BST 2018 kdhedger68713@gmail.com

 * This file (editor.cpp) is part of LFSEdit.

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

#include "globals.h"

/* ======================= Low level terminal handling ====================== */

void disableRawMode(int fd)
{
	/* Don't even check the return value as it's too late. */
	if(editorPage.rawmode)
		{
			tcsetattr(fd,TCSAFLUSH,&orig_termios);
			editorPage.rawmode=0;
		}
}

/* Called at exit to avoid remaining in raw mode. */
void editorAtExit(void)
{
	disableRawMode(STDIN_FILENO);
}

/* Set an editor status message for the second line of the status,at the
 * end of the screen. */
void editorSetStatusMessage(const char *fmt,...)
{
	va_list ap;
	va_start(ap,fmt);
	vsnprintf(editorPage.statusmsg,sizeof(editorPage.statusmsg),fmt,ap);
	va_end(ap);
	editorPage.statusmsg_time=time(NULL);
}

/* This function writes the whole screen using VT100 escape characters
 * starting from the logical state of the editor in the global state 'E'. */
void editorRefreshScreen(void)
{
	int y;
	editorRow *r;
	char buf[32];
	appendBuffer ab=ABUF_INIT;

	abAppend(&ab,"\x1b[?25l",6); /* Hide cursor. */
	abAppend(&ab,"\x1b[H",3); /* Go home. */
	for (y=0; y < editorPage.screenrows; y++)
		{
			int fileditorRow=editorPage.rowoff+y;

			if(fileditorRow >= editorPage.numrows)
				{
					if(editorPage.numrows==0 && y==editorPage.screenrows/3)
						{
							char welcome[80];
							int welcomelen=snprintf(welcome,sizeof(welcome),
							                        "LFSEdit editor -- verison %s\x1b[0K\r\n",VERSION);
							int padding=(editorPage.screencols-welcomelen)/2;
							if(padding)
								{
									abAppend(&ab,"~",1);
									padding--;
								}
							while(padding--) abAppend(&ab," ",1);
							abAppend(&ab,welcome,welcomelen);
						}
					else
						{
							abAppend(&ab,"~\x1b[0K\r\n",7);
						}
					continue;
				}

			r=&editorPage.row[fileditorRow];

			int len=r->rsize - editorPage.coloff;
			int current_color=-1;
			if(len > 0)
				{
					if(len > editorPage.screencols) len=editorPage.screencols;
					char *c=r->render+editorPage.coloff;
					unsigned char *hl=r->hl+editorPage.coloff;
					int j;
					for (j=0; j < len; j++)
						{
							if(hl[j]==HL_NONPRINT)
								{
									char sym;
									abAppend(&ab,"\x1b[7m",4);
									if(c[j] <= 26)
										sym='@'+c[j];
									else
										sym='?';
									abAppend(&ab,&sym,1);
									abAppend(&ab,"\x1b[0m",4);
								}
							else if(hl[j]==HL_NORMAL)
								{
									if(current_color != -1)
										{
											abAppend(&ab,"\x1b[39m",5);
											current_color=-1;
										}
									abAppend(&ab,c+j,1);
								}
							else
								{
									int color=editorSyntaxToColor(hl[j]);
									if(color != current_color)
										{
											char buf[16];
											int clen=snprintf(buf,sizeof(buf),"\x1b[%dm",color);
											current_color=color;
											abAppend(&ab,buf,clen);
										}
									abAppend(&ab,c+j,1);
								}
						}
				}
			abAppend(&ab,"\x1b[39m",5);
			abAppend(&ab,"\x1b[0K",4);
			abAppend(&ab,"\r\n",2);
		}

	/* Create a two rows status. First row: */
	abAppend(&ab,"\x1b[0K",4);
	abAppend(&ab,"\x1b[7m",4);
	char status[80],rstatus[80];
	int len=snprintf(status,sizeof(status),"%.20s - %d lines %s",
	                 editorPage.filename,editorPage.numrows,editorPage.dirty ? "(modified)" : "");
	int rlen=snprintf(rstatus,sizeof(rstatus),
	                  "%d/%d",editorPage.rowoff+editorPage.cy+1,editorPage.numrows);
	if(len > editorPage.screencols) len=editorPage.screencols;
	abAppend(&ab,status,len);
	while(len < editorPage.screencols)
		{
			if(editorPage.screencols - len==rlen)
				{
					abAppend(&ab,rstatus,rlen);
					break;
				}
			else
				{
					abAppend(&ab," ",1);
					len++;
				}
		}
	abAppend(&ab,"\x1b[0m\r\n",6);

	/* Second row depends on editorPage.statusmsg and the status message update time. */
	abAppend(&ab,"\x1b[0K",4);
	int msglen=strlen(editorPage.statusmsg);
	if(msglen && time(NULL)-editorPage.statusmsg_time < 5)
		abAppend(&ab,editorPage.statusmsg,msglen <= editorPage.screencols ? msglen : editorPage.screencols);

	/* Put cursor at its current position. Note that the horizontal position
	 * at which the cursor is displayed may be different compared to 'editorPage.cx'
	 * because of TABs. */
	int j;
	int cx=1;
	int fileditorRow=editorPage.rowoff+editorPage.cy;
	editorRow *row=(fileditorRow >= editorPage.numrows) ? NULL : &editorPage.row[fileditorRow];
	if(row)
		{
			for (j=editorPage.coloff; j < (editorPage.cx+editorPage.coloff); j++)
				{
					if(j < row->size && row->chars[j]==TAB) cx += 7-((cx)%8);
					cx++;
				}
		}
	snprintf(buf,sizeof(buf),"\x1b[%d;%dH",editorPage.cy+1,cx);
	abAppend(&ab,buf,strlen(buf));
	abAppend(&ab,"\x1b[?25h",6); /* Show cursor. */
	write(STDOUT_FILENO,ab.b,ab.len);
	abFree(&ab);
}
