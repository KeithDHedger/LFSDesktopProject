/*
 *
 * ©K. D. Hedger. Thu Jan 11 13:01:45 GMT 2018 keithdhedger@gmail.com

 * This file (disks.cpp) is part of LFSDesktop.

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

#include "disks.h"

void setDiskType(diskDataStruct *dnode)
{
	char		*out=NULL;
	int			disktype=HDDDISK;

//cdrom
	out=wc->globalLib->LFSTK_oneLiner("/sbin/udevadm info --query=property --name=%s|grep 'ID_CDROM=1'",dnode->devName);
	if(strlen(out)>0)
		{
			disktype=CDROM;
			free(out);
		}
//usb
	out=wc->globalLib->LFSTK_oneLiner("/sbin/udevadm info --query=property --name=%s|grep 'usb'",dnode->devName);
	if(strlen(out)>0)
		{
			free(out);
			out=wc->globalLib->LFSTK_oneLiner("/sbin/udevadm info --query=property --name=%s|grep 'ID_DRIVE_THUMB'",dnode->devName);
			if(strlen(out)>0)
				{
					disktype=THUMBDISK;
				}
			else
				{
					disktype=USBHDD;
				}
			free(out);
		}

//disk type
	dnode->diskType=disktype;
}

bool doDiskMenuSelect(void *p,void* ud)
{
	char			*command;
	int				retval=0;
	char			*filename;
	diskDataStruct *dnode=NULL;
	if(p!=NULL)
		{
			wc->popupLoop=false;
			dnode=static_cast<diskDataStruct*>(wc->popupFromGadget->userData);

			switch((long)ud)
				{
					case MOUNTDISK:
					case UNMOUNTDISK:
					case EJECTDISK:
						asprintf(&command,"%s \"%s\" \"/media/%s\" %lu &>/dev/null",HELPERAPP,dnode->uuid,dnode->label,(unsigned long)ud+1);
						retval=system(command);
						free(command);
						if((WEXITSTATUS(retval)==0) && ((long)ud)==MOUNTDISK)
							updateDisks();

					case OPENDISK:
						if(((long)ud==MOUNTDISK) || ((long)ud==OPENDISK))
							if(dnode->mounted==true)
								{
									asprintf(&command,"findmnt -lno TARGET -S UUID=\"%s\"|xargs xdg-open & &>/dev/null",dnode->uuid);
									system(command);
									free(command);
								}
						break;
					case CUSTOMICONDISK:
						diskWindow->LFSTK_hideWindow();
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
								asprintf(&filename,"%s/%s.rc",cacheDisksPath,dnode->uuid);
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
						setIconImage(dnode);
						dnode->diskImage->LFSTK_setImageFromPath(dnode->pathToIcon,TOOLBAR,true);
						dnode->diskImage->LFSTK_clearWindow();
						asprintf(&filename,"%s/%s.rc",cacheDisksPath,dnode->uuid);
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

bool diskUpCB(void *p,void* ud)
{
	char			*diskfile;
	diskDataStruct	*dnode=(diskDataStruct*)ud;
	geometryStruct	geom;
	int 			realposx;
	int 			realposy;

	if(dnode->diskImage->currentButton!=Button1)
		return(true);

	if(dnode->diskImage->isDoubleClick==true)
		{
			wc->popupFromGadget=dnode->diskImage;
			doDiskMenuSelect(p,(void*)MOUNTDISK);
			wc->popupFromGadget=NULL;
			return(true);
		}

	dnode->diskImage->LFSTK_getGeom(&geom);

	asprintf(&diskfile,"%s/%s.rc",cacheDisksPath,dnode->uuid);

	diskUUID=dnode->uuid;
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
	dnode->diskImage->LFSTK_clearWindow();
	return(true);
}

void setDiskLabel(diskDataStruct *dnode)
{
	char	*holdlabel=NULL;

	holdlabel=wc->globalLib->LFSTK_oneLiner("/sbin/udevadm info --query=property --name='%s'|grep 'ID_FS_LABEL'|awk -F= '{print $2}'",dnode->devName);
	if(strlen(holdlabel)==0)
		{
			free(holdlabel);
			holdlabel=wc->globalLib->LFSTK_oneLiner("/sbin/udevadm info --query=property --name='%s'|sed -rn  's/(ID_MODEL|ID_VENDOR)=//gp'|sed -z 's/\\n/-/'",dnode->devName);
			if(strlen(holdlabel)==0)
				{
					free(holdlabel);
					holdlabel=strdup(dnode->devName);
				}
		}

	if(dnode->label!=NULL)
		{
			if(strcmp(dnode->label,holdlabel)!=0)
				{
					dnode->dirty=true;
					free(dnode->label);
					dnode->label=strdup(holdlabel);
				}
		}
	else
		dnode->label=strdup(holdlabel);
}


bool setDiskData(diskDataStruct *dnode)
{
	char	*buffer=NULL;
	bool	oldmounted=dnode->mounted;

	buffer=wc->globalLib->LFSTK_oneLiner("/sbin/udevadm info --query=property '%s' 2>/dev/null|grep 'ID_FS_UUID='|awk -F= '{print $2}'",dnode->devName);
	if(strlen(buffer)==0)
		{
			freeAndNull(&dnode->label);
			freeAndNull(&dnode->uuid);
			freeAndNull(&dnode->pathToIcon);
			dnode->mounted=false;
			dnode->driveHasMedia=false;
			free(buffer);
			return(false);
		}

	dnode->dirty=false;
	setDiskType(dnode);
	dnode->uuid=buffer;
	setDiskLabel(dnode);
	buffer=wc->globalLib->LFSTK_oneLiner("findmnt -no TARGET '%s'",dnode->devName);
	if(strlen(buffer)>0)
		dnode->mounted=true;
	else
		dnode->mounted=false;

	if(oldmounted!=dnode->mounted)
		dnode->dirty=true;

	dnode->driveHasMedia=true;
	free(buffer);

	return(true);
}

void addDiskData(diskDataStruct *dnode,const char *devname,int x,int y)
{
	char	*diskfile;
	int		realposx;
	int		realposy;

	dnode->devName=strdup(devname);

	if(setDiskData(dnode)==true)
		{
//			setIconImage(dnode);

			asprintf(&diskfile,"%s/%s.rc",cacheDisksPath,dnode->uuid);
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
			setIconImage(dnode);

			freeAndNull(&diskUUID);
			freeAndNull(&iconPath);
			customIcon=false;
			free(diskfile);

			dnode->diskImage=new LFSTK_buttonClass(wc,dnode->label,dnode->posx,dnode->posy,iconSize,iconSize,BUTTONGRAV);

			dnode->diskImage->LFSTK_setFontString(fontFace,true);
			setImageSize(dnode);
			getRealXY(dnode,&realposx,&realposy);
			dnode->diskImage->LFSTK_moveGadget(realposx,realposy);

			dnode->diskImage->LFSTK_setCanDrag(true);
			dnode->diskImage->LFSTK_setStyle(BEVELNONE);
			dnode->diskImage->LFSTK_snapSize(gridBorderLeft+iconSize);
			dnode->diskImage->LFSTK_snapSize(8);
			dnode->diskImage->LFSTK_setImageFromPath(dnode->pathToIcon,TOOLBAR,true);
			dnode->diskImage->LFSTK_setUseWindowPixmap(true);
			dnode->diskImage->LFSTK_setCallBack(NULL,diskUpCB,(void*)dnode);
			dnode->diskImage->LFSTK_setContextWindow(diskWindow);

			dnode->diskImage->LFSTK_setLabelBGColour(0.75,0.75,0.75,strtod(backAlpha,NULL));
			wc->globalLib->LFSTK_setColourFromName(wc->display,wc->cm,&dnode->diskImage->labelBGColour,backCol);
			dnode->diskImage->LFSTK_setFontColourName(NORMALCOLOUR,foreCol,false)	;	
			dnode->diskImage->drawLabelBG=true;
			dnode->diskImage->autoLabelBGColour=false;

			dnode->diskImage->userData=(void*)dnode;

			if(dnode->mounted==true)
				dnode->diskImage->LFSTK_setAlpha(1.0);
			else
				dnode->diskImage->LFSTK_setAlpha(0.5);
		}
}

void checkForChanges(diskDataStruct *dnode)
{
	if(dnode->dataType==FILEDATATYPE)
		return;

	if(setDiskData(dnode)==false)
		{
			if(dnode->diskImage!=NULL)
				{
					dnode->diskImage->LFSTK_hideGadget();
					wc->LFSTK_clearWindow();
				}
		}
	else
		{
			if(dnode->diskImage!=NULL)
				{
					dnode->diskImage->LFSTK_showGadget();
					if(dnode->mounted==true)
						dnode->diskImage->LFSTK_setAlpha(1.0);
					else
						dnode->diskImage->LFSTK_setAlpha(0.5);
					if(dnode->dirty==true)
						{
							dnode->diskImage->LFSTK_setLabel(dnode->label);
							setImageSize(dnode);
							dnode->diskImage->LFSTK_clearWindow();
							wc->LFSTK_clearWindow();
						}
				}
			else
				{
					addDiskData(dnode,dnode->devName,dnode->posx,dnode->posy);
				}
		}
}

void updateDisks(void)
{
	diskLinkedList	*list=diskLL;

	if(list==NULL)
		return;

	do
		{
			if(list->data==NULL)
				{
					list=list->next;
					continue;
				}
			checkForChanges(list->data);
			list=list->next;
		}
	while(list!=NULL);
}

void loadDisks(void)
{
	char		*command;
	FILE		*fd=NULL;
	char		buffer[2048];
	int			x;
	int			y;

	diskLinkedList	*disklistnode=NULL;
 
	asprintf(&command,"find /dev -maxdepth 1 -mindepth 1  -regextype sed -regex \"%s\"|grep -v \"%s\"|sort --version-sort",includeList,excludeList);
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
							disklistnode=isInList(buffer);
							if(disklistnode==NULL)
								{
					//printf("---%s---\n",buffer);
									newNode();
									disklistnode=diskLL;
									getFreeGridXY(&x,&y);
									addDiskData(disklistnode->data,buffer,x,y);
									disklistnode->data->dataType=DISKDATATYPE;
									disklistnode->data->dirty=true;
									//printDiskData(disklistnode->data);
								}
							buffer[0]=0;
						}
				}
			pclose(fd);
		}
}
