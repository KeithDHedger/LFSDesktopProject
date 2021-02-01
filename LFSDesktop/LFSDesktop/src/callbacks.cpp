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
					data=g_strstrip(data);
					data=g_strdelimit(data,"\"'\t <>=.(){}\\|",'_');
					snprintf(name,16,"%s",data);
					asprintf(&command,"echo '%s' > '%s/%s'",lwc->droppedData.data,desktopPath,name);
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

	if(p!=NULL)
		{
			winnum=lwc->app->LFSTK_findWindow(lwc);
			wc->app->windows->at(winnum).loopFlag=false;

			switch((long)ud)
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
								asprintf(&command,"findmnt -lno TARGET -S UUID=\"%s\"|head -n1|xargs xdg-open &>/dev/null & &>/dev/null",desktopItems.at((unsigned long)gadg->userData).uuid);
								//printf("command=%s\n",command);
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

					case DESKITEMEXECUTE:
						if(desktopItems.at((unsigned long)gadg->userData).type==ISDESKTOPFILE)
							{
								GKeyFile	*gkf=g_key_file_new();
								char		*value=NULL;
								if(g_key_file_load_from_file(gkf,desktopItems.at((unsigned long)gadg->userData).itemPath,G_KEY_FILE_NONE,NULL)==true)
									{
										value=g_key_file_get_string(gkf,"Desktop Entry",G_KEY_FILE_DESKTOP_KEY_EXEC,NULL);
										asprintf(&command,"%s &",value);
										free(value);
									}
								g_key_file_free(gkf);
							}
						else
							asprintf(&command,"%s &",desktopItems.at((unsigned long)gadg->userData).itemPath);

						if(command!=NULL)
							{
								printf("command=%s\n",command);
								system(command);
								free(command);
							}
						break;
					case CUSTOMICONDISK://TODO//
						break;
					case REMOVECUSTOMDISK://TODO//
						break;
				}
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

	if(p!=NULL)
		{
			gadg=static_cast<LFSTK_buttonClass*>(p);
			gadg->LFSTK_getGeom(&geom);
			if(gadg->isDoubleClick==true)
				{
				//	fprintf(stderr,"double click x=%i y=%i on %s @%lu\n",geom.x,geom.y,gadg->LFSTK_getLabel(),itemnum);
					switch(desktopItems.at((unsigned long)gadg->userData).type)
						{
							case ISDESKTOPFILE:
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
								asprintf(&command,"%s \"%s\" \"/media/%s\" %i &>/dev/null",HELPERAPP,desktopItems.at((unsigned long)gadg->userData).uuid,desktopItems.at((unsigned long)gadg->userData).label,1);
								system(command);
								free(command);
								updateMounted();
								asprintf(&command,"findmnt -lno TARGET -S UUID=\"%s\"|head -n1|xargs xdg-open &>/dev/null & &>/dev/null",desktopItems.at((unsigned long)gadg->userData).uuid);
								system(command);
								free(command);
								break;
							default:
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
					saveVarsToFile(cachefilepath,cacheFileDataPrefs);	
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

	out=apc->globalLib->LFSTK_oneLiner("%s","ls -1 /dev/disk/by-partuuid|md5sum|awk '{print $1}'");
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

	out=apc->globalLib->LFSTK_oneLiner("ls -1 '%s'|md5sum|awk '{print $1}'",desktopPath);
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
														desktopItems.at(j).item->LFSTK_moveGadget(-1000,-1000);//TODO//
												}
										}
								}
						}
					makeFileButtons(true);
					wc->LFSTK_showWindow(true);
				}
			freeAndNull(&out);
		}

	updateMounted();
	return(true);
}
