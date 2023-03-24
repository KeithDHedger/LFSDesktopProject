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

//this->mainClass->DEBUG_printEventData(&e,false);
			if(this->mainClass->messages->whatMsg==QUITLFSWM)
				break;

			XNextEvent(this->mainClass->display,&e);
			cccontrol=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->LFSWM2_getParentWindow(e.xany.window));
			if(cccontrol!=NULL)
				{
					if(cccontrol->LFSWM2_handleControls(&e)==true)
						continue;
				}
			cccontrol=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e.xany.window);
			if(cccontrol!=NULL)
				{
					overide=false;
					if(cccontrol->LFSWM2_handleEvents(&e)==true)
						{
							overide=true;
							this->mainClass->doingMove=false;
							cccontrol=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e.xany.window);//in case window has died
							if(cccontrol!=NULL)
								cccontrol->adjustContentWindow();
							cccontrol=NULL;
							e.type=0;
							//continue;
						}
				}

			switch(e.type)
				{
					case ButtonRelease:
						fprintf(stderr,"ButtonRelease eventnumber %i\n",when++);
						start.subwindow=None;
						cc=NULL;
						this->mainClass->doingMove=false;
						break;
					case ButtonPress:
						//fprintf(stderr,"ButtonPress eventnumber %i\n",when++);
						start=e.xbutton;
						this->mainClass->doingMove=false;
						this->mainClass->restackCnt=0;
						this->sy=e.xbutton.y;
						cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e.xbutton.window);
						if(cc!=NULL)
							{
								inmenu=false;
								if((e.xbutton.state&Mod4Mask)==Mod4Mask)//TODO//???windows key for now used to move window wihout restacking
									break;
								this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atoms.at("_NET_ACTIVE_WINDOW"),XA_WINDOW,32,&cc->contentWindow,1);
								XSetInputFocus(this->mainClass->display,cc->contentWindow,RevertToNone,CurrentTime);
								XAllowEvents(this->mainClass->display,ReplayPointer,e.xbutton.time);
							}
						break;
					case MapNotify:
						{
						//fprintf(stderr,"MapNotify >>>>>>>>>>>>>>>>>window=%x when=%i\n",e.xmap.window,when++);
							Atom					*v=NULL;
							long unsigned int	nitems_return;

							if(overide==false)
								this->mainClass->restackCnt=4;
							v=(Atom*)this->mainClass->mainWindowClass->LFSWM2_getProp(this->mainClass->rootWindow,this->mainClass->atoms.at("_NET_ACTIVE_WINDOW"),XA_WINDOW,&nitems_return);
							if(v!=NULL)
								{
									if(e.xmap.window!=v[0])
										inmenu=true;
									XFree(v);
								}
						}
						break;
					case MapRequest:
						//fprintf(stderr,"MapRequest >>>>>>>>>>>>>>>>>window=%x when=%i\n",e.xmaprequest.window,when++);
							cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e.xmaprequest.window);
							if(cc==NULL)
								{
									hintsDataStruct	hs=this->mainClass->mainWindowClass->LFSWM2_getWindowHints(e.xmaprequest.window,true);
									XFree(hs.sh);
								}

						XMapWindow(this->mainClass->display,e.xmaprequest.window);
						this->mainClass->mainWindowClass->LFSWM2_createClient(e.xmaprequest.window);
						XRaiseWindow(this->mainClass->display,e.xmaprequest.window);
						break;

					case 2000://TODO//NEXT
						cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e.xconfigurerequest.window);
						if((cc==NULL))
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
							}
						break;


					case ConfigureRequest://TODO//NEXT
					{
					//break;
						fprintf(stderr,"ConfigureRequest from event>>>>>>>>>>>>>>>>>window=%x when=%i\n",e.xmaprequest.window,when++);
this->mainClass->DEBUG_printBinary(e.xconfigurerequest.value_mask);
								cerr<<e.xconfigurerequest.value_mask<<" "<<e.xconfigurerequest.send_event<<endl;
						
						if(e.xconfigurerequest.send_event==false)
						{
	LFSWM2_clientClass	*cc;
cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e.xconfigurerequest.window);
						if(cc!=NULL)
							{
							//if((e.xconfigurerequest.value_mask & (CWWidth|CWHeight|CWX|CWY)) == (CWWidth|CWHeight|CWX|CWY))
							if((e.xconfigurerequest.value_mask & (CWWidth|CWHeight|CWX|CWY)) !=0)
							{
							 Window root_return, child_return;
      int root_x_return, root_y_return;
      int win_x_return, win_y_return;
      unsigned int mask_return;
					 		XQueryPointer(this->mainClass->display,cc->frameWindow, &root_return, &child_return, &root_x_return, &root_y_return, & win_x_return, &win_y_return, &mask_return);
							cerr<<"(CWWidth|CWHeight|CWX|CWY) cy="<<std::dec<<e.xconfigurerequest.y<<" sy="<<this->sy<<"root_y_return="<<root_y_return<<" win_y_return="<<win_y_return<<endl;
						XWindowChanges ch;
						ch.x=e.xconfigurerequest.x;
						ch.y=e.xconfigurerequest.y;
						ch.width=e.xconfigurerequest.width;
						ch.height=e.xconfigurerequest.height;
							//if(e.xconfigurerequest.value_mask & CWX
							XConfigureWindow(this->mainClass->display,cc->contentWindow,e.xconfigurerequest.value_mask& (CWWidth|CWHeight),&ch);
						ch.width=e.xconfigurerequest.width+4;
						ch.height=e.xconfigurerequest.height+28;
						if(win_y_return==this->sy)
						{
							this->sy=win_y_return-20;
							ch.y-=20;
						}
						//ch.x-=2;
							XConfigureWindow(this->mainClass->display,cc->frameWindow,e.xconfigurerequest.value_mask,&ch);
							//if((e.xconfigurerequest.value_mask & (CWWidth|CWHeight)) != (CWWidth|CWHeight))
							//if((e.xconfigurerequest.value_mask & (CWWidth|CWHeight)) != (CWWidth|CWHeight))
							//	cc->setWindowRects(true);
							//cc->adjustContentWindow();
							}
//						else if((e.xconfigurerequest.value_mask & (CWWidth|CWHeight)) == (CWWidth|CWHeight))
//						{
//						cerr<<"here "<<(e.xconfigurerequest.value_mask & (CWWidth|CWHeight))<<"(CWWidth|CWHeight)"<<endl;
//						XWindowChanges ch;
//						ch.x=e.xconfigurerequest.x;
//						ch.y=e.xconfigurerequest.y;
//						ch.width=e.xconfigurerequest.width;
//						ch.height=e.xconfigurerequest.height;
//						ch.width=e.xconfigurerequest.width+4;
//						ch.height=e.xconfigurerequest.height+28;
//						XConfigureWindow(this->mainClass->display,cc->frameWindow,(e.xconfigurerequest.value_mask& (CWWidth|CWHeight)),&ch);
//						ch.width=e.xconfigurerequest.width;
//						ch.height=e.xconfigurerequest.height;
//						XConfigureWindow(this->mainClass->display,cc->contentWindow,(e.xconfigurerequest.value_mask& (CWWidth|CWHeight)),&ch);
//						//cc->setWindowRects(true);
//					
//						}
//						else if((e.xconfigurerequest.value_mask & (CWX|CWY)) == (CWX|CWY))
//						{
//						cerr<<">>>>>>>>>>>here "<<(e.xconfigurerequest.value_mask & (CWWidth|CWHeight))<<"(CWX|CWY)"<<endl;
//						XWindowChanges ch;
//						ch.x=e.xconfigurerequest.x;
//						ch.y=e.xconfigurerequest.y;
//						
//							XConfigureWindow(this->mainClass->display,cc->frameWindow,e.xconfigurerequest.value_mask,&ch);
//							cc->setWindowRects(true);
//						}
					if((e.xconfigurerequest.value_mask & (CWX|CWY)) == (CWX|CWY))
						cc->setWindowRects(true);
						
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






						cerr<<"no cc"<<endl;
						 if((e.xconfigurerequest.value_mask & (CWX|CWY)) == (CWX|CWY))
						 {
						XWindowChanges ch;
						ch.x=e.xconfigurerequest.x;
						ch.y=e.xconfigurerequest.y;
						
							XConfigureWindow(this->mainClass->display,e.xconfigurerequest.window,e.xconfigurerequest.value_mask,&ch);
							}
						else if((e.xconfigurerequest.value_mask & (CWWidth|CWHeight)) == (CWWidth|CWHeight))
						{
						XWindowChanges ch;
						ch.x=e.xconfigurerequest.x;
						ch.y=e.xconfigurerequest.y;
						ch.width=e.xconfigurerequest.width;
						ch.height=e.xconfigurerequest.height;
						ch.width=e.xconfigurerequest.width+4;
						ch.height=e.xconfigurerequest.height+28;
						XConfigureWindow(this->mainClass->display,e.xconfigurerequest.parent,e.xconfigurerequest.value_mask,&ch);
						ch.width=e.xconfigurerequest.width;
						ch.height=e.xconfigurerequest.height;
						XConfigureWindow(this->mainClass->display,e.xconfigurerequest.window,e.xconfigurerequest.value_mask& (CWWidth|CWHeight),&ch);						}
						
						}
						}
					cerr<<"ConfigureRequest from event<<<<<<<<<<<<<<<<<<<<< when="<<when<<endl;
