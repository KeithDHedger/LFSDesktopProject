/*
 *
 * ©K. D. Hedger. Wed 23 May 20:19:58 BST 2018 kdhedger68713@gmail.com

 * This file (terminal.h) is part of LFSEdit.

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


#ifndef _TERMINAL_
#define _TERMINAL_

#define ABUF_INIT {NULL,0}

/* We define a very simple "append buffer" structure,that is an heap
 * allocated string where we can append to. This is useful in order to
 * write all the escape sequences in a buffer and flush them to the standard
 * output in a single call,to avoid flickering effects.
*/

struct appendBuffer
{
	char *b;
	int len;
};

void abAppend(struct appendBuffer *ab,const char *s,int len);
void abFree(struct appendBuffer *ab);

#endif
