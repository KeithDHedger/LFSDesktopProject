/*
 *
 * ©K. D. Hedger. Sun  4 Nov 19:37:12 GMT 2018 keithdhedger@gmail.com

 * This file (LFSTKMenuItem.cpp) is part of LFSToolKit.

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

#include "lfstk/LFSTKGlobals.h"

LFSTK_menuItemClass::~LFSTK_menuItemClass()
{
//	if(this->subwc!=NULL)
//		delete this->subwc;
}

/**
* Main Label constructor.
*
* \param parentwc Main parent window class.
* \param label Displayed name.
* \param x X pos.
* \param y Y pos.
* \param w Width.
* \param h Height.
* \param menu menuStruct array.
* \param gravity Label gravity
* \note optional, label gravity defaults to CENTRE.
* \note gadget gravity defaults to NorthWestGravity.
*/
//LFSTK_menuItemClass::LFSTK_menuItemClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,unsigned w,unsigned h,menuStruct *menu,int gravity,int bgrav)
LFSTK_menuItemClass::LFSTK_menuItemClass(LFSTK_toolWindowClass* parentwc,LFSTK_menuClass *mainmenu,const char* label,int x,int y,unsigned w,unsigned h,menuStruct *menu,std::vector<LFSTK_toolWindowClass*> *windows)
{
	XSetWindowAttributes	wa;

	this->LFSTK_setCommon(parentwc,label,x,y,w,h,BUTTONGRAV);

	wa.win_gravity=BUTTONGRAV;
	wa.save_under=true;
	this->window=XCreateWindow(this->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
	this->gc=XCreateGC(this->display,this->window,0,NULL);
	this->wc->globalLib->LFSTK_setCairoSurface(this->display,this->window,this->visual,&this->sfc,&this->cr,w,h);
	this->LFSTK_setCairoFontData();
	XSelectInput(this->display,this->window,ButtonReleaseMask | ButtonPressMask | ExposureMask | EnterWindowMask | LeaveWindowMask);

	this->style=BEVELNONE;

	this->ml->function=&LFSTK_lib::LFSTK_gadgetEvent;
	this->ml->gadget=this;
	this->ml->type=MENUITEMGADGET;
	this->wc->LFSTK_addMappedListener(this->window,ml);

	if(this->wc->globalLib->LFSTK_getUseTheme()==true)
		this->LFSTK_setTile(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEMENUITEMTILE),-1);
	else
		this->useTile=false;

	this->LFSTK_setFontColourName(NORMALCOLOUR,this->wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEMENUITEMFONTCOLOUR),true);
	this->LFSTK_setFontString(this->wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEMENUITEMFONT),true);
	gadgetDetails= {&this->colourNames[NORMALCOLOUR],BEVELNONE,DISCLOSURE,NORMALCOLOUR,0,true,{0,0,w,h},{(int)(w-TRIANGLESIZE-(this->pad*2)),(int)((h/2)-(TRIANGLESIZE/2)+(this->pad/2)),TRIANGLESIZE,TRIANGLESIZE},menu->hasSubMenu};
	this->LFSTK_setLabelGravity(MENU);
	this->menuData=menu;
	this->subwindows=windows;
	this->menu=mainmenu;
}

