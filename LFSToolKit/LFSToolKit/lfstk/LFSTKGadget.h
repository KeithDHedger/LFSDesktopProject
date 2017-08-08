/*
 *
 * ©K. D. Hedger. Wed  5 Aug 13:53:33 BST 2015 kdhedger68713@gmail.com

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

//enum bevelType {BEVELIN=0,BEVELOUT,BEVELNONE};
//enum indicatorType {CHECK=0,RADIO,PICTURE,DISCLOSURE,NOINDICATOR};

/**
 *
 * \brief Base class for LFSToolKit gadgets.
 *
 * Base class for LFSToolKit gadgets.
 *
 */
class LFSTK_gadgetClass
{
	public:
		LFSTK_gadgetClass();
		virtual ~LFSTK_gadgetClass();

//window events
		virtual void		LFSTK_clearWindow(void);
		virtual void		LFSTK_resizeWindow(int w,int h);
		virtual bool		clientMessage(XEvent *e);
		virtual bool		gotFocus(XEvent *e);
		virtual bool		lostFocus(XEvent *e);

//mouse events
		virtual bool		mouseUp(XButtonEvent *e);
		virtual bool		mouseDown(XButtonEvent *e);
		virtual bool		mouseExit(XButtonEvent *e);
		virtual bool		mouseEnter(XButtonEvent *e);
		virtual bool		mouseDrag(XMotionEvent *e);
		virtual bool		keyRelease(XKeyEvent *e);

//DnD routines
		virtual void		LFSTK_dropData(propertyStruct* data);

		Window				LFSTK_getWindow(void);
		void				LFSTK_setCommon(LFSTK_windowClass* parentwc,const char* label,int x,int y,unsigned int w,unsigned int h,int gravity);
		void				LFSTK_setCallBack(bool (*downcb)(void *,void*),bool (*releasecb)(void *,void*),void* ud);

//colours
		colourStruct		fontColourNames[MAXCOLOURS];
		colourStruct		colourNames[MAXCOLOURS];

		void				LFSTK_setColourName(int p,const char* colour);
		const char			*LFSTK_getColourName(int p);
		void				LFSTK_setFontString(const char *s);
		void				LFSTK_setFontColourName(int p,const char* colour,bool usewindow);
		void				LFSTK_getFontGeom(geometryStruct *geom,const char *label);
		void				LFSTK_reloadColours(void);

		void				LFSTK_setActive(bool active);
		void				LFSTK_setLabelAutoColour(bool setauto);
		void				LFSTK_setCairoFontDataParts(const char* fmt,...);

		void				LFSTK_setLabel(const char *newlabel);
		const char			*LFSTK_getLabel(void);
		void				LFSTK_setLabelGravity(int orient);
		void				drawImage();

		cairo_status_t 		LFSTK_setImageFromPath(const char *file,int orient,bool scale);

		int					LFSTK_gadgetOnMonitor(void);
		void				LFSTK_setTile(const char *path,int size);

//geometry
		void				LFSTK_getGlobalGeom(geometryStruct *geom);
		void				LFSTK_getGeom(geometryStruct *geom);
		int					LFSTK_getTextWidth(const char* text);

//fonts
		void				LFSTK_setCairoFontData(void);

		LFSTK_windowClass	*wc;

		Display				*display;
		Window				parent;
		GC					gc;
		Window				window;

		int					screen;
		Visual				*visual;
		Window				rootWindow;		
		Colormap			cm;

		bool				useImage;
		bool				gotIcon;
		bool				gadgetAcceptsDnD;
		bool				isSubMenu;
		gadgetStruct		gadgetDetails={&this->colourNames[NORMALCOLOUR],BEVELOUT,NOINDICATOR,NULL,NORMALCOLOUR,0,false,{0,0,0,0},{0,0,0,0},false};

	private:
		void				initGadget(void);

	protected:
		void				drawBox(geometryStruct* g,gadgetState state,bevelType bevel);
		void				drawIndicator(gadgetStruct* details);
		bevelType			getActiveBevel(void);

		geometryStruct		gadgetGeom;
		int					pad;

		int					blackColour;
		int					whiteColour;
		XftColor			blackXftColour;
		XftColor			whiteXftColour;


		buttonCB			callback;

//font and label stuff

		char				*label=NULL;
		char				*fontString=NULL;
		char				*monoFontString=NULL;
		fontStruct			*font;
		cairo_text_extents_t	extents;
		cairo_font_weight_t	weight;
		cairo_font_slant_t	slant;
		int					fontSize;
		char				*fontName=NULL;

//		colourStruct		fontColourNames[MAXCOLOURS];
//		colourStruct		colourNames[MAXCOLOURS];
		bool				inWindow;
		bool				isActive;
		bool				autoLabelColour;
		int					labelGravity;

		bevelType			style;
		int					labelOffset;
		unsigned int		iconSize;
		bool				freeOnDelete;
		Pixmap				icon[2];
		Pixmap				tile[2];
		Imlib_Image			image;

		cairo_t				*cr=NULL;
		cairo_surface_t 	*sfc=NULL;
		cairo_surface_t		*cImage=NULL;
		int					imageWidth=0;
		int					imageHeight=0;
		int					imageGravity;
		float				ratio;
		bool				useTile;
		cairo_pattern_t		*pattern=NULL;

		bevelType			newbevel=BEVELOUT;
		gadgetState			state=NORMALCOLOUR;

//gadget graphics details
		void				drawGagetDetails(void);
		void				clearBox(gadgetStruct* details);
		void				drawBevel(geometryStruct* geom,bevelType bevel);
		void				drawLabel(gadgetStruct* details);
};

#endif
