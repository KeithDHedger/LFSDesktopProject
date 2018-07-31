/*
 *
 * ©K. D. Hedger. Tue 31 Jul 13:31:30 BST 2018 keithdhedger@gmail.com

 * This file (debug.h) is part of Projects.

 * Projects is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * Projects is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Projects.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <search.h>
#include <magic.h>
#include <stdarg.h>
#include <alloca.h>

#define MAXBUFFER 512
#define DEBUGFUNC(x,...) errLine=__LINE__,errFile=__FILE__,errFunc=__func__;debugFunc((const char*)x,__VA_ARGS__);
#define debugfunc  DEBUGFUNC
#define MESSAGE(s) DEBUGFUNC("%s",s);
#define message MESSAGE
#define CHECKPOINT errLine=__LINE__,errFile=__FILE__,errFunc=__func__;
#define checkpoint CHECKPOINT
#define LASTCHECKPOINT(s) debugFunc("%s",s);
#define lastcheckpoint LASTCHECKPOINT

extern int			errLine;
extern const char	*errFile;
extern const char	*errFunc;
static void debugFunc(const char *fmt, ...);

void debugFunc(const char *fmt, ...)
{
	va_list	ap;
	char	*buffer,*subbuffer;

	buffer=(char*)alloca(MAXBUFFER);
	subbuffer=(char*)alloca(MAXBUFFER);

	buffer[0]=0;
	subbuffer[0]=0;
	va_start(ap, fmt);
	while (*fmt)
		{
			subbuffer[0]=0;
			if(fmt[0]=='%')
				{
					fmt++;
					switch(*fmt)
						{
							case 's':
								sprintf(subbuffer,"%s",va_arg(ap,char*));
								break;
							case 'i':
								sprintf(subbuffer,"%i",va_arg(ap,int));
								break;
							default:
								sprintf(subbuffer,"%c",fmt[0]);
								break;
						}
				}
			else
				sprintf(subbuffer,"%c",fmt[0]);
			strcat(buffer,subbuffer);
			fmt++;
		}
	va_end(ap);
	fprintf(stderr,"File: %s,Func: %s,Line: %i\n",errFile,errFunc,errLine);
	fprintf(stderr,">>%s<<\n",buffer);
}


#endif
