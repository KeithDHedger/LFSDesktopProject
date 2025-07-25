/*
 *
 * ©K. D. Hedger. Wed  5 Aug 13:53:33 BST 2015 keithdhedger@gmail.com

 * This file (LFSTKGadget.h) is part of LFSToolKit.

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


#ifndef _LFSTKGADGET_
#define _LFSTKGADGET_

#include <X11/Xlib.h>
#include <Imlib2.h>

/**
 * \brief Base class for LFSToolKit gadgets.
*/
class LFSTK_gadgetClass
{
	public:
		LFSTK_gadgetClass();
		virtual ~LFSTK_gadgetClass();

//window events
		virtual void			LFSTK_clearWindow(void);
		virtual void			LFSTK_resizeWindow(int w,int h);
		virtual void			LFSTK_moveGadget(int x,int y);
		void					LFSTK_showGadget(void);
		void					LFSTK_hideGadget(void);
		void					LFSTK_reParentWindow(Window win,int newx,int newy);
		virtual bool			clientMessage(XEvent *e);
		virtual bool			gotFocus(XEvent *e);
		virtual bool			lostFocus(XEvent *e);

//other events
		virtual bool			selectionRequest(XSelectionRequestEvent *e);

//mouse events
		virtual bool			mouseUp(XButtonEvent *e);
		virtual bool			mouseDown(XButtonEvent *e);
		virtual bool			mouseExit(XButtonEvent *e);
		virtual bool			mouseEnter(XButtonEvent *e);
		virtual bool			mouseDrag(XMotionEvent *e);

//key events
		virtual bool			keyRelease(XKeyEvent *e) {this->keyEvent=NULL;return(true);};
		virtual bool			keyPress(XKeyEvent *e) {return(true);};

		bool					LFSTK_getCanDrag(void);
		void					LFSTK_setCanDrag(bool candrag);
		void					LFSTK_snapSize(int sze);
		void					LFSTK_allowXMovement(bool confine);
		void					LFSTK_allowYMovement(bool confine);
		void					LFSTK_setLimits(int minx,int miny,int maxx,int maxy);
		void					LFSTK_getLimits(rectStruct *rect);

//events etc
		Time					lastTime=0;
		bool					firstClick=false;
		bool					isDoubleClick=false;
		unsigned int			currentButton=-1;
		long					gadgetEventMask=(ButtonReleaseMask | ButtonPressMask | ExposureMask | EnterWindowMask | LeaveWindowMask|ButtonMotionMask|FocusChangeMask|KeyReleaseMask|KeyPressMask);
		XEvent				*xEvent=NULL;

//context window
		LFSTK_windowClass 	*LFSTK_getContextWindow(void);
		void					LFSTK_setContextWindow(LFSTK_windowClass *wc);
		int					contextYOffset=0;
		int					contextXOffset=0;
		int					windowNumber=-1;

//DnD routines etc
		virtual void			LFSTK_dropData(propertyStruct* data);
		dropDataStruct		droppedData={DROPINVALID,NULL,-1,-1};

//odds
		Window				LFSTK_getWindow(void);
		void					LFSTK_setCommon(LFSTK_windowClass* parentwc,const char* label,int x,int y,unsigned int w,unsigned int h,int gravity=NorthWestGravity);
		bool					isTransparent=false;

//callbacks
		void					LFSTK_setIgnores(bool runcb,bool ignoreorphanmod);
		void					LFSTK_setMouseCallBack(bool (*downcb)(void*,void*),bool (*releasecb)(void*,void*),void* ud);
		void					LFSTK_setKeyCallBack(bool (*downcb)(void *,void*),bool (*releasecb)(void *,void*),void* ud);
		void					LFSTK_setGadgetDropCallBack(bool (*dropped)(void*,propertyStruct *data,void*),void* ud=NULL);
		void					LFSTK_setCallBacks(callbackStruct cbs);
		void					LFSTK_setMouseMoveCallBack(bool (*entercb)(LFSTK_gadgetClass*,void*),bool (*exitcb)(LFSTK_gadgetClass*,void*),void* ud);
		bool					noRunCB=false;

		callbackStruct		callBacks;

//new colours
		std::vector<colourStruct>	newGadgetBGColours={{},{},{},{}};
		std::vector<colourStruct>	newGadgetFGColours={{},{},{},{}};
		colourStruct 		LFSTK_setColour(std::string colour);
		void					LFSTK_setGadgetColours(gadgetColourType type,std::string normal,std::string prelight,std::string active,std::string inactive);
		void					LFSTK_setGadgetColourPair(gadgetState state,std::string back,std::string fore);

//colours
		std::string			LFSTK_getColourName(int p);
		void					LFSTK_setFontString(std::string s,bool setfontdata=false);
		void					LFSTK_reloadColours(void);

		void					LFSTK_setActive(bool active);
		void					LFSTK_setLabelAutoColour(bool setauto);
		void					LFSTK_setCairoFontDataParts(const char* fmt,...);

