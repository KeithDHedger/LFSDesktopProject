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
#include <iostream>
#include <map>

#include <cairo.h>
#include <cairo-xlib.h>

#define COPYRITE			"Copyright © 2013-2017 K.D.Hedger"
#define PERSONAL			"keithhedger@keithhedger.darktech.org"
#define	MYWEBSITE			"http://keithhedger.darktech.org/"

#define DEFAULTFONT			"sans:size=10"
#define DISCLOSURESIZE		6

#define USERDATA(x)			(void*)(long)x
#define GETUSERDATA(x)		(long)x

#define DIALOGWIDTH			450
#define DIALOGHITE			800
#define DIALOGMIDDLE		DIALOGWIDTH/2

#define GADGETWIDTH			80
#define GADGETHITE			24
#define HALFGADGETWIDTH		GADGETWIDTH/2

#define BORDER				16
#define YSPACING			32
#define HALFYSPACING		YSPACING/2
#define BUTTONGRAV			NorthWestGravity

enum {CANCEL=0,APPLY};
enum {NONE=0,LEFT,CENTRE,RIGHT,AUTO,MENU,PRESERVEASPECT,TOOLBAR};
enum gadgetState {NORMALCOLOUR=0,PRELIGHTCOLOUR,ACTIVECOLOUR,INACTIVECOLOUR,MAXCOLOURS};

enum {BUTTONGADGET=1,MENUBUTTONGADGET,LINEEDITGADGET,LABELGADGET,TOGGLEGADGET,IMAGEGADGET,LISTGADGET,DIALOGGADGET};
enum {TYPEINT=1,TYPESTRING,TYPEBOOL};

enum bevelType {BEVELIN=0,BEVELOUT,BEVELNONE};
enum indicatorType {CHECK=0,RADIO,PICTURE,DISCLOSURE,NOINDICATOR};

struct args
{
	const char*				name;
	int						type;
	void*					data;
};

struct buttonCB
{
	bool					(*pressCallback)(void *,void*);
	bool					(*releaseCallback)(void *,void*);
	void					*userData;
	bool					ignoreCallback;
};

struct geometryStruct
{
	int						x,y;
	unsigned				w,h;
	unsigned				monitor;
};

struct pointStruct
{
	int						x,y;
};

struct	cairoColor
{
	double					r,g,b,a;
};

struct colourStruct
{
	char					*name;
	long					pixel;
	cairoColor				RGBAColour;
};

class	LFSTK_gadgetClass;
struct	mappedListener
{
	bool				(*function)(void *,XEvent *,int);
	LFSTK_gadgetClass	*gadget;
	int					type;
};

struct gadgetStruct
{
	colourStruct	*colour;
	bevelType		bevel;
	indicatorType	indic;
	geometryStruct	*indicGeom;
	int				state;
	int				reserveSpace;
	bool			buttonTile;
	geometryStruct	gadgetGeom;
	geometryStruct	indicatorGeom;
	bool			hasIndicator;
	bool			useWindowPixmap;
};


#include "LFSTKLib.h"
#include "LFSTKWindow.h"
#include "LFSTKGadget.h"
#include "LFSTKLabel.h"
#include "LFSTKImage.h"
#include "LFSTKButton.h"
#include "LFSTKMenuButton.h"
#include "LFSTKLineEdit.h"
#include "LFSTKToggleButton.h"
#include "LFSTKListGadget.h"
#include "LFSTKFileDialog.h"
#include "LFSTKFontDialog.h"

#endif