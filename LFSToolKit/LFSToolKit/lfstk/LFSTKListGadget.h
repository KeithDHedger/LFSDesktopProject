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
	std::string		label;
	int				imageType;
	void				*userData;
	int				listPos;
	std::string		imagePath;
	cairo_surface_t	*surface;
};

/**
 * \brief List class for LFSToolKit.
*/
class LFSTK_listGadgetClass  : public  LFSTK_gadgetClass
{
	public:
		~LFSTK_listGadgetClass();
		LFSTK_listGadgetClass(LFSTK_windowClass *parentwc,const char *label,int x,int y,unsigned w,unsigned h,int gravity=NorthWestGravity);

		int								LFSTK_getCurrentListItem(void);
		void								LFSTK_updateList(void);
		void								LFSTK_setListFromFile(const char *filepath,bool includeempty);
		const char						*LFSTK_getSelectedLabel(void);
		const char						*LFSTK_getLabelAtIndex(int index);
		void								LFSTK_freeList(void);
		void								LFSTK_appendToList(listLabelStruct data);
		void								LFSTK_selectByIndex(int index);
		int								LFSTK_findByLabel(const char *needle,bool select=true);
		
		bool								mouseExit(XButtonEvent *e) {return(true);};
		bool								mouseEnter(XButtonEvent *e) {XSetInputFocus(this->wc->app->display,this->window,RevertToParent,CurrentTime);return(true);};
		bool								mouseUp(XButtonEvent *e);
		bool								keyRelease(XKeyEvent *e);

		void								LFSTK_resetListHeight(int newheight);
		int								LFSTK_getListMaxWidth(void);

		unsigned							listCnt=0;
		int								currentItem=0;

		void								LFSTK_setListItemsColours(gadgetColourType type,std::string normal,std::string prelight,std::string active,std::string inactive);

		std::vector <LFSTK_buttonClass*>	*labelsArray;
		std::vector <listLabelStruct>	*listDataArray;

		bool								freeCairoImages=true;
		unsigned							maxShowing;
		LFSTK_scrollBarClass				*scrollBar=NULL;
	private:
		unsigned							listOffset=0;
		listData							*data=NULL;

		void								freeList(void);
		void 							setCurrentItem(int item);
		void								setNavSensitive(void);

		static bool						select(void *object,void* userdata);
		static bool						setFocusToList(void *object,void* userdata);
		static bool						selectKey(void *object,void* userdata);
		static bool						scrollCB(void *object,void* userdata);
		static bool						scrollListCB(void *object,void* userdata);
};

#endif
