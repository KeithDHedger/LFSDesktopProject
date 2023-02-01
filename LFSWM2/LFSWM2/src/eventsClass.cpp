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

//				this->LFSWM2_restack();
/*
	this->mainClass->mainEventClass->LFSWM2_restack();

	XSync(this->mainClass->display,false);
	while(XPending(this->mainClass->display)!=0)
		{
			XSync(this->mainClass->display,false);
		}

*/

	while(true)
		{
			XEvent e;
			if(firstrun==true)
				{
					if(XPending(this->mainClass->display)==0)
						{
							this->LFSWM2_restack();
							firstrun=false;
						}
				}

			if(this->mainClass->messages->whatMsg==QUITLFSWM)
				break;

			XNextEvent(this->mainClass->display,&e);

//this->mainClass->messages->LFSWM2_readMsg(this->mainClass->messages);

//if(e.xany.window=this->mainClass->rootWindow)
//	fprintf(stderr,"in root\n");
///fprintf(stderr,">>e.type==%p window=%p\n",e.type,e.xany.window);
			cccontrol=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->LFSWM2_getParentWindow(e.xany.window));
			if(cccontrol!=NULL)
				{
					if(cccontrol->LFSWM2_handleControls(&e)==true)
						continue;
				}

			switch(e.type)
				{
					case EnterNotify:
						//fprintf(stderr,"rootwin=%x child of event window=%x event window=%x\n",e.xbutton.root,e.xbutton.subwindow,e.xbutton.window);
						//fprintf(stderr,"EnterNotify\n");
						break;
					case LeaveNotify:
						//fprintf(stderr,"rootwin=%x child of event window=%x event window=%x\n",e.xbutton.root,e.xbutton.subwindow,e.xbutton.window);
						//fprintf(stderr,"LeaveNotify\n");
						break;
					case KeyPress:
						//fprintf(stderr,"KeyPress eventnumber %i\n",when++);
						if(e.xbutton.subwindow!=0)
							XRaiseWindow(this->mainClass->display,e.xkey.subwindow);
						break;
					case KeyRelease:
						//fprintf(stderr,"KeyRelease eventnumber %i\n",when++);
						break;
					case ButtonRelease:
						start.subwindow=None;
						cc=NULL;
						this->skipnext=false;
						break;
						//fprintf(stderr,"ButtonRelease eventnumber %i\n",when++);
						break;
					case ButtonPress:
						this->skipnext=false;

						//fprintf(stderr,"ButtonPress eventnumber %i\n",when++);
						start=e.xbutton;
						cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e.xbutton.window);
						if(cc!=NULL)
							{
								XGetWindowAttributes(this->mainClass->display,cc->frameWindow, &attr);
								if((e.xbutton.state&Mod4Mask)==Mod4Mask)
									break;
								XRaiseWindow(this->mainClass->display,cc->frameWindow);
								XSetInputFocus(this->mainClass->display,cc->contentWindow,RevertToParent,CurrentTime);
								this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atoms.at("_NET_ACTIVE_WINDOW"),XA_WINDOW,32,&cc->contentWindow,1);
								if(cc->onBottom==false)
									this->LFSWM2_moveToTop(cc->contentWindow);
								//cc->onBottom=false;
								//TODO//this->LFSWM2_restack();
//this->LFSWM2_restack();
						XAllowEvents(this->mainClass->display,ReplayPointer,e.xbutton.time);
						XSync(this->mainClass->display,false);
							}
						break;
					case MotionNotify:
						{
							bool domove=false;
							if(cc!=NULL)
								{
								if((cc->frameWindow==e.xmotion.window) && (e.xmotion.state==Button1Mask))
									domove=true;

								if((cc->contentWindow==e.xmotion.window) && (e.xmotion.state==Button1Mask+Mod4Mask))
									domove=true;
							
								if(domove==true)
									{
										int xdiff=e.xbutton.x_root-start.x_root;
										int ydiff=e.xbutton.y_root-start.y_root;
										XWindowAttributes	ttattr;
										XWindowAttributes	frameattr;
										rectStruct r;
										XGetWindowAttributes(this->mainClass->display,cc->contentWindow,&ttattr);
										XMoveWindow(this->mainClass->display,cc->frameWindow,attr.x+xdiff,attr.y+ydiff);
								//notify client
										XGetWindowAttributes(this->mainClass->display,cc->frameWindow,&frameattr);
										r.x=frameattr.x+2;
										r.y=frameattr.y+16;
										r.width=ttattr.width;
										r.height=ttattr.height;
										this->LFSWM2_sendConfigureEvent(cc->contentWindow,r);
									}
								}
						}
						break;

					case Expose:
						{
						//fprintf(stderr,"Expose IN eventnumber %i\n",when++);
							LFSWM2_clientClass	*cc;
							const char			*name;

							if(e.xexpose.count>0)
								break;

							cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e.xexpose.window);
							if(cc!=NULL)
								{
									this->mainClass->mainWindowClass->LFSWM2_refreshFrame(cc,&(e.xexpose));//TODO//
								}
						//fprintf(stderr,"Expose OUT eventnumber %i\n",when++);
						}
						break;
					case FocusIn:
						//fprintf(stderr,"FocusIn eventnumber %i\n",when++);
						break;
					case FocusOut:
						//fprintf(stderr,"FocusOut eventnumber %i\n",when++);
						break;
					case GraphicsExpose:
						//fprintf(stderr,"GraphicsExpose eventnumber %i\n",when++);
						break;
					case NoExpose:
						//fprintf(stderr,"NoExpose eventnumber %i\n",when++);
						break;
					case VisibilityNotify:
						fprintf(stderr,"VisibilityNotify eventnumber %i\n",when++);
						break;
					case CreateNotify:
						//fprintf(stderr,"CreateNotify eventnumber %i\n",when++);
						break;
					case DestroyNotify:
						{
						//	fprintf(stderr,"DestroyNotify eventnumber %i\n",when++);
						//suppress spurious warnings
							this->mainClass->LFSWM2_pushXErrorHandler();

							if(this->mainClass->mainWindowClass->clientList.count(e.xdestroywindow.event)==0)
								{
									if(this->mainClass->mainWindowClass->LFSWM2_getWindowType(e.xdestroywindow.event)!=-1)//TODO//
										XRemoveFromSaveSet(this->mainClass->display,e.xdestroywindow.event);
									if(this->mainClass->mainWindowClass->clientList.count(e.xdestroywindow.window)==0)
										break;
								}
							this->mainClass->mainWindowClass->LFSWM2_destroyClient(e.xdestroywindow.event);
							this->mainClass->LFSWM2_popXErrorHandler();
						}
						break;
					case UnmapNotify:
						{
							//fprintf(stderr,"UnmapNotify IN eventnumber %i\n",when++);
							LFSWM2_clientClass	*cc;

							cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e.xexpose.window);
							if(cc!=NULL)
								{
									XUnmapWindow(this->mainClass->display,cc->frameWindow);
								}
							//fprintf(stderr,"UnmapNotify OUT eventnumber %i\n",when);
						}
						break;
					case MapNotify:
						{
							Atom				da;
							int						di;
							int					status;
							unsigned char		*propret=NULL;
							unsigned long		dl;
							LFSWM2_clientClass	*ccmap;

							status=XGetWindowProperty(this->mainClass->display,e.xmap.window,this->mainClass->atoms.at("_NET_WM_WINDOW_TYPE"),0L,sizeof (Atom),false,XA_ATOM,&da,&di,&dl,&dl,&propret);

							if (status==Success&&propret)
								{
									if(((Atom *)propret)[0]==this->mainClass->atoms.at("_NET_WM_WINDOW_TYPE_MENU"))
										{
											XRaiseWindow(this->mainClass->display,e.xmap.window);
											break;
										}

									if(((Atom *)propret)[0]==this->mainClass->atoms.at("_NET_WM_WINDOW_TYPE_DIALOG"))
										{

											ccmap=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e.xmap.window);
											if(ccmap==NULL)
												{
													this->mainClass->mainWindowClass->LFSWM2_createClient(e.xmap.window);
												}
										}
								}
							XMapWindow(this->mainClass->display,e.xmap.window);
							XRaiseWindow(this->mainClass->display,e.xmap.window);
							XSync(this->mainClass->display,false);
						}
						break;

					case MapRequest:
						//fprintf(stderr,"MapRequest window=%x when=%i\n",e.xmaprequest.window,when++);
						XMapWindow(this->mainClass->display,e.xmaprequest.window);
						XRaiseWindow(this->mainClass->display,e.xmap.window);
						this->mainClass->mainWindowClass->LFSWM2_createClient(e.xmaprequest.window);
						break;
					case ReparentNotify:
						//fprintf(stderr,"ReparentNotify eventnumber %i\n",when++);
						break;
					case GravityNotify:
					//	fprintf(stderr,"GravityNotify eventnumber %i\n",when++);
						break;
					case ResizeRequest:
						//fprintf(stderr,"ResizeRequest eventnumber %i\n",when++);
						break;
					case ConfigureRequest:
						{
						//fprintf(stderr,"ResizeRequest eventnumber %i\n",when++);
							if(e.xconfigurerequest.detail==Below)
								{
									LFSWM2_clientClass	*cc;
//									XClientMessageEvent	em;
									cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e.xconfigurerequest.window);
									if(cc!=NULL)
										{
											this->mainClass->mainWindowClass->LFSWM2_changeState(cc->contentWindow,NET_WM_STATE_REMOVE,this->mainClass->atoms.at("_NET_WM_STATE_ABOVE"));
											this->mainClass->mainWindowClass->LFSWM2_changeState(cc->contentWindow,NET_WM_STATE_TOGGLE,this->mainClass->atoms.at("_NET_WM_STATE_BELOW"));
											cc->onBottom=!cc->onBottom;
											if(cc->onBottom==true)
												this->LFSWM2_moveToBottom(cc->contentWindow);
											//TODO//this->LFSWM2_restack();
											//fprintf(stderr,"ConfigureRequest eventnumber %i detail=%i ccbelloe=%i\n",when++,e.xconfigurerequest.detail,cc->onBottom);
											XSync(this->mainClass->display,false);
										}

									break;
								}
							LFSWM2_clientClass	*cc;
							XWindowChanges		changes;

							// Copy fields from e to changes.
							changes.x=e.xconfigurerequest.x;
							changes.y=e.xconfigurerequest.y;
							changes.width=e.xconfigurerequest.width;
							changes.height=e.xconfigurerequest.height;
							changes.border_width=e.xconfigurerequest.border_width;
							changes.sibling=e.xconfigurerequest.above;
							changes.stack_mode=e.xconfigurerequest.detail;

							cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e.xexpose.window);
							if(cc!=NULL)
								{
									XConfigureWindow(this->mainClass->display,e.xconfigurerequest.window,e.xconfigurerequest.value_mask,&changes);
									XMoveResizeWindow(this->mainClass->display,cc->frameWindow,changes.x,changes.y,changes.width+(this->mainClass->sideBarSize*2),changes.height+this->mainClass->titleBarSize+this->mainClass->bottomBarSize);

									XMoveWindow(this->mainClass->display,cc->contentWindow,this->mainClass->sideBarSize-1,this->mainClass->titleBarSize-1);
									cc->clientWindowRect=this->mainClass->mainWindowClass->LFSWM2_getWindowRect(cc->contentWindow,this->mainClass->rootWindow);
									cc->frameWindowRect=this->mainClass->mainWindowClass->LFSWM2_getWindowRect(cc->frameWindow,this->mainClass->rootWindow);
									XSync(this->mainClass->display,false);
								}
							else
								{
									XConfigureWindow(this->mainClass->display,e.xconfigurerequest.window,e.xconfigurerequest.value_mask,&changes);
									XSync(this->mainClass->display,false);
								}
						}
						break;
					case CirculateNotify:
						fprintf(stderr,"CirculateNotify eventnumber %i\n",when++);
						break;
					case CirculateRequest:
						fprintf(stderr,"CirculateRequest eventnumber %i\n",when++);
						break;
					case PropertyNotify:
						{
							//fprintf(stderr,"PropertyNotify IN eventnumber %i atom name=%s\n",when++,XGetAtomName(this->mainClass->display,e.xproperty.atom));
							LFSWM2_clientClass	*cc;

//	if (e.xproperty.atom==this->mainClass->atoms.at("_NET_CURRENT_DESKTOP") )

{
				//	fprintf(stderr,">>>>>>>>>>>>window=%p rootwin=%p\n",e.xproperty.window,this->mainClass->rootWindow);
//if(e.xproperty.window==this->mainClass->rootWindow)
//{
//fprintf(stderr,"send to root\n");
//}
//			if(this->mainClass->currentDesktop!=e->data.l[0])
//				{
//			//	int h=e->data.l[0];
//					//this->mainClass->LFSWM2_setCurrentDesktop(e->data.l[0]);
//					//this->mainClass->currentDesktop=h;
//					//if((e->data.l[0]==NOSRC) || (e->data.l[0]==PAGERSRC))
//					//if(e->send_event==false)
//					fprintf(stderr,"00000000000000000000\n");
//			for(int j=0;j<5;j++)
//				{
//					fprintf(stderr,"data[%i]=%p\n",j,e->data.l[j]);
//					this->mainClass->DEBUG_printAtom(e->data.l[j]);
//				}
//				fprintf(stderr,"message_type=%p\n",e->message_type);
//				fprintf(stderr,"type=%p\n",e->type);
//					//if(this->skipRestack==false)
//					if((e->window==this->mainClass->rootWindow))
//					{
//					this->mainClass->LFSWM2_setCurrentDesktop(e->data.l[0]);
//							this->LFSWM2_restack();
//						}
//					
//					//this->skipRestack=false;
//				}
//			//if(this->skipRestack==false)
//			//	this->LFSWM2_restack();
//			//this->skipRestack=false;	
//			//else
//			//	this->LFSWM2_restack();
//		//	this->mainClass->DEBUG_printAtom(e->message_type);//Atom 0x14d name=_NET_WM_STATE
//		//	for(int j=0;j<5;j++)
//		//		{
//		//			fprintf(stderr,"data[%i]=%p\n",j,e->data.l[j]);
//		//			this->mainClass->DEBUG_printAtom(e->data.l[j]);
//		//		}

		}






							if(e.xproperty.state==PropertyNewValue)
								{
								
								//fprintf(stderr,">>>>>>WM_STATE window=%p sendevent=%i<<<<<\n",e.xproperty.window,e.xproperty.send_event);
								if(e.xproperty.atom=this->mainClass->atoms.at("WM_STATE"))
								{
								//this->skipnext=true;
									LFSWM2_clientClass	*ccmessage;

								//this->mainClass->DEBUG_printAtom(e.xproperty.atom);
								ccmessage=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->LFSWM2_getParentWindow(e.xproperty.window));
								if(ccmessage!=NULL)
									{
									if(ccmessage->onDesk!=this->mainClass->currentDesktop)
										this->mainClass->LFSWM2_setCurrentDesktop(ccmessage->onDesk);
									XRaiseWindow(this->mainClass->display,ccmessage->frameWindow);
									}
									this->skipnext=false;
								break;
								}

									if(e.xproperty.atom=this->mainClass->atoms.at("_NET_WM_DESKTOP"))
										{
										
										break;
											cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e.xproperty.window);
											if(cc==NULL)
												break;
											fprintf(stderr,"desktop=%p\n",e.xproperty.atom);
											this->mainClass->LFSWM2_setCurrentDesktop(cc->onDesk);
											//this->mainClass->currentDesktop=cc->onDesk;
											this->LFSWM2_restack();
											break;
										}
									if(e.xproperty.atom=this->mainClass->atoms.at("_NET_WM_NAME"))
										{	
											cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e.xproperty.window);
											if(cc==NULL)
												break;

											cc->LFSWM2_setWindowName();
											this->mainClass->mainWindowClass->LFSWM2_refreshFrame(cc,NULL);
										}
								}
							//fprintf(stderr,"PropertyNotify OUT eventnumber %i atom name=%s\n",when++,XGetAtomName(this->mainClass->display,e.xproperty.atom));
						}
						break;
					case SelectionClear:
						//fprintf(stderr,"SelectionClear eventnumber %i\n",when++);
						break;
					case SelectionRequest:
						//fprintf(stderr,"SelectionRequest eventnumber %i\n",when++);
						break;
					case SelectionNotify:
						//fprintf(stderr,"SelectionNotify eventnumber %i\n",when++);
						break;
					case ColormapNotify:
						//fprintf(stderr,"ColormapNotify eventnumber %i\n",when++);
						break;
					case ClientMessage:
					#if 0
						//fprintf(stderr,"ClientMessage eventnumber %i\n",when++);
						if(e.xclient.window==this->mainClass->rootWindow)
							{
							XClientMessageEvent *ee=&e.xclient;
								if (ee->message_type==this->mainClass->atoms.at("_NET_CURRENT_DESKTOP") && ee->format==32)
		{
//		if(e->window==this->mainClass->rootWindow)
//		{
//			for(int j=0;j<5;j++)
//				{
//					fprintf(stderr,"data[%i]=%p\n",j,e->data.l[j]);
//					this->mainClass->DEBUG_printAtom(e->data.l[j]);
//				}
//				fprintf(stderr,"message_type=%p\n",e->message_type);
//				fprintf(stderr,"type=%p\n",e->type);
//				fprintf(stderr,"serial=%p\n",e->serial);
//				fprintf(stderr,"send_event=%p\n",e->send_event);
//				fprintf(stderr,"window=%p\n",e->window);
//				if(this->mainClass->currentDesktop==e->data.l[0])
//					{
//					//skipnext=true;
//					return;
//					}
//			}
//			if(skipnext==true)
//				{
//					skipnext=false;
//					return;
//				}

			if(this->mainClass->currentDesktop!=ee->data.l[0])
				{
					if((ee->window==this->mainClass->rootWindow))
						{
							//fprintf(stderr,"e->data.l[0]=%p currentDesktop=%p oldd=%p\n",e->data.l[0],this->mainClass->currentDesktop,oldd);
							if(ee->data.l[0]!=this->mainClass->currentDesktop)
								this->mainClass->LFSWM2_setCurrentDesktop(ee->data.l[0]);
							this->LFSWM2_restack();
						}
				}
		}
							}
						else
						#endif
						
							this->LFSWM2_doClientMsg(e.xclient.window,&e.xclient);
						break;
					case MappingNotify:
						fprintf(stderr,"MappingNotify eventnumber %i\n",when++);
						break;
					case KeymapNotify:
					//	fprintf(stderr,"KeymapNotify eventnumber %i\n",when++);
						break;
				}
			XAllowEvents(this->mainClass->display, ReplayPointer, CurrentTime);
		}
}

void LFSWM2_eventsClass::LFSWM2_doClientMsg(Window id,XClientMessageEvent *e)
{
	LFSWM2_clientClass	*ccmessage;

	if(e->message_type==this->mainClass->atoms.at("_NET_ACTIVE_WINDOW") && e->format==32)
		{
		//	fprintf(stderr,"this->mainClass->atoms.at(_NET_ACTIVE_WINDOW) && e->format==32)\n");
			ccmessage=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->LFSWM2_getParentWindow(e->window));
			if(ccmessage!=NULL)
				{	
					if(this->mainClass->currentDesktop!=ccmessage->onDesk)
						{
							this->mainClass->LFSWM2_setCurrentDesktop(ccmessage->onDesk);
							this->LFSWM2_moveToTop(ccmessage->contentWindow);
							XRaiseWindow(this->mainClass->display,ccmessage->frameWindow);
							XRaiseWindow(this->mainClass->display,ccmessage->contentWindow);
							this->LFSWM2_restack();
						}

					XMapWindow(this->mainClass->display,ccmessage->frameWindow);
					if((e->data.l[0]==NOSRC) || (e->data.l[0]==PAGERSRC))
						XRaiseWindow(this->mainClass->display,ccmessage->frameWindow);
					this->mainClass->mainWindowClass->LFSWM2_setWindowState(id,NormalState);
					this->mainClass->mainWindowClass->LFSWM2_changeState(ccmessage->contentWindow,NET_WM_STATE_REMOVE,this->mainClass->atoms.at("_NET_WM_STATE_HIDDEN"));
					this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atoms.at("_NET_ACTIVE_WINDOW"),XA_WINDOW,32,(void*)&ccmessage->contentWindow,1);
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

	if(e->message_type==this->mainClass->atoms.at("_NET_WM_DESKTOP") && e->format==32)
		{
			this->skipnext=false;
			if(this->mainClass->currentDesktop==e->data.l[0])
				this->skipnext=true;
			ccmessage=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->LFSWM2_getParentWindow(e->window));
			if(ccmessage!=NULL)
				{
					ccmessage->onDesk=e->data.l[0];
					this->mainClass->mainWindowClass->LFSWM2_setProp(ccmessage->contentWindow,this->mainClass->atoms.at("_NET_WM_DESKTOP"),XA_CARDINAL,32,(void*)&ccmessage->onDesk,1);
					if(ccmessage->onDesk!=this->mainClass->currentDesktop)
						ccmessage->LFSWM2_hideWindow();
					else
					{
						ccmessage->LFSWM2_showWindow();
						this->mainClass->mainWindowClass->LFSWM2_refreshFrame(ccmessage);
					}
						
					XSync(this->mainClass->display,true);
				}
			return;
		}

	if ((e->message_type==this->mainClass->atoms.at("WM_CHANGE_STATE")) && (e->format==32))
		{
			fprintf(stderr,"e->message_type==WM_CHANGE_STATE) && (e->format==32) e->data.l[0]==%p\n",e->data.l[0]);
			if(e->data.l[0]==IconicState)
				{
					ccmessage=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->LFSWM2_getParentWindow(e->window));
					if(ccmessage!=NULL)
						ccmessage->LFSWM2_minWindow();
					return;
				}
		}


//	if (e->message_type==this->mainClass->atoms.at("_WM_STATE") && e->format==32)
//		{
///*
//Atom 0x14d name=_NET_WM_STATE
//data[0]=0x1
//Atom 0x1 name=PRIMARY
//data[1]=0x14e
//Atom 0x14e name=_NET_WM_STATE_ABOVE
//data[2]=(nil)
//Atom (nil) name=(null)
//data[3]=0x2
//Atom 0x2 name=SECONDARY
//data[4]=(nil)
//Atom (nil) name=(null)
//
//*/
////	this->mainClass->DEBUG_printAtom(e->message_type);//Atom 0x14d name=_NET_WM_STATE
////	for(int j=0;j<5;j++)
////		{
////			fprintf(stderr,"data[%i]=%p\n",j,e->data.l[j]);
////			this->mainClass->DEBUG_printAtom(e->data.l[j]);
////		}
//}


	if (e->message_type==this->mainClass->atoms.at("_NET_WM_STATE") && e->format==32)
		{
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
//	this->mainClass->DEBUG_printAtom(e->message_type);//Atom 0x14d name=_NET_WM_STATE
//	for(int j=0;j<5;j++)
//		{
//			fprintf(stderr,"data[%i]=%p\n",j,e->data.l[j]);
//			this->mainClass->DEBUG_printAtom(e->data.l[j]);
//		}
//data[0]=0x1 -> NET_WM_STATE_ADD
//data[1]=0x14e -> NET_WM_STATE_ABOVE
//data[3]=0x2 -> 0-unk,1-normal app,2-pager ->PAGERSRC
//id window
//how=e->data.l[0]

			if(e->data.l[1]==this->mainClass->atoms.at("_NET_WM_STATE_ABOVE"))
				{
					this->mainClass->mainWindowClass->LFSWM2_changeState(id,e->data.l[0],e->data.l[1]);
					ccmessage=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->LFSWM2_getParentWindow(id));
					if(ccmessage!=NULL)
						{
							if(e->data.l[0]==NET_WM_STATE_REMOVE)
								{
									ccmessage->onTop=false;
									return;
								}
							XMapWindow(this->mainClass->display,ccmessage->frameWindow);
							XRaiseWindow(this->mainClass->display,ccmessage->frameWindow);
							ccmessage->onTop=true;
							ccmessage->onBottom=false;
							this->mainClass->mainWindowClass->LFSWM2_changeState(ccmessage->contentWindow,NET_WM_STATE_REMOVE,this->mainClass->atoms.at("_NET_WM_STATE_BELOW"));

							this->LFSWM2_moveToTop(ccmessage->contentWindow);
							//TODO//this->LFSWM2_restack();
						}
					return;
				}

			else if(e->data.l[1]==this->mainClass->atoms.at("_NET_WM_STATE_BELOW"))
				{
					this->mainClass->mainWindowClass->LFSWM2_changeState(id,e->data.l[0],e->data.l[1]);
					ccmessage=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->LFSWM2_getParentWindow(id));
					if(ccmessage!=NULL)
						{
							if(e->data.l[0]==NET_WM_STATE_REMOVE)
								{
									ccmessage->onBottom=false;
									return;
								}
							XLowerWindow(this->mainClass->display,ccmessage->frameWindow);
							ccmessage->onBottom=true;
							this->LFSWM2_moveToBottom(ccmessage->contentWindow);
							//TODO//this->LFSWM2_restack();
						}
					return;
				}

			int how=e->data.l[0];
			for (int i=1; i <= 2; i++)
				{
					if (e->data.l[i] != 0)
						{
							this->mainClass->mainWindowClass->LFSWM2_changeState(id,how,e->data.l[i]);
						}
				}
		}

	if (e->message_type==this->mainClass->atoms.at("_NET_CURRENT_DESKTOP") && e->format==32)
		{
//		if(e->window==this->mainClass->rootWindow)
//		{
//			for(int j=0;j<5;j++)
//				{
//					fprintf(stderr,"data[%i]=%p\n",j,e->data.l[j]);
//					this->mainClass->DEBUG_printAtom(e->data.l[j]);
//				}
//				fprintf(stderr,"message_type=%p\n",e->message_type);
//				fprintf(stderr,"type=%p\n",e->type);
//				fprintf(stderr,"serial=%p\n",e->serial);
//				fprintf(stderr,"send_event=%p\n",e->send_event);
//				fprintf(stderr,"window=%p\n",e->window);
//				if(this->mainClass->currentDesktop==e->data.l[0])
//					{
//					//skipnext=true;
//					return;
//					}
//			}
			if(skipnext==true)
				{
					skipnext=false;
					return;
				}

			if(this->mainClass->currentDesktop!=e->data.l[0])
				{
					if((e->window==this->mainClass->rootWindow))
						{
							//fprintf(stderr,"e->data.l[0]=%p currentDesktop=%p oldd=%p\n",e->data.l[0],this->mainClass->currentDesktop,oldd);
							if(e->data.l[0]!=this->mainClass->currentDesktop)
								this->mainClass->LFSWM2_setCurrentDesktop(e->data.l[0]);
							this->LFSWM2_restack();
						}
				}
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
	ce.width=r.width;
	ce.height=r.height;
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

	v=(Atom*)this->mainClass->mainWindowClass->LFSWM2_getProp(this->mainClass->rootWindow,this->mainClass->atoms.at("_NET_ACTIVE_WINDOW"),XA_WINDOW,&nitems_return);
	for(int j=0;j<this->mainClass->mainWindowClass->windowIDList.size();j++)
		{
			ccs=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->windowIDList.at(j));
			if(ccs!=NULL)
				{
					if(ccs->onTop==true)
						{
							sl.emplace(sl.begin(),ccs->frameWindow);
							cl.emplace(cl.begin(),ccs->contentWindow);
						}
					else if((v!=NULL) && (ccs->contentWindow==v[0]))
						{
							this->LFSWM2_moveToTop(ccs->contentWindow);
							sl.emplace(sl.begin(),ccs->frameWindow);
							cl.emplace(cl.begin(),ccs->contentWindow);
						}
					else
						{
							sl.push_back(ccs->frameWindow);
							cl.push_back(ccs->contentWindow);
						}
				}
		}

	for(int j=0;j<this->mainClass->mainWindowClass->windowIDList.size();j++)
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
		}

	XRestackWindows(this->mainClass->display,sl.data(),sl.size());

	this->mainClass->mainWindowClass->windowIDList=cl;
	this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atoms.at("_NET_CLIENT_LIST"),XA_WINDOW,32,cl.data(),cl.size());
	this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atoms.at("_NET_CLIENT_LIST_STACKING"),XA_WINDOW,32,cl.data(),cl.size());

	for(int j=this->mainClass->mainWindowClass->windowIDList.size()-1;j>-1;j--)
		{
			ccs=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->windowIDList.at(j));
			if(ccs!=NULL)
				{
					if((ccs->onDesk==this->mainClass->currentDesktop) && (ccs->isMinimized==false))
						XMapRaised(this->mainClass->display,ccs->frameWindow);
					else
						ccs->LFSWM2_hideWindow();
				}
		}
}

