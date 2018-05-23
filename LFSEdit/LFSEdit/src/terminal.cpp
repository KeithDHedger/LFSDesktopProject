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
