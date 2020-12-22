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

bool LFSTK_listGadgetClass::setFocusToList(void *object,void* userdata)
{
	LFSTK_listGadgetClass	*list;
	listData				*d=(listData*)userdata;	

	list=static_cast<LFSTK_listGadgetClass*>(d->mainObject);
	XSetInputFocus(list->display,list->window,RevertToParent,CurrentTime);
	return(true);
}

bool LFSTK_listGadgetClass::select(void *object,void* userdata)
{
	LFSTK_listGadgetClass	*list;
	LFSTK_buttonClass		*label;
	listData				*d=(listData*)userdata;
	unsigned				datax;

	label=static_cast<LFSTK_buttonClass*>(object);
	list=static_cast<LFSTK_listGadgetClass*>(d->mainObject);
	datax=d->userData;
	list->setCurrentItem(datax);

	switch(list->labels[list->currentItem-list->listOffset]->xEvent->xbutton.button)
		{
			case Button5:
				list->scrollBar->LFSTK_scrollByLine(false);
				return(true);
				break;
			case Button4:
				list->scrollBar->LFSTK_scrollByLine(true);
				return(true);
				break;
		}

	for(int j=0;j<list->maxShowing;j++)
		{
			list->labels[j]->LFSTK_setColourName(NORMALCOLOUR,list->wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPELISTTROUGHCOLOUR));
			list->labels[j]->LFSTK_clearWindow();
		}
	label->LFSTK_setColourName(NORMALCOLOUR,label->LFSTK_getColourName(ACTIVECOLOUR));
	label->LFSTK_clearWindow();

	list->isDoubleClick=label->isDoubleClick;

	if(list->runCallback(MOUSERELEASECB)==true)
		return(list->mouseCB.releaseCallback(list,list->mouseCB.userData));

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
	size_t			linelen=0;
	ssize_t			read=0;
	int				linecnt=0;
	char			*lines=NULL;
	long			userdata=0;

	this->freeList();
	listLabelStruct ls;

	if(filepath!=NULL)
		{
			lines=this->wc->globalLib->LFSTK_oneLiner("wc -l %s",filepath);
			linecnt=atoi(lines);
			free(lines);
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
										ls.imageType=NOTHUMB;
										if(includeempty==true)
											{
												ls.label=strdup(buffer);
												ls.userData=(void*)userdata;
												this->LFSTK_appendToList(ls);
											}
										else
											{
												if(strlen(buffer)>0)
													{
														ls.label=strdup(buffer);
														ls.userData=(void*)userdata;
														this->LFSTK_appendToList(ls);
													}
											}
									}
								free(buffer);
								userdata++;
						}
					fclose(file);
					this->LFSTK_updateList();
				}
		}
}

bool LFSTK_listGadgetClass::scrollListCB(void *object,void* userdata)
{
	return(true);
}