//void LFSWM2_eventsClass::LFSWM2_restack(void)
//{
//	LFSWM2_clientClass	*ccmessage;
//
//	for(int j=this->mainClass->mainWindowClass->windowIDList.size()-1;j>-1;j--)
//		{
//			ccmessage=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->windowIDList.at(j));
//			if((ccmessage!=NULL) && (ccmessage->onTop==true))
//				XRaiseWindow(this->mainClass->display,ccmessage->frameWindow);
//		}
//
//	for(int j=this->mainClass->mainWindowClass->windowIDList.size()-1;j>-1;j--)
//		{
//			if(this->mainClass->mainWindowClass->LFSWM2_getWindowType(this->mainClass->mainWindowClass->windowIDList.at(j))==DOCKWINDOW)
//				XRaiseWindow(this->mainClass->display,this->mainClass->mainWindowClass->windowIDList.at(j));
//			if(this->mainClass->mainWindowClass->LFSWM2_getWindowType(this->mainClass->mainWindowClass->windowIDList.at(j))==MENUWINDOW)
//				XRaiseWindow(this->mainClass->display,this->mainClass->mainWindowClass->windowIDList.at(j));
//		}
//	XSync(this->mainClass->display,false);
//}

void LFSWM2_eventsClass::LFSWM2_moveToTop(Window id)
{
	for(int j=0;j<this->mainClass->mainWindowClass->windowIDList.size();j++)
		{
			if(this->mainClass->mainWindowClass->windowIDList.at(j)==id)
				{
					this->mainClass->mainWindowClass->windowIDList.erase(this->mainClass->mainWindowClass->windowIDList.begin()+j);
					this->mainClass->mainWindowClass->windowIDList.emplace (this->mainClass->mainWindowClass->windowIDList.begin(),id);
				}
		}
				
	this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atoms.at("_NET_CLIENT_LIST"),XA_WINDOW,32,this->mainClass->mainWindowClass->windowIDList.data(),this->mainClass->mainWindowClass->windowIDList.size());
	this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atoms.at("_NET_CLIENT_LIST_STACKING"),XA_WINDOW,32,this->mainClass->mainWindowClass->windowIDList.data(),this->mainClass->mainWindowClass->windowIDList.size());
					//return;
	//for(int j=0;j<this->mainClass->mainWindowClass->windowIDList.size();j++)
	//	fprintf(stderr,"window=%p\n",this->mainClass->mainWindowClass->windowIDList.at(j));
}

void LFSWM2_eventsClass::LFSWM2_moveToBottom(Window id)
{
	for(int j=0;j<this->mainClass->mainWindowClass->windowIDList.size();j++)
		{
			if(this->mainClass->mainWindowClass->windowIDList.at(j)==id)
				{
					this->mainClass->mainWindowClass->windowIDList.erase(this->mainClass->mainWindowClass->windowIDList.begin()+j);
					this->mainClass->mainWindowClass->windowIDList.emplace (this->mainClass->mainWindowClass->windowIDList.end(),id);
				}
		}
				
	this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atoms.at("_NET_CLIENT_LIST"),XA_WINDOW,32,this->mainClass->mainWindowClass->windowIDList.data(),this->mainClass->mainWindowClass->windowIDList.size());
	this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atoms.at("_NET_CLIENT_LIST_STACKING"),XA_WINDOW,32,this->mainClass->mainWindowClass->windowIDList.data(),this->mainClass->mainWindowClass->windowIDList.size());
}