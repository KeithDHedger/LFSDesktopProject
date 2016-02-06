/*
 *
 * ©K. D. Hedger. Fri 24 Jul 19:33:15 BST 2015 kdhedger68713@gmail.com

 * This file (LFSTKGlobals.h) is part of LFSToolKit.

 * LFSToolKit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSToolKit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSToolKit.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _LFSTKGLOBALS_
#define _LFSTKGLOBALS_

#include <X11/Xft/Xft.h>

#define DEFAULTFONT "sans:size=10"
#define DISCLOSURESIZE 6

enum {LEFT=0,CENTRE,RIGHT};
enum gadgetState {NORMALCOLOUR=0,PRELIGHTCOLOUR,ACTIVECOLOUR,INACTIVECOLOUR,MAXCOLOURS};

enum {BUTTONGADGET=1,MENUBUTTONGADGET,LINEEDITGADGET,LABELGADGET,TOGGLEGADGET,IMAGEGADGET};
enum {TYPEINT=1,TYPESTRING,TYPEBOOL};

struct args
{
	const char*		name;
	int				type;
	void*			data;
};

struct buttonCB
{
	bool		(*pressCallback)(void *,void*);
	bool		(*releaseCallback)(void *,void*);
	void		*userData;
	bool		ignoreCallback;
};

struct fontStruct
{
	int			ascent;
	int			descent;
	int			size;
	XftFont		*data;
};

struct listener
{
	bool		(*function)(void *,XEvent *,int);
	void		*pointer;
	int			type;
};

struct geometryStruct
{
	int			x,y;
	unsigned	w,h;
	unsigned	monitor;
};

struct colourStruct
{
	char		*name;
	long		pixel;
	XftColor	xftcol;
};

struct	gadgetList;
class	LFSTK_gadgetClass;

#include <lfstk/LFSTKLib.h>
#include <lfstk/LFSTKWindow.h>
#include <lfstk/LFSTKGadget.h>
#include <lfstk/LFSTKLabel.h>
#include <lfstk/LFSTKImage.h>
#include <lfstk/LFSTKButton.h>
#include <lfstk/LFSTKMenuButton.h>
#include <lfstk/LFSTKLineEdit.h>
#include <lfstk/LFSTKToggleButton.h>

#endif