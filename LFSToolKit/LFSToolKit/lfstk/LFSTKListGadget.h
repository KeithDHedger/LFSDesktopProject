/*
 *
 * ©K. D. Hedger. Sun 11 Sep 19:08:54 BST 2016 keithdhedger@gmail.com

 * This file (LFSTKListGadget.h) is part of LFSToolKit.

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

#ifndef _LFSTKLISTGADGET_
#define _LFSTKLISTGADGET_

#define LABELHITE 24
#define LISTDATA(x) (void*)(&this->data[x])
#define SCROLLDATA(x) (void*)(&this->scrollData[x])
#define LGAP 4
#define	NAVBUTTONSIZE 16
#define LICONSIZE 20
#define SCROLLBARWIDTH 16

enum {LUP=0,LDOWN,LHOME,LEND};

class LFSTK_listGadgetClass  : public  LFSTK_gadgetClass
{
	public:
		~LFSTK_listGadgetClass();
		LFSTK_listGadgetClass(LFSTK_windowClass *parentwc,const char *label,int x,int y,unsigned w,unsigned h,int gravity,char **list,unsigned cnt);

		int					LFSTK_getCurrentListItem(void);
		const char			*LFSTK_getListString(int listnum);
		void				LFSTK_setList(char **list,unsigned numitems);
		void				LFSTK_setListFromFile(const char *filepath,bool includeempty);
		void				LFSTK_setImageList(char **list,unsigned numitems);
		int					currentItem;

		bool				mouseExit(XButtonEvent *e) {return(true);};
		bool				mouseEnter(XButtonEvent *e) {return(true);};
		unsigned			listCnt;

	private:
		struct listData
			{
				LFSTK_listGadgetClass	*mainObject;
				unsigned				userData;
			};
		char				**listStrings;
		char				**listImages;
		unsigned			listImageCnt;
		unsigned			listOffset;

		unsigned			maxShowing;
		listData			*data;
		listData			*scrollData;

		LFSTK_buttonClass	**labels;
//		LFSTK_buttonClass	*buttonUp;
//		LFSTK_buttonClass	*buttonDown;
//		LFSTK_buttonClass	*buttonHome;
//		LFSTK_buttonClass	*buttonEnd;
		
		LFSTK_scrollBarClass	*scrollBar;

		void 				setCurrentItem(int item);
		void				setNavSensitive(void);

		static bool			select(void *object,void* userdata);
		//static bool			scrollCB(void *object,void* userdata);
		static bool			newscrollCB(void *object,void* userdata);
};

#endif
