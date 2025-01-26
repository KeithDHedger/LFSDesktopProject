/*
 *
 * ©K. D. Hedger. Thu 23 Jul 16:15:33 BST 2015 keithdhedger@gmail.com

 * This file (LFSTKwindow.h) is part of LFSToolKit.

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

#ifndef _LFSTKWINDOW_
#define _LFSTKWINDOW_

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <cairo.h>
#include <cairo-xlib.h>
#include <map>

/**
* Drop data structure.
* \param type 0=text, 1=uri, -1 invalid.
* \param char*. 
*/
struct dropDataStruct
{
	dropDataType	type;
	char			*data;
	int				x,y;
	LFSTK_gadgetClass	*gadget=NULL;
};

enum {XDNDENTER=0,XDNDPOSITION,XDNDSTATUS,XDNDTYPELIST,XDNDACTIONCOPY,XDNDDROP,XDNDLEAVE,XDNDFINISHED,XDNDSELECTION,XDNDPROXY,XA_CLIPBOARD,XA_COMPOUND_TEXT,XA_UTF8_STRING,XA_TARGETS,PRIMARY,SECONDARY,DNDATOMCOUNT};

enum setWindowGeomFlags {WINDSETXY,WINDSETWH,WINDSETALL};

//motif hints
#define MWM_HINTS_FUNCTIONS (1L << 0)
#define MWM_HINTS_DECORATIONS (1L << 1)

#define MWM_FUNC_ALL (1L << 0)
#define MWM_FUNC_RESIZE (1L << 1)
#define MWM_FUNC_MOVE (1L << 2)
#define MWM_FUNC_MINIMIZE (1L << 3)
#define MWM_FUNC_MAXIMIZE (1L << 4)
#define MWM_FUNC_CLOSE (1L << 5)

#define MWM_DECOR_ALL (1L << 0)
#define MWM_DECOR_BORDER (1L << 1)
#define MWM_DECOR_RESIZEH (1L << 2)
#define MWM_DECOR_TITLE (1L << 3)
#define MWM_DECOR_MENU (1L << 4)
#define MWM_DECOR_MINIMIZE (1L << 5)
#define MWM_DECOR_MAXIMIZE (1L << 6)

/**
 * \brief Window class for LFSToolKit.
 * \note The main window class MUST be passed to child widgets.
*/
class LFSTK_windowClass
{
	public:
		LFSTK_windowClass();
		~LFSTK_windowClass();
		LFSTK_windowClass(int x,int y,int w,int h,const char* name,bool override,bool loadvars=true,bool shutdisplayonexit=false);
		LFSTK_windowClass(windowInitStruct *wi);
	
		LFSTK_windowClass(windowInitStruct *wi,LFSTK_applicationClass *app);

		void					LFSTK_showWindow(bool all=true);
		void					LFSTK_hideWindow(void);
		void					LFSTK_setWindowTitle(const char *title);

		void 				LFSTK_clearWindow(bool cleargadgets=false);
		void					LFSTK_resizeWindow(int w,int h,bool tellx=true);
		void					LFSTK_moveWindow(int x,int y,bool tellx=true);
		void					LFSTK_moveResizeWindow(int x,int y,int w,int h,bool tellx=true);
		const geometryStruct	*LFSTK_getWindowGeom(void);
		void					setWindowGeom(int x,int y,int h,int w,setWindowGeomFlags flags);

		void					LFSTK_setFontString(std::string s);
		void					LFSTK_setFontColourName(int p,const char *colour);
		void					LFSTK_setWindowColourName(int p,const char* colour);
		void					LFSTK_setDecorated(bool isDecorated);
		void					LFSTK_setDecorations(bool canmax=true,bool canmin=true,bool canresize=true,bool canclose=true);
		void					LFSTK_setKeepAbove(bool set);
		void					LFSTK_setKeepBelow(bool set);
		void					LFSTK_setActive(bool set);
		bool					LFSTK_getActive(void);
		void					LFSTK_setSticky(bool set);
		bool					LFSTK_getSticky(void);
		void					LFSTK_setWindowType(Atom type);

		void					LFSTK_setTransientFor(Window w);
	
		void					LFSTK_reloadGlobals(void);
		void					LFSTK_setXProperty(Atom property,Atom type,int format,void *dataptr,int propcnt);
		void					LFSTK_sendMessage(const char *msg,unsigned long data0,unsigned long data1,unsigned long data2,unsigned long data3,unsigned long data4);

