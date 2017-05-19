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

#define DEFAULTFONT "sans:size=10"
#define DISCLOSURESIZE 6

#define USERDATA(x) (void*)(long)x
#define GETUSERDATA(x) (long)x

#define BUTTONWIDTH 72
#define BUTTONHITE 24

enum {LEFT=0,CENTRE,RIGHT};
enum gadgetState {NORMALCOLOUR=0,PRELIGHTCOLOUR,ACTIVECOLOUR,INACTIVECOLOUR,MAXCOLOURS};

enum {BUTTONGADGET=1,MENUBUTTONGADGET,LINEEDITGADGET,LABELGADGET,TOGGLEGADGET,IMAGEGADGET,LISTGADGET,DIALOGGADGET,MENULISTGADGET};
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

struct geometryStruct
{
	int			x,y;
	unsigned	w,h;
	unsigned	monitor;
};

struct pointStruct
{
	int			x,y;
};

struct colourStruct
{
	char		*name;
	long		pixel;
	XftColor	xftcol;
};

struct	gadgetList;
class	LFSTK_gadgetClass;
struct	mappedListener
{
	bool				(*function)(void *,XEvent *,int);
	LFSTK_gadgetClass	*gadget;
	int					type;
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
#include "LFSTKFontButton.h"
#include "LFSTKListGadget.h"
#include "LFSTKFileDialog.h"

#endif