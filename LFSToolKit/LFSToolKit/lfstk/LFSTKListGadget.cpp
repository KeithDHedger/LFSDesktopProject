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
	for(int j=0;j<this->labelsArray->size();j++)
		this->labelsArray->at(j)->LFSTK_reParentWindow(this->wc->window,1,1);

	this->freeList();
	delete this->labelsArray;
	delete this->listDataArray;
	delete[] this->data;
	//XSync(this->wc->app->display,true);
}

bool LFSTK_listGadgetClass::setFocusToList(void *object,void* userdata)
{
	LFSTK_listGadgetClass	*list;
	listData				*d=(listData*)userdata;
	list=static_cast<LFSTK_listGadgetClass*>(d->mainObject);
	XSetInputFocus(list->wc->app->display,list->window,RevertToParent,CurrentTime);
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
	list->setCurrentItem(datax+list->listOffset);

	switch(list->labelsArray->at(datax)->xEvent->xbutton.button)
		{
			case Button5:
				list->scrollBar->LFSTK_scrollByLine(false);
				return(true);
				break;
			case Button4:
				list->scrollBar->LFSTK_scrollByLine(true);
				return(true);
				break;
			default:
				break;				
		}

	label->LFSTK_clearWindow();

	list->isDoubleClick=label->isDoubleClick;

	if(list->callBacks.validCallbacks & MOUSERELEASECB)
		return(list->callBacks.mouseReleaseCallback(list,list->callBacks.mouseUserData));

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
	//char			*lines=NULL;
	long			userdata=0;
	std::string		lines;
	this->freeList();
	listLabelStruct ls;

	if(filepath!=NULL)
		{
			lines=this->wc->app->globalLib->LFSTK_oneLiner("wc -l %s",filepath);//TODO//
			linecnt=std::stoi(lines,nullptr,10);
			//freeAndNull(&lines);
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
												ls.userData=USERDATA(userdata);
												this->LFSTK_appendToList(ls);
											}
										else
											{
												if(strlen(buffer)>0)
													{
														ls.label=strdup(buffer);
														ls.userData=USERDATA(userdata);
														this->LFSTK_appendToList(ls);
													}
											}
									}
								freeAndNull(&buffer);
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

	if(this->listDataArray->size()>maxShowing)
		this->scrollBar->LFSTK_setScale(0,this->listDataArray->size()-this->maxShowing);
	else
		this->scrollBar->LFSTK_setScale(0,0);
	this->listCnt=this->listDataArray->size();

	for(int j=0;j<this->labelsArray->size();j++)
		{
			this->labelsArray->at(j)->LFSTK_hideGadget();
			if((j+this->listOffset)<this->listDataArray->size())
				{
					if(j<this->maxShowing)
						{
							this->labelsArray->at(j)->LFSTK_setLabel(this->listDataArray->at(j+this->listOffset).label);
							this->labelsArray->at(j)->LFSTK_setStyle(BEVELNONE);
							this->labelsArray->at(j)->userData=USERDATA(j+this->listOffset);
							this->labelsArray->at(j)->LFSTK_setActive(true);
							if(this->listDataArray->at(j+this->listOffset).imageType==NOTHUMB)
								this->labelsArray->at(j)->LFSTK_setLabelGravity(LEFT);
							else
								this->labelsArray->at(j)->LFSTK_setLabelGravity(MENU);
							this->listDataArray->at(j+this->listOffset).listPos=j;
							if(this->listDataArray->at(j+this->listOffset).imageType==CAIROTHUMB)
								this->labelsArray->at(j)->LFSTK_setImageFromSurface(this->listDataArray->at(j+this->listOffset).data.surface,MENU,true);
							if(this->listDataArray->at(j+this->listOffset).imageType==FILETHUMB)
								this->labelsArray->at(j)->LFSTK_setImageFromPath(this->listDataArray->at(j+this->listOffset).data.imagePath,MENU,true);
							this->labelsArray->at(j)->LFSTK_showGadget();
							this->labelsArray->at(j)->LFSTK_clearWindow();
						}
				}
		}
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
	list->LFSTK_updateList();
	return(true);
}

