/*
 *
 * ©K. D. Hedger. Thu 13 Aug 16:53:55 BST 2015 kdhedger68713@gmail.com

 * This file (files.cpp) is part of LFSDesktop.

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

#include "files.h"


#if 0
char* getMimeType(char *filepath)
{
	const char			*mime;
	struct magic_set	*magic;
	char				*returnstr=NULL;

	magic=magic_open(MAGIC_MIME_TYPE|MAGIC_SYMLINK);
	magic_load(magic,NULL);
	mime=magic_file(magic,filepath);
	if(mime!=NULL)
		returnstr=strdup(mime);
	magic_close(magic);
	return(returnstr);
}
#endif

void launchDesktopFile(const char *name)
{
	char	*out=NULL;
	out=wc->globalLib->LFSTK_oneLiner("sed -n 's/^Exec=\\(.*\\)/\\1 \\&/Ip' '%s/%s'|sed 's/%%.//g'",desktopPath,name);
	system(out);
	free(out);	
}

void doGetIconPath(void)
{
	XEvent				event;

	iconChooser->LFSTK_showWindow();
	iconChooser->LFSTK_setKeepAbove(true);
	iconChooser->LFSTK_setTransientFor(wc->window);
	while(dialogLoop==true)
		{
			while (XPending(display) && (dialogLoop==true))
				{
					XNextEvent(iconChooser->display,&event);
					mappedListener *ml=iconChooser->LFSTK_getMappedListener(event.xany.window);
					if(ml!=NULL)
						ml->function(ml->gadget,&event,ml->type);

					switch(event.type)
						{
							case Expose:
								iconChooser->LFSTK_clearWindow();
								break;

							case ConfigureNotify:
								iconChooser->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height,false);
								iconChooser->globalLib->LFSTK_setCairoSurface(iconChooser->display,iconChooser->window,iconChooser->visual,&iconChooser->sfc,&iconChooser->cr,event.xconfigurerequest.width,event.xconfigurerequest.height);
								break;

							case ClientMessage:
							case SelectionNotify:
								{
									if (event.xclient.message_type == XInternAtom(iconChooser->display, "WM_PROTOCOLS", 1) && (Atom)event.xclient.data.l[0] == XInternAtom(iconChooser->display, "WM_DELETE_WINDOW", 1))
										{
											iconChooser->LFSTK_hideWindow();
											dialogLoop=false;
											dialogRetVal=0;
										}
//dnd for edit box
									if(iconChooser->acceptDnd==true)
										{
											iconChooser->LFSTK_handleDnD(&event);
											iconChooser->droppedData.type=-1;
										}
								}
						}
				}
		}
	iconChooser->LFSTK_hideWindow();
}

const char* getSuffix(const char *path)
{
	return(strrchr(path,'.'));
}

void setDeskType(diskDataStruct *dnode)
{
	char		*out=NULL;
	char		*ticon=NULL;
	const char	*suffix=NULL;

	dnode->diskType=DESKFILE;
	suffix=getSuffix(dnode->devName);
	if(suffix!=NULL)
		{
			if(strcasecmp(suffix,".desktop")==0)
				{
				
					out=wc->globalLib->LFSTK_oneLiner("sed -n 's/^name=\\(.*\\)$/\\1/Ip' '%s/%s'",desktopPath,dnode->devName);
					if(strlen(out)>0)
						{
							freeAndNull(&dnode->label);
							dnode->label=strdup(out);
						}
					free(out);
				
					dnode->diskType=DESKTOPFILE;
					out=wc->globalLib->LFSTK_oneLiner("sed -n 's/^icon=\\(.*\\)$/\\1/Ip' '%s/%s'",desktopPath,dnode->devName);
					if(strlen(out)>0)
						{
							ticon=wc->globalLib->LFSTK_findThemedIcon(iconTheme,out,"");
							if(ticon!=NULL)
								dnode->pathToIcon=ticon;
							free(out);
							return;
						}
				}
		}

	out=wc->globalLib->LFSTK_oneLiner("file -bL --mime-type '%s/%s'|awk -F/ '{print \"-\" $2}'",desktopPath,dnode->devName);
	if(strcmp(out,"-directory")==0)
		ticon=wc->globalLib->LFSTK_findThemedIcon(iconTheme,out,"places");
	else
		ticon=wc->globalLib->LFSTK_findThemedIcon(iconTheme,out,"mimetypes");

	if(ticon==NULL)
		ticon=wc->globalLib->LFSTK_findThemedIcon(iconTheme,"text-plain","mimetypes");
	free(out);
	dnode->pathToIcon=ticon;
}

bool doDeskItemMenuSelect(void *p,void* ud)
{
	diskDataStruct *dnode=NULL;
	char			*filename;
	if(p!=NULL)
		{
			wc->popupLoop=false;
			dnode=static_cast<diskDataStruct*>(wc->popupFromGadget->userData);
			
			switch((long)ud)
				{
					case OPENDISK:
						if(dnode->diskType==DESKTOPFILE)
							launchDesktopFile(dnode->devName);
						if(dnode->diskType==DESKFILE)
							{
								filename=wc->globalLib->LFSTK_oneLiner("xdg-open '%s/%s' &",desktopPath,dnode->devName);
								free(filename);
							}
						break;
					case CUSTOMICONDISK:
						fileWindow->LFSTK_hideWindow();
						dialogLoop=true;
						dialogRetVal=DIALOGRETERROR;
						dialogRun(iconChooser);
						if(dialogRetVal==DIALOGRETAPPLY)
							{
								freeAndNull(&dnode->pathToIcon);
								dnode->pathToIcon=strdup(iconChooserEdit->LFSTK_getBuffer()->c_str());
								dnode->hasCustomIcon=true;
								dnode->diskImage->LFSTK_setImageFromPath(dnode->pathToIcon,TOOLBAR,true);
								dnode->diskImage->LFSTK_clearWindow();
								asprintf(&filename,"%s/%s.rc",cacheDeskPath,dnode->devName);
								iconPath=dnode->pathToIcon;
								customIcon=dnode->hasCustomIcon;
								dataType=dnode->dataType;
								xPos=dnode->posx;
								yPos=dnode->posy;
								saveVarsToFile(filename,diskData);
								diskUUID=NULL;
								iconPath=NULL;
								customIcon=false;
								dataType=TYPENONE;
								free(filename);
							}
						break;
					case REMOVECUSTOMDISK:
						freeAndNull(&dnode->pathToIcon);
						dnode->hasCustomIcon=false;
						setDeskType(dnode);
						dnode->diskImage->LFSTK_setImageFromPath(dnode->pathToIcon,TOOLBAR,true);
						dnode->diskImage->LFSTK_clearWindow();
						asprintf(&filename,"%s/%s.rc",cacheDeskPath,dnode->devName);
						iconPath=dnode->pathToIcon;
						customIcon=dnode->hasCustomIcon;
						dataType=dnode->dataType;
						xPos=dnode->posx;
						yPos=dnode->posy;
						saveVarsToFile(filename,diskData);
						diskUUID=NULL;
						iconPath=NULL;
						customIcon=false;
						dataType=TYPENONE;
						free(filename);
						break;
				}
		}
	return(true);
}

bool deskUpCB(void *p,void* ud)
{
	char			*diskfile;
	int 			realposx;
	int 			realposy;
	diskDataStruct	*dnode=(diskDataStruct*)ud;
	geometryStruct	geom;
	char			*filename=NULL;

	if(dnode->diskImage->currentButton!=Button1)
		return(true);

	if(dnode->diskImage->isDoubleClick==true)
		{
			if(dnode->diskType==DESKTOPFILE)
				launchDesktopFile(dnode->devName);
			if(dnode->diskType==DESKFILE)
				{
					filename=wc->globalLib->LFSTK_oneLiner("xdg-open '%s/%s' &",desktopPath,dnode->devName);
					free(filename);
				}
			return(true);
		}
	dnode->diskImage->LFSTK_getGeom(&geom);

	asprintf(&diskfile,"%s/%s.rc",cacheDeskPath,dnode->devName);

	diskUUID=NULL;
	iconPath=dnode->pathToIcon;

	customIcon=dnode->hasCustomIcon;
	dataType=dnode->dataType;
	setGridXY(dnode,geom.x,geom.y);
	xPos=dnode->posx;
	yPos=dnode->posy;
	saveVarsToFile(diskfile,diskData);

	getRealXY(dnode,&realposx,&realposy);	
	dnode->diskImage->LFSTK_moveGadget(realposx,realposy);

	diskUUID=NULL;
	iconPath=NULL;
	customIcon=false;
	dataType=TYPENONE;
	free(diskfile);
	free(filename);
	dnode->diskImage->LFSTK_clearWindow();

	return(true);
}

void setDeskData(diskDataStruct *dnode)
{
	char	*base=NULL;
	char	*suffixptr=NULL;

	base=strdup(dnode->devName);

	if(showSuffix==false)
		{
			suffixptr=strrchr(base,'.');
			if(suffixptr!=NULL)
				*suffixptr=0;
		}
	dnode->label=strdup(basename(base));
	free(base);
	setDeskType(dnode);
	return;
}

void addDeskData(diskDataStruct *dnode,const char *devname,int x,int y)
{
	char	*diskfile;
	int		realposx;
	int		realposy;

	dnode->devName=strdup(devname);

	setDeskData(dnode);

	asprintf(&diskfile,"%s/%s.rc",cacheDeskPath,dnode->devName);
	if(loadVarsFromFile(diskfile,diskData)==true)
		{
			dnode->posx=xPos;
			dnode->posy=yPos;
			dnode->hasCustomIcon=customIcon;
			if(dnode->hasCustomIcon==true)
				{
					free(dnode->pathToIcon);
					dnode->pathToIcon=strdup(iconPath);
				}
		}
	else
		{
			dnode->posx=x;
			dnode->posy=y;
		}

	freeAndNull(&diskUUID);
	freeAndNull(&iconPath);
	customIcon=false;
	free(diskfile);

	dnode->diskImage=new LFSTK_buttonClass(wc,dnode->label,dnode->posx,dnode->posy,iconSize,iconSize,NorthWestGravity);
	setImageSize(dnode);
	getRealXY(dnode,&realposx,&realposy);
	dnode->diskImage->LFSTK_moveGadget(realposx,realposy);

	dnode->diskImage->LFSTK_setCanDrag(true);
	dnode->diskImage->LFSTK_setStyle(BEVELNONE);
	dnode->diskImage->LFSTK_setImageFromPath(dnode->pathToIcon,TOOLBAR,true);
	dnode->diskImage->LFSTK_setUseWindowPixmap(true);
	dnode->diskImage->LFSTK_setCallBack(NULL,deskUpCB,(void*)dnode);
	dnode->diskImage->LFSTK_setContextWindow(fileWindow);

	dnode->diskImage->LFSTK_setLabelBGColour(0.75,0.75,0.75,strtod(backAlpha,NULL));			
	wc->globalLib->LFSTK_setColourFromName(wc->display,wc->cm,&dnode->diskImage->labelBGColour,backCol);
	dnode->diskImage->drawLabelBG=true;
	dnode->diskImage->autoLabelBGColour=false;

	dnode->diskImage->userData=(void*)dnode;
}

void addDeskItem(const char *name)
{
	int				x;
	int				y;
	diskLinkedList	*disklistnode=NULL;

	newNode();
	disklistnode=diskLL;
	getFreeGridXY(&x,&y);

	addDeskData(disklistnode->data,name,x,y);
	disklistnode->data->dataType=FILEDATATYPE;
	disklistnode->data->dirty=true;
	disklistnode->data->diskImage->LFSTK_clearWindow();
	wc->LFSTK_clearWindow();
}

void deleteDeskItem(const char *name)
{
	geometryStruct	geom;
	diskLinkedList	*disklistnode=NULL;
	diskLinkedList	*prevnode=NULL;
	diskLinkedList	*nextnode=NULL;
	diskDataStruct	*data=NULL;
	disklistnode=isInList(name);

	if(disklistnode!=NULL)
		{
			prevnode=disklistnode->prev;
			nextnode=disklistnode->next;
			if(prevnode!=NULL)
				prevnode->next=nextnode;

			if(nextnode!=NULL)
				nextnode->prev=prevnode;

			data=disklistnode->data;
			if(data!=NULL)
				{
					freeAndNull(&data->label);
					freeAndNull(&data->devName);
					freeAndNull(&data->uuid);
					freeAndNull(&data->pathToIcon);
					data->diskImage->LFSTK_getGeom(&geom);
					delete data->diskImage;
				}
			if(disklistnode==diskLL)
				diskLL=nextnode;
			free(disklistnode);
		}
}

void loadDesktopItems(void)
{
	char		*command;
	FILE		*fd=NULL;
	char		buffer[2048];
	int			x;
	int			y;
	char		*itemname=NULL;

//printf(">>loadDesktopItems<<\n");
	diskLinkedList	*disklistnode=NULL;
	asprintf(&command,"find %s -maxdepth 1 -mindepth 1 |sort",desktopPath);

	fd=popen(command,"r");
	if(fd!=NULL)
		{
			while(feof(fd)==0)
				{
					buffer[0]=0;
					fgets(buffer,2048,fd);
					if(strlen(buffer)>0)
						{
							buffer[strlen(buffer)-1]=0;
							itemname=basename(buffer);
							//removeDeleted();
							disklistnode=isInList(itemname);
							if(disklistnode==NULL)
								{
								
									newNode();
									disklistnode=diskLL;
									getFreeGridXY(&x,&y);
									addDeskData(disklistnode->data,itemname,x,y);
									disklistnode->data->dataType=FILEDATATYPE;
									disklistnode->data->dirty=true;
									//printDiskData(disklistnode->data);
									disklistnode->data->diskImage->LFSTK_clearWindow();
									wc->LFSTK_clearWindow();
								}
						}
					buffer[0]=0;
				}
			pclose(fd);
		}
}