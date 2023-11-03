/*
 *
 * ©K. D. Hedger. Fri 29 Jul 13:51:22 BST 2022 keithdhedger@gmail.com

 * This file (eventsClass.cpp) is part of LFSWM2.

 * LFSWM2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * LFSWM2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSWM2.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include <stdio.h>

#include "eventsClass.h"

LFSWM2_eventsClass::~LFSWM2_eventsClass(void)
{
}

LFSWM2_eventsClass::LFSWM2_eventsClass(LFSWM2_Class *mainclass)
{
	this->mainClass=mainclass;
}

void LFSWM2_eventsClass::LFSWM2_mainEventLoop(void)
{
	XWindowAttributes	attr;
	XButtonEvent			start;
	int					when=0;
	LFSWM2_clientClass	*cc;
	LFSWM2_clientClass	*cccontrol;
	bool					firstrun=true;
	bool					overide=false;
	bool					inmenu=false;
	int					lastbutton=-1;
	int					lasttime=0;

//XSynchronize(this->mainClass->display,true);
	this->mainClass->runLevel=RL_STARTUP;
	this->mainClass->LFSWM2_pushXErrorHandler();
		this->mainClass->mainWindowClass->LFSWM2_buildClientList();
	this->mainClass->LFSWM2_popXErrorHandler();
	this->mainClass->runLevel=RL_NORMAL;

	this->mainClass->restackCnt=0;
	this->mainClass->LFSWM2_setCurrentDesktop(this->mainClass->currentDesktop,true);
	this->LFSWM2_restack();

	while(true)
		{
			XEvent e;
			if(firstrun==true)
				{
					if(XPending(this->mainClass->display)==0)
						{
							this->mainClass->restackCnt=0;
							this->LFSWM2_restack();
							firstrun=false;
						}
				}

			if(this->mainClass->messages->whatMsg==QUITLFSWM)
				break;

			if(this->mainClass->messages->whatMsg==RESTARTLFSWM)
				{
					char self[PATH_MAX]={0};
					int nchar=readlink("/proc/self/exe",self,sizeof self);
					if(nchar>1)
						execv(self,this->mainClass->argv);
					continue;
				}

			if(this->mainClass->messages->whatMsg==REFRESHTHEME)
				{
					int mokeyshold=this->mainClass->modKeys;
					this->mainClass->mainWindowClass->LFSWM2_reloadTheme();
					this->mainClass->messages->whatMsg=NOMSG;
					LFSWM2_clientClass *ccs;
					for(long unsigned j=0;j<this->mainClass->mainWindowClass->windowIDList.size();j++)
						{
							ccs=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->windowIDList.at(j));
							if(ccs!=NULL)
								{
									if(this->mainClass->mainWindowClass->theme.gotPart[this->mainClass->prefs.LFSTK_hashFromKey("menu-active")]==false)
										XMoveWindow(this->mainClass->display,ccs->menuButton,-1000,-1000);
									if(this->mainClass->mainWindowClass->theme.gotPart[this->mainClass->prefs.LFSTK_hashFromKey("shade-active")]==false)
										XMoveWindow(this->mainClass->display,ccs->shadeButton,-1000,-1000);

									if(ccs->isFullscreen==false)
										{
											if(ccs->isBorderless==false)
												{
													XShapeCombineMask(this->mainClass->display,ccs->frameWindow,ShapeBounding,0,0,None,ShapeSet);
													ccs->setWindowRects(true);
													this->mainClass->mainWindowClass->LFSWM2_setControlRects(ccs);
													ccs->resetContentWindow();
													this->mainClass->mainWindowClass->LFSWM2_refreshFrame(ccs);
													ccs->resizeMode=this->mainClass->resizeMode;
												}
										}
									
									XUngrabButton(this->mainClass->display,Button1,mokeyshold,ccs->contentWindow);
									XGrabButton(this->mainClass->display,Button1,(this->mainClass->modKeys),ccs->contentWindow,False,ButtonPressMask|ButtonReleaseMask|PointerMotionMask,GrabModeAsync,GrabModeAsync,None,None);
									XUngrabKey(this->mainClass->display,XKeysymToKeycode(this->mainClass->display,XK_Escape),mokeyshold,ccs->contentWindow);
									XGrabKey(this->mainClass->display,XKeysymToKeycode(this->mainClass->display,XK_Escape),(this->mainClass->modKeys),ccs->contentWindow,False,GrabModeSync,GrabModeAsync);
								}
						}
					this->mainClass->restackCnt=1;
					this->noRestack=false;
				}

			XNextEvent(this->mainClass->display,&e);
			cccontrol=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->LFSWM2_getParentWindow(e.xany.window));
			if(cccontrol!=NULL)
				{
					if(cccontrol->LFSWM2_handleControls(&e)==true)
						{
							this->noRestack=false;
							continue;
						}
				}
			cccontrol=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e.xany.window);
			if(cccontrol!=NULL)
				{
					overide=false;
					if(cccontrol->LFSWM2_handleEvents(&e)==true)
						{
							this->noRestack=false;
							overide=true;
							cccontrol=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e.xany.window);//in case window has died
							if(cccontrol!=NULL)
								{
									cccontrol->adjustContentWindow();
								}
							cccontrol=NULL;
							e.type=0;
							this->mainClass->restackCnt=1;
							continue;
						}
				}

			for(int j=0;j<this->mainClass->mainWindowClass->windowIDList.size();j++)
				{
					cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->windowIDList.at(j));
					if(cc!=NULL)
						{
							if(cc->isBorderless==true)
								continue;
							if(cc->rendered==false)
								{
									cc->renderFrame(false);
									this->noRestack=false;
									this->mainClass->restackCnt=0;
									XRaiseWindow(this->mainClass->display,cc->frameWindow);
									//continue;
								}
						}
				}

			switch(e.type)
				{
					case KeyRelease:
						//fprintf(stderr,"e.xkey.keycode=%i\n",e.xkey.keycode);
						if((e.xkey.keycode==XKeysymToKeycode(this->mainClass->display,XK_Escape)) && (e.xkey.state&(this->mainClass->modKeys))==(this->mainClass->modKeys))
							{
								cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e.xkey.window);
								if(cc!=NULL)
									{
										if(cc->isFullscreen==true)
											cc->LFSWM2_fullscreenWindow(false);
									}
							}
						break;

					case ButtonRelease:
//						fprintf(stderr,"ButtonRelease eventnumber %i button number %i win=%x time=%d\n",when++,e.xbutton.button,e.xbutton.window,e.xbutton.time);
						if((e.xbutton.window==this->mainClass->rootWindow) && (e.xbutton.time-lasttime>250))
							{
								this->noRestack=false;
								int	direction=1;
								int cd=this->mainClass->currentDesktop;
								if(e.xbutton.button==Button4)
									direction=-1;
								if((e.xbutton.button==Button5) || (e.xbutton.button==Button4))
									{
										cd+=direction;
										if(cd<0)
											cd=this->mainClass->numberOfDesktops-1;
										if(cd>=this->mainClass->numberOfDesktops)
											cd=0;
										this->mainClass->LFSWM2_setCurrentDesktop(cd);
										this->mainClass->restackCnt=0;
										lasttime=e.xbutton.time;
									}
							}
						
						start.subwindow=None;
						cc=NULL;
						break;
					case ButtonPress:
						//fprintf(stderr,"ButtonPress eventnumber %i\n",when++);
						this->noRestack=false;
						start=e.xbutton;
						this->mainClass->restackCnt=0;
						this->sy=e.xbutton.y;
						cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e.xbutton.window);
						if(cc!=NULL)
							{
								inmenu=false;
								if((e.xbutton.state&(this->mainClass->modKeys))==(this->mainClass->modKeys))//TODO//???windows key for now used to move window wihout restacking
									break;

								this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_ACTIVE_WINDOW")),XA_WINDOW,32,&cc->contentWindow,1);
								XSetInputFocus(this->mainClass->display,cc->contentWindow,RevertToNone,CurrentTime);
								XAllowEvents(this->mainClass->display,ReplayPointer,e.xbutton.time);
								XRaiseWindow(this->mainClass->display,cc->contentWindow);
							}
						break;

					case MapNotify:
						{
						//fprintf(stderr,"MapNotify main event loop window=%x when=%i\n",e.xmap.window,when++);
							this->noRestack=false;
							Atom					*v=NULL;
							long unsigned int	nitems_return;

							if(overide==false)
								this->mainClass->restackCnt=4;
							v=(Atom*)this->mainClass->mainWindowClass->LFSWM2_getProp(this->mainClass->rootWindow,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_ACTIVE_WINDOW")),XA_WINDOW,&nitems_return);
							if(v!=NULL)
								{
									if(e.xmap.window!=v[0])
										inmenu=true;
									XFree(v);
								}
						}
						break;

					case MapRequest:
						{
							//fprintf(stderr,"MapRequest main event loop window=%x when=%i\n",e.xmap.window,when++);
							this->noRestack=false;
							this->mainClass->mainWindowClass->LFSWM2_removeProp(this->mainClass->rootWindow,this->mainClass->prefs.LFSTK_hashFromKey("_NET_ACTIVE_WINDOW"));
							XWindowAttributes	x_window_attrs;
							hintsDataStruct		hs;
							XGetWindowAttributes(this->mainClass->display,e.xmaprequest.window,&x_window_attrs);
							XMapWindow(this->mainClass->display,e.xmaprequest.window);
							hs=this->mainClass->mainWindowClass->LFSWM2_getWindowHints(e.xmaprequest.window);
							if((hs.mHints!=NULL) && (hs.mHints->decorations==0))
								{
									if(this->mainClass->mainWindowClass->LFSWM2_createUnframedWindow(e.xmaprequest.window)==true)
										{
											this->noRestack=false;
											this->mainClass->restackCnt=0;
											break;
										}
									break;
								}
							XMoveWindow(this->mainClass->display,e.xmaprequest.window,-1000000,-1000000);
							if(this->mainClass->mainWindowClass->LFSWM2_createClient(e.xmaprequest.window,hs)==false)
								this->mainClass->mainWindowClass->LFSWM2_freeHints(hs);
							
							XMoveResizeWindow(this->mainClass->display,e.xmaprequest.window,this->mainClass->leftSideBarSize,this->mainClass->titleBarSize,x_window_attrs.width,x_window_attrs.height);
							this->noRestack=false;
							this->mainClass->restackCnt=0;
							//XRaiseWindow(this->mainClass->display,);
							//XRaiseWindow(this->mainClass->display,this->mainClass->mainWindowClass->clientList.at(e.xmaprequest.window)->frameWindow);
						}
						break;

					case ConfigureRequest://TODO//NEXT
						//fprintf(stderr,"ConfigureRequest from main event loop window=%x when=%i\n",e.xmaprequest.window,when++);
						{
							this->noRestack=false;
							LFSWM2_clientClass	*cc;
							cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e.xconfigurerequest.window);
							if((e.xconfigurerequest.x<0) || (e.xconfigurerequest.y<0))
								break;

							if(e.xconfigurerequest.send_event==false)
								{
									cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e.xconfigurerequest.window);
									if(cc!=NULL)
										{
											XWindowChanges	ch;
											cc->configCnt++;
											if((cc->isBorderless==true) && (cc->configCnt<MAXCONFIGCNT))
												{
													XWindowAttributes	x_window_attrs;
													XGetWindowAttributes(this->mainClass->display,e.xconfigurerequest.window,&x_window_attrs);
													hintsDataStruct		hs;
													hs=this->mainClass->mainWindowClass->LFSWM2_getWindowHints(e.xconfigurerequest.window);
													if((hs.mHints!=NULL) && (hs.mHints->decorations!=0))
														XMoveResizeWindow(this->mainClass->display,e.xconfigurerequest.window,hs.pt.x,hs.pt.y,hs.sh->min_width,hs.sh->min_height);
												break;
												}
											cc->configCnt=0;
											if((e.xconfigurerequest.value_mask & (CWWidth|CWHeight)) !=0)
												{
													ch.width=e.xconfigurerequest.width+this->mainClass->riteSideBarSize+this->mainClass->leftSideBarSize;
													ch.height=e.xconfigurerequest.height+this->mainClass->titleBarSize+this->mainClass->bottomBarSize;
													if((cc->buttonDown==false) || (cc->isBorderless==false))
														{
															if(cc->isBorderless==false)
																XResizeWindow(this->mainClass->display,cc->frameWindow,e.xconfigurerequest.width+this->mainClass->riteSideBarSize+this->mainClass->leftSideBarSize,e.xconfigurerequest.height+this->mainClass->titleBarSize+this->mainClass->bottomBarSize);
														}
													ch.width=e.xconfigurerequest.width;
													ch.height=e.xconfigurerequest.height;
													XResizeWindow(this->mainClass->display,cc->contentWindow,ch.width,ch.height);
												}
											
											if((e.xconfigurerequest.value_mask & (CWX|CWY)) !=0)
												{
													ch.x=e.xconfigurerequest.x;
													ch.y=e.xconfigurerequest.y;
													if(cc->isBorderless==false)
														XMoveWindow(this->mainClass->display,cc->frameWindow,ch.x,ch.y);
													break;
												}

											cc->setWindowRects(true);

											if((cc->buttonDown==false) && (cc->resizeMode==SCALERESIZE))
												XMoveWindow(this->mainClass->display,cc->resizeWindow,-100000,-100000);

											break;
										}
									else
										{
											XWindowChanges		changes;
											changes.x=e.xconfigurerequest.x;
											changes.y=e.xconfigurerequest.y;
											changes.width=e.xconfigurerequest.width;
											changes.height=e.xconfigurerequest.height;
											changes.border_width=e.xconfigurerequest.border_width;
											changes.sibling=e.xconfigurerequest.above;
											changes.stack_mode=e.xconfigurerequest.detail;

											XConfigureWindow(this->mainClass->display,e.xconfigurerequest.window,e.xconfigurerequest.value_mask,&changes);
											this->mainClass->restackCnt=1;
											
											break;

											if((e.xconfigurerequest.value_mask & (CWX|CWY)) == (CWX|CWY))
												{
													XWindowChanges ch;
													ch.x=e.xconfigurerequest.x;
													ch.y=e.xconfigurerequest.y;
													XConfigureWindow(this->mainClass->display,e.xconfigurerequest.window,e.xconfigurerequest.value_mask,&ch);
												}
											else if((e.xconfigurerequest.value_mask & (CWWidth|CWHeight)) == (CWWidth|CWHeight))
												{
													XWindowChanges	ch;
													ch.x=e.xconfigurerequest.x;
													ch.y=e.xconfigurerequest.y;
													ch.width=e.xconfigurerequest.width+this->mainClass->riteSideBarSize+this->mainClass->leftSideBarSize;
													ch.height=e.xconfigurerequest.height+this->mainClass->titleBarSize+this->mainClass->bottomBarSize;
													XConfigureWindow(this->mainClass->display,e.xconfigurerequest.parent,e.xconfigurerequest.value_mask,&ch);
													ch.width=e.xconfigurerequest.width;
													ch.height=e.xconfigurerequest.height;
													XConfigureWindow(this->mainClass->display,e.xconfigurerequest.window,e.xconfigurerequest.value_mask& (CWWidth|CWHeight),&ch);
												}
										}
								}
							break;
						}

					case PropertyNotify:
						//fprintf(stderr,"PropertyNotify IN eventnumber %i atom name=%s\n",when++,XGetAtomName(this->mainClass->display,e.xproperty.atom));
						{
							LFSWM2_clientClass	*cc;
							this->mainClass->restackCnt++;//???
							if(false)
								{
									fprintf(stderr,"PropertyNotify eventnumber %i\n",when++);
									fprintf(stderr,"type=%i 28=PropertyNotify\n",e.xproperty.type);
									fprintf(stderr,"window=%p \n",(void*)e.xproperty.window);
									if(e.xproperty.state==PropertyNewValue)
										fprintf(stderr,"state=%x =PropertyNewValue \n",e.xproperty.state);
									else
										fprintf(stderr,"state=%x =PropertyDelete \n",e.xproperty.state);
									fprintf(stderr,"send_event=%i\n",e.xproperty.send_event);
#ifdef __DEBUG__
									this->mainClass->DEBUG_printAtom(e.xproperty.atom);
#endif
								}

							if((e.xproperty.state==PropertyNewValue) || (e.xproperty.state==PropertyDelete))
								{
									if(e.xproperty.atom==this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_DESKTOP")))
										{
										this->noRestack=false;
											if(e.xproperty.window==this->mainClass->rootWindow)
												{
												}
											break;
										}

									if(e.xproperty.atom==this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_STATE")))
										{
											this->noRestack=false;
											//fprintf(stderr,"PropertyNotify OUT WM_STATE eventnumber %i\n",when++);
											cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e.xproperty.window);
											if(cc!=NULL)
												{
													cc->LFSWM2_setWMState(&e);
													break;
												}
										}

									if(e.xproperty.atom==this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_NAME")))
										{
											cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e.xproperty.window);
											if(cc!=NULL)
												{
													cc->LFSWM2_setWindowName();
													cc->mainClass->mainWindowClass->LFSWM2_refreshFrame(cc,NULL);
													//fprintf(stderr,"PropertyNotify OUT _NET_WM_NAME eventnumber %i\n",when++);
													break;
												}
										}

									cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e.xproperty.window);
									if(cc!=NULL)
										{
											this->mainClass->mainWindowClass->LFSWM2_reloadWindowState(cc->contentWindow);
										}
								}
						}
						//fprintf(stderr,"PropertyNotify OUT eventnumber %i\n",when++);
						break;

					case ClientMessage:
						//fprintf(stderr,"ClientMessage eventnumber %i\n",when++);
						this->noRestack=false;
						this->mainClass->restackCnt=0;
						this->LFSWM2_doClientMsg(e.xclient.window,&e.xclient);
						break;

					case DestroyNotify:
						//std::cout<<"DestroyNotify from main event loop"<<std::endl;
						break;
					default:
						//fprintf(stderr,"default eventnumber %i\n",when++);
						if(overide==false)
							this->mainClass->restackCnt=2;//TODO//
						break;
				}

			if(this->noRestack==false)
				{
					this->mainClass->restackCnt--;
					if(this->mainClass->restackCnt<1)
						{
							//fprintf(stderr,"this->mainClass->restackCnt<1 eventnumber %i\n",when++);
							Atom					*v=NULL;
							long unsigned int	n;
							v=(Atom*)this->mainClass->mainWindowClass->LFSWM2_getProp(this->mainClass->rootWindow,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_CURRENT_DESKTOP")),XA_CARDINAL,&n);

							this->LFSWM2_restack();
							if((v!=NULL) && (this->mainClass->currentDesktop!=(long unsigned int)v[0]))
								this->mainClass->mainWindowClass->LFSWM2_setVisibilityForDesk(this->mainClass->currentDesktop);

							this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_CURRENT_DESKTOP")),XA_CARDINAL,32,&this->mainClass->currentDesktop,1);

							this->mainClass->restackCnt=1;
							overide=false;
						}
				}
			XAllowEvents(this->mainClass->display,ReplayPointer,CurrentTime);
		}
}

void LFSWM2_eventsClass::LFSWM2_doClientMsg(Window id,XClientMessageEvent *e)
{
	LFSWM2_clientClass	*ccmessage;

#if 0
	fprintf(stderr,"message_type=%p\n",e->message_type);
	fprintf(stderr,"type=%p\n",e->type);
	fprintf(stderr,"serial=%p\n",e->serial);
	fprintf(stderr,"send_event=%p\n",e->send_event);
	fprintf(stderr,"window=%p\n",e->window);
	fprintf(stderr,"e->data.l[0]=%p\n",e->data.l[0]);
	fprintf(stderr,"e->data.l[1]=%p\n",e->data.l[1]);
	fprintf(stderr,"e->data.l[2]=%p\n",e->data.l[2]);
	fprintf(stderr,"e->data.l[3]=%p\n",e->data.l[3]);
	fprintf(stderr,"e->data.l[4]=%p\n",e->data.l[4]);
	fprintf(stderr,"e->data.l[1]name=");
	this->mainClass->DEBUG_printAtom(e->data.l[1]);
	this->mainClass->DEBUG_printAtom(e->message_type);
#endif

	if(e->message_type==this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_DESKTOP")) && e->format==32)
		{
			ccmessage=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->LFSWM2_getParentWindow(e->window));
			if(ccmessage!=NULL)
				{
					this->mainClass->mainWindowClass->LFSWM2_setProp(e->window,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_DESKTOP")),XA_CARDINAL,32,&e->data.l[0],1);
					ccmessage->onDesk=e->data.l[0];
					return;
				}
			else
				return;
		}

	if(e->message_type==this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_ACTIVE_WINDOW")) && e->format==32)
		{
			ccmessage=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e->window);
			if(ccmessage!=NULL)
				{
					this->mainClass->LFSWM2_pushXErrorHandler();
						this->mainClass->LFSWM2_setCurrentDesktop(ccmessage->onDesk);
						this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_ACTIVE_WINDOW")),XA_WINDOW,32,(void*)&ccmessage->contentWindow,1);
						this->mainClass->mainWindowClass->LFSWM2_removeProp(e->window,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_STATE_HIDDEN")));
						ccmessage->visible=true;
						XRaiseWindow(this->mainClass->display,ccmessage->contentWindow);
						XSetInputFocus(this->mainClass->display,ccmessage->contentWindow,RevertToNone,CurrentTime);
						//XSetInputFocus(this->mainClass->display,None,RevertToNone,0);
						this->mainClass->restackCnt=0;
					this->mainClass->LFSWM2_popXErrorHandler();

				}
			return;
		}

	if(e->message_type==this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_CURRENT_DESKTOP")) && e->format==32)
		{
			if(e->window==this->mainClass->rootWindow)
				{
					if((unsigned long)e->data.l[0]>this->mainClass->numberOfDesktops)
						return;

					if(this->mainClass->currentDesktop!=(unsigned long)e->data.l[0])
						{
							this->mainClass->currentDesktop=(unsigned long)e->data.l[0];
							this->mainClass->restackCnt++;
							this->noRestack=true;
							return;
						}
					this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_CURRENT_DESKTOP")),XA_CARDINAL,32,&this->mainClass->currentDesktop,1);
					this->mainClass->restackCnt++;
					this->noRestack=true;
					return;
				}
			return;
		}

	if(e->message_type==this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_CLOSE_WINDOW")) && e->format==32)
		{
			ccmessage=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->LFSWM2_getParentWindow(e->window));
			if(ccmessage!=NULL)
				ccmessage->LFSWM2_sendCloseWindow();
			return;
		}

	if((e->message_type==this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("WM_CHANGE_STATE"))) && (e->format==32))
		{
			if(e->data.l[0]==IconicState)
				{
					this->mainClass->mainWindowClass->LFSWM2_addState(e->window,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_STATE_HIDDEN")));
					this->mainClass->mainWindowClass->LFSWM2_reloadWindowState(e->window);
					this->mainClass->restackCnt++;
					return;
				}
		}

	if(e->message_type==this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_STATE")) && e->format==32)
		{
			this->mainClass->LFSWM2_pushXErrorHandler();

/*
Atom 0x14d name=_NET_WM_STATE
data[0]=0x1
Atom 0x1 name=PRIMARY
data[1]=0x14e
Atom 0x14e name=_NET_WM_STATE_ABOVE
data[2]=(nil)
Atom (nil) name=(null)
data[3]=0x2
Atom 0x2 name=SECONDARY
data[4]=(nil)
Atom (nil) name=(null)
*/
			if(e->data.l[1]==(long)this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_STATE_ABOVE")))
				{
					this->mainClass->mainWindowClass->LFSWM2_changeState(id,e->data.l[0],e->data.l[1]);
					ccmessage=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->LFSWM2_getParentWindow(id));
					if(ccmessage!=NULL)
						{
							if(e->data.l[0]==NET_WM_STATE_REMOVE)
								{
									ccmessage->onTop=false;
									goto exitit;
								}
									//if(ccmessage->frameWindow!=None)
							if(ccmessage->isBorderless==false)
								{
									XMapWindow(this->mainClass->display,ccmessage->frameWindow);
									XRaiseWindow(this->mainClass->display,ccmessage->frameWindow);
								}
							ccmessage->onTop=true;
							ccmessage->onBottom=false;
							this->mainClass->mainWindowClass->LFSWM2_changeState(ccmessage->contentWindow,NET_WM_STATE_REMOVE,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_STATE_BELOW")));
							this->LFSWM2_moveToTop(ccmessage->contentWindow);
							this->mainClass->restackCnt++;
						}
					goto exitit;
					return;
				}

			else if(e->data.l[1]==(long)this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_STATE_BELOW")))
				{
					this->mainClass->mainWindowClass->LFSWM2_changeState(id,e->data.l[0],e->data.l[1]);
					ccmessage=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->LFSWM2_getParentWindow(id));
					if(ccmessage!=NULL)
						{
							if(e->data.l[0]==NET_WM_STATE_REMOVE)
								{
									ccmessage->onBottom=false;
									goto exitit;
								}
//	if(ccmessage->frameWindow!=None)
							if(ccmessage->isBorderless==false)
								XLowerWindow(this->mainClass->display,ccmessage->frameWindow);
							ccmessage->onBottom=true;
							this->mainClass->restackCnt++;
						}
					goto exitit;
				}
			else if(e->data.l[1]==(long)this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_STATE_FULLSCREEN")) && e->format==32)
				{
					ccmessage=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->LFSWM2_getParentWindow(e->window));
					if(ccmessage!=NULL)
						{
							this->mainClass->mainWindowClass->LFSWM2_changeState(id,e->data.l[0],e->data.l[1]);
						}
					goto exitit;
				}

			{
				int how=e->data.l[0];
				for (int i=1; i <= 2; i++)
					{
						if (e->data.l[i] != 0)
							this->mainClass->mainWindowClass->LFSWM2_changeState(id,how,e->data.l[i]);
					}
			}
exitit://TODO//just to even up poperror to be removed.
			this->mainClass->LFSWM2_popXErrorHandler();
		}
	this->mainClass->mainWindowClass->LFSWM2_reloadWindowState(id);
}