void LFSTK_listGadgetClass::setNavSensitive(void)
{
//TODO//
}

/**
* Reset list size on window, not needed by user.
*
*/
void LFSTK_listGadgetClass::LFSTK_resetListHeight(int newheight)
{
	LFSTK_buttonClass	*button;
	int					sy=1;
	this->maxShowing=((newheight)/LABELHITE);

	if(this->labelsArray->size()<this->maxShowing)
		{
			for(int j=this->labelsArray->size();j<this->maxShowing;j++)
				{
					button=new LFSTK_buttonClass(this->wc,"",0,0,this->gadgetGeom.w-(this->squeezeW+2),LABELHITE-1,NorthGravity);
					button->LFSTK_reParentWindow(this->window,1,sy);
					button->toParent=true;
					button->LFSTK_setLabelAutoColour(true);
					button->gadgetDetails.state=NORMALCOLOUR;
					button->gadgetDetails.bevel=BEVELNONE;			
					button->LFSTK_setTile(NULL,0);
					button->LFSTK_setFontString(this->monoFontString);
					button->LFSTK_setCairoFontData();

					this->labelsArray->push_back(button);
					button->LFSTK_setGadgetColours(GADGETBG,this->labelsArray->at(0)->newGadgetBGColours.at(NORMALCOLOUR).name,
					this->labelsArray->at(0)->newGadgetBGColours.at(PRELIGHTCOLOUR).name,
					this->labelsArray->at(0)->newGadgetBGColours.at(ACTIVECOLOUR).name,
					this->labelsArray->at(0)->newGadgetBGColours.at(INACTIVECOLOUR).name);

					button->LFSTK_setGadgetColours(GADGETFG,this->labelsArray->at(0)->newGadgetBGColours.at(NORMALCOLOUR).name,
					this->labelsArray->at(0)->newGadgetBGColours.at(PRELIGHTCOLOUR).name,
					this->labelsArray->at(0)->newGadgetBGColours.at(ACTIVECOLOUR).name,
					this->labelsArray->at(0)->newGadgetBGColours.at(INACTIVECOLOUR).name);					
					button->gadgetDetails.colour=&button->newGadgetBGColours.at(NORMALCOLOUR);
					sy+=(LABELHITE-1);
				}
		}

	if(this->data!=NULL)
		delete[] this->data;

	this->data=new listData[this->maxShowing];
	for(int j=0;j<this->maxShowing;j++)
		{
			this->data[j].mainObject=this;
			this->data[j].userData=j;
			this->labelsArray->at(j)->LFSTK_setKeyCallBack(setFocusToList,NULL,LISTDATA(j));
			this->labelsArray->at(j)->LFSTK_setMouseCallBack(setFocusToList,select,LISTDATA(j));
		}

	if(this->scrollBar!=NULL)
		this->scrollBar->LFSTK_resetHeight(newheight);
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
LFSTK_listGadgetClass::LFSTK_listGadgetClass(LFSTK_windowClass *parentwc,const char *label,int x,int y,unsigned w,unsigned h,int gravity)
{
	unsigned				sx;
	XSetWindowAttributes	wa;
	int						sy=1;

	this->squeezeW=SCROLLBARWIDTH;

	this->LFSTK_setCommon(parentwc,"",x,y,w,h,gravity);

	wa.win_gravity=gravity;
	wa.save_under=true;
	this->window=XCreateWindow(this->wc->app->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
	this->gc=XCreateGC(this->wc->app->display,this->window,0,NULL);

	this->LFSTK_setFontString(this->monoFontString);
	this->wc->globalLib->LFSTK_setCairoSurface(this->wc->app->display,this->window,this->wc->app->visual,&this->sfc,&this->cr,w,h);
	this->LFSTK_setCairoFontData();

	XSelectInput(this->wc->app->display,this->window,this->gadgetEventMask);

	this->ml->function=&LFSTK_lib::LFSTK_gadgetEvent;
	this->ml->gadget=this;
	this->ml->type=LISTGADGET;
	this->wc->LFSTK_addMappedListener(this->window,ml);

	this->currentItem=0;
	this->listOffset=0;

	sx=x+1;
	this->labelsArray=new std::vector <LFSTK_buttonClass*>;
	this->listDataArray=new std::vector <listLabelStruct>;
	this->LFSTK_resetListHeight(h);

//navigate
	this->scrollBar=new LFSTK_scrollBarClass(this->wc,true,0,0,SCROLLBARWIDTH,h,gravity);
	this->scrollBar->LFSTK_reParentWindow(this->window,w-SCROLLBARWIDTH,0);
	this->scrollBar->toParent=true;	
	this->scrollBar->LFSTK_setScale(0,1);
	this->scrollBar->LFSTK_setLineScroll(1);
	this->scrollBar->LFSTK_setPageScroll(maxShowing-1);
	this->scrollBar->LFSTK_setMouseCallBack(NULL,scrollCB,this);
	this->scrollBar->LFSTK_setValue(0);
	this->style=BEVELIN;

	this->newGadgetBGColours[NORMALCOLOUR]=LFSTK_setColour(this->wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPELISTTROUGHCOLOUR));
	this->newGadgetBGColours[INACTIVECOLOUR]=LFSTK_setColour(this->wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPELISTTROUGHCOLOUR));
	this->gadgetDetails={&this->newGadgetBGColours.at(NORMALCOLOUR),BEVELIN,NOINDICATOR,NORMALCOLOUR,0,true,{0,0,w,h},{0,0,0,0},false,false,false};

	this->clearBox(&this->gadgetDetails);
}

