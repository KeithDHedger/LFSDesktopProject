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
enum {NOTHUMB=0,CAIROTHUMB,FILETHUMB};

struct listLabelStruct
{
	char	*label;
	int		imageType;
	void	*userData;
	int		listPos;
	union	imageData
		{
			char			*imagePath;
			cairo_surface_t	*surface;
		} data;
};

class LFSTK_listGadgetClass  : public  LFSTK_gadgetClass
{
	public:
		~LFSTK_listGadgetClass();
		LFSTK_listGadgetClass(LFSTK_windowClass *parentwc,const char *label,int x,int y,unsigned w,unsigned h,int gravity,char **list,unsigned cnt);

		int						LFSTK_getCurrentListItem(void);
		void					LFSTK_setList(listLabelStruct **list,unsigned numitems);
		void					LFSTK_setListFromFile(const char *filepath,bool includeempty);
		const char				*LFSTK_getSelectedLabel(void);
		int						currentItem;

		bool					mouseExit(XButtonEvent *e) {printf("exit list\n");return(true);};
		bool					mouseEnter(XButtonEvent *e) {printf("enter list\n");return(true);};
		bool					mouseDown(XButtonEvent *e) {return(true);};
		bool					mouseUp(XButtonEvent *e) {return(true);};
		unsigned				listCnt;

		//int						useThumbNail=NOTHUMB;
		listLabelStruct			**labelData=NULL;
		bool					freeCairoImages=true;
	private:
		LFSTK_scrollBarClass	*scrollBar;
		unsigned				listOffset;
		unsigned				maxShowing;
		listData				*data;

		LFSTK_buttonClass		**labels;		

		void					freeList(void);
		void 					setCurrentItem(int item);
		void					setNavSensitive(void);

		static bool				select(void *object,void* userdata);
		static bool				scrollCB(void *object,void* userdata);
};

#endif