void LFSWM2_eventsClass::LFSWM2_sendConfigureEvent(Window wid,rectStruct r)
{
	XConfigureEvent	ce;

	ce.type=ConfigureNotify;
	ce.event=wid;
	ce.window=wid;
	ce.x=r.x;
	ce.y=r.y;
	ce.width=r.w;
	ce.height=r.h;
	ce.border_width=2;//TODO//
	ce.above=None;
	ce.override_redirect=False;
	ce.send_event=true;

	XSendEvent(this->mainClass->display,wid,true,StructureNotifyMask,(XEvent*)&ce);
}

void LFSWM2_eventsClass::LFSWM2_restack(void)//TODO// still dont like this code
{
	std::vector<Window>	fromwl;
	std::vector<Window>	towldesktop;
	std::vector<Window>	towlbelow;
	std::vector<Window>	towlnormal;
	std::vector<Window>	towlabove;
	std::vector<Window>	towlpanel;
	std::vector<Window>	towlmenu;
	std::vector<Window>	towl;
	std::vector<Window>	netclientwl;
	std::vector<Window>	netclientstackwl;

	long unsigned int	nitems_return;
	int					wtype;
	Atom					*v=NULL;
	LFSWM2_clientClass	*cc=NULL;
	int					a=0;

	v=(Atom*)this->mainClass->mainWindowClass->LFSWM2_getProp(this->mainClass->rootWindow,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_ACTIVE_WINDOW")),XA_WINDOW,&nitems_return);



	if((v!=NULL) && (v[0]!=None))
		{
			hintsDataStruct		hs;
			hs=this->mainClass->mainWindowClass->LFSWM2_getWindowHints(v[0]);
			if((hs.mHints!=NULL) && (hs.mHints->decorations==0))
				{
					this->mainClass->mainWindowClass->LFSWM2_removeProp(this->mainClass->rootWindow,this->mainClass->prefs.LFSTK_hashFromKey("_NET_ACTIVE_WINDOW"));
					v[0]=0;
				}
		}
						
	for(int a=0;a<this->mainClass->mainWindowClass->windowIDList.size();a++)
		{
			cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->windowIDList.at(a));
			if(cc!=NULL)
				{
					if(cc->rendered==false)
						continue;
					cc->doneRestack=false;
					if((v!=NULL) && (cc->contentWindow==v[0]) && (cc->isActive==false))
						{
							cc->isActive=true;
							cc->mainClass->mainWindowClass->LFSWM2_refreshFrame(cc);
							move(this->mainClass->mainWindowClass->windowIDList,a,0);
						}
					else if((v!=NULL) && (cc->contentWindow!=v[0]) && (cc->isActive==true))
						{
							cc->isActive=false;
							cc->mainClass->mainWindowClass->LFSWM2_refreshFrame(cc);
						}
				}
		}

	for(int j=0;j<this->mainClass->mainWindowClass->windowIDList.size();j++)
		{
			cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->windowIDList.at(j));
			if(cc!=NULL)
				{
					cc->doneRestack=false;
				}
		}