		int					LFSTK_windowOnMonitor(void);

		void					LFSTK_setTile(const char *path,int size);

//gadget management
		void					LFSTK_addMappedListener(int mapwindow,mappedListener* ml);
		mappedListener		*LFSTK_getMappedListener(int window);
		int					LFSTK_gadgetCount(void);
		LFSTK_gadgetClass	*LFSTK_findGadgetByPos(int x,int y);
		bool					LFSTK_deleteGadget(LFSTK_gadgetClass *gadget);
		void					LFSTK_redrawAllGadgets(void);

//dnd
		void					LFSTK_initDnD(bool acceptwindowdrops=false);
		Atom					LFSTK_getDnDAtom(int atomnum);
		void					LFSTK_handleDnD(XEvent *event);
		dropDataStruct		droppedData={DROPINVALID,NULL,-1,-1};
		bool					acceptOnThis=false;
		LFSTK_gadgetClass	*dropGadget;
		void					LFSTK_setWindowDropCallBack(bool (*dropped)(LFSTK_windowClass*,void*),void* ud);
		callbackStruct		callBacks;

//window graphics
		void					LFSTK_setWindowPixmap(Pixmap pixmap,int w,int h,bool updategadgets=false);
		Pixmap				px=None;
		bool					usePixmap=false;
		Window				window;
		GC					gc;
		cairo_surface_t 		*sfc=NULL;
		cairo_t				*cr=NULL;
		cairo_pattern_t		*pattern=NULL;
		Visual				*visual;
		Colormap				cmap;

		std::string			fontString;
		char					*fontColourNames[MAXCOLOURS]={NULL,};
		colourStruct			windowColourNames[MAXCOLOURS]={{},};//   {{NULL,0,{0,0,0,0}},};
		bool					autoLabelColour=false;
		LFSTK_lib			*globalLib;
		int					x;
		int					y;
		int					w;
		int					h;
		double				windowNormalAlpha=-1;

//user data and dirs
		Window				parentWindow=None;
		bool					useTile;
		bool					acceptDnd;
		void					*userData=NULL;
		LFSTK_gadgetClass	*popupFromGadget=NULL;

		std::map<int,mappedListener*>	*LFSTK_getGadgets(void);

//context
		contextPostition		contextWindowPos=CONTEXTATMOUSE;
		LFSTK_windowClass	*contextWC=NULL;
		bool					ignoreContext=false;
		int					contextXOffset=0;
		int					contextYOffset=0;
		LFSTK_windowClass 	*LFSTK_getContextWindow(void);
		void					LFSTK_setContextWindow(LFSTK_windowClass *wc);

//events etc
		void					LFSTK_setDoubleClickTime(unsigned interval);
		unsigned				dbClick=500;
		bool					popupLoop=false;
		int					LFSTK_handleWindowEvents(XEvent *event);
		bool					mainLoop=false;
		bool					mainLoopQuit=false;
		bool					isVisible=false;
		bool					passEventToRoot=false;

//copy/paste buffer etc
		std::string			clipBuffer;
		std::map<int,mappedListener*>	gadgetMap;

//app
		LFSTK_applicationClass			*app=NULL;
		bool								inWindow=false;

	protected:
		bool					isActive;
		//char					*windowName=NULL;
		std::string			windowName="LFSTK Window";
		void					initWindow(bool loadvars);
	
	private:
//window routines
		void					windowClassInitCommon(windowInitStruct *wi);
		void					loadGlobalColours(void);
		void					sendUTF8(XSelectionRequestEvent *sev);

//DnD routines
		void					LFSTK_dropData(propertyStruct* data);
		propertyStruct		*readProperty(Window src,Atom property);
		std::string			getAtomName(Atom a);
		Atom					pickTargetFromList(Atom* atom_list,int nitems);
		Atom					pickTargetFromTargets(propertyStruct* p);
		Atom					pickTargetFromAtoms(Atom t1, Atom t2, Atom t3);

		bool					isSticky;
		geometryStruct		windowGeom;


//Atoms etc for Xdnd/cut/copy/paste
		Atom					dNdAtoms[DNDATOMCOUNT];
		std::map<std::string,int> dNdTypes;
		Atom					toBeRequested;
		Window				sourceWindow;
		int					xDnDVersion;
};

#endif
