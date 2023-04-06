/*
 *
 * ©K. D. Hedger. Sat 12 Dec 12:31:33 GMT 2020 keithdhedger@gmail.com

 * This file (LFSTKMultiGadget.h) is part of LFSToolKit.

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


#ifndef _LFSTKMULTIGADGET_
#define _LFSTKMULTIGADGET_

struct	hitRect
{
	rectStruct			rect={0,0,0,0};
	LFSTK_gadgetClass	*gadget=NULL;
};

/**
 * \brief lockType.
 * \note Affects gadget as a whole.
*/
enum lockType			{LOCKTOLEFT,LOCKTORIGHT,LOCKTOTOP,LOCKTOBOTTOM,LOCKTOCENTRE,ABSOLUTE};
/**
 * \brief stretchType.
 * \note Affects sub gadgets.
*/
enum stretchType		{STRETCH,MOVE,NOCHANGE,SPACESPREADX,SPACESPREADY,SPACELEFT,SPACERIGHT};

/**
 * \brief Multi gadget class for LFSToolKit.
*/
class LFSTK_ExpanderGadgetClass : public LFSTK_gadgetClass
{
	public:
		LFSTK_ExpanderGadgetClass();
		~LFSTK_ExpanderGadgetClass();
		LFSTK_ExpanderGadgetClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,unsigned w,unsigned h,int gravity=NorthWestGravity);

		void					LFSTK_setHitRects(std::vector<hitRect> hr);
		void					LFSTK_updateGadget(geometryStruct oldgeom);

//Not normally used by user.
		void					LFSTK_resetGadgets(void);
		void					LFSTK_dropData(propertyStruct* data);

		bool					mouseExit(XButtonEvent *e) {this->inWindow=false;return(false);};
		bool					mouseEnter(XButtonEvent *e) {this->inWindow=true;return(false);};
		bool					mouseDown(XButtonEvent *e) {this->inWindow=true;return(false);};

		bool					stretchX=false;
		bool					stretchY=false;
		lockType				lockX=LOCKTOCENTRE;
		lockType				lockY=LOCKTOTOP;
		stretchType			gadgetStretch=NOCHANGE;
		int					spacing=BORDER;
		bool					done=false;
		std::vector<hitRect>	hitRects;
		int					spacePad=16;
		bool					liveUpdate=false;
	private:
		void					updateInternalGadgets(geometryStruct oldgadggeom);
		void					justifyHitRects(bool rezizeandmove);

};

#endif
