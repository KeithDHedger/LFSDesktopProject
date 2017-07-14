/*
 *
 * ©K. D. Hedger. Sun 11 Sep 19:09:02 BST 2016 kdhedger68713@gmail.com

 * This file (LFSTKListGadget.cpp) is part of LFSToolKit.

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
 
#include <unistd.h>
#include <stdlib.h>

#include "lfstk/LFSTKGlobals.h"

LFSTK_listGadgetClass::~LFSTK_listGadgetClass()
{
	delete this->data;
	delete this->scrollData;
	if(this->listStrings!=NULL)
		{
			for(int j=0;j<this->listCnt;j++)
				free(this->listStrings[j]);
			delete this->listStrings;
		}
	if(this->listImages!=NULL)
		{
			for(int j=0;j<this->listImageCnt;j++)
				free(this->listImages[j]);
			delete this->listImages;
		}
}

bool LFSTK_listGadgetClass::select(void *object,void* userdata)
{
	LFSTK_listGadgetClass	*list;
	LFSTK_labelClass		*label;

	listData				*d=(listData*)userdata;
	unsigned				data;

	label=static_cast<LFSTK_labelClass*>(object);

	list=static_cast<LFSTK_listGadgetClass*>(d->mainObject);
	data=d->userData;
	list->setCurrentItem(data);
	for(int j=0;j<list->maxShowing;j++)
		{
			list->labels[j]->LFSTK_setColourName(NORMALCOLOUR,"white");
			list->labels[j]->LFSTK_clearWindow();
		}
	label->LFSTK_setColourName(NORMALCOLOUR,label->LFSTK_getColourName(ACTIVECOLOUR));
	label->LFSTK_clearWindow();

	if(list->callback.releaseCallback!=NULL)
		return(list->callback.releaseCallback(list,list->callback.userData));

	return(true);
}

void LFSTK_listGadgetClass::setCurrentItem(int item)
{
	this->currentItem=item;
}

/**
* Get the current list number.
*
* \return int currenty selected list item.
*/
int LFSTK_listGadgetClass::LFSTK_getCurrentListItem(void)
{
	return(this->currentItem);
}

/**
* Get the string from the list number.
*
* \param listnum Number in list to get.
* \return const char* to string.
* \note String should not be freed.
* If listnum<0 or >than max items in list then current item returned.
* will always return a string.
*/
const char* LFSTK_listGadgetClass::LFSTK_getListString(int listnum)
{
	if((listnum>0) && (listnum<this->listCnt))
		return(listStrings[listnum]);
	else
		{
			if(this->currentItem<0)
				this->currentItem=0;
			if(this->currentItem>=this->listCnt)
				this->currentItem=this->listCnt-1;
			return(listStrings[this->currentItem]);
		}
}

/**
* Set new image list.
*
* \param list char** list of strings.
* \param numitems Number of items in list.
* \note Strings are copied so list can be disposed of by caller.
* \note image list MUST be set before setting list.
*/
void LFSTK_listGadgetClass::LFSTK_setImageList(char **list,unsigned numitems)
{
	if(this->listImages!=NULL)
		{
			for(int j=0;j<this->listImageCnt;j++)
				if(this->listImages[j]!=NULL)
					free(this->listImages[j]);
			delete this->listImages;
		}

	this->listImageCnt=numitems;
	this->listImages=new char*[numitems];
	for(int j=0;j<this->listImageCnt;j++)
		if(list[j]!=NULL)
			this->listImages[j]=strdup(list[j]);
		else
			this->listImages[j]=NULL;
}

