/*
 *
 * ©K. D. Hedger. Fri 21 Aug 12:29:04 BST 2015 kdhedger68713@gmail.com

 * This file (LFSTK_lib.h) is part of LFSToolKit.

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

#ifndef _LFSTK_LIB_
#define _LFSTK_LIB_

#include <Imlib2.h>

#define	MAXVARS 100
#define MAXBUFFER 512
enum	{TYPEWINDOW=0,TYPEWINDOWTILE,TYPEBUTTON,TYPEBUTTONTILE,TYPEMENUITEM,TYPEMENUITEMTILE,TYPEFONTCOLOUR,TYPEMENUITEMFONTCOLOUR,TYPEMENUITEMFONT,TYPEFONT};

/**
 *
 * \brief Library class for LFSToolKit.
 *
 * Library class for LFSToolKit.
 *
 */
class LFSTK_lib
{
	public:
		LFSTK_lib(bool loadvars);
		~LFSTK_lib();
		bool		LFSTK_loadVarsFromFile(const char* filepath,const args* dataptr);
		void		LFSTK_saveVarsToFile(const char* filepath,const args* dataptr);
		const args	*LFSTK_getTKArgs(void);
		const char	*LFSTK_getGlobalString(int state,int type);
		void		LFSTK_setGlobalString(int state,int type,const char *str);
		bool		LFSTK_getAutoLabelColour(void);
		void		LFSTK_setAutoLabelColour(bool toset);
		bool		LFSTK_getUseTheme(void);
		void		LFSTK_setUseTheme(bool use);
	
		const char	*bestFontColour(long pixel);
//		int			LFSTK_getTextwidth(Display *disp,XftFont *font,const char *str);
		int			LFSTK_getTextwidth(Display *disp,XftFont *font,const char *str);
		fontStruct	*LFSTK_loadFontxx(Display *disp,int scr,const char *name);
		static bool	LFSTK_gadgetEvent(void *self,XEvent *e,int type);

		bool		LFSTK_setPixmapsFromPath(Display *display,Visual *visual,Colormap cm,Window w,const char *file,Pixmap *image,Pixmap *mask,int size);
		const char	*LFSTK_findThemedIcon(const char *theme,const char *icon,const char *catagory);
		char		*LFSTK_oneLiner(const char* fmt,...);

		bool		LFSTK_pointInRect(pointStruct *point,geometryStruct *geom);

//graphics
		void		LFSTK_setCairoSurface(Display *display,Window window,Visual *visual,cairo_surface_t **sfc,cairo_t **cr,int width,int height);
//		cairo_status_t LFSTK_lib::LFSTK_setImageFromPath(const char *file,int grav,bool scale);

	private:
		static int callback(const char *fpath,const struct stat *sb,int typeflag);

		args	*lfsToolKitGlobals;
//window stirngs
		char	*globalWindowColours[MAXCOLOURS];
		char	*globalButtonColours[MAXCOLOURS];
		char	*globalMenuItemColours[MAXCOLOURS];
		char	*globalFontString;
		char	*globalWindowTile;
		char	*globalButtonTile;
		char	*globalMenuItemTile;

//gadget strings
		char	*globalFontColourNames[MAXCOLOURS];
		char	*globalMenuItemFontString;
		char	*globalMenuItemFontColourNames[MAXCOLOURS];
//other
		bool	autoLabelColour;
		bool	useTheme;
};
#endif
