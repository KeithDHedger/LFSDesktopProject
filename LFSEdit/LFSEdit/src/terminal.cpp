/*
 *
 * ©K. D. Hedger. Wed 23 May 20:20:06 BST 2018 kdhedger68713@gmail.com

 * This file (terminal.cpp) is part of LFSEdit.

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

/* ============================= Terminal update ============================ */

void abAppend(struct appendBuffer *ab,const char *s,int len)
{
	char *newBuf=(char*)realloc(ab->b,ab->len+len);

	if(newBuf==NULL)
		return;
	memcpy(newBuf+ab->len,s,len);
	ab->b=newBuf;
	ab->len += len;
}

void abFree(struct appendBuffer *ab)
{
	free(ab->b);
}

/* Raw mode: 1960 magic shit. */
int enableRawMode(int fd)
{
	struct termios raw;

	if(editorPage.rawmode) return 0; /* Already enabled. */
	if(!isatty(STDIN_FILENO)) goto fatal;
	atexit(editorAtExit);
	if(tcgetattr(fd,&orig_termios)==-1) goto fatal;

	raw=orig_termios;  /* modify the original mode */
	/* input modes: no break,no CR to NL,no parity check,no strip char,
	 * no start/stop output control. */
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	/* output modes - disable post processing */
	raw.c_oflag &= ~(OPOST);
	/* control modes - set 8 bit chars */
	raw.c_cflag |= (CS8);
	/* local modes - choing off,canonical off,no extended functions,
	 * no signal chars (^Z,^C) */
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	/* control chars - set return condition: min number of bytes and timer. */
	raw.c_cc[VMIN]=0; /* Return each byte,or zero for timeout. */
	raw.c_cc[VTIME]=1; /* 100 ms timeout (unit is tens of second). */

	/* put terminal in raw mode after flushing */
	if(tcsetattr(fd,TCSAFLUSH,&raw) < 0) goto fatal;
	editorPage.rawmode=1;
	return 0;

fatal:
	errno=ENOTTY;
	return -1;
}

/* Use the ESC [6n escape sequence to query the horizontal cursor position
 * and return it. On error -1 is returned,on success the position of the
 * cursor is stored at *rows and *cols and 0 is returned. */
int getCursorPosition(int ifd,int ofd,int *rows,int *cols)
{
	char buf[32];
	unsigned int i=0;

	/* Report cursor location */
	if(write(ofd,"\x1b[6n",4) != 4) return -1;

	/* Read the response: ESC [ rows ; cols R */
	while (i < sizeof(buf)-1)
		{
			if(read(ifd,buf+i,1) != 1) break;
			if(buf[i]=='R') break;
			i++;
		}
	buf[i]='\0';

	/* Parse it. */
	if(buf[0] != ESC || buf[1] != '[') return -1;
	if(sscanf(buf+2,"%d;%d",rows,cols) != 2) return -1;
	return 0;
}

/* Try to get the number of columns in the current terminal. If the ioctl()
 * call fails the function will try to query the terminal itself.
 * Returns 0 on success,-1 on error. */
int getWindowSize(int ifd,int ofd,int *rows,int *cols)
{
	struct winsize ws;

	if(ioctl(1,TIOCGWINSZ,&ws)==-1 || ws.ws_col==0)
		{
			/* ioctl() failed. Try to query the terminal itself. */
			int orig_row,orig_col,retval;

			/* Get the initial position so we can restore it later. */
			retval=getCursorPosition(ifd,ofd,&orig_row,&orig_col);
			if(retval==-1) goto failed;

			/* Go to right/bottom margin and get position. */
			if(write(ofd,"\x1b[999C\x1b[999B",12) != 12) goto failed;
			retval=getCursorPosition(ifd,ofd,rows,cols);
			if(retval==-1) goto failed;

			/* Restore position. */
			char seq[32];
			snprintf(seq,32,"\x1b[%d;%dH",orig_row,orig_col);
			if(write(ofd,seq,strlen(seq))==-1)
				{
					/* Can't recover... */
				}
			return 0;
		}
	else
		{
			*cols=ws.ws_col;
			*rows=ws.ws_row;
			return 0;
		}

failed:
	return -1;
}
