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

enum lockType			{LOCKTOLEFT,LOCKTORIGHT,LOCKTOTOP,LOCKTOBOTTOM,LOCKTOCENTRE,ABSOLUTE};
enum stretchType		{STRETCH,MOVE,SPACE};

/**
 * \brief Multi gadget class for LFSToolKit.
*/
class LFSTK_MultiGadgetClass : public LFSTK_gadgetClass
{
	public:
		LFSTK_MultiGadgetClass();
		~LFSTK_MultiGadgetClass();
		LFSTK_MultiGadgetClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,unsigned w,unsigned h,int gravity=NorthWestGravity);

		void		LFSTK_setHitRects(std::vector<hitRect> hr);
		void		LFSTK_updateGadget(geometryStruct oldgeom);
		void		updateInternalGadgets(geometryStruct oldgadggeom);

		bool		stretchX=true;
		bool		stretchY=true;
		lockType	lockX=LOCKTOLEFT;
		lockType	lockY=ABSOLUTE;
		stretchType	gadgetStretch=STRETCH;
		
		std::vector<hitRect>	hitRects;
	private:

};

#endif