/**
* Set new list.
*
* \param list char** list of strings.
* \param numitems Number of items in list.
* \note strings are copied so list can be disposed of by caller.
*/
void LFSTK_listGadgetClass::LFSTK_setList(char **list,unsigned numitems)
{
	geometryStruct	geom;
	int orient=LEFT;

	if(this->listStrings!=NULL)
		{
			for(int j=0;j<this->listCnt;j++)
				if(this->listStrings[j]!=NULL)
					free(this->listStrings[j]);
			delete this->listStrings;
		}

	if(this->listImages!=NULL)
		orient=RIGHT;

	this->listStrings=new char*[numitems];
	this->listCnt=numitems;
	for(int j=0;j<this->listCnt;j++)
		this->listStrings[j]=strdup(list[j]);
			
	this->listOffset=0;
	this->currentItem=0;

	for(int j=0;j<this->maxShowing;j++)
		{
			this->data[j].userData=j;
			if(j<this->listCnt)
				{
					this->labels[j]->LFSTK_setLabel(this->listStrings[j]);
					this->labels[j]->LFSTK_setActive(true);
					this->labels[j]->LFSTK_setLabelOriention(orient);

					if((this->listImages!=NULL) && (this->listImages[j]!=NULL))
						{
							this->labels[j]->LFSTK_getGeom(&geom);
							this->labels[j]->LFSTK_setImageFromPath(this->listImages[j],LEFT);
						}
				}
			else
				{
					this->labels[j]->LFSTK_setLabel("");
					this->labels[j]->LFSTK_setActive(false);
					this->labels[j]->LFSTK_setImageFromPath(NULL,LEFT);
				}
			this->labels[j]->LFSTK_clearWindow();
		}

	this->setNavSensitive();
}

bool LFSTK_listGadgetClass::scrollCB(void *object,void* userdata)
{
	LFSTK_listGadgetClass	*list;
	listData				*d=(listData*)userdata;
	unsigned				data;

	int						start;
	list=static_cast<LFSTK_listGadgetClass*>(d->mainObject);
	data=d->userData;

	start=list->listOffset;
	switch(data)
		{
			case LUP:
				start-=list->maxShowing;
				if(start<0)
					list->listOffset=0;
				else
					list->listOffset=start;
				
				break;
			case LDOWN:
				start+=list->maxShowing;
				
				if((start+list->maxShowing) > (list->listCnt-list->maxShowing))
					list->listOffset=list->listCnt-list->maxShowing;
				else
					list->listOffset=start;
				break;

			case LHOME:
				list->listOffset=0;
				break;
			case LEND:
				list->listOffset=list->listCnt-list->maxShowing;
				break;
		}

	list->setNavSensitive();

	for(int j=0;j<list->maxShowing;j++)
		{
			list->labels[j]->LFSTK_setLabel(list->listStrings[j+list->listOffset]);
			list->labels[j]->LFSTK_setImageFromPath(list->listImages[j+list->listOffset],LEFT);
			list->data[j].userData=j+list->listOffset;
			list->labels[j]->LFSTK_clearWindow();
		}
	return(true);
}

void LFSTK_listGadgetClass::setNavSensitive(void)
{
	int fing;

	this->buttonDown->LFSTK_setActive(true);
	this->buttonUp->LFSTK_setActive(true);
	this->buttonHome->LFSTK_setActive(true);
	this->buttonEnd->LFSTK_setActive(true);

	if(this->listOffset<=0)
		{
			this->buttonUp->LFSTK_setActive(false);
			this->buttonHome->LFSTK_setActive(false);
		}

	if(this->listOffset>=this->listCnt-this->maxShowing)
		{
			this->buttonDown->LFSTK_setActive(false);
			this->buttonEnd->LFSTK_setActive(false);
		}

	fing=this->currentItem-this->listOffset;
	for(int j=0;j<this->maxShowing;j++)
		{
			if(j<this->listCnt)
				{
					this->labels[j]->LFSTK_setColourName(NORMALCOLOUR,"white");
					this->labels[j]->LFSTK_clearWindow();
				}
		}

	if((fing>=0) && (fing<this->maxShowing))
		{
			this->labels[fing]->LFSTK_setColourName(NORMALCOLOUR,this->labels[fing]->LFSTK_getColourName(ACTIVECOLOUR));
			this->labels[fing]->LFSTK_clearWindow();
		}

	this->buttonDown->LFSTK_clearWindow();
	this->buttonUp->LFSTK_clearWindow();
	this->buttonHome->LFSTK_clearWindow();
	this->buttonEnd->LFSTK_clearWindow();
}