		void					LFSTK_setLabel(std::string newlabel,bool clearwindow=true);
		std::string			LFSTK_getLabel(void);
		void					LFSTK_setLabelGravity(int orient);

//graphics
		void					drawImage();
		void					LFSTK_setIndicator(indicatorType indictype);
		cairo_status_t 		LFSTK_setImageFromPath(std::string file,int orient,bool scale);
		void 				LFSTK_setImageFromSurface(cairo_surface_t *sfc,int orient,bool scale);

		int					LFSTK_gadgetOnWhichMonitor(void);
		void					LFSTK_setTile(const char *path,int size);
		void					LFSTK_setUseWindowTile(bool usebutton);
		void					LFSTK_setUseWindowPixmap(bool usepixmap);
		void					LFSTK_setAlpha(double alph);
		void					LFSTK_setStyle(bevelType s);

		std::string			imagePath;
//geometry
		void					LFSTK_getGeom(geometryStruct *geom);
		void					LFSTK_getGeomWindowRelative(geometryStruct *geom,Window win);
		double				LFSTK_getTextRealWidth(std::string text);
		int					LFSTK_getTextWidth(std::string text);
		int					LFSTK_getTextHeight(std::string text);
		void					LFSTK_setGadgetSize(int width,int height);

//fonts
		void					LFSTK_setCairoFontData(void);

		LFSTK_windowClass	*wc;
		Window				parent;
		GC					gc;
		Window				window;
		Visual				*visual;
		Colormap				cmap;

		bool					drawLabelBG=false;
		bool					autoLabelBGColour=false;
		colourStruct			labelBGColour={"",false,0,{1.0,1.0,1.0,1.0}};
		void					LFSTK_setLabelBGColour(double r,double g,double b,double a);
		void					LFSTK_setLabelBGColour(const char* colour,double alpha);
		void					LFSTK_setShowIndicator(bool show);
		bool					useImage;
		bool					gotIcon;
		bool					gadgetAcceptsDnD;
		bool					isSubMenu;
		bool					showIndicator;
		gadgetStruct			gadgetDetails={&this->newGadgetBGColours.at(NORMALCOLOUR),BEVELOUT,NOINDICATOR,NORMALCOLOUR,0,false,{0,0,0,0},{0,0,0,0},false,false,false,false,false};
		std::string			monoFontString;

//user data
		void					*userData=NULL;
		XButtonEvent			*mouseEvent=NULL;
		XKeyEvent			*keyEvent=NULL;
		bool					toParent=false;
		bool					isMapped=false;
//context
		contextPostition		contextWindowPos=CONTEXTRIGHT;
		bool					inWindow;//TODO//MMMMmmmmmmm
		int					imageWidth=0;


		cairo_t				*cr=NULL;
		cairo_surface_t 		*sfc=NULL;
		cairo_pattern_t		*pattern=NULL;
		unsigned				pad;

		int					gadgetType=NOGADGET;

	private:
		void					initGadget(void);
		void					selectBevel(bool mousedown);
		cairo_surface_t		*link=NULL;
		cairo_surface_t		*broken=NULL;
		void					resizeCairoParts(int width,int height);

	protected:
		mappedListener		*ml=NULL;
		void					drawBox(geometryStruct* g,gadgetState state,bevelType bevel);
		void					drawIndicator(gadgetStruct* details);
		bevelType			getActiveBevel(void);

		geometryStruct		gadgetGeom;

//font and label stuff
		std::string			label;
		std::string			fontString;

		cairo_text_extents_t	textExtents;
		cairo_font_extents_t	fontExtents;
		cairo_font_weight_t	weight;
		cairo_font_slant_t	slant;
		int					fontSize;
		std::string			fontName;
		double				maxTextHeight;

		bool					isActive;
		bool					autoLabelColour=false;
		int					labelGravity;

		bevelType			style;
		int					labelOffset;
		unsigned int			iconSize;

//graphics
//		cairo_t				*cr=NULL;
//		cairo_surface_t 		*sfc=NULL;
		cairo_surface_t		*cImage=NULL;
//		cairo_pattern_t		*pattern=NULL;
		
		int					imageHeight=0;
		int					imageGravity=CENTRE;
		float				ratio;
		bool					useTile;

		bevelType			newbevel=BEVELOUT;
		gadgetState			state=NORMALCOLOUR;

//gadget graphics details
		void					drawGagetDetails(void);
		void					clearBox(gadgetStruct* details);
		void					drawBevel(geometryStruct* geom,bevelType bevel);
		void					drawLabel(gadgetStruct* details);
		int					squeezeW=0;
		int					squeezeH=0;

//drag stuff
		bool					canDrag=false;
		int					mouseDownX=0;
		int					mouseDownY=0;
		int					snap=1;
		double				alpha=1.0;
		bool					allowX=true;
		bool					allowY=true;
		int					minX=-1;
		int					maxX=-1;
		int					minY=-1;
		int					maxY=-1;

//context
		LFSTK_windowClass	*contextWC=NULL;
};

#endif
