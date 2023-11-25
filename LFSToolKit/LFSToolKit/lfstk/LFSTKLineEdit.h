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


#ifndef _LFSTKLINEEDIT_
#define _LFSTKLINEEDIT_

#include <string>
#include <X11/Xatom.h>

#define CURSORCHAR "█"

/**
 * \brief Line edit class for LFSToolKit.
*/
class LFSTK_lineEditClass  : public  LFSTK_gadgetClass
{
	public:
		LFSTK_lineEditClass();
		~LFSTK_lineEditClass();
		LFSTK_lineEditClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,unsigned w,unsigned h,int gravity=BUTTONGRAV);

		void					LFSTK_clearWindow(void);
		void					LFSTK_resizeWindow(int w,int h);

		std::string			LFSTK_getBuffer(void);
		void					LFSTK_setBuffer(const char *str);
		const char			*LFSTK_getCStr(void);

		void					LFSTK_setCallbackOnReturn(bool onreturn);
		bool					LFSTK_getCallbackOnReturn(void);

		bool					mouseDown(XButtonEvent *e);
		bool					keyRelease(XKeyEvent *e);
		bool					lostFocus(XEvent *e);
		bool					gotFocus(XEvent *e);
		bool					clientMessage(XEvent *e);
		void					LFSTK_setFocus(void);

		void					LFSTK_dropData(propertyStruct* data);
		void					LFSTK_setFormatedText(const char *txt,bool replace);
		void					LFSTK_setCursorColourName(const char* colour);
		std::string			LFSTK_getCursorColourName(void);

		KeySym				LFSTK_getKey(void);
		unsigned int			LFSTK_getModifier(void);
		const char			*LFSTK_getKeySym(void);

	protected:
		colourStruct			cursorColour={"",false,0,{0,0,0,0.8}};
		double				charWidth;
		bool					startUpMDFlag=false;
		bool					callbackOnReturn=true;
	
	private:
		void					drawLabel(void);
		void					getClip(void);
		void					setOffsetcurs(int step);
		std::string			buffer;
		int					cursorPos=0;
		int					visCursorPos=0;
		bool					isFocused=false;
		int					offsetCurs=0;
		KeySym				keysym_return;
		unsigned int			state;

//contxt window
		LFSTK_windowClass	*editWindow=NULL;
		static bool			contextCB(void *p,void* ud);
};

#endif
