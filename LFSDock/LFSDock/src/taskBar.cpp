/*
 *
 * ©K. D. Hedger. Tue 28 Nov 11:35:20 GMT 2023 keithdhedger@gmail.com

 * This file (taskBar.cpp) is part of LFSDock.

 * LFSDock is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * LFSDock is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSDock.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "taskBar.h"

int						oldwidth=0;
bool						movetohere=false;

std::vector<taskStruct>	holdtasks;
std::vector<taskStruct>	filltasks;
std::vector<taskStruct>	tasks;

bool compareTaskWID(const taskStruct &a,const taskStruct &b)
{
	return(a.winid<b.winid);
}

bool compareTaskClass(const taskStruct &a,const taskStruct &b)
{
	return(a.taskClass[0]<b.taskClass[0]);
}

bool taskSwitcherExitCB(LFSTK_gadgetClass*p,void* ud)
{
//fprintf(stderr,"exit>>>>%p--%p<<<<\n",currentTask,p);

//if(checkInBorder(p)==true)
//{
//	for(int j=0;j<tasks.size();j++)
//		{
//			if(moveGadget==true)
//					setGadgetPosition(taskbuttons[j],false);
//		}
//}

	//if(moveGadget==true)
	//	setGadgetPosition(p,false);

	exitPopList(p,ud);
	inSomeWindow=false;
	dockWindow->LFSTK_redrawAllGadgets();
	return(true);
}

bool taskSwitcherEnterCB(LFSTK_gadgetClass*p,void* ud)
{
	std::string				label;
	unsigned long			d;
	infoDataStruct			ls;
	LFSTK_buttonClass		*bc=static_cast<LFSTK_buttonClass*>(p);
	std::vector<taskStruct>	listtasks;
//fprintf(stderr,"enter >>>>%p--%p<<<<\n",currentTask,p);
	exitPopList(p,ud);

	if(moveGadget==true)
		setGadgetPosition(p,true);

	popActionList->LFSTK_freeList();	

	d=(unsigned long)bc->userData;	
	listtasks.push_back(filltasks.at(d));

	for(int j=0;j<filltasks.at(d).tasks.size();j++)
		listtasks.push_back(filltasks.at(d).tasks.at(j));

	std::sort(listtasks.begin(),listtasks.end(),compareTaskWID);

	for(int j=0;j<listtasks.size();j++)
		{
			label=listtasks.at(j).taskName;
			ls.label=label;
			ls.imageType=NOTHUMB;
			ls.userData=USERDATA(listtasks.at(j).winid);
			popActionList->LFSTK_appendToList(ls);	
		}

	popActionList->LFSTK_updateList();
	popActionList->LFSTK_moveGadget(-1,-1);
	popActionWindow->userData=USERDATA(TASKSWITCHER);
	popActionWindow->LFSTK_resizeWindow(popActionList->LFSTK_getListMaxWidth()-2,(GADGETHITE*(filltasks.at(d).tasks.size()+1))-4);
	showhidetActionList(p,popActionWindow,popActionList);
	XFlush(apc->display);
	inSomeWindow=true;
	return(true);
}

bool taskSelect(void *object,void* userdata)
{
	propReturn		pr;
	unsigned long	d=popActionList->LFSTK_getCurrentListItem();
	Window			win=(Window)popActionList->listDataArray->at(popActionList->LFSTK_getCurrentListItem()).userData;

	if(movetohere==true)
		{
			pr=apc->globalLib->LFSTK_getSingleProp(apc->display,apc->rootWindow,NET_CURRENT_DESKTOP,XA_CARDINAL);
			sendClientMessage(win,"_NET_WM_DESKTOP",pr.integer,0,0,0,0);
			movetohere=false;
		}
	XMapWindow(apc->display,win);

	XWMHints *xh=XAllocWMHints();
	xh->flags=StateHint;
	xh->initial_state=NormalState;
	XSetWMHints(dockWindow->app->display, win, xh);
	sendClientMessage(win,"WM_HINTS",0,0,0,0,0);
	XFree(xh);
	sendClientMessage(win,"_NET_ACTIVE_WINDOW",0,0,0,0,0);

	inSomeWindow=false;
	showhidetActionList(NULL,popActionWindow,popActionList);
	return(true);
}

bool taskListCBDown(void* p,void* ud)
{
	Window			root_return;
	Window			child_return;
	int				root_x_return,root_y_return,win_x_return,win_y_return;
	unsigned int		mask_return;

	XQueryPointer(apc->display,popActionWindow->window,&root_return,&child_return,&root_x_return,&root_y_return,&win_x_return,&win_y_return,&mask_return);

	if((mask_return & Button3Mask)!=0)
		movetohere=true;
	return(true);
}

bool taskListCB(void* p,void* ud)
{
	LFSTK_buttonClass	*bc=static_cast<LFSTK_buttonClass*>(p);

	if(bc!=NULL)
		{
			sendClientMessage((Window)filltasks.at((unsigned long)bc->userData).winid,"_NET_ACTIVE_WINDOW",0,0,0,0,0);
		}
	return(true);
}

Window doTreeWalkForTasks(Window wind)
{
	Window			root,parent;
	Window				*children;
	Window				thewin;
	unsigned int			n_children;
	int					i;
	unsigned long		winid;
	char					*wname;
	void					*ptr=NULL;
	unsigned long		count=32;
	Atom					rtype;
	int					rfmt;
	unsigned long		rafter;
	unsigned long		n=0;
	Status				st;
	taskStruct			tsk;
	XWindowAttributes	wattr;

	if (!XQueryTree(apc->display,wind,&root,&parent,&children,&n_children))
		return None;

	if (!children)
		return None;

	/* Check each child for WM_STATE and other validity */
	thewin=None;
	wname=NULL;
	winid=-1;

	for (int j=n_children-1;j>=0; j--)
		{
			if(!hasProp(children[j],WM_STATE))
				continue;
			else
				{
					XGetWindowAttributes(apc->display,children[j],&wattr);
					if(wattr.map_state==IsUnmapped)
						continue;
				}

			if(hasProp(children[j],WM_CLASS))
				{
					propReturn	pr=apc->globalLib->LFSTK_getSingleProp(apc->display,children[j],XInternAtom(apc->display,"WM_CLASS",false),XA_STRING);
						{
							if(pr.strlist.size()>1)
								{
									tsk.taskClass[0]=pr.strlist.at(0);
									tsk.taskClass[1]=pr.strlist.at(1);
								}
							else
								tsk.taskClass[0]=pr.strlist.at(0);
						}

					tsk.taskName=getWindowName(children[j]);

					if(hasProp(children[j],NET_WM_PID))
						{
							propReturn	pr=apc->globalLib->LFSTK_getSingleProp(apc->display,children[j],NET_WM_PID,XA_CARDINAL);
							tsk.pid=pr.integer;
						}

					tsk.winid=children[j];
					tasks.push_back(tsk);
			}

			/* Got one */
			thewin=children[j];
			winid=children[j];
			break;
		}

	thewin=None;
	/* No children matched, now descend into each child */
	for (i=(int) n_children - 1; i >= 0; i--)
		{
			if (children[i]==None)
				continue;
			
			thewin=doTreeWalkForTasks(children[i]);
			if (thewin != None)
				break;
		}

	if(wname!=NULL)
		XFree(wname);
	XFree(ptr);
	XFree(children);
	return thewin;
}

void updateTaskBar(bool force)//TODO//
{
	int						cnt=0;
	bool						skipflag=false;
	std::string				icon;
	dataStruct				*datam=NULL;
	bool						goodkey;	
	bool						unequal=false;

	if(dockIsHidden==true)
		return;

	tasks.clear();
	doTreeWalkForTasks(apc->rootWindow);
	std::sort(tasks.begin(),tasks.end(),compareTaskClass);

	if(force==false)
		{
			if(tasks.size()==holdtasks.size())
				{
					for(int j=0;j<holdtasks.size();j++)
						{
							if((holdtasks.at(j).winid!=tasks.at(j).winid) || (holdtasks.at(j).taskName!=tasks.at(j).taskName))
								{
									unequal=true;
									goto skiplabel;
								}
						}
				}
			else
				unequal=true;
		}
	else
		{
			unequal=true;
			oldwidth=1;
		}

skiplabel:
	if((unequal==false) && (force==false))
		return;

	if(tasks.size()==0)
		{
			if(oldwidth==0)
				return;
			oldwidth=0;
			moveDock(0);
			resizeDock(windowWidth,iconWidth+extraSpace);
			return;
		}

	if(tasks.size()>0)
		{
			holdtasks=tasks;
			filltasks.clear();
			filltasks.push_back(tasks.at(0));
			tasks.erase(tasks.begin());

			while(tasks.size()>0)
				{
					skipflag=false;
					for(int j=0;j<tasks.size();j++)
						{
							if(tasks.at(j).taskClass[0].compare(filltasks.at(cnt).taskClass[0])==0)
								{
									filltasks.at(cnt).tasks.push_back(tasks.at(j));
									tasks.erase(tasks.begin()+j);
									skipflag=true;
									break;
								}
						}
					if(skipflag==false)
						{
							filltasks.push_back(tasks.at(0));
							tasks.erase(tasks.begin());
							cnt++;
						}
				}

			for(int j=0;j<filltasks.size();j++)
				{
					icon=dockWindow->globalLib->LFSTK_findThemedIcon(desktopTheme,filltasks.at(j).taskClass[0],"");
					if(icon.empty()==true)
						icon=dockWindow->globalLib->LFSTK_findThemedIcon(desktopTheme,filltasks.at(j).taskClass[1],"");

					if((LFSTK_UtilityClass::LFSTK_strStr(icon,".png",true).empty()==true) && (LFSTK_UtilityClass::LFSTK_strStr(icon,".jpg",true).empty()==true))
						icon="";

					if(icon.empty()==false)
						{
							if(taskbuttons[j]->imagePath!=icon)
								taskbuttons[j]->LFSTK_setImageFromPath(icon,CENTRE,true);
						}
					else
						{
							datam=NULL;
							taskbuttons[j]->LFSTK_setImageFromPath(DATADIR "/pixmaps/command.png",CENTRE,true);
							for(int i=0;i<2;i++)
								{
									datam=gFind->LFSTK_findNamed(filltasks.at(j).taskClass[i],std::string(".desktop"));
									if(datam!=NULL)
										{
											std::map<unsigned long,std::vector<std::string>> lines;
											std::string										entryname="Desktop Entry";

											lines=LFSTK_UtilityClass::LFSTK_readFullDesktopFile(datam->path);
											if(lines.empty()==false)
												{
													std::string	keyicon;
													keyicon=LFSTK_UtilityClass::LFSTK_getFullEntry(entryname,"Icon",lines);
													if(keyicon.empty()==false)
														{
															icon=dockWindow->globalLib->LFSTK_findThemedIcon(desktopTheme,keyicon,"");
															if(icon.empty()==false)
																{
																	if(taskbuttons[j]->imagePath!=icon)
																		taskbuttons[j]->LFSTK_setImageFromPath(icon,CENTRE,true);
																}
														}
													continue;									
												}
										}
								}
						}
					taskbuttons[j]->userData=USERDATA(j);
				}
		}

	if(oldwidth!=windowWidth+(iconWidth*filltasks.size()))
		{
			moveDock(filltasks.size()*iconWidth);
			resizeDock(windowWidth+((iconWidth+ICONSPACE)*filltasks.size()),iconWidth+extraSpace);
			oldwidth=windowWidth+(iconWidth*filltasks.size());
		}
}

