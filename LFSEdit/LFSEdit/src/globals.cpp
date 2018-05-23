/*
 *
 * ©K. D. Hedger. Wed 23 May 14:12:28 BST 2018 kdhedger68713@gmail.com

 * This file (globals.cpp) is part of LFSEdit.

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

//term
termios orig_termios; /* In order to restore at exit.*/

//editor
editorConfig	editorPage;

//prefs

//style
char			*lang=NULL;
unsigned		langFlags=HL_HIGHLIGHT_STRINGS | HL_HIGHLIGHT_NUMBERS;

/* =========================== Syntax highlights DB =========================
 *
 * In order to add a new syntax,define two arrays with a list of file name
 * matches and keywords. The file name matches are used in order to match
 * a given syntax with a given file name: if a match pattern starts with a
 * dot,it is matched as the last past of the filename,for example ".c".
 * Otherwise the pattern is just searched inside the filenme,like "Makefile").
 *
 * The list of keywords to highlight is just a list of words,however if they
 * a trailing '|' character is added at the end,they are highlighted in
 * a different color,so that you can have two different sets of keywords.
 *
 * Finally add a stanza in the HLDB global variable with two two arrays
 * of strings,and a set of flags in order to enable highlighting of
 * comments and numbers.
 *
 * The characters for single and multi line comments must be exactly two
 * and must be provided as well (see the C language example).
 *
 * There is no support to highlight patterns currently. */

/* C / C++ */
const char		*langKeywords[]= {"switch","if","while","for","break","continue","return","else","struct","union","typedef","static","enum","class","int|","long|","double|","float|","char|","unsigned|","signed|","void|",NULL};
const char		*langComments[]= {"//","/*","*/",NULL};