break;
}
if(e.xconfigurerequest.send_event==false)
								{
								this->mainClass->DEBUG_printBinary(e.xconfigurerequest.value_mask);
								//break;
								if(e.xconfigurerequest.value_mask>0x100)
									break;
								
	LFSWM2_clientClass	*cc;
cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e.xconfigurerequest.window);
						if(cc!=NULL)
							{
								e.xconfigurerequest.parent=cc->frameWindow;
								e.xconfigurerequest.window=cc->contentWindow;
								e.xconfigurerequest.type=ConfigureRequest;
								e.xconfigurerequest.value_mask=CWWidth|CWHeight|CWX|CWY|CWMyframe;
								e.xconfigurerequest.width=e.xconfigurerequest.width-4;
								e.xconfigurerequest.height=e.xconfigurerequest.height-28;

								e.xconfigurerequest.send_event=true;
								//XSendEvent(this->mainClass->display,cc->contentWindow,true,SubstructureNotifyMask,(XEvent*)&e);
								XSendEvent(this->mainClass->display,cc->contentWindow,true,StructureNotifyMask,(XEvent*)&e);
								XSync(this->mainClass->display,false);
								e.type=0;
								continue;
								break;
								
								
	XConfigureRequestEvent	ce;
						cerr<<"w="<<e.xconfigurerequest.width<<" h="<<e.xconfigurerequest.height<<endl;
								ce.display=cc->mainClass->display;
								ce.parent=cc->frameWindow;
								ce.window=cc->contentWindow;
								ce.type=ConfigureRequest;
								ce.serial=0;
								ce.send_event=false;
								ce.border_width=BORDER_WIDTH;//TODO//
								ce.value_mask=CWWidth|CWHeight|CWX|CWY|CWMyframe;
								//ce.value_mask=CWWidth|CWHeight|CWX|CWY;
								ce.value_mask=CWWidth|CWHeight|CWMyframe;
								ce.detail=Above;
								ce.above=None;
								ce.x=e.xconfigurerequest.x;
								ce.y=e.xconfigurerequest.y;
								ce.width=e.xconfigurerequest.width;
								ce.height=e.xconfigurerequest.height;

								XSendEvent(this->mainClass->display,cc->contentWindow,true,StructureNotifyMask,(XEvent*)&ce);
								continue;
}
}						
						break;
						cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e.xconfigurerequest.window);
								XWindowChanges		changes;
								changes.x=e.xconfigurerequest.x;
								changes.y=e.xconfigurerequest.y;
								changes.width=e.xconfigurerequest.width;
								changes.height=e.xconfigurerequest.height;
								changes.border_width=e.xconfigurerequest.border_width;
								changes.sibling=e.xconfigurerequest.above;
								changes.stack_mode=e.xconfigurerequest.detail;
						if((cc==NULL))
							{

								XConfigureWindow(this->mainClass->display,e.xconfigurerequest.window,e.xconfigurerequest.value_mask,&changes);
								this->mainClass->restackCnt=1;
								break;
							}
						else
							{
							if(e.xconfigurerequest.send_event==false)
								{
								fprintf(stderr,"changes.x=%i y=%i\n",changes.x,changes.y);
								fprintf(stderr,"send=%p\n",e.xmaprequest.send_event);
									
								changes.width+=6;
								changes.height+=28;
									//XMoveResizeWindow(this->mainClass->display,cc->frameWindow,changes.x,changes.y,changes.width,changes.height);
									//XConfigureWindow(this->mainClass->display,e.xconfigurerequest.parent,e.xconfigurerequest.value_mask,&changes);
									//cc->adjustContentWindow();
									XMoveResizeWindow(this->mainClass->display,e.xconfigurerequest.window,changes.x,changes.y,changes.width,changes.height);
									//cc->setWindowRects(true);
								}
							}
						break;
					case PropertyNotify:
						{
							//fprintf(stderr,"PropertyNotify IN eventnumber %i atom name=%s\n",when++,XGetAtomName(this->mainClass->display,e.xproperty.atom));
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
									if(e.xproperty.atom==this->mainClass->atoms.at("_NET_WM_DESKTOP"))
										{
											if(e.xproperty.window==this->mainClass->rootWindow)
												{
												}
											break;
										}

									
									if(e.xproperty.atom==this->mainClass->atoms.at("_NET_WM_STATE"))
										{
											//fprintf(stderr,"PropertyNotify OUT WM_STATE eventnumber %i\n",when++);
											cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e.xproperty.window);
											if(cc!=NULL)
												{
													cc->LFSWM2_setWMState(&e);
													break;
												}
										}

									if(e.xproperty.atom==this->mainClass->atoms.at("_NET_WM_NAME"))
										{	
											cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e.xproperty.window);
											if(cc!=NULL)
												{
													cc->LFSWM2_setWindowName();
													cc->LFSWM2_refreshFrame(NULL);
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
						this->LFSWM2_doClientMsg(e.xclient.window,&e.xclient);
						break;
					default:
						if(overide==false)
							this->mainClass->restackCnt=2;
						break;
				}

			if(this->mainClass->doingMove==false)
				{
					this->mainClass->restackCnt--;
					if(this->mainClass->restackCnt<1)
						{
							Atom					*v=NULL;
							long unsigned int	n;
							v=(Atom*)this->mainClass->mainWindowClass->LFSWM2_getProp(this->mainClass->rootWindow,this->mainClass->atoms.at("_NET_CURRENT_DESKTOP"),XA_CARDINAL,&n);
							if(v!=NULL)
								{
									if((long unsigned int)v[0]!=this->mainClass->currentDesktop)
										inmenu=false;
									XFree(v);
								}
					
							this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atoms.at("_NET_CURRENT_DESKTOP"),XA_CARDINAL,32,&this->mainClass->currentDesktop,1);
					//fprintf(stderr,"restack = %i inmenu=%s cd=%p x=%p\n",this->mainClass->restackCnt,this->mainClass->DEBUG_printBool(inmenu),this->mainClass->currentDesktop,x);
							this->LFSWM2_restack();
							this->mainClass->mainWindowClass->LFSWM2_setVisibilityForDesk(this->mainClass->currentDesktop);
							if(inmenu==false)
								{
									v=(Atom*)this->mainClass->mainWindowClass->LFSWM2_getProp(this->mainClass->rootWindow,this->mainClass->atoms.at("_NET_ACTIVE_WINDOW"),XA_WINDOW,&n);
									if(v!=NULL)
										{
											if(this->mainClass->mainWindowClass->LFSWM2_getWindowType(v[0])!=UNKNOWNTYPE)
												XSetInputFocus(this->mainClass->display,v[0],RevertToNone,CurrentTime);
											XFree(v);
										}
								}

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

//	fprintf(stderr,"message_type=%p\n",e->message_type);
//	fprintf(stderr,"type=%p\n",e->type);
//	fprintf(stderr,"serial=%p\n",e->serial);
//	fprintf(stderr,"send_event=%p\n",e->send_event);
//	fprintf(stderr,"window=%p\n",e->window);
//	fprintf(stderr,"e->data.l[0]=%p\n",e->data.l[0]);
//	fprintf(stderr,"e->data.l[1]=%p\n",e->data.l[1]);
//	fprintf(stderr,"e->data.l[2]=%p\n",e->data.l[2]);
//	fprintf(stderr,"e->data.l[3]=%p\n",e->data.l[3]);
//	fprintf(stderr,"e->data.l[4]=%p\n",e->data.l[4]);
//	fprintf(stderr,"e->data.l[1]name=");
//	this->mainClass->DEBUG_printAtom(e->data.l[1]);
//	this->mainClass->DEBUG_printAtom(e->message_type);

	if(e->message_type==this->mainClass->atoms.at("_NET_WM_DESKTOP") && e->format==32)
		{
			ccmessage=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->LFSWM2_getParentWindow(e->window));
			if(ccmessage!=NULL)
				{
					this->mainClass->mainWindowClass->LFSWM2_setProp(e->window,this->mainClass->atoms.at("_NET_WM_DESKTOP"),XA_CARDINAL,32,&e->data.l[0],1);
					ccmessage->onDesk=e->data.l[0];
					return;
				}
			else
				return;
		}

	if(e->message_type==this->mainClass->atoms.at("_NET_ACTIVE_WINDOW") && e->format==32)
		{
	//	fprintf(stderr,"active=%p\n",e->window);
			//ccmessage=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->LFSWM2_getParentWindow(e->window));
			ccmessage=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e->window);
			if(ccmessage!=NULL)
				{
					this->mainClass->LFSWM2_pushXErrorHandler();
						this->mainClass->LFSWM2_setCurrentDesktop(ccmessage->onDesk);
						this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atoms.at("_NET_ACTIVE_WINDOW"),XA_WINDOW,32,(void*)&ccmessage->contentWindow,1);
						this->mainClass->mainWindowClass->LFSWM2_removeProp(e->window,this->mainClass->atoms.at("_NET_WM_STATE_HIDDEN"));
						ccmessage->visible=true;
						XRaiseWindow(this->mainClass->display,ccmessage->contentWindow);
						XSetInputFocus(this->mainClass->display,ccmessage->contentWindow,RevertToPointerRoot,CurrentTime);
						this->mainClass->restackCnt=0;
					this->mainClass->LFSWM2_popXErrorHandler();

				}
			return;
		}

	if(e->message_type==this->mainClass->atoms.at("_NET_CURRENT_DESKTOP") && e->format==32)
		{
			if(e->window==this->mainClass->rootWindow)
				{
					if((unsigned long)e->data.l[0]>this->mainClass->numberOfDesktops)
						return;

					if(this->mainClass->currentDesktop!=(unsigned long)e->data.l[0])
						{
							this->mainClass->currentDesktop=(unsigned long)e->data.l[0];
							this->mainClass->restackCnt++;
							return;
						}
					this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atoms.at("_NET_CURRENT_DESKTOP"),XA_CARDINAL,32,&this->mainClass->currentDesktop,1);
					this->mainClass->restackCnt++;
					return;
				}
			return;
		}

	if(e->message_type==this->mainClass->atoms.at("_NET_CLOSE_WINDOW") && e->format==32)
		{
			ccmessage=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->LFSWM2_getParentWindow(e->window));
			if(ccmessage!=NULL)
				ccmessage->LFSWM2_sendCloseWindow();
			return;
		}

	if((e->message_type==this->mainClass->atoms.at("WM_CHANGE_STATE")) && (e->format==32))
		{
			if(e->data.l[0]==IconicState)
				{
					this->mainClass->mainWindowClass->LFSWM2_addState(e->window,this->mainClass->atoms.at("_NET_WM_STATE_HIDDEN"));
					this->mainClass->mainWindowClass->LFSWM2_reloadWindowState(e->window);
					this->mainClass->restackCnt++;
					return;
				}
		}

	if(e->message_type==this->mainClass->atoms.at("_NET_WM_STATE") && e->format==32)
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
			if(e->data.l[1]==(long)this->mainClass->atoms.at("_NET_WM_STATE_ABOVE"))
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
							XMapWindow(this->mainClass->display,ccmessage->frameWindow);
							XRaiseWindow(this->mainClass->display,ccmessage->frameWindow);
							ccmessage->onTop=true;
							ccmessage->onBottom=false;
							this->mainClass->mainWindowClass->LFSWM2_changeState(ccmessage->contentWindow,NET_WM_STATE_REMOVE,this->mainClass->atoms.at("_NET_WM_STATE_BELOW"));
							this->LFSWM2_moveToTop(ccmessage->contentWindow);
							this->mainClass->restackCnt++;
						}
					goto exitit;
					return;
				}

			else if(e->data.l[1]==(long)this->mainClass->atoms.at("_NET_WM_STATE_BELOW"))
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
							XLowerWindow(this->mainClass->display,ccmessage->frameWindow);
							ccmessage->onBottom=true;
							this->mainClass->restackCnt++;
						}
					goto exitit;
				}
			else if(e->data.l[1]==(long)this->mainClass->atoms.at("_NET_WM_STATE_FULLSCREEN") && e->format==32)
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

