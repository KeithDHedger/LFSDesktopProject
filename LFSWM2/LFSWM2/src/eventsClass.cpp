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

	this->LFSWM2_restack();

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
								this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atoms.at("_NET_ACTIVE_WINDOW"),XA_WINDOW,32,&cc->contentWindow,1);
								this->mainClass->needsRestack=true;
								XAllowEvents(this->mainClass->display,ReplayPointer,e.xbutton.time);
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
										cc->frameWindowRect=this->mainClass->mainWindowClass->LFSWM2_getWindowRect(cc->frameWindow,this->mainClass->rootWindow);
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
						//fprintf(stderr,"VisibilityNotify eventnumber %i\n",when++);
						break;
					case CreateNotify:
						//fprintf(stderr,"CreateNotify eventnumber %i\n",when++);
						break;
					case DestroyNotify:
						cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e.xdestroywindow.event);
						if(cc!=NULL)
							delete cc;
						break;
					case UnmapNotify:
						{
						break;
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
						break;
							//fprintf(stderr,"MapNotify window=%x when=%i\n",e.xmap.window,when++);
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
						//fprintf(stderr,"GravityNotify eventnumber %i\n",when++);
						break;
					case ResizeRequest:
						//fprintf(stderr,"ResizeRequest eventnumber %i\n",when++);
						break;
					case ConfigureRequest:
						{
							//fprintf(stderr,"ConfigureRequest eventnumber %i\n",when++);
							this->mainClass->LFSWM2_pushXErrorHandler();
								if(false)
									{
										fprintf(stderr,"ConfigureRequest eventnumber %i\n",when++);
										fprintf(stderr,"type=%i 23=ConfigureRequest\n",e.xconfigurerequest.type);
										fprintf(stderr,"parent=%p \n",e.xconfigurerequest.parent);
										fprintf(stderr,"window=%p \n",e.xconfigurerequest.window);
										fprintf(stderr,"detail=%p \n",e.xconfigurerequest.detail);
									}
								if(e.xconfigurerequest.detail==Below)
									{
										LFSWM2_clientClass	*cc;
										cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e.xconfigurerequest.window);
										if(cc!=NULL)
											{
												this->mainClass->mainWindowClass->LFSWM2_changeState(cc->contentWindow,NET_WM_STATE_REMOVE,this->mainClass->atoms.at("_NET_WM_STATE_ABOVE"));
												this->mainClass->mainWindowClass->LFSWM2_changeState(cc->contentWindow,NET_WM_STATE_TOGGLE,this->mainClass->atoms.at("_NET_WM_STATE_BELOW"));
												cc->onBottom=!cc->onBottom;
												if(cc->onBottom==true)
													this->LFSWM2_moveToBottom(cc->contentWindow);
												this->mainClass->needsRestack=true;
											}
										this->mainClass->LFSWM2_popXErrorHandler();
										break;
									}
							this->mainClass->LFSWM2_popXErrorHandler();

							LFSWM2_clientClass	*cc;
							XWindowChanges		changes;
//TODO//>>>>>>>>>>>>>>>>>>
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
									XMoveResizeWindow(this->mainClass->display,cc->frameWindow,changes.x,changes.y,changes.width+(this->mainClass->sideBarSize*3),changes.height+this->mainClass->titleBarSize+this->mainClass->bottomBarSize+2);//TODO//constant yuck

									XMoveWindow(this->mainClass->display,cc->contentWindow,this->mainClass->sideBarSize,this->mainClass->titleBarSize);
									cc->clientWindowRect=this->mainClass->mainWindowClass->LFSWM2_getWindowRect(cc->contentWindow,this->mainClass->rootWindow);
									cc->frameWindowRect=this->mainClass->mainWindowClass->LFSWM2_getWindowRect(cc->frameWindow,this->mainClass->rootWindow);
									cc->LFSWM2_resizeControls();
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
						//fprintf(stderr,"CirculateNotify eventnumber %i\n",when++);
						break;
					case CirculateRequest:
						//fprintf(stderr,"CirculateRequest eventnumber %i\n",when++);
						break;
					case PropertyNotify:
						{
							//fprintf(stderr,"PropertyNotify IN eventnumber %i atom name=%s\n",when++,XGetAtomName(this->mainClass->display,e.xproperty.atom));
							LFSWM2_clientClass	*cc;

							if(false)
								{
									fprintf(stderr,"PropertyNotify eventnumber %i\n",when++);
									fprintf(stderr,"type=%i 28=PropertyNotify\n",e.xproperty.type);
									fprintf(stderr,"window=%p \n",e.xproperty.window);
									if(e.xproperty.state==PropertyNewValue)
										fprintf(stderr,"state=%p =PropertyNewValue \n",e.xproperty.state);
									else
										fprintf(stderr,"state=%p =PropertyDelete \n",e.xproperty.state);
									fprintf(stderr,"send_event=%p\n",e.xproperty.send_event);
									this->mainClass->DEBUG_printAtom(e.xproperty.atom);
								}

							if((e.xproperty.state==PropertyNewValue) || (e.xproperty.state==PropertyDelete))
								{
									if(e.xproperty.atom=this->mainClass->atoms.at("WM_STATE"))
										{
											cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e.xproperty.window);
											if(cc!=NULL)
												{
													cc->LFSWM2_setWMState(&e);
													this->skipnext=false;
													XSync(this->mainClass->display,false);
													this->mainClass->needsRestack=true;
												}

											this->skipnext=false;
										}

									if(e.xproperty.atom=this->mainClass->atoms.at("_NET_WM_NAME"))
										{	
											cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e.xproperty.window);
											if(cc==NULL)
												break;

											cc->LFSWM2_setWindowName();
											this->mainClass->mainWindowClass->LFSWM2_refreshFrame(cc,NULL);
										}
									if(e.xproperty.atom=this->mainClass->atoms.at("_NET_WM_USER_TIME"))
										{
											this->mainClass->needsRestack=false;
											break;
										}

								}
						}
						//fprintf(stderr,"PropertyNotify OUT eventnumber %i atom name=%s\n",when++,XGetAtomName(this->mainClass->display,e.xproperty.atom));
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
						fprintf(stderr,"ClientMessage eventnumber %i\n",when++);
						this->LFSWM2_doClientMsg(e.xclient.window,&e.xclient);
						break;
					case MappingNotify:
						//fprintf(stderr,"MappingNotify eventnumber %i\n",when++);
						break;
					case KeymapNotify:
					//	fprintf(stderr,"KeymapNotify eventnumber %i\n",when++);
						break;
				}

			if(this->mainClass->needsRestack==true)
				{
					this->LFSWM2_restack();
					this->mainClass->needsRestack=false;
				}
			XAllowEvents(this->mainClass->display,ReplayPointer,CurrentTime);
		}
}