/**
* Mouse exit callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_menuItemClass::mouseExit(XButtonEvent *e)
{
//	if((this->isActive==false) || (this->callback.ignoreCallback==true))
//		return(true);
//
//	if(strcmp(this->label,"--")==0)
//		return(true);;

	this->gadgetDetails.colour=&this->colourNames[NORMALCOLOUR];
	this->gadgetDetails.state=NORMALCOLOUR;
	this->gadgetDetails.bevel=BEVELOUT;
	this->LFSTK_clearWindow();
	this->inWindow=false;
	if(this->subwc!=NULL)
		{
//printf("mouse exit\n");
	return(true);
//printf("this->subwc->inWindow=%i \n",this->subwc->inWindow);
			if((this->leaveSub==true) && (this->leaveThis==true) )
			{
		//	if(this->subwc->inWindow==false)
				//return(true);
				{
			this->subwc->mainLoop=false;
//			this->subwc->mainLoopQuit==true;
			this->subwc->LFSTK_hideWindow();
			XFlush(this->display);
			XSync(this->subwc->display,false);
			this->leaveSub=false;
			this->leaveThis=false;
			this->subwc->inWindow=false;
			}
			}
	//	else
			if(this->subwc->inWindow==false)
				//return(true);
				{
			this->subwc->mainLoop=false;
//			this->subwc->mainLoopQuit==true;
			this->subwc->LFSTK_hideWindow();
			XFlush(this->display);
			XSync(this->subwc->display,false);
			this->leaveSub=false;
			this->leaveThis=false;
			this->subwc->inWindow=false;
				}

//		if((this->subwc->isVisible==true) && (this->subwc->inWindow==false))
//			{
//			this->subwc->mainLoop=false;
////			this->subwc->mainLoopQuit==true;
//			this->subwc->LFSTK_hideWindow();
//			XFlush(this->display);
//			XSync(this->subwc->display,false);
//			this->leaveSub=false;
//			this->leaveThis=false;
//			}
		}

	return(true);
}
#include <unistd.h>


bool LFSTK_menuItemClass::lostFocus(XEvent *e)
{
	printf("lost focus>>>>>>\n");
	return(true);
}


/**
* Mouse enter callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_menuItemClass::mouseEnter(XButtonEvent *e)
{
//	if((this->isActive==false) || (this->callback.ignoreCallback==true))
//		return(true);
//
//	if(strcmp(this->label,"--")==0)
//		return(true);

	this->gadgetDetails.colour=&this->colourNames[PRELIGHTCOLOUR];
	this->gadgetDetails.state=PRELIGHTCOLOUR;
	this->gadgetDetails.bevel=BEVELOUT;
	this->LFSTK_clearWindow();
	this->inWindow=true;

	if(this->menuData->hasSubMenu==true)
		{
			LFSTK_menuItemClass	*label;
			int					sy=0;
			XEvent	event;
			XEvent	lookevent;
			bool	gotlooked=false;
	
			if(this->subwc==NULL)
				{
					this->subwc=new LFSTK_toolWindowClass(this->display,this->wc,"_NET_WM_WINDOW_TYPE_MENU",this->gadgetGeom.x,this->gadgetGeom.y,GADGETWIDTH,GADGETHITE*this->menuData->subMenuCnt,"menu window");
//					this->subwc=new LFSTK_toolWindowClass(this->gadgetGeom.x,this->gadgetGeom.y,GADGETWIDTH,GADGETHITE*this->menuData->subMenuCnt,"menu window",true,true,false);
					//this->subwc->display=this->display;
					//subwc->LFSTK_setWindowType("_NET_WM_WINDOW_TYPE_MENU");
					//this->subwindows->push_back(this->subwc);
					for(int j=0; j<this->menuData->subMenuCnt; j++)
						{
							label=new LFSTK_menuItemClass(this->subwc,this->menu,this->menuData->subMenus[j]->label,0,sy,GADGETWIDTH,GADGETHITE,this->menuData->subMenus[j],this->subwindows);
							label->LFSTK_setCallBack(this->callback.pressCallback,this->callback.releaseCallback,this->menuData->subMenus[j]->userData);
							if(this->menuData->subMenus[j]->imageType==FILETHUMB)
								label->LFSTK_setImageFromPath(this->menuData->subMenus[j]->data.imagePath,MENU,true);
							if(this->menuData->subMenus[j]->imageType==CAIROTHUMB)
								label->LFSTK_setImageFromSurface(this->menuData->subMenus[j]->data.surface,MENU,true);
							sy+=GADGETHITE;
						}
					//this->subwindows->push_back(this->subwc);
					this->menu->subwindows->push_back(this->subwc);
				}
			this->subwc->inWindow=true;
			//Display *disp=XOpenDisplay(NULL);
			//XSynchronize(disp,true);
			XSynchronize(this->subwc->display,true);
			//XSetInputFocus(disp,this->window,None,CurrentTime);
			this->subwc->mainLoop=true;
			this->subwc->LFSTK_showWindow(true);
		//	XSetInputFocus(this->display,this->subwc->window,None,CurrentTime);
		//	XSetInputFocus(this->display,this->menu->mainMenuWindow->window,RevertToNone,CurrentTime);
 
	while(this->subwc->mainLoop==true)
				{
					XNextEvent(this->display,&event);
					mappedListener *ml=this->subwc->LFSTK_getMappedListener(event.xany.window);
					switch(event.type)
						{
							case FocusOut:
								for(int j=0;j<this->subwindows->size();j++)
								{
								//printf("top focus out >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
								this->subwindows->at(j)->LFSTK_hideWindow();
								this->subwindows->at(j)->mainLoop=false;
								//this->subwc->LFSTK_hideWindow();
								//this->subwc->mainLoop=false;
								//XSetInputFocus(this->display,this->window,RevertToParent,CurrentTime);
								}
								this->menu->mainMenuWindow->LFSTK_hideWindow();
								this->menu->mainLoop=false;
								continue;
								break;
						}

if(event.xcrossing.window==this->wc->window)
{
//	printf("this->wc->window=%p\n",this->wc->window);
	continue;
}
					if(ml!=NULL)
						ml->function(ml->gadget,&event,ml->type);

					if(this->subwc->LFSTK_handleWindowEvents(&event)<0)
						this->subwc->mainLoop=false;

					switch(event.type)
						{
							case FocusOut:
								printf("focus out....\n");
								{
								//LFSTK_toolWindowClass *tw=(LFSTK_toolWindowClass*)this->wc;
								//bool level=false;
								//while(tw->topLevel==false)
								//	{
								//		if(tw->wc!=NULL)
								//			tw->wc->LFSTK_hideWindow();
								//		tw=(LFSTK_toolWindowClass*)tw->wc;
								//	
								//	this->subwc->LFSTK_hideWindow();
								//	this->subwc->mainLoop=false;
									//}
								}
								break;
							case ButtonRelease:
							//case ButtonPress:
								//	this->subwc->LFSTK_hideWindow();
								//	this->subwc->mainLoop=false;
								for(int j=0;j<this->subwindows->size();j++)
									{
										//printf("ButtonRelease>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
										this->subwindows->at(j)->LFSTK_hideWindow();
										this->subwindows->at(j)->mainLoop=false;
								//this->subwc->LFSTK_hideWindow();
								//this->subwc->mainLoop=false;
								//XSetInputFocus(this->display,this->window,RevertToParent,CurrentTime);
								}
								this->menu->mainMenuWindow->LFSTK_hideWindow();
								this->menu->mainLoop=false;
								continue;

								printf("ButtonRelease....\n");
								break;
							case Expose:
								if (event.xexpose.count==0)
									{
										Window dw;
										int x;
										int y;
										unsigned int w,h,dump;
										XTranslateCoordinates(this->subwc->display,this->wc->window,this->subwc->rootWindow, 0, 0, &x, &y, &dw );
										this->subwc->LFSTK_moveWindow(x+GADGETWIDTH,y+this->gadgetGeom.y,true);
									}
								break;

//							case EnterNotify:
//										if(event.xcrossing.window==this->wc->window)
//											printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
//							printf("enter\n");
//						printf("EnterNotify thiswindow=%p evwindow=%p focus=%i time=%i\n",this->window,event.xcrossing.window,event.xcrossing.focus,event.xcrossing.time);
////								if(((event.xcrossing.window!=this->window) && (this->subwc->inWindow==false)) && (event.xcrossing.window!=this->subwc->window))
//								if(((event.xcrossing.window!=this->window) && (this->subwc->inWindow==false)) )
//									{
//										this->subwc->LFSTK_hideWindow();
//										this->subwc->mainLoop=false;
//									//	delete this->subwc;
//									//	this->subwc=NULL;
//									}
//								break;
						}


					gotlooked=XCheckTypedWindowEvent(this->display,this->subwc->window,EnterNotify,&lookevent);
					if(gotlooked==false)
						{
							switch(event.type)
								{
									case LeaveNotify:
									//if(event.xcrossing.window==this->wc->window)
									//		printf("---------------->>>>>>>>>\n");
										if(event.xcrossing.window==this->window)
											{
											this->mouseExit(&event.xbutton);
												this->subwc->LFSTK_hideWindow();
												this->subwc->mainLoop=false;
												//delete this->subwc;
												//this->subwc=NULL;
												//this->subwc->inWindow=false;
											//	XSync(this->subwc->display,false);
											//	continue;
											}
										break;
								}
						}
					//else
					//	this->subwc->inWindow=true;

					gotlooked=XCheckTypedWindowEvent(this->display,this->subwc->window,LeaveNotify,&lookevent);
					if(gotlooked==true)
						{
						//if(lookevent.xcrossing.window==this->wc->window)
						//					printf("---------------->>>>>>>>>\n");
						gotlooked=XCheckTypedWindowEvent(this->display,this->wc->window,EnterNotify,&lookevent);
						if(gotlooked==true)
						{
						gotlooked=XCheckTypedWindowEvent(this->display,this->window,EnterNotify,&lookevent);
						if(gotlooked==false)
						{
							this->subwc->LFSTK_hideWindow();
												this->subwc->mainLoop=false;
						this->mouseExit(&event.xbutton);
						}
						//printf("got leav\n");
						//printf("LeaveNotify thiswindow=%p evwindow=%p focus=%i time=%i\n",this->window,lookevent.xcrossing.window,lookevent.xcrossing.focus,lookevent.xcrossing.time);
						this->subwc->inWindow=false;
						}
						}
				}
	
	
return(true);	
	
			while(this->subwc->mainLoop==true)
				{
					XNextEvent(this->subwc->display,&event);
					//XNextEvent(disp,&event);
					mappedListener *ml=this->subwc->LFSTK_getMappedListener(event.xany.window);

					if(ml!=NULL)
						ml->function(ml->gadget,&event,ml->type);

					if(this->subwc->LFSTK_handleWindowEvents(&event)<0)
						this->subwc->mainLoop=false;

					//printf("itemwin=%p evwin=%p subwin=%p\n",this->window,event.xany.window,this->subwc->window);
					switch(event.type)
						{
//							case FocusOut:
//								if(this->window,event.xcrossing.window==this->window)
//								break;
//							case FocusIn:
//								break;
							case LeaveNotify:
								//printf("LeaveNotify itemwin=%p evwin=%p subwindow=%p subwcwin=%p\n",this->window,event.xcrossing.window,event.xcrossing.subwindow,this->subwc->window);
								printf("LeaveNotify thiswindow=%p evwindow=%p focus=%i time=%i\n",this->window,event.xcrossing.window,event.xcrossing.focus,event.xcrossing.time);
								if(event.xcrossing.window==this->window)
									{
									//	this->leaveThis=true;
									//	this->mouseExit(&event.xbutton);
//										this->subwc->LFSTK_hideWindow();
//										this->subwc->mainLoop=false;
//										XSync(this->subwc->display,false);
										//continue;

									}
								//if((event.xcrossing.window!=this->window) && (event.xcrossing.subwindow!=this->window))
								
								if(event.xcrossing.window==this->subwc->window)
									{
									//	leaveSub=true;
									//	this->subwc->inWindow=false;
									//	this->mouseExit(&event.xbutton);
//										this->subwc->LFSTK_hideWindow();
//										this->subwc->mainLoop=false;
//										XSync(this->subwc->display,false);
										//continue;
									}
								break;
							case EnterNotify:
								printf("EnterNotify thiswindow=%p evwindow=%p focus=%i time=%i\n",this->window,event.xcrossing.window,event.xcrossing.focus,event.xcrossing.time);
								if(event.xcrossing.window!=this->window)
									if(this->subwc->inWindow==true)
										{
											this->subwc->LFSTK_hideWindow();
											this->subwc->mainLoop=false;
											this->subwc->inWindow=false;
											XSync(this->subwc->display,false);
											this->mouseExit(&event.xbutton);
											continue;
										}
										
							//	if(event.xcrossing.window==this->subwc->window)
							//		this->subwc->inWindow=true;
								//printf("EnterNotify itemwin=%p evwin=%p subwin=%p\n",this->window,event.xcrossing.window,this->subwc->window);
								break;
							case Expose:
								if (event.xexpose.count==0)
									{
										Window dw;
										int x;
										int y;
										unsigned int w,h,dump;
										XTranslateCoordinates(this->subwc->display,this->wc->window,this->subwc->rootWindow, 0, 0, &x, &y, &dw );
										this->subwc->LFSTK_moveWindow(x+GADGETWIDTH,y+this->gadgetGeom.y,true);
									//		if(this->wc->isVisible==true)
									//			XSetInputFocus(this->subwc->display,this->parent,RevertToParent,CurrentTime);
//												XSetInputFocus(this->subwc->display,this->wc->window,RevertToParent,CurrentTime);
									//		else
									//			XSetInputFocus(this->subwc->display,None,RevertToParent,CurrentTime);

									}
								break;
						}
				}

		}
return(true);
#if 0




//			this->mainLoop=true;
			this->subwc->mainLoop=true;
			//XSync(this->subwc->display,true);
			subwc->LFSTK_showWindow(true);
			//XSetInputFocus(this->subwc->display,this->parent,RevertToParent,CurrentTime);
subwc->mainLoopQuit=false;
			while((this->subwc->mainLoop==true) && (subwc->mainLoopQuit==false))
				{
//				if(subwc->mainLoopQuit==true)
//					{
//					sleep(1000);
//					return(true);
//					}
				//printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
#if 1
					if(XPending(this->display))
						{
					//	printf("XNextEvent(this->display,&event);\n");
							XNextEvent(this->display,&event);
							switch(event.type)
								{
									case FocusOut:
									printf("focus out-----------\n");
										//for(int j=0;j<this->subwindows->size();j++)
										//	{
										//		subwindows->at(j)->LFSTK_hideWindow();
										//		subwindows->at(j)->mainLoop=false;
										//	}

								//		this->subwc->LFSTK_hideWindow();
										//this->wc->LFSTK_hideWindow();
								//		this->subwc->mainLoop=false;
								//		this->subwc->mainLoopQuit=true;
										//this->wc->mainLoop=false;
										break;

									
									case LeaveNotify:
									printf("formw=%p subwcw=%p\n",event.xcrossing.window,this->subwc->window);
										//this->subwc->LFSTK_hideWindow();
									//	this->subwc->mainLoop=false;
									//	this->subwc->mainLoopQuit=true;
										break;
								}
						}
#endif
#if 1
					if(XPending(subwc->display))
						{
						bool flag=true;
//while(flag==true)
{
						//printf("XNextEvent(subwc->display,&event);<<<<<<<<\n");
							XNextEvent(subwc->display,&event);
							mappedListener *ml=subwc->LFSTK_getMappedListener(event.xany.window);

							if(ml!=NULL)
								ml->function(ml->gadget,&event,ml->type);

							if(subwc->LFSTK_handleWindowEvents(&event)<0)
								this->subwc->mainLoop=false;
					//	printf("XNextEvent(subwc->display,&event);>>>>>>>>>>\n");

							switch(event.type)
								{
									case LeaveNotify:
								//	flag=false;
										//this->subwc->LFSTK_hideWindow();
										//this->subwc->mainLoop=false;
										//this->subwc->mainLoopQuit=true;
										
										break;
									case ConfigureNotify:
									case Expose:
										//if (event.xexpose.count==0)
										{
											Window dw;
											int x;
											int y;
											unsigned int w,h,dump;
											XTranslateCoordinates( this->subwc->display,this->parent,this->subwc->rootWindow, 0, 0, &x, &y, &dw );
											this->subwc->LFSTK_moveWindow(x+GADGETWIDTH,y+this->gadgetGeom.y,true);
									//		if(this->wc->isVisible==true)
									//			XSetInputFocus(this->subwc->display,this->parent,RevertToParent,CurrentTime);
//												XSetInputFocus(this->subwc->display,this->wc->window,RevertToParent,CurrentTime);
									//		else
									//			XSetInputFocus(this->subwc->display,None,RevertToParent,CurrentTime);

										}
										break;

									case EnterNotify:
									printf("XNextEvent(subwc->display,&event);<<<<<<<<\n");
									//XSetInputFocus(this->subwc->display,this->wc->window,None,CurrentTime);
									//	this->subwc->mainLoop=true;
									//	this->subwc->mainLoopQuit=false;
										break;

									case ButtonRelease:
										{
//										LFSTK_windowClass	*pwc=NULL;
//										LFSTK_menuItemClass *pmc=NULL;
//										pmc=this;
//										pwc=this->wc;
//										while(1)
//										{
//											
//											pmc->subwc->LFSTK_hideWindow();
//											pwc->LFSTK_hideWindow();
//											pwc=pwc->wc;
//											
//											
//										}
										//this->subwc->LFSTK_hideWindow();
										//this->wc->LFSTK_hideWindow();
										//this->subwc->wc->LFSTK_hideWindow();
										for(int j=0;j<this->subwindows->size();j++)
											{
											//	subwindows->at(j)->LFSTK_hideWindow();
											//	subwindows->at(j)->mainLoop=false;
											//	subwindows->at(j)->mainLoopQuit=true;
												//XSetInputFocus(subwindows->at(j)->display,subwindows->at(j)->rootWindow,RevertToParent,CurrentTime);
												//delete subwindows->at(j);
												//return(true);
											}
										//XSetInputFocus(this->display,PointerRoot,RevertToParent,CurrentTime);
										XFlush(this->subwc->display);
										XFlush(this->wc->display);
										XSync(this->subwc->display,true);
										XSync(this->wc->display,true);
										
										//this->subwc->mainLoop=false;
										//subwc->mainLoopQuit=true;
															//sleep(5);

									//	return(true);
									//	continue;
										}
										break;
									case FocusOut:
									printf("focus out>>>>>>>>>>>>>>>>\n");
//										for(int j=0;j<this->subwindows->size();j++)
//											{
//												subwindows->at(j)->LFSTK_hideWindow();
//												subwindows->at(j)->mainLoop=false;
//												subwindows->at(j)->mainLoopQuit=true;
//												//XSetInputFocus(subwindows->at(j)->display,subwindows->at(j)->rootWindow,RevertToParent,CurrentTime);
//												//delete subwindows->at(j);
//												//return(true);
//											}
									//	this->subwc->LFSTK_hideWindow();
									//	this->wc->LFSTK_hideWindow();
									//	this->subwc->mainLoop=false;
									//	this->subwc->mainLoopQuit=true;
										break;

								}
//							if(subwc->LFSTK_handleWindowEvents(&event)<0)
//								this->subwc->mainLoop=false;
						//printf("XNextEvent(subwc->display,&event);------------------\n");


						}
						}
#endif
					//	printf("XNextEvent(subwc->display,&event);=====================\n");
				}
					//	printf("XNextEvent(subwc->display,&event);++++++++++++++++++++++\n");
			subwc->LFSTK_hideWindow();
			this->mouseExit(&event.xbutton);
		}
#endif
	return(true);
}