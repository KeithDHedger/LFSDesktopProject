/*
 *
 * ©K. D. Hedger. Thu 13 Aug 16:55:10 BST 2015 kdhedger68713@gmail.com

 * This file (graphics.h) is part of LFSDesktop.

 * LFSDesktop is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSDesktop is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSDesktop.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef _GRAPHICS_
#define _GRAPHICS_

#include <cairo.h>

#define BUFFERSIZE 256

struct Hints
{
	unsigned long   flags;
	unsigned long   functions;
	unsigned long   decorations;
	long            inputMode;
	unsigned long   status;
};

struct	diskIconStruct
	{
		double			scale;
		cairo_surface_t	*cairoImage;
	};

//struct	cairoColor
//{
//	double	r,g,b,a;
//};
//
extern cairo_t				*cr;
extern char					*fontName;
extern cairo_font_weight_t	weight;
extern cairo_font_slant_t	slant;
extern int					fontSize;
extern cairoColor			backColour;
extern cairoColor			foreColour;

void drawImage(char *type,const char *catagory,int x,int y,bool mounted);

int get_argb_visual(Visual** vis,int *depth);
void createDesktopWindow(void);
void drawIcons(void);

#endif