void LFSWM2_eventsClass::LFSWM2_doClientMsg(Window id,XClientMessageEvent *e)
{
	LFSWM2_clientClass	*ccmessage;

fprintf(stderr,"message_type=%i\n",e->message_type);

	if (e->message_type==this->mainClass->atoms.at("_NET_CURRENT_DESKTOP") && e->format==32)
		{
		
				fprintf(stderr,"message_type=%p\n",e->message_type);
				fprintf(stderr,"type=%p\n",e->type);
				fprintf(stderr,"serial=%p\n",e->serial);
				fprintf(stderr,"send_event=%p\n",e->send_event);
				fprintf(stderr,"window=%p\n",e->window);

		
			if(e->window==this->mainClass->rootWindow)
			{
			fprintf(stderr,"_NET_WM_DESKTOP e->data.l[0]=%p cdesk=%p\n",e->data.l[0],this->mainClass->currentDesktop);
			this->mainClass->LFSWM2_setCurrentDesktop(e->data.l[0]);
			for(int j=0;j<this->mainClass->mainWindowClass->windowIDList.size();j++)
				{
					ccmessage=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->windowIDList.at(j));
					if(ccmessage!=NULL)
						{
							if(ccmessage->onDesk==e->data.l[0])
							{
								ccmessage->LFSWM2_showWindow();
							}
							else
								ccmessage->LFSWM2_hideWindow();
						}
				}
			this->mainClass->needsRestack=true;
			XSync(this->mainClass->display,false);
			}
		//return;
		}




//
//	if(e->message_type==this->mainClass->atoms.at("_NET_WM_DESKTOP") && e->format==32)
//		{
//			fprintf(stderr,"_NET_WM_DESKTOP\n");
//		this->skipnext=false;
//		//	if(this->mainClass->currentDesktop==e->data.l[0])
//		//		this->skipnext=true;
////			ccmessage=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->LFSWM2_getParentWindow(e->window));
////			if(ccmessage!=NULL)
////				{
////				fprintf(stderr,">>>>>>>>>>>>>>>>>>>>>>>\n");
////					ccmessage->onDesk=e->data.l[0];
////					this->mainClass->mainWindowClass->LFSWM2_setProp(ccmessage->contentWindow,this->mainClass->atoms.at("_NET_WM_DESKTOP"),XA_CARDINAL,32,(void*)&ccmessage->onDesk,1);
////					if(ccmessage->onDesk!=this->mainClass->currentDesktop)
////						ccmessage->LFSWM2_hideWindow();
////					else
////					{
////						ccmessage->LFSWM2_showWindow();
////						this->mainClass->mainWindowClass->LFSWM2_refreshFrame(ccmessage);
////					}
//
//
//			for(int j=0;j<this->mainClass->mainWindowClass->windowIDList.size();j++)
//				{
//					ccmessage=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->windowIDList.at(j));
//					if(ccmessage!=NULL)
//						{
//							if(ccmessage->onDesk==e->data.l[0])
//								ccmessage->LFSWM2_showWindow();
//							else
//								ccmessage->LFSWM2_hideWindow();
//						}
//				}
//
//						
//					//XSync(this->mainClass->display,true);
//				//}
//			return;
//		}
//



	if(e->message_type==this->mainClass->atoms.at("_NET_ACTIVE_WINDOW") && e->format==32)
		{
		fprintf(stderr,"_NET_ACTIVE_WINDOW\n");
			ccmessage=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->LFSWM2_getParentWindow(e->window));
			if(ccmessage!=NULL)
				{	
					if(this->mainClass->currentDesktop!=ccmessage->onDesk)
						{
							this->mainClass->LFSWM2_setCurrentDesktop(ccmessage->onDesk);
							this->LFSWM2_moveToTop(ccmessage->contentWindow);
							XRaiseWindow(this->mainClass->display,ccmessage->frameWindow);
							XRaiseWindow(this->mainClass->display,ccmessage->contentWindow);
						}

					XMapWindow(this->mainClass->display,ccmessage->frameWindow);
					if((e->data.l[0]==NOSRC) || (e->data.l[0]==PAGERSRC))
						XRaiseWindow(this->mainClass->display,ccmessage->frameWindow);
					this->mainClass->mainWindowClass->LFSWM2_setWindowState(id,NormalState);
					this->mainClass->mainWindowClass->LFSWM2_changeState(ccmessage->contentWindow,NET_WM_STATE_REMOVE,this->mainClass->atoms.at("_NET_WM_STATE_HIDDEN"));
					this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atoms.at("_NET_ACTIVE_WINDOW"),XA_WINDOW,32,(void*)&ccmessage->contentWindow,1);
				}
	XSetInputFocus(this->mainClass->display,e->window,RevertToParent,CurrentTime);

			this->mainClass->needsRestack=true;
			//return;
		}

	if(e->message_type==this->mainClass->atoms.at("_NET_CLOSE_WINDOW") && e->format==32)
		{
		fprintf(stderr,"_NET_CLOSE_WINDOW\n");
			ccmessage=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->LFSWM2_getParentWindow(e->window));
			if(ccmessage!=NULL)
				ccmessage->LFSWM2_sendCloseWindow();
			return;
		}

//	if(e->message_type==this->mainClass->atoms.at("_NET_WM_DESKTOP") && e->format==32)
//		{
//			fprintf(stderr,"_NET_WM_DESKTOP\n");
//		this->skipnext=false;
//		//	if(this->mainClass->currentDesktop==e->data.l[0])
//		//		this->skipnext=true;
////			ccmessage=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->LFSWM2_getParentWindow(e->window));
////			if(ccmessage!=NULL)
////				{
////				fprintf(stderr,">>>>>>>>>>>>>>>>>>>>>>>\n");
////					ccmessage->onDesk=e->data.l[0];
////					this->mainClass->mainWindowClass->LFSWM2_setProp(ccmessage->contentWindow,this->mainClass->atoms.at("_NET_WM_DESKTOP"),XA_CARDINAL,32,(void*)&ccmessage->onDesk,1);
////					if(ccmessage->onDesk!=this->mainClass->currentDesktop)
////						ccmessage->LFSWM2_hideWindow();
////					else
////					{
////						ccmessage->LFSWM2_showWindow();
////						this->mainClass->mainWindowClass->LFSWM2_refreshFrame(ccmessage);
////					}
//
//
//			for(int j=0;j<this->mainClass->mainWindowClass->windowIDList.size();j++)
//				{
//					ccmessage=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->windowIDList.at(j));
//					if(ccmessage!=NULL)
//						{
//							if(ccmessage->onDesk==e->data.l[0])
//								ccmessage->LFSWM2_showWindow();
//							else
//								ccmessage->LFSWM2_hideWindow();
//						}
//				}
//
//						
//					//XSync(this->mainClass->display,true);
//				//}
//			return;
//		}

	if ((e->message_type==this->mainClass->atoms.at("WM_CHANGE_STATE")) && (e->format==32))
		{
			fprintf(stderr,"WM_CHANGE_STATE\n");
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
							this->mainClass->needsRestack=true;
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
							this->mainClass->needsRestack=true;
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

	if (e->message_type==this->mainClass->atoms.at("_NET_ACTIVE_WINDOW") && e->format==32)
		{
		ccmessage=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->LFSWM2_getParentWindow(e->window));
					//ccmessage=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e->window);
					if(ccmessage!=NULL)
						{
						//ccmessage->LFSWM2_showWindow();
						//XRaiseWindow(this->mainClass->display,ccmessage->frameWindow);
						this->LFSWM2_moveToTop(e->window);
						this->mainClass->needsRestack=true;
						return;
				}
		}
#if 0
	if (e->message_type==this->mainClass->atoms.at("_NET_CURRENT_DESKTOP") && e->format==32)
		{
		
				fprintf(stderr,"message_type=%p\n",e->message_type);
				fprintf(stderr,"type=%p\n",e->type);
				fprintf(stderr,"serial=%p\n",e->serial);
				fprintf(stderr,"send_event=%p\n",e->send_event);
				fprintf(stderr,"window=%p\n",e->window);

		
			if(e->window==this->mainClass->rootWindow)
			{
			fprintf(stderr,"_NET_WM_DESKTOP e->data.l[0]=%p cdesk=%p\n",e->data.l[0],this->mainClass->currentDesktop);
			this->mainClass->LFSWM2_setCurrentDesktop(e->data.l[0]);
			for(int j=0;j<this->mainClass->mainWindowClass->windowIDList.size();j++)
				{
					ccmessage=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->windowIDList.at(j));
					if(ccmessage!=NULL)
						{
							if(ccmessage->onDesk==e->data.l[0])
							{
								ccmessage->LFSWM2_showWindow();
							}
							else
								ccmessage->LFSWM2_hideWindow();
						}
				}
			this->mainClass->needsRestack=true;
			}			
//		return;
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
//
//			if(this->mainClass->currentDesktop!=e->data.l[0])
//				{
//				fprintf(stderr,"_NET_WM_DESKTOP=%i\n",this->mainClass->currentDesktop);
//					if((e->window==this->mainClass->rootWindow))
//						{
//							if(e->data.l[0]!=this->mainClass->currentDesktop)
//								this->mainClass->LFSWM2_setCurrentDesktop(e->data.l[0]);
//							this->mainClass->needsRestack=true;
//						}
//				}
		}
#endif
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

	cl.clear();
	sl.clear();

	v=(Atom*)this->mainClass->mainWindowClass->LFSWM2_getProp(this->mainClass->rootWindow,this->mainClass->atoms.at("_NET_ACTIVE_WINDOW"),XA_WINDOW,&nitems_return);

	for(int j=0;j<this->mainClass->mainWindowClass->windowIDList.size();j++)
		{
			ccs=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->windowIDList.at(j));
			if(ccs!=NULL)
				{
					if((v!=NULL) && (ccs->contentWindow==v[0]))
						{
							XSetInputFocus(this->mainClass->display,ccs->contentWindow,RevertToParent,CurrentTime);
							if((ccs->onBottom==false))
								{
									sl.emplace(sl.begin(),ccs->frameWindow);
									cl.emplace(cl.begin(),ccs->contentWindow);
									break;
								}
						}
				}
		}

//TODO//
#if 0
	for(int j=0;j<this->mainClass->mainWindowClass->windowIDList.size();j++)
		{
			ccs=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->windowIDList.at(j));
			if((ccs!=NULL) && ((v!=NULL) && (ccs->contentWindow!=v[0])))
				{
					if(ccs->onTop==true)
						{
							sl.emplace(sl.begin(),ccs->frameWindow);
							cl.emplace(cl.begin(),ccs->contentWindow);
						}
				}
		}

	for(int j=0;j<this->mainClass->mainWindowClass->windowIDList.size();j++)
		{
			ccs=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->windowIDList.at(j));
			if((ccs!=NULL) && ((v!=NULL) && (ccs->contentWindow!=v[0])))
				{
					if((ccs->onBottom==false) && (ccs->onTop==false))
						{
							sl.push_back(ccs->frameWindow);
							cl.push_back(ccs->contentWindow);
						}
					else if(ccs->onBottom==true)
						{
							sl.push_back(ccs->frameWindow);
							cl.push_back(ccs->contentWindow);
						}
				}
		}

#else
	for(int j=0;j<this->mainClass->mainWindowClass->windowIDList.size();j++)
		{
			ccs=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->windowIDList.at(j));
			if((ccs!=NULL) && ((v!=NULL) && (ccs->contentWindow!=v[0])))
				{
					if(ccs->onTop==true)
						{
							sl.emplace(sl.begin(),ccs->frameWindow);
							cl.emplace(cl.begin(),ccs->contentWindow);
						}
				}
		}

	for(int j=0;j<this->mainClass->mainWindowClass->windowIDList.size();j++)
		{
			ccs=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->windowIDList.at(j));
			if((ccs!=NULL) && ((v!=NULL) && (ccs->contentWindow!=v[0])))
				{
					if((ccs->onBottom==false) && (ccs->onTop==false))
						{
							sl.push_back(ccs->frameWindow);
							cl.push_back(ccs->contentWindow);
						}
				}
		}

	for(int j=0;j<this->mainClass->mainWindowClass->windowIDList.size();j++)
		{
			ccs=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->windowIDList.at(j));
			if((ccs!=NULL))
				{
					if(ccs->onBottom==true)
						{
							sl.push_back(ccs->frameWindow);
							cl.push_back(ccs->contentWindow);
						}
				}
		}
#endif

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
}

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
}