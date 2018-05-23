/*
 *
 * ©K. D. Hedger. Wed 23 May 14:12:51 BST 2018 kdhedger68713@gmail.com

 * This file (highlight.h) is part of LFSEdit.

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

#ifndef _HIGHLIGHT_
#define _HIGHLIGHT_

/* Syntax highlight types */
#define HL_NORMAL 0
#define HL_NONPRINT 1
#define HL_COMMENT 2   /* Single line comment. */
#define HL_MLCOMMENT 3 /* Multi-line comment. */
#define HL_KEYWORD1 4
#define HL_KEYWORD2 5
#define HL_STRING 6
#define HL_NUMBER 7
#define HL_MATCH 8      /* Search match. */

#define HL_HIGHLIGHT_STRINGS (1<<0)
#define HL_HIGHLIGHT_NUMBERS (1<<1)

struct hlcolor
{
	int r,g,b;
};

struct editorSyntax
{
	char **keywords;
	char singleline_comment_start[8];
	char multiline_comment_start[8];
	char multiline_comment_end[8];
	int flags;
};


#endif