/**
* Main button constructor.
*
* \param parentwc Main parent window class.
* \param label Displayed name.
* \param x X pos.
* \param y Y pos.
* \param w Width.
* \param h Height.
* \param gravity Button gravity.
* \param newlist Ptr->List of strings (char**).
* \param cnt Number of items in list.
* \note newlist owned by caller dont free till after object destroyed ( TODO ).
*/
LFSTK_listGadgetClass::LFSTK_listGadgetClass(LFSTK_windowClass *parentwc,const char *label,int x,int y,int w,int h,int gravity,char **newlist,unsigned cnt)
{
	unsigned				sx;
	unsigned				sy;

	XSetWindowAttributes	wa;
	mappedListener			*ml=new mappedListener;
	this->LFSTK_setCommon(parentwc,label,x-1,y-1,w+2,h+2,gravity);

	wa.win_gravity=gravity;
	this->window=XCreateWindow(this->display,this->parent,x-1,y-1,w+2,h+2,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
	XSelectInput(this->display,this->window,ButtonReleaseMask | ButtonPressMask | ExposureMask | EnterWindowMask | LeaveWindowMask);

	ml->function=&LFSTK_lib::LFSTK_gadgetEvent;
	ml->gadget=this;
	ml->type=BUTTONGADGET;
	this->wc->LFSTK_addMappedListener(this->window,ml);

	this->currentItem=0;
	this->scrollData=new listData[4];
	this->listOffset=0;
	this->listStrings=NULL;
	this->listImages=NULL;
	this->listCnt=0;
	this->listImageCnt=0;

	sx=x;
	sy=y;
	this->maxShowing=(h/LABELHITE);
	this->labels=new LFSTK_buttonClass*[this->maxShowing];
	this->data=new listData[this->maxShowing];

	for(int j=0;j<this->maxShowing;j++)
		{
			this->labels[j]=new LFSTK_buttonClass(parentwc,"",sx,sy,w,LABELHITE,gravity);
			this->labels[j]->LFSTK_setActive(false);
			this->labels[j]->LFSTK_setStyle(BEVELNONE);
			this->labels[j]->LFSTK_setTile(NULL,0);
			this->labels[j]->LFSTK_setColourName(NORMALCOLOUR,"white");
			this->labels[j]->LFSTK_setColourName(INACTIVECOLOUR,"white");
			this->labels[j]->LFSTK_setLabelAutoColour(true);
			this->labels[j]->LFSTK_setCallBack(NULL,select,LISTDATA(j));
			this->data[j].mainObject=this;
			this->data[j].userData=j;
			sy+=LABELHITE;
		}

	if(newlist!=NULL)
		this->LFSTK_setList(newlist,cnt);

//navigate
//line up/down
			this->buttonUp=new LFSTK_buttonClass(parentwc,"↑",this->gadgetGeom.x+this->gadgetGeom.w+LGAP,this->gadgetGeom.y,NAVBUTTONSIZE,NAVBUTTONSIZE,NorthEastGravity);
			this->scrollData[LUP].mainObject=this;
			this->scrollData[LUP].userData=LUP;
			buttonUp->LFSTK_setCallBack(NULL,scrollCB,SCROLLDATA(LUP));
			buttonDown=new LFSTK_buttonClass(parentwc,"↓",this->gadgetGeom.x+this->gadgetGeom.w+LGAP,this->gadgetGeom.y+this->gadgetGeom.h-NAVBUTTONSIZE,NAVBUTTONSIZE,NAVBUTTONSIZE,NorthEastGravity);
			this->scrollData[LDOWN].mainObject=this;
			this->scrollData[LDOWN].userData=LDOWN;
			buttonDown->LFSTK_setCallBack(NULL,scrollCB,SCROLLDATA(LDOWN));
//page/updown
			this->buttonHome=new LFSTK_buttonClass(parentwc,"⇤",this->gadgetGeom.x+this->gadgetGeom.w+LGAP,this->gadgetGeom.y+NAVBUTTONSIZE+LGAP,NAVBUTTONSIZE,NAVBUTTONSIZE,NorthEastGravity);
			this->scrollData[LHOME].mainObject=this;
			this->scrollData[LHOME].userData=LHOME;
			buttonHome->LFSTK_setCallBack(NULL,scrollCB,SCROLLDATA(LHOME));
			buttonEnd=new LFSTK_buttonClass(parentwc,"⇥",this->gadgetGeom.x+this->gadgetGeom.w+LGAP,this->gadgetGeom.y+this->gadgetGeom.h-(NAVBUTTONSIZE*2)-LGAP,NAVBUTTONSIZE,NAVBUTTONSIZE,NorthEastGravity);
			this->scrollData[LEND].mainObject=this;
			this->scrollData[LEND].userData=LEND;
			buttonEnd->LFSTK_setCallBack(NULL,scrollCB,SCROLLDATA(LEND));

	this->style=BEVELIN;
	if(newlist!=NULL)
		this->LFSTK_setList(newlist,cnt);
}