//move transients
	a=0;
	while(a<this->mainClass->mainWindowClass->windowIDList.size())
		{
			cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->windowIDList.at(a));
			if(cc!=NULL)
				{
					if((cc->transientFor!=0) && (cc->doneRestack==false))
						{
							for(int b=0;b<this->mainClass->mainWindowClass->windowIDList.size();b++)
								{
									if((this->mainClass->mainWindowClass->windowIDList.at(b)==cc->transientFor) && ((v!=NULL) && (this->mainClass->mainWindowClass->windowIDList.at(b)==v[0])))
										{
											cc->doneRestack=true;
											move(this->mainClass->mainWindowClass->windowIDList,a,0);
											a=0;
											continue;
										}
								}
						}
				}
			a++;
		}

	if(v!=NULL)
		XFree(v);

	fromwl=this->mainClass->mainWindowClass->windowIDList;
	towldesktop.clear();
	towlbelow.clear();
	towlnormal.clear();
	towlabove.clear();
	towlpanel.clear();
	towlmenu.clear();
	netclientwl.clear();
	netclientstackwl.clear();
	towl.clear();

	for(int j=0;j<fromwl.size();j++)
		{
			wtype=this->mainClass->mainWindowClass->LFSWM2_getWindowType(fromwl.at(j));
			switch(wtype)
				{
					case MENUWINDOW:
						//menus
						towlmenu.push_back(fromwl.at(j));
						break;
					case DESKTOPWINDOW:
						//desktop windows
						towldesktop.push_back(fromwl.at(j));
						break;
					case NORMALWINDOW:
					case DIALOGWINDOW:
					case TOOLWINDOW:
						if(this->mainClass->mainWindowClass->LFSWM2_hasState(fromwl.at(j),this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_STATE_BELOW"))))
							{
								cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(fromwl.at(j));
								if(cc!=NULL)
									towlbelow.push_back(cc->frameWindow);
								else
									towlbelow.push_back(fromwl.at(j));
								break;
							}
						if(this->mainClass->mainWindowClass->LFSWM2_hasState(fromwl.at(j),this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_STATE_ABOVE"))))
							{
								cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(fromwl.at(j));
								if(cc!=NULL)
									towlabove.push_back(cc->frameWindow);	
								else
									towlabove.push_back(fromwl.at(j));	
								break;
							}
						cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(fromwl.at(j));
							if(cc!=NULL)
								towlnormal.push_back(cc->frameWindow);	
							else
								towlnormal.push_back(fromwl.at(j));	
						break;
					case DOCKWINDOW:
						//panels
						towlpanel.push_back(fromwl.at(j));
						break;
				}
		}

	if(towlmenu.size()!=0)
		{
			towl=towlmenu;
			towl.insert(towl.end(),towlpanel.begin(),towlpanel.end());
		}
	else
		towl=towlpanel;

//	towl.insert(towl.end(),towlpanel.begin(),towlpanel.end());
	towl.insert(towl.end(),towlabove.begin(),towlabove.end());
	towl.insert(towl.end(),towlnormal.begin(),towlnormal.end());
	towl.insert(towl.end(),towlbelow.begin(),towlbelow.end());
	towl.insert(towl.end(),towldesktop.begin(),towldesktop.end());

	XRestackWindows(this->mainClass->display,towl.data(),towl.size());

	for(int j=0;j<towl.size();j++)
		{
			cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(fromwl.at(j));
			if(cc!=NULL)
				{
					if(cc->isWithdrawn==false)
						netclientwl.push_back(cc->contentWindow);
				}
			else
				{
					if(this->mainClass->mainWindowClass->LFSWM2_getWindowType(fromwl.at(j))!=DESKTOPWINDOW)
						netclientwl.push_back(fromwl.at(j));
				}
		}
	this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_CLIENT_LIST")),XA_WINDOW,32,netclientwl.data(),netclientwl.size());


	for(int j=towl.size()-1;j>-1;j--)
		{
			cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(towl.at(j));
			if(cc!=NULL)
				{
					if(cc->isWithdrawn==false)
						netclientstackwl.push_back(cc->contentWindow);
				}
			else
				{
					if(this->mainClass->mainWindowClass->LFSWM2_getWindowType(towl.at(j))!=DESKTOPWINDOW)
						netclientstackwl.push_back(towl.at(j));
				}
		}
	this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_CLIENT_LIST_STACKING")),XA_WINDOW,32,netclientstackwl.data(),netclientstackwl.size());

}

void LFSWM2_eventsClass::LFSWM2_moveToTop(Window id)
{
	for(long unsigned j=0;j<this->mainClass->mainWindowClass->windowIDList.size();j++)
		{
			if(this->mainClass->mainWindowClass->windowIDList.at(j)==id)
				{
					this->mainClass->mainWindowClass->windowIDList.erase(this->mainClass->mainWindowClass->windowIDList.begin()+j);
					this->mainClass->mainWindowClass->windowIDList.emplace (this->mainClass->mainWindowClass->windowIDList.begin(),id);
				}
		}
}

void LFSWM2_eventsClass::LFSWM2_moveToBottom(Window id)
{
	for(long unsigned j=0;j<this->mainClass->mainWindowClass->windowIDList.size();j++)
		{
			if(this->mainClass->mainWindowClass->windowIDList.at(j)==id)
				{
					this->mainClass->mainWindowClass->windowIDList.erase(this->mainClass->mainWindowClass->windowIDList.begin()+j);
					this->mainClass->mainWindowClass->windowIDList.emplace (this->mainClass->mainWindowClass->windowIDList.end(),id);
				}
		}
}