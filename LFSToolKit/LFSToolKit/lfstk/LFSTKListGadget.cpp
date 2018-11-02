/*
 *
 * ©K. D. Hedger. Sun 11 Sep 19:09:02 BST 2016 keithdhedger@gmail.com

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
	delete[] this->data;
	this->freeList();
	delete[] this->labels;
}

bool LFSTK_listGadgetClass::select(void *object,void* userdata)
{
	LFSTK_listGadgetClass	*list;
	LFSTK_labelClass		*label;
	listData				*d=(listData*)userdata;
	unsigned				datax;

	label=static_cast<LFSTK_labelClass*>(object);
	list=static_cast<LFSTK_listGadgetClass*>(d->mainObject);
	datax=d->userData;
	list->setCurrentItem(datax);
	for(int j=0;j<list->maxShowing;j++)
		{
			list->labels[j]->LFSTK_setColourName(NORMALCOLOUR,"white");
			list->labels[j]->LFSTK_clearWindow();
		}
	label->LFSTK_setColourName(NORMALCOLOUR,label->LFSTK_getColourName(ACTIVECOLOUR));
	label->LFSTK_clearWindow();

	list->isDoubleClick=label->isDoubleClick;
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
* Set new list from contents of file.
*
* \param list const char * file path.
* \note Passing NULL dletes current list.
*/
void LFSTK_listGadgetClass::LFSTK_setListFromFile(const char *filepath,bool includeempty)
{
	FILE			*file=NULL;
	char			*buffer;
	int				cnt=0;
	size_t			linelen=0;
	ssize_t			read=0;
	int				linecnt=0;
	char			*lines=NULL;
	listLabelStruct	**labelLst=NULL;
	char			**strings=NULL;

	this->freeList();

	if(filepath!=NULL)
		{
			lines=this->wc->globalLib->LFSTK_oneLiner("wc -l %s",filepath);
			linecnt=atoi(lines);
			free(lines);
			strings=new char*[linecnt];
			file=fopen(filepath,"r");
			if(file!=NULL)
				{
					while(!feof(file))
						{
							buffer=NULL;
							linelen=0;
							read=getline(&buffer,&linelen,file);
								if(read>0)
									{
										buffer[strlen(buffer)-1]=0;
										if(includeempty==true)
											{
												strings[cnt++]=strdup(buffer);
											}
										else
											{
												if(strlen(buffer)>0)
													strings[cnt++]=strdup(buffer);
											}
									}
								free(buffer);
						}
					fclose(file);
					labelLst=new listLabelStruct*[cnt];
					for(int j=0;j<cnt;j++)
						{
							labelLst[j]=new listLabelStruct;
							labelLst[j]->label=strings[j];
							labelLst[j]->imageType=NOTHUMB;
							//labelLst[j]->imageType=FILETHUMB;
							//labelLst[j]->data.imagePath=strdup("/media/LinuxData/Development64/Projects/LFSDesktopProject/LFSToolKit/LFSToolKit/resources/pixmaps/lfstkpngs/documents.png");
						}
					this->LFSTK_setList(labelLst,cnt);
				}
			delete[] strings;
		}
}

/**
* Set new list.
*
* \param listLabelStruct**.
* \param numitems Number of items in list.
* \note list takes ownership of listLabelStruct**, DO NOT FREE.
*/
void LFSTK_listGadgetClass::LFSTK_setList(listLabelStruct **list,unsigned numitems)
{
	int orient=MENU;

	this->freeList();
	this->labelData=list;
	this->listOffset=0;
	this->currentItem=0;

	if(numitems>maxShowing)
		this->scrollBar->LFSTK_setScale(0,numitems-this->maxShowing);
	else
		this->scrollBar->LFSTK_setScale(0,0);

	this->listCnt=numitems;
	for(int j=0;j<this->maxShowing;j++)
		{
			this->data[j].userData=j;
			if(j<this->listCnt)
				{
					this->labels[j]->LFSTK_setLabel(this->labelData[j]->label);
					this->labels[j]->LFSTK_setActive(true);
					if(this->labelData[j]->imageType==NOTHUMB)
						this->labels[j]->LFSTK_setLabelGravity(LEFT);
					else
						this->labels[j]->LFSTK_setLabelGravity(MENU);
					this->labelData[j]->listPos=j;

					if(this->labelData[j]->imageType==CAIROTHUMB)
						this->labels[j]->LFSTK_setImageFromSurface(this->labelData[j]->data.surface,MENU,true);

					if(this->labelData[j]->imageType==FILETHUMB)
						this->labels[j]->LFSTK_setImageFromPath(this->labelData[j]->data.imagePath,MENU,true);
				}
			else
				{
					this->labels[j]->LFSTK_setLabel("");
					this->labels[j]->LFSTK_setActive(false);
					this->labels[j]->LFSTK_setImageFromPath(NULL,MENU,true);
				}
			this->labels[j]->gadgetDetails.colour=&this->labels[j]->colourNames[NORMALCOLOUR];
			this->labels[j]->gadgetDetails.state=NORMALCOLOUR;
			this->labels[j]->LFSTK_clearWindow();
		}
	this->wc->LFSTK_clearWindow(true);
	this->currentItem=0;
	this->setNavSensitive();

}

bool LFSTK_listGadgetClass::scrollCB(void *object,void* userdata)
{
	LFSTK_listGadgetClass	*list;

	if((object==NULL) || (userdata==NULL))
		return(true);

	LFSTK_scrollBarClass	*sb=static_cast<LFSTK_scrollBarClass*>(object);
	list=static_cast<LFSTK_listGadgetClass*>(userdata);

	//list->setNavSensitive();

	list->listOffset=sb->value;
	for(int j=0;j<list->maxShowing;j++)
		{
			if(j+list->listOffset<list->listCnt)
				{
					list->labels[j]->LFSTK_setLabel(list->labelData[j+list->listOffset]->label,false);
					if(list->labelData[j+list->listOffset]->imageType==CAIROTHUMB)
						list->labels[j]->LFSTK_setImageFromSurface(list->labelData[j+list->listOffset]->data.surface,MENU,true);

					if(list->labelData[j+list->listOffset]->imageType==FILETHUMB)
						list->labels[j]->LFSTK_setImageFromPath(list->labelData[j+list->listOffset]->data.imagePath,MENU,true);

					list->data[j].userData=j+list->listOffset;
				}
		}

	for(int j=0;j<list->maxShowing;j++)
		list->labels[j]->LFSTK_clearWindow();
	return(true);
}

void LFSTK_listGadgetClass::setNavSensitive(void)
{
//TODO//
//	XFlush(this->display);
//	XSync(this->display,false);
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
LFSTK_listGadgetClass::LFSTK_listGadgetClass(LFSTK_windowClass *parentwc,const char *label,int x,int y,unsigned w,unsigned h,int gravity,char **newlist,unsigned cnt)
{
	unsigned				sx;
	unsigned				sy;
	XSetWindowAttributes	wa;
	unsigned int			adjwidth=w-SCROLLBARWIDTH-2;

	this->LFSTK_setCommon(parentwc,label,x-1,y-1,adjwidth+2,h+2,gravity);

	wa.win_gravity=gravity;
	wa.save_under=true;
	this->window=XCreateWindow(this->display,this->parent,x,y,adjwidth,h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
	this->gc=XCreateGC(this->display,this->window,0,NULL);

	this->LFSTK_setFontString(this->monoFontString);
	this->wc->globalLib->LFSTK_setCairoSurface(this->display,this->window,this->visual,&this->sfc,&this->cr,adjwidth,h);
	this->LFSTK_setCairoFontData();

	XSelectInput(this->display,this->window,ButtonReleaseMask | ButtonPressMask | ExposureMask | EnterWindowMask | LeaveWindowMask);

	this->ml->function=&LFSTK_lib::LFSTK_gadgetEvent;
	this->ml->gadget=this;
	this->ml->type=LISTGADGET;
	this->wc->LFSTK_addMappedListener(this->window,ml);

	this->currentItem=0;
	this->listOffset=0;
	this->listCnt=0;

	sx=x+1;
	sy=y+1;
	this->maxShowing=((h)/LABELHITE);
	this->labels=new LFSTK_buttonClass*[this->maxShowing];
	this->data=new listData[this->maxShowing];

	for(int j=0;j<this->maxShowing;j++)
		{
			this->labels[j]=new LFSTK_buttonClass(parentwc,"",sx,sy,adjwidth-2,LABELHITE-4,gravity);
			this->labels[j]->LFSTK_setLabelAutoColour(true);
			this->labels[j]->LFSTK_reloadColours();
			this->labels[j]->LFSTK_setColourName(NORMALCOLOUR,"white");
			this->labels[j]->LFSTK_setColourName(INACTIVECOLOUR,"white");

			this->labels[j]->gadgetDetails.colour=&this->labels[j]->colourNames[NORMALCOLOUR];
			this->labels[j]->gadgetDetails.state=NORMALCOLOUR;
			this->labels[j]->gadgetDetails.bevel=BEVELNONE;
			
			this->labels[j]->LFSTK_setActive(false);
			this->labels[j]->LFSTK_setStyle(BEVELNONE);
			this->labels[j]->LFSTK_setTile(NULL,0);
			this->labels[j]->LFSTK_setCallBack(NULL,select,LISTDATA(j));
			this->labels[j]->LFSTK_setFontString(this->monoFontString);
			this->labels[j]->LFSTK_setFontColourName(NORMALCOLOUR,"black",false);
			this->labels[j]->LFSTK_setFontColourName(INACTIVECOLOUR,"black",false);
			this->labels[j]->LFSTK_setCairoFontData();

			this->data[j].mainObject=this;
			this->data[j].userData=j;
			sy+=LABELHITE;
		}

//navigate
	this->scrollBar=new LFSTK_scrollBarClass(this->wc,true,x+w-SCROLLBARWIDTH,y,SCROLLBARWIDTH,h,gravity);
	this->scrollBar->LFSTK_setScale(1,1);
	this->scrollBar->LFSTK_setLineScroll(1);
	this->scrollBar->LFSTK_setCallBack(NULL,scrollCB,this);

	this->style=BEVELIN;

	this->LFSTK_setColourName(NORMALCOLOUR,"white");
	this->LFSTK_setColourName(INACTIVECOLOUR,"white");
	this->gadgetDetails={&this->colourNames[NORMALCOLOUR],BEVELIN,NOINDICATOR,NORMALCOLOUR,0,true,{0,0,adjwidth,h},{0,0,0,0},false};
	this->clearBox(&this->gadgetDetails);
}

/**
* Free list data.
*/
void LFSTK_listGadgetClass::freeList(void)
{
	if(this->labelData!=NULL)
		{
			for(int j=0;j<this->listCnt;j++)
				{
					if(this->labelData[j]!=NULL)
						{
							free(this->labelData[j]->label);
							if((this->labelData[j]->imageType==CAIROTHUMB) && (this->labelData[j]->data.surface!=NULL) && (this->freeCairoImages==true))
								cairo_surface_destroy(this->labelData[j]->data.surface);
							if((this->labelData[j]->imageType==FILETHUMB) && (this->labelData[j]->data.imagePath!=NULL))
								free(this->labelData[j]->data.imagePath);
							delete this->labelData[j];
						}
				}
			delete[] this->labelData;
			this->labelData=NULL;
		}
}

/**
* Get selected label.
* \return const char *label;
*/
const char	*LFSTK_listGadgetClass::LFSTK_getSelectedLabel(void)
{
	return(this->labelData[this->currentItem]->label);
}