/**
* Update new list.
*/
void LFSTK_listGadgetClass::LFSTK_updateList(void)
{
	int orient=MENU;

	this->listOffset=0;
	this->currentItem=0;

	if(this->listDataArray->size()>maxShowing)
		this->scrollBar->LFSTK_setScale(0,this->listDataArray->size()-this->maxShowing);
	else
		this->scrollBar->LFSTK_setScale(0,0);
	this->listCntNew=this->listDataArray->size();
	for(int j=0;j<this->maxShowing;j++)
		{
			this->data[j].userData=j;
			if(j<this->listCntNew)
				{
					this->labels[j]->LFSTK_setLabel(this->listDataArray->at(j).label);
					this->labels[j]->LFSTK_setActive(true);
					if(this->listDataArray->at(j).imageType==NOTHUMB)
						this->labels[j]->LFSTK_setLabelGravity(LEFT);
					else
						this->labels[j]->LFSTK_setLabelGravity(MENU);
					this->listDataArray->at(j).listPos=j;

					if(this->listDataArray->at(j).imageType==CAIROTHUMB)
						this->labels[j]->LFSTK_setImageFromSurface(this->listDataArray->at(j).data.surface,MENU,true);

					if(this->listDataArray->at(j).imageType==FILETHUMB)
						this->labels[j]->LFSTK_setImageFromPath(this->listDataArray->at(j).data.imagePath,MENU,true);
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
	this->scrollBar->LFSTK_setValue(0);
	this->wc->LFSTK_clearWindow(true);
	this->currentItem=0;
	this->setNavSensitive();
}

/*
 * Private callback.
*/
bool LFSTK_listGadgetClass::scrollCB(void *object,void* userdata)
{
	LFSTK_listGadgetClass	*list;

	if((object==NULL) || (userdata==NULL))
		return(true);

	LFSTK_scrollBarClass	*sb=static_cast<LFSTK_scrollBarClass*>(object);
	list=static_cast<LFSTK_listGadgetClass*>(userdata);

	list->listOffset=sb->LFSTK_getValue();
	for(int j=0;j<list->maxShowing;j++)
		{
			if(j+list->listOffset<list->listCntNew)
				{
					list->labels[j]->LFSTK_setLabel(list->listDataArray->at(j+list->listOffset).label,false);
					if(list->listDataArray->at(j+list->listOffset).imageType==CAIROTHUMB)
						list->labels[j]->LFSTK_setImageFromSurface(list->listDataArray->at(j+list->listOffset).data.surface,MENU,true);

					if(list->listDataArray->at(j+list->listOffset).imageType==FILETHUMB)
						list->labels[j]->LFSTK_setImageFromPath(list->listDataArray->at(j+list->listOffset).data.imagePath,MENU,true);

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
* Main Gadget constructor.
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

	this->LFSTK_setCommon(parentwc,"",x-1,y-1,adjwidth+2,h+2,gravity);

	wa.win_gravity=gravity;
	wa.save_under=true;
	this->window=XCreateWindow(this->display,this->parent,x,y,adjwidth,h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
	this->gc=XCreateGC(this->display,this->window,0,NULL);

	this->LFSTK_setFontString(this->monoFontString);
	this->wc->globalLib->LFSTK_setCairoSurface(this->display,this->window,this->visual,&this->sfc,&this->cr,adjwidth,h);
	this->LFSTK_setCairoFontData();

	XSelectInput(this->display,this->window,this->gadgetEventMask);

	this->ml->function=&LFSTK_lib::LFSTK_gadgetEvent;
	this->ml->gadget=this;
	this->ml->type=LISTGADGET;
	this->wc->LFSTK_addMappedListener(this->window,ml);

	this->currentItem=0;
	this->listOffset=0;

	sx=x+1;
	sy=y+1;
	this->maxShowing=((h)/LABELHITE);
	this->labels=new LFSTK_buttonClass*[this->maxShowing];
	this->data=new listData[this->maxShowing];
	this->listDataArray=new std::vector <listLabelStruct>;

	for(int j=0;j<this->maxShowing;j++)
		{
			this->labels[j]=new LFSTK_buttonClass(parentwc,"",sx,sy,adjwidth-2,LABELHITE-4,gravity);
			this->labels[j]->LFSTK_setLabelAutoColour(true);
			this->labels[j]->LFSTK_reloadColours();
			this->labels[j]->LFSTK_setColourName(NORMALCOLOUR,this->wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPELISTTROUGHCOLOUR));
			this->labels[j]->LFSTK_setColourName(INACTIVECOLOUR,this->wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPELISTTROUGHCOLOUR));

			this->labels[j]->gadgetDetails.colour=&this->labels[j]->colourNames[NORMALCOLOUR];
			this->labels[j]->gadgetDetails.state=NORMALCOLOUR;
			this->labels[j]->gadgetDetails.bevel=BEVELNONE;
			
			this->labels[j]->LFSTK_setActive(false);
			this->labels[j]->LFSTK_setStyle(BEVELNONE);
			this->labels[j]->LFSTK_setTile(NULL,0);

			this->labels[j]->LFSTK_setMouseCallBack(setFocusToList,select,LISTDATA(j));
			this->labels[j]->LFSTK_setKeyCallBack(setFocusToList,select,LISTDATA(j));

			this->labels[j]->LFSTK_setFontString(this->monoFontString);
			this->labels[j]->LFSTK_setFontColourName(NORMALCOLOUR,"black",false);
			this->labels[j]->LFSTK_setFontColourName(INACTIVECOLOUR,"black",false);
			this->labels[j]->LFSTK_setCairoFontData();

			this->data[j].mainObject=this;
			this->data[j].userData=j;
			sy+=LABELHITE;
		}

//navigate
	this->scrollBar=new LFSTK_scrollBarClass(this->wc,true,x+w-SCROLLBARWIDTH-2,y,SCROLLBARWIDTH,h,gravity);
	this->scrollBar->LFSTK_setScale(1,1);
	this->scrollBar->LFSTK_setLineScroll(1);
	this->scrollBar->LFSTK_setPageScroll(maxShowing-1);
	this->scrollBar->LFSTK_setMouseCallBack(NULL,scrollCB,this);

	this->style=BEVELIN;

	this->LFSTK_setColourName(NORMALCOLOUR,this->wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPELISTTROUGHCOLOUR));
	this->LFSTK_setColourName(INACTIVECOLOUR,this->wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPELISTTROUGHCOLOUR));
	this->gadgetDetails={&this->colourNames[NORMALCOLOUR],BEVELIN,NOINDICATOR,NORMALCOLOUR,0,true,{0,0,adjwidth,h},{0,0,0,0},false,false,false};
	this->clearBox(&this->gadgetDetails);
	//DEBUGFUNC("%r",this->gadgetGeom);
}

/**
* Private function.
* Free list data.
*/
void LFSTK_listGadgetClass::freeList(void)
{
	for(int j=0;j<this->listDataArray->size();j++)
		{
			free(this->listDataArray->at(j).label);
			if((this->listDataArray->at(j).imageType==CAIROTHUMB) && (this->listDataArray->at(j).data.surface!=NULL) && (this->freeCairoImages==true))
				cairo_surface_destroy(this->listDataArray->at(j).data.surface);
			if((this->listDataArray->at(j).imageType==FILETHUMB) && (this->listDataArray->at(j).data.imagePath!=NULL))
				free(this->listDataArray->at(j).data.imagePath);
		}
			this->listDataArray->clear();
}

/**
* Free list data.
*/
void LFSTK_listGadgetClass::LFSTK_freeList(void)
{
	this->freeList();
}

/**
* Get selected label.
* \return const char *label;
*/
const char	*LFSTK_listGadgetClass::LFSTK_getSelectedLabel(void)
{
	return(this->listDataArray->at(this->currentItem).label);
}

/**
* Get label at index.
* \param int Index.
* \return const char *label;
*/
const char	*LFSTK_listGadgetClass::LFSTK_getLabelAtIndex(int index)
{
	if((index>this->listDataArray->size()-1) || (index<0))
		return(NULL);

	return(this->listDataArray->at(index).label);
}

/**
* Private scroll if bewtween list items.
* \return true;
*/
bool LFSTK_listGadgetClass::mouseUp(XButtonEvent *e)
{
	XSetInputFocus(this->display,this->window,RevertToParent,CurrentTime);
	if(e->button==Button5)
		this->scrollBar->LFSTK_scrollByLine(false);
	if(e->button==Button4)
		this->scrollBar->LFSTK_scrollByLine(true);
	return(true);
}

/**
* Private scroll if bewtween list items.
* \return true;
*/
bool LFSTK_listGadgetClass::keyRelease(XKeyEvent *e)
{
	char	c[255];
	KeySym	keysym_return;
	XLookupString(e,(char*)&c,255,&keysym_return,NULL);

	switch(keysym_return)
		{
			case XK_Prior:
				this->scrollBar->LFSTK_scrollByPage(true);
				break;
			case XK_Next:
				this->scrollBar->LFSTK_scrollByPage(false);
				break;
			case XK_Up:
				this->scrollBar->LFSTK_scrollByLine(true);
				break;
			case XK_Down:
				this->scrollBar->LFSTK_scrollByLine(false);
				break;
		}

	return(true);
}

/**
* Private scroll by key.
* \return true;
*/
bool LFSTK_listGadgetClass::selectKey(void *object,void* userdata)
{
	listData				*d=(listData*)userdata;
	LFSTK_listGadgetClass	*list;
	char					c[255];
	KeySym					keysym_return;

	LFSTK_buttonClass	*button=static_cast<LFSTK_buttonClass*>(object);
	list=dynamic_cast<LFSTK_listGadgetClass*>(d->mainObject);

	if(button==NULL)
		return(true);

	if(button->keyEvent!=NULL)
		{
			XLookupString(button->keyEvent,(char*)&c,255,&keysym_return,NULL);
			switch(keysym_return)
				{
					case XK_Prior:
						list->scrollBar->LFSTK_scrollByPage(true);
						break;
					case XK_Next:
						list->scrollBar->LFSTK_scrollByPage(false);
						break;
					case XK_Up:
						list->scrollBar->LFSTK_scrollByLine(true);
						break;
					case XK_Down:
						list->scrollBar->LFSTK_scrollByLine(false);
						break;

				}
		}

	return(true);
}

/**
* Append data to current list.
* \param data listLabelStruct
*/
void LFSTK_listGadgetClass::LFSTK_appendToList(listLabelStruct data)
{
	this->listDataArray->push_back(data);
}

/**
* Select list by index.
* \param int index
* \note index<0 selects 1st item, index>max items selects last item.
*/
void LFSTK_listGadgetClass::LFSTK_selectByIndex(int index)
{
	this->currentItem=index;
	this->scrollBar->LFSTK_setValue(index);
}

/**
* Find item by label and ( optionally select );
* \param const char *needle.
* \param bool select ( default =true ).
*/
int LFSTK_listGadgetClass::LFSTK_findByLabel(const char *needle,bool select)
{
	for(int j=0;j<this->listDataArray->size();j++)
		{
			if(strcmp(this->listDataArray->at(j).label,needle)==0)
				{
					if(select==true)
						this->LFSTK_selectByIndex(j);
					return(j);
				}
		}
	return(-1);
}








