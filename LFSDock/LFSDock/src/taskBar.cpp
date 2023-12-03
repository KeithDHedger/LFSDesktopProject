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

std::vector<taskStruct>	tasks;
int						oldwidth=0;
std::vector<taskStruct>	holdtasks;
std::vector<taskStruct>	filltasks;
LFSTK_listGadgetClass	*taskList=NULL;
LFSTK_windowClass		*taskWindow=NULL;
bool						taskSwitchIsUp=false;

bool showhidetTaskList(LFSTK_buttonClass	*bc,bool what)
{
	geometryStruct			geom;

	if((bc!=NULL) && (what==true))
		{	
			unsigned long	d=(unsigned long)bc->userData;
			bc->LFSTK_getGeomWindowRelative(&geom,apc->rootWindow);
			switch(panelGravity)
				{
					case PANELNORTH:
						taskWindow->LFSTK_moveWindow(geom.x+(geom.w/2)-(iconSize/2),geom.y+geom.h,true);
						break;
					case PANELSOUTH:
						taskWindow->LFSTK_moveWindow(geom.x+(geom.w/2)-(taskList->LFSTK_getListMaxWidth()/2),geom.y-(GADGETHITE*(filltasks.at(d).tasks.size()+1)),true);
						break;
				}
			taskWindow->LFSTK_showWindow(true);
			taskWindow->LFSTK_redrawAllGadgets();
			apc->windows->at(apc->LFSTK_findWindow(taskWindow)).showing=true;
		}
	else
		{
			taskWindow->LFSTK_hideWindow();
			apc->windows->at(apc->LFSTK_findWindow(taskWindow)).showing=false;
		}
	return(true);
}

bool taskSwitcherExitCB(LFSTK_gadgetClass*p,void* ud)
{
	geometryStruct	geom2;
	int				adj;

	adj=extraSpace*posMultiplier;
	p->LFSTK_getGeom(&geom2);	
	p->LFSTK_moveGadget(geom2.x,geom2.y+adj);
	taskSwitchIsUp=false;
	return(true);
}

bool taskSwitcherEnterCB(LFSTK_gadgetClass*p,void* ud)
{
	geometryStruct		geom;
	int					adj;
	LFSTK_buttonClass	*bc=static_cast<LFSTK_buttonClass*>(p);
	std::string			label;
	listLabelStruct		ls;
	unsigned long		d;

	if(taskSwitchIsUp==true)
		return(true);

	adj=extraSpace*posMultiplier;
	p->LFSTK_getGeom(&geom);	
	p->LFSTK_moveGadget(geom.x,geom.y-adj);
	taskSwitchIsUp=true;
	taskList->LFSTK_freeList();
	d=(unsigned long)bc->userData;
	label=filltasks.at(d).taskName;
	ls.label=strdup((char*)label.c_str());
	ls.imageType=NOTHUMB;
	ls.data.imagePath=NULL;
	ls.userData=USERDATA(filltasks.at(d).winid);
	taskList->LFSTK_appendToList(ls);

	for(int j=0;j<filltasks.at(d).tasks.size();j++)
		{
			label=filltasks.at(d).tasks.at(j).taskName;
			ls.label=strdup((char*)label.c_str());
			ls.imageType=NOTHUMB;
			ls.data.imagePath=NULL;
			ls.userData=USERDATA(filltasks.at(d).tasks.at(j).winid);
			taskList->LFSTK_appendToList(ls);
		}

	taskList->LFSTK_moveGadget(-1,-1);
	taskWindow->LFSTK_resizeWindow(taskList->LFSTK_getListMaxWidth()-2,(GADGETHITE*(filltasks.at(d).tasks.size()+1))-4);
	showhidetTaskList(bc,true);
	return(true);
}

bool taskSelect(void *object,void* userdata)
{
	LFSTK_listGadgetClass	*list=static_cast<LFSTK_listGadgetClass*>(object);
	unsigned long			d=list->LFSTK_getCurrentListItem();

	sendClientMessage((Window)list->listDataArray->at(list->LFSTK_getCurrentListItem()).userData,"_NET_ACTIVE_WINDOW",0,0,0,0,0);
	showhidetTaskList(NULL,false);

	return(true);
}

bool taskListCB(void* p,void* ud)
{
	LFSTK_buttonClass		*bc=static_cast<LFSTK_buttonClass*>(p);

	if(bc!=NULL)
		sendClientMessage((Window)filltasks.at((unsigned long)bc->userData).winid,"_NET_ACTIVE_WINDOW",0,0,0,0,0);
	return(true);
}

Window doTreeWalk(Window wind)
{
	Window			root,parent;
	Window			*children;
	Window			thewin;
	unsigned int		n_children;
	int				i;
	unsigned long	winid;
	char				*wname;
	void				*ptr=NULL;
	unsigned long	count=32;
	Atom				rtype;
	int				rfmt;
	unsigned long	rafter;
	unsigned long	n=0;
	Status			st;
	taskStruct		tsk;

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

			if(hasProp(children[j],WM_CLASS))
				{
					propReturn	pr=apc->globalLib->LFSTK_getSingleProp(apc->display,children[j],XInternAtom(apc->display,"WM_CLASS",false),XA_STRING);
						{
							if(pr.strlist.size()>1)
								{
									tsk.taskClass[0]=pr.strlist.at(1);
									tsk.taskClass[1]=pr.strlist.at(0);
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
			
			thewin=doTreeWalk(children[i]);
			if (thewin != None)
				break;
		}

	if(wname!=NULL)
		XFree(wname);
	XFree(ptr);
	XFree(children);
	return thewin;
}

void updateTaskBar()
{
	int						cnt=0;
	bool						skipflag=false;
	char						*icon=NULL;
	dataStruct				*datam=NULL;
	bool						goodkey;	
	char						*keyicon=NULL;
	bool						unequal=false;


	if(taskWindow->inWindow==false)
		 showhidetTaskList(NULL,false);
	tasks.clear();
	doTreeWalk(apc->rootWindow);
	if(tasks.size()==holdtasks.size())//TODO//
		{
			for(int j=0;j<holdtasks.size();j++)
				{
					if(holdtasks.at(j).winid!=tasks.at(j).winid)
						{
							break;
							unequal=true;
						}
				}
		}
	else
		unequal=true;

	if(unequal==false)
		return;

	holdtasks=tasks;
//fprintf(stderr,"holdtasks size=%i tasks=%i\n",holdtasks.size(),tasks.size());
//	for(int j=0;j<holdtasks.size();j++)
//		{
//			std::cout<<"holdtasks name="<<holdtasks.at(j).taskName<<" class name="<<holdtasks.at(j).taskClass<<" pid="<< std::uppercase<<holdtasks.at(j).pid<<" winid="<<std::hex<< std::setfill('0')<<" 0x"<<holdtasks.at(j).winid<<std::dec<< std::endl;
//		}

//	for(int j=0;j<tasks.size();j++)
//		{
//			std::cout<<"task name="<<tasks.at(j).taskName<<" class name="<<tasks.at(j).taskClass[0]<<"--"<<tasks.at(j).taskClass[1]<<" pid="<< std::uppercase<<tasks.at(j).pid<<" winid="<<std::hex<< std::setfill('0')<<" 0x"<<tasks.at(j).winid<<std::dec<< std::endl;
//		}

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
		//	std::cout<<"number of tasks="<<filltasks.at(j).tasks.size()<<   " task name="<<filltasks.at(j).taskName<<" class name="<<filltasks.at(j).taskClass<<" pid="<< std::uppercase<<filltasks.at(j).pid<<" winid="<<std::hex<< std::setfill('0')<<" 0x"<<filltasks.at(j).winid<<std::dec<< std::endl;

			icon=NULL;
			icon=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,filltasks.at(j).taskClass[0].c_str(),"");
			if(icon==NULL)
				icon=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,filltasks.at(j).taskClass[1].c_str(),"");

			if(icon!=NULL)
				{
					if(taskbuttons[j]->imagePath!=icon)
						taskbuttons[j]->LFSTK_setImageFromPath(icon,LEFT,true);
					free(icon);
				}
			else
				{
					datam=NULL;
					datam=gFind->LFSTK_findNamed(filltasks.at(j).taskClass[0],std::string(".desktop"));
					if(datam!=NULL)
						{
							goodkey=g_key_file_load_from_file(kf,datam->path.c_str(),G_KEY_FILE_NONE,NULL);
							if(goodkey==true)
								{
									keyicon=NULL;
									keyicon=g_key_file_get_string(kf,"Desktop Entry",G_KEY_FILE_DESKTOP_KEY_ICON,NULL);
									if(keyicon!=NULL)
										{
											icon=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,keyicon,"");
											if(icon!=NULL)
												{
													if(taskbuttons[j]->imagePath!=icon)
														taskbuttons[j]->LFSTK_setImageFromPath(icon,LEFT,true);
													free(icon);
												}
											g_free(keyicon);
										}
									continue;
								}
						}

					datam=gFind->LFSTK_findNamed(filltasks.at(j).taskClass[1],".desktop");
					if(datam!=NULL)
						{
							goodkey=g_key_file_load_from_file(kf,datam->path.c_str(),G_KEY_FILE_NONE,NULL);
							if(goodkey==true)
								{
									keyicon=NULL;
									keyicon=g_key_file_get_string(kf,"Desktop Entry",G_KEY_FILE_DESKTOP_KEY_ICON,NULL);
									if(keyicon!=NULL)
										{
											icon=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,keyicon,"");
											if(icon!=NULL)
												{
													if(taskbuttons[j]->imagePath!=icon)
														taskbuttons[j]->LFSTK_setImageFromPath(icon,LEFT,true);
													free(icon);
												}
											g_free(keyicon);
										}
									continue;
								}
						}
					taskbuttons[j]->LFSTK_setImageFromPath(DATADIR "/pixmaps/command.png",LEFT,true);
				}
			taskbuttons[j]->userData=USERDATA(j);
		}

	if(oldwidth!=windowWidth+(iconSize*filltasks.size()))
		{
			moveDock(filltasks.size()*iconSize);
			mainwind->LFSTK_resizeWindow(windowWidth+(iconSize*filltasks.size()),iconSize+extraSpace,true);
			oldwidth=windowWidth+(iconSize*filltasks.size());
			mainwind->LFSTK_clearWindow(true);
			XSync(apc->display,false);
		}
}