/**
* Private function.
* Free list data.
*/
void LFSTK_listGadgetClass::freeList(void)
{
	for(int j=0;j<this->listDataArray->size();j++)
		{
			freeAndNull(&this->listDataArray->at(j).label);
			if((this->listDataArray->at(j).imageType==CAIROTHUMB) && (this->listDataArray->at(j).data.surface!=NULL) && (this->freeCairoImages==true))
				cairo_surface_destroy(this->listDataArray->at(j).data.surface);
			if((this->listDataArray->at(j).imageType==FILETHUMB) && (this->listDataArray->at(j).data.imagePath!=NULL))
				freeAndNull(&this->listDataArray->at(j).data.imagePath);
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
const char *LFSTK_listGadgetClass::LFSTK_getSelectedLabel(void)
{
	if(this->listDataArray->size()>this->currentItem)
		return(this->listDataArray->at(this->currentItem).label);

	return("");
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
	XSetInputFocus(this->wc->app->display,this->window,RevertToParent,CurrentTime);
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

/**
* Set the colours for the list items.
* \param gadgetColourType Colour type BG/FG
* \param std::string normal Colour name.
* \param std::string prelight Colour name.
* \param std::string active Colour name.
* \param std::string inactive Colour name.
*/
void LFSTK_listGadgetClass::LFSTK_setListItemsColours(gadgetColourType type,std::string normal,std::string prelight,std::string active,std::string inactive)
{
	for(int j=0;j<this->maxShowing;j++)
		{
			this->labelsArray->at(j)->LFSTK_setGadgetColours(type,normal,prelight,active,inactive);
			this->labelsArray->at(j)->LFSTK_clearWindow();
		}
}

/**
* Get max width of list items.
* \return with of longest item.
*/
int LFSTK_listGadgetClass::LFSTK_getListMaxWidth(void)
{
	double largest=0;
	double thiswid=0;
	for(int j=0;j<this->listDataArray->size();j++)
		{
			thiswid=4+LFSTK_getTextRealWidth(this->listDataArray->at(j).label);
			if(thiswid>=largest)
				largest=thiswid+4;//TODO//BEVEL?
		}
	return((int)(largest+0.5));
}
