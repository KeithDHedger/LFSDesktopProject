/*
 *
 * ©K. D. Hedger. Mon 25 Jan 11:39:52 GMT 2021 keithdhedger@gmail.com

 * This file (callbacks.cpp) is part of LFSDesktop.

 * LFSDesktop is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSDesktop is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSDesktop.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "globals.h"

bool windowDrop(LFSTK_windowClass *lwc,void* ud)
{
	char	*command;
	char	*data;
	char	name[17];

	if(lwc!=NULL)
		{
			if(lwc->droppedData.type==DROPINVALID)
				return(false);

			if(lwc->droppedData.type==DROPURI)
				{
					asprintf(&command,"cp '%s' '%s'",lwc->droppedData.data,desktopPath);
					system(command);
					free(command);
				}

			if(lwc->droppedData.type==DROPTEXT)
				{
					data=strdup(lwc->droppedData.data);
					snprintf(name,16,"%s",data);
					for(int j=0;j<15;j++)
						{
							if((!isalpha(name[j])) && (!isdigit(name[j])) )
								name[j]='_';
						}
					asprintf(&command,"echo -n '%s' > '%s/%s'",lwc->droppedData.data,desktopPath,name);
					system(command);
					free(command);
					free(data);
				}
		}
	return(true);
}

bool doDiskMenuSelect(void *p,void* ud)
{
	char				*command;
	int					retval=0;
	LFSTK_windowClass	*lwc=static_cast<LFSTK_gadgetClass*>(p)->wc;
	LFSTK_gadgetClass	*gadg=static_cast<LFSTK_gadgetClass*>(p)->wc->popupFromGadget;
	int					winnum;
	std::string			retstr;

	if(p!=NULL)
		{
			winnum=lwc->app->LFSTK_findWindow(lwc);
			wc->app->windows->at(winnum).loopFlag=false;

			switch((long)ud)//TODO//
				{
					case MOUNTDISK:
					case UNMOUNTDISK:
					case EJECTDISK:
						asprintf(&command,"%s \"%s\" \"/media/%s\" %lu &>/dev/null",HELPERAPP,desktopItems.at((unsigned long)gadg->userData).uuid,desktopItems.at((unsigned long)gadg->userData).label,(unsigned long)ud+1);
						retval=system(command);
						free(command);
						if(WEXITSTATUS(retval)==0)
							updateMounted();

					case OPENDISK:
						if(((long)ud==MOUNTDISK) || ((long)ud==OPENDISK))
							{
								asprintf(&command,"xdg-open '%s' & ",apc->globalLib->LFSTK_getNthNeedle(apc->globalLib->LFSTK_grepInFile("/proc/mounts",desktopItems.at((unsigned long)gadg->userData).itemPath),2," ").c_str());
								system(command);
								free(command);
							}
							
						break;
					case CUSTOMICONDISK:
//TODO//
						break;
					case REMOVECUSTOMDISK:
//TODO//
						break;
				}

			gadg->gadgetDetails.state=NORMALCOLOUR;
			gadg->LFSTK_clearWindow();
			XSync(apc->display,false);
		}

	return(true);
}

bool doDeskItemMenuSelect(void *p,void* ud)
{
	LFSTK_windowClass	*lwc=static_cast<LFSTK_gadgetClass*>(p)->wc;
	LFSTK_gadgetClass	*gadg=static_cast<LFSTK_gadgetClass*>(p)->wc->popupFromGadget;
	int					winnum;
	char				*command=NULL;

	if(p!=NULL)
		{
			winnum=lwc->app->LFSTK_findWindow(lwc);
			wc->app->windows->at(winnum).loopFlag=false;

			switch((long)ud)
				{
					case DESKITEMOPEN:
						asprintf(&command,"xdg-open '%s' &",desktopItems.at((unsigned long)gadg->userData).itemPath);
						system(command);
						free(command);
						break;

					case DESKITEMEXECUTE://TODO// int term
						if(desktopItems.at((unsigned long)gadg->userData).type==ISDESKTOPFILE)
							{
								GKeyFile	*gkf=g_key_file_new();
								char		*value=NULL;
								if(g_key_file_load_from_file(gkf,desktopItems.at((unsigned long)gadg->userData).itemPath,G_KEY_FILE_NONE,NULL)==true)
									{
										value=g_key_file_get_string(gkf,"Desktop Entry",G_KEY_FILE_DESKTOP_KEY_EXEC,NULL);
										asprintf(&command,"%s &",value);
										freeAndNull(&value);
									}
								g_key_file_free(gkf);
							}
						else
							asprintf(&command,"%s &",desktopItems.at((unsigned long)gadg->userData).itemPath);

						if(command!=NULL)
							{
								system(command);
								free(command);
							}
						break;

					case DESKITEMCUSTOMICON:
						{
							LFSTK_fileDialogClass	*filedialogfile;
							char					*pth;

							fileWindow->LFSTK_hideWindow();
							filedialogfile=new LFSTK_fileDialogClass(wc,"Select File",NULL,FILEDIALOG);
							filedialogfile->LFSTK_showFileDialog(NULL,"Select A File");
							if(filedialogfile->LFSTK_isValid()==true)
								{
									desktopItems.at((unsigned long)gadg->userData).hasCustomIcon=true;
									freeAndNull(&desktopItems.at((unsigned long)gadg->userData).pathToCustomIcon);
									desktopItems.at((unsigned long)gadg->userData).pathToCustomIcon=strdup(filedialogfile->LFSTK_getCurrentPath().c_str());
									asprintf(&pth,"%s/%s",cachePath,desktopItems.at((unsigned long)gadg->userData).uuid);
									saveCacheFile(pth,&(desktopItems.at((unsigned long)gadg->userData)));
									desktopItems.at((unsigned long)gadg->userData).item->LFSTK_setImageFromPath(desktopItems.at((unsigned long)gadg->userData).pathToCustomIcon,TOOLBAR,true);
									free(pth);
								}
						}
						break;
					case DESKITEMREMOVECUSTOM:
						{
							char	*pth;

							desktopItems.at((unsigned long)gadg->userData).hasCustomIcon=false;
							freeAndNull(&desktopItems.at((unsigned long)gadg->userData).pathToCustomIcon);
							asprintf(&pth,"%s/%s",cachePath,desktopItems.at((unsigned long)gadg->userData).uuid);
							setIconImage(&(desktopItems.at((unsigned long)gadg->userData)));
							desktopItems.at((unsigned long)gadg->userData).item->LFSTK_setImageFromPath((desktopItems.at((unsigned long)gadg->userData)).iconPath,TOOLBAR,true);
							free(pth);
						}
						break;
				}
			gadg->gadgetDetails.state=NORMALCOLOUR;
			gadg->LFSTK_clearWindow();
			XSync(apc->display,false);
		}
	return(true);
}

bool mouseUpCB(void *p,void* ud)
{
	geometryStruct		geom;
	unsigned long		itemnum=(unsigned long)ud;
	char				*cachefilepath;
	LFSTK_buttonClass	*gadg;
	char				*command;
	std::string			retstr;

	if(p!=NULL)
		{
			gadg=static_cast<LFSTK_buttonClass*>(p);
			gadg->LFSTK_getGeom(&geom);
			if(gadg->isDoubleClick==true)
				{
					switch(desktopItems.at((unsigned long)gadg->userData).type)
						{
							case ISHOMEFOLDER:
								system("xdg-open ~");
								break;
							case ISCOMPUTER:
								system("xdg-open /");
								break;
							case ISDOCUMENTSFOLDER:
								system("xdg-open ~/Documents");
								break;
							case ISDESKTOPFILE://TODO// in term
								{
									GKeyFile	*gkf=g_key_file_new();
									char		*value=NULL;
									if(g_key_file_load_from_file(gkf,desktopItems.at((unsigned long)gadg->userData).itemPath,G_KEY_FILE_NONE,NULL)==true)
										{
											value=g_key_file_get_string(gkf,"Desktop Entry",G_KEY_FILE_DESKTOP_KEY_EXEC,NULL);
											asprintf(&command,"%s &",value);
											free(value);
											system(command);
											free(command);
										}
									g_key_file_free(gkf);
								}
								break;
							case ISHDDDISK:
							case ISUSBHDD:
							case ISTHUMBDISK:
							case ISCDROM:
							case ISDVDROM:
							case ISIPOD:
							case ISSSD:
								asprintf(&command,"%s \"%s\" \"/media/%s\" %i &>/dev/null",HELPERAPP,desktopItems.at((unsigned long)gadg->userData).uuid,desktopItems.at((unsigned long)gadg->userData).label,1);
								system(command);
								free(command);
								updateMounted();
								asprintf(&command,"xdg-open '%s' &",apc->globalLib->LFSTK_getNthNeedle(apc->globalLib->LFSTK_grepInFile("/proc/mounts",desktopItems.at((unsigned long)gadg->userData).itemPath),2," ").c_str());
								system(command);
								free(command);
								break;
							default:
								if((doubleClickExecute==true) && (access(desktopItems.at((unsigned long)gadg->userData).itemPath,X_OK)==0))
									{
										if(desktopItems.at((unsigned long)gadg->userData).isAFolder==true)
											asprintf(&command,"xdg-open '%s' &",desktopItems.at((unsigned long)gadg->userData).itemPath);
										else
											asprintf(&command,"'%s' &",desktopItems.at((unsigned long)gadg->userData).itemPath);
									}
								else
									asprintf(&command,"xdg-open '%s' &",desktopItems.at((unsigned long)gadg->userData).itemPath);
								system(command);
								free(command); 
						}
				}
			else
				{
					int midicon=geom.x+(geom.w/2);
					int grid=midicon/gridSize;
					int realx;
					int realy;
					realx=grid*gridSize-(geom.w/2);
					midicon=geom.y+(geom.h/2);
					grid=midicon/gridSize;
					realy=grid*gridSize-(geom.h/2);
					desktopItems.at(itemnum).item->LFSTK_moveGadget(realx,realy);
					desktopItems.at(itemnum).posx=realx;
					desktopItems.at(itemnum).posy=realy;
					cacheFileData=desktopItems.at(itemnum);
					asprintf(&cachefilepath,"%s/%s",cachePath,desktopItems.at(itemnum).uuid);
					saveCacheFile(cachefilepath,&desktopItems.at(itemnum));
					freeAndNull(&cachefilepath);
				}
		}
	return(true);
}

void readMsg(void);

bool timerCB(LFSTK_applicationClass *p,void* ud)
{
	char	*out=NULL;
	char	uuid[256]={0,};
	char	fullpath[256]={0,};
	int		retval=0;

	if(apc->isDragging==true)
		return(true);
	readMsg();

	out=strdup(apc->globalLib->LFSTK_oneLiner("%s","ls -1 /dev/disk/by-uuid|md5sum|awk '{print $1}'").c_str());//TODO//
	if(out!=NULL)
		{
			if(strcmp(out,diskWatch)!=0)
				{
					freeAndNull(&diskWatch);
					diskWatch=strdup(out);
					for(unsigned j=0;j<desktopItems.size();j++)
						{
							if(desktopItems.at(j).isADisk==true)
								{
									if(desktopItems.at(j).uuid!=NULL)
										{
											sprintf(uuid,"findfs UUID='%s' &>/dev/null",desktopItems.at(j).uuid);
											retval=system(uuid);
											if(retval!=0)
												desktopItems.at(j).item->LFSTK_moveGadget(-1000,-1000);
										}
								}
						}
					makeDiskButtons();
					wc->LFSTK_showWindow(true);
				}
			freeAndNull(&out);
		}

	out=strdup(apc->globalLib->LFSTK_oneLiner("ls -1 '%s'|md5sum|awk '{print $1}'",desktopPath).c_str());//TODO//
	if(out!=NULL)
		{
			if(strcmp(out,desktopWatch)!=0)
				{
					freeAndNull(&desktopWatch);
					desktopWatch=strdup(out);
					for(unsigned j=0;j<desktopItems.size();j++)
						{
							if(desktopItems.at(j).isADisk==false)
								{
									if(desktopItems.at(j).uuid!=NULL)
										{
											sprintf(fullpath,"%s/%s",desktopPath,desktopItems.at(j).uuid);
											if((strcmp(desktopItems.at(j).uuid,"Home")!=0) && (strcmp(desktopItems.at(j).uuid,"Computer")!=0) && (strcmp(desktopItems.at(j).uuid,"Documents")!=0))
												{
													retval=access(fullpath,F_OK);
													if(retval!=0)
														desktopItems.at(j).item->LFSTK_moveGadget(-20000,10000);//TODO//
												}
										}
								}
						}
					makeFileButtons(true);
					wc->LFSTK_showWindow(true);
					wc->LFSTK_redrawAllGadgets();
				}
			freeAndNull(&out);
		}

	updateMounted();

	return(true);
}
