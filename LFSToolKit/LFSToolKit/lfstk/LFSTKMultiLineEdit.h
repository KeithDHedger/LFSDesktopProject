/*
 *
 * ©K. D. Hedger. Wed  5 Aug 12:36:06 BST 2015 keithdhedger@gmail.com

 * This file (LFSTKLineEdit.h) is part of LFSToolKit.

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


#ifndef _LFSTKMULTILINEEDIT_
#define _LFSTKMULTILINEEDIT_

#include <string>
#include <X11/Xatom.h>
#include <vector>

#define BARCURSORCHAR '|'
#define STRBARCURSORCHAR "|"

struct lineStruct
{
	char			*line;
	int			cursorPos;
	double		xpos;
	double		ypos;
	double		width;
	double		height;
};

struct highlightStruct
{
	int			sx;
	int			sy;
	int			len;
	cairoColor	colour;
};

/**
 * \brief Mult-Line edit class for LFSToolKit.
*/
class LFSTK_multiLineEditClass  : public  LFSTK_lineEditClass
{
	public:
		LFSTK_multiLineEditClass();
		~LFSTK_multiLineEditClass();
		LFSTK_multiLineEditClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,unsigned w,unsigned h,int gravity);

		void LFSTK_clearWindow(void);
		void LFSTK_resizeWindow(int w,int h);

		std::string	LFSTK_getBuffer(void);
		void LFSTK_setBuffer(const char *str);
		const char *LFSTK_getCStr(void);

		bool keyRelease(XKeyEvent *e);
		bool lostFocus(XEvent *e);
		bool gotFocus(XEvent *e);
		void LFSTK_setFocus(void);

		void LFSTK_dropData(propertyStruct* data);
		void LFSTK_setFormatedText(const char *txt,bool replace);
		void LFSTK_upDateText(void);

		void LFSTK_addHighLights(int x,int y,int len,cairoColor col);

		std::vector<highlightStruct>		highLights;
		double							totalTextHeight=0;
		geometryStruct		gadgetGeom;

	private:
		void highLightText(void);
		void drawText(void);
		void getClip(void);

		bool								doHighlight=false;
		unsigned							cursorPos;
		std::string						buffer;
		bool								isFocused=false;
		void								setDisplayLines(void);
		int								topLine;
		std::vector<lineStruct*>			lines;
		bool								fromlf=false;
};

#endif
