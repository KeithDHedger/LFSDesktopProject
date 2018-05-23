/*
 *
 * ©K. D. Hedger. Wed 23 May 14:12:35 BST 2018 kdhedger68713@gmail.com

 * This file (globals.h) is part of LFSEdit.

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

#ifndef _GLOBALS_
#define _GLOBALS_

#include <termios.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdarg.h>

#include "highlight.h"
#include "editor.h"

//temp foworawrd ref
void editorSetStatusMessage(const char *fmt,...);


//editor
extern editorConfig	editorPage;

//term
extern termios orig_termios;

//prefs

//style
extern char			*lang;
extern unsigned		langFlags;
extern const char	*langKeywords[];
extern const char	*langComments[];


#endif