void LFSWM2_eventsClass::LFSWM2_restack(void)
{
	LFSWM2_clientClass	*ccs;
	std::vector<Window>	sl;
	std::vector<Window>	cl;
	Atom					*v=NULL;
	long unsigned int	nitems_return;
	int					middleindex=0;
	int					bottomindex=0;
	int					topindex=0;

	cl.clear();
	sl.clear();

	v=(Atom*)this->mainClass->mainWindowClass->LFSWM2_getProp(this->mainClass->rootWindow,this->mainClass->atoms.at("_NET_ACTIVE_WINDOW"),XA_WINDOW,&nitems_return);

	for(long unsigned j=0;j<this->mainClass->mainWindowClass->windowIDList.size();j++)
		{
			ccs=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->windowIDList.at(j));
			if((ccs!=NULL) && ((v!=NULL)))
				{
					if(ccs->contentWindow==v[0])//TODO//oribble!!!!!
						{
							if((ccs->onBottom==false))
								{
									sl.insert(sl.begin()+topindex,ccs->frameWindow);
									cl.insert(cl.begin()+topindex,ccs->contentWindow);
									middleindex++;
									bottomindex++;
									continue;
								}
							else if((ccs->onTop==true) )
								{
									sl.insert(sl.begin()+middleindex,ccs->frameWindow);
									cl.insert(cl.begin()+middleindex,ccs->contentWindow);
									topindex=middleindex;
									middleindex++;
									bottomindex++;
									continue;
								}
						}

					if(ccs->onTop==true)
						{
							sl.insert(sl.begin()+middleindex,ccs->frameWindow);
							cl.insert(cl.begin()+middleindex,ccs->contentWindow);
							topindex++;
							middleindex++;
							bottomindex++;
							continue;
						}

					if((ccs->onBottom==false) && (ccs->onTop==false))
						{
							sl.insert(sl.begin()+middleindex,ccs->frameWindow);
							cl.insert(cl.begin()+middleindex,ccs->contentWindow);
							middleindex++;
							bottomindex++;
							continue;
						}

					if(ccs->onBottom==true)
						{
							sl.insert(sl.begin()+bottomindex,ccs->frameWindow);
							cl.insert(cl.begin()+bottomindex,ccs->contentWindow);
							bottomindex++;
							continue;
						}
				}
		}

	for(long unsigned j=0;j<this->mainClass->mainWindowClass->windowIDList.size();j++)
		{
			if(this->mainClass->mainWindowClass->LFSWM2_getWindowType(this->mainClass->mainWindowClass->windowIDList.at(j))==DOCKWINDOW)
				{
					sl.emplace(sl.begin(),this->mainClass->mainWindowClass->windowIDList.at(j));
					cl.emplace(cl.begin(),this->mainClass->mainWindowClass->windowIDList.at(j));
					continue;
				}
			if(this->mainClass->mainWindowClass->LFSWM2_getWindowType(this->mainClass->mainWindowClass->windowIDList.at(j))==MENUWINDOW)
				{
					sl.emplace(sl.begin(),this->mainClass->mainWindowClass->windowIDList.at(j));
					cl.emplace(cl.begin(),this->mainClass->mainWindowClass->windowIDList.at(j));
					continue;
				}
			if(this->mainClass->mainWindowClass->LFSWM2_getWindowType(this->mainClass->mainWindowClass->windowIDList.at(j))==DESKTOPWINDOW)
				{
					sl.push_back(this->mainClass->mainWindowClass->windowIDList.at(j));
					cl.push_back(this->mainClass->mainWindowClass->windowIDList.at(j));
					continue;
				}
		}

	XRestackWindows(this->mainClass->display,sl.data(),sl.size());

	this->mainClass->mainWindowClass->windowIDList=cl;
	this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atoms.at("_NET_CLIENT_LIST"),XA_WINDOW,32,cl.data(),cl.size());
	this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atoms.at("_NET_CLIENT_LIST_STACKING"),XA_WINDOW,32,cl.data(),cl.size());
	//this->mainClass->mainWindowClass->LFSWM2_setVisibilityForDesk(this->mainClass->currentDesktop);
	XFree(v);
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