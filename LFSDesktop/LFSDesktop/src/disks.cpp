/*
 *
 * ©K. D. Hedger. Thu 13 Aug 16:55:19 BST 2015 kdhedger68713@gmail.com

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


#include <X11/extensions/shape.h>
#include <stdio.h>
#include <stdlib.h>
#include <libudev.h>

#include "config.h"
#include "prefs.h"
#include "files.h"
#include "disks.h"

const char	*iconDiskType[]= {"harddisk","harddisk-usb","dev-cdrom","dev-dvd","media-removable","multimedia-player","flash","user-home","computer","customicon"};

void clearDeskEntry(int num,bool clearslot)
{
	if(deskIconsArray[num].label!=NULL)
		free(deskIconsArray[num].label);
	deskIconsArray[num].label=NULL;
	if(deskIconsArray[num].uuid!=NULL)
		free(deskIconsArray[num].uuid);
	deskIconsArray[num].uuid=NULL;
	if(deskIconsArray[num].mountpoint!=NULL)
		free(deskIconsArray[num].mountpoint);
	deskIconsArray[num].mountpoint=NULL;
	if(deskIconsArray[num].partname!=NULL)
		free(deskIconsArray[num].partname);
	deskIconsArray[num].partname=NULL;
	if(deskIconsArray[num].dev!=NULL)
		free(deskIconsArray[num].dev);
	deskIconsArray[num].dev=NULL;
	if(deskIconsArray[num].mime!=NULL)
		free(deskIconsArray[num].mime);
	deskIconsArray[num].mime=NULL;
	if(deskIconsArray[num].icon!=NULL)
		free(deskIconsArray[num].icon);
	deskIconsArray[num].icon=NULL;

	if(clearslot==true)
		xySlot[deskIconsArray[num].x][deskIconsArray[num].y]=0;

	deskIconsArray[num].dvd=false;
	deskIconsArray[num].cdrom=false;
	deskIconsArray[num].usb=false;
	deskIconsArray[num].file=false;
	deskIconsArray[num].iconhint=-1;
	deskIconsArray[num].installed=false;
	deskIconsArray[num].customicon=false;
}

void mountDisk(int what)
{
	char	*command;
	char	*termcommand;
	bool	interm=false;

	if(isDisk==false)
		{
			switch (what)
				{
					case BUTTONOPEN:
						if(strstr(deskIconsArray[foundDiskNumber].mountpoint,".desktop")!=0)
							{
								interm=false;
								command=oneLiner("awk -F= '/Terminal=/{print $2}' \"%s\"",deskIconsArray[foundDiskNumber].mountpoint);
								if(strcasecmp(command,"true")==0)
									interm=true;
								free(command);
								command=oneLiner("echo \"$(awk -F= '/Exec=/{print $2}' \"%s\"|sed 's/%%.//g') &\"",deskIconsArray[foundDiskNumber].mountpoint);
								if(interm==true)
									{
										asprintf(&termcommand,"%s %s",terminalCommand,command);
										system(termcommand);
										free(termcommand);
									}	
								else
									system(command);
							}
						else
							{
								if(strcmp(deskIconsArray[foundDiskNumber].mime,"application-x-executable")==0)
									{
										asprintf(&command,"\"%s\" &",deskIconsArray[foundDiskNumber].mountpoint);
										system(command);
									}
								else
									{
										asprintf(&command,"xdg-open \"%s\" &",deskIconsArray[foundDiskNumber].mountpoint);
										system(command);
									}
							}
						free(command);
						return;
						break;
				}
		}
	else
		{
#ifdef _USESUIDHELPER_
			asprintf(&command,"%s \"%s\" \"/media/%s\" %i",HELPERAPP,deskIconsArray[foundDiskNumber].uuid,deskIconsArray[foundDiskNumber].label,what);
#else
			asprintf(&command,"udevil mount `findfs UUID=%s`",deskIconsArray[foundDiskNumber].uuid);
#endif
			system(command);
			free(command);
			if(what==BUTTONMOUNT)
				{
					asprintf(&command,"findmnt -lno TARGET -S UUID=\"%s\"|xargs xdg-open &",deskIconsArray[foundDiskNumber].uuid);
					system(command);
					free(command);
				}

			if(what==BUTTONEJECT)
				clearDeskEntry(foundDiskNumber,true);
		}
}

int getUSBData(const char *ptr)
{
	if(ptr!=NULL)
		{
			if(strcasestr(ptr,"apple")!=NULL)
				return(IPOD);
			if(strcasestr(ptr,"sandisk")!=NULL)
				return(CARD);
			if(strcasestr(ptr,"generic")!=NULL)
				return(CARD);
		}
	return(USB);
}

void addExtraIconSpace(void)
{
	if(deskIconsCnt>=deskIconsMaxCnt)
		{
			deskIconsMaxCnt+=10;
			deskIconsArray=(deskIcons*)realloc(deskIconsArray,deskIconsMaxCnt*sizeof(deskIcons));
			for(int j=deskIconsCnt;j<deskIconsMaxCnt;j++)
				{
					deskIconsArray[j].label=NULL;
					deskIconsArray[j].uuid=NULL;
					deskIconsArray[j].dev=NULL;
					deskIconsArray[j].mountpoint=NULL;
					deskIconsArray[j].mime=NULL;
					deskIconsArray[j].partname=NULL;
					deskIconsArray[j].icon=NULL;
					deskIconsArray[j].installed=false;
					deskIconsArray[j].ignore=false;
					deskIconsArray[j].iconhint=0;
				}
		}
}

void fillDesk(void)
{
	struct udev		*udev;
	udev_device 	*thedev;
	udev_device 	*usbdev;
	char			buffer[BUFFERSIZE];
	char			path[BUFFERSIZE];
	char			*uuid=NULL;
	int				iconhint=0;
	bool			iscd=false;
	bool			isdvd=false;
	bool			isusb=false;
	FILE			*fp;
	const char		*ptr;
	char			*holdfilename=NULL;

	/* Create the udev object */
	udev = udev_new();
	if (!udev) {
		printf("Can't create udev\n");
		exit(1);
	}

	deskIconsCnt=RESERVED;

	fp=popen("ls -1 /dev/disk/by-uuid","r");
	if(fp!=NULL)
		{
			buffer[0]=0;
			while(fgets(buffer,BUFFERSIZE,fp))
				{
					addExtraIconSpace();
					clearDeskEntry(deskIconsCnt,false);
					buffer[strlen(buffer)-1]=0;
					asprintf(&uuid,"%s",buffer);
					sprintf(path,"/dev/disk/by-uuid/%s",buffer);
					buffer[readlink(path,buffer,BUFFERSIZE)]=0;
					ptr=strrchr(buffer,'/');
					ptr++;
					sprintf(path,"/dev/%s",ptr);
					if(strcmp(path,rootDev)==0)
						continue;
					deskIconsArray[deskIconsCnt].partname=strdup(ptr);
					thedev=udev_device_new_from_subsystem_sysname(udev,"block",ptr);
					if(thedev==NULL)
						{
							printf("no dev for %s from %s\n",ptr,buffer);
							exit(1);
						}
					else
						{
							if(strcmp(udev_device_get_property_value(thedev,"ID_FS_USAGE"),"filesystem")==0)
								{
									ptr=udev_device_get_property_value(thedev,"ID_FS_LABEL");
									if(ptr==NULL)
										ptr=udev_device_get_property_value(thedev,"ID_SERIAL");
									if(ptr==NULL)
										continue;

									iconhint=0;
									deskIconsArray[deskIconsCnt].label=strdup(ptr);
									deskIconsArray[deskIconsCnt].uuid=uuid;

									isdvd=false;
									iscd=false;
									isusb=false;
									if(udev_device_get_property_value(thedev,"ID_CDROM_MEDIA_DVD")!=NULL)
										{
											iconhint=DVD;
											isdvd=true;
										}
									if(udev_device_get_property_value(thedev,"ID_CDROM_MEDIA_CD")!=NULL)
										{
											iconhint=CDROM;
											iscd=true;
										}
									
									usbdev=udev_device_get_parent_with_subsystem_devtype(thedev,"usb","usb_device");
									if(usbdev!=NULL)
										{
											isusb=true;
											if(udev_device_get_property_value(thedev,"ID_DRIVE_THUMB")!=NULL)
												iconhint=STICK;
											else
												iconhint=getUSBData(udev_device_get_property_value(thedev,"ID_VENDOR"));
										}
									asprintf(&deskIconsArray[deskIconsCnt].dev,"/dev/%s",deskIconsArray[deskIconsCnt].partname);
									deskIconsArray[deskIconsCnt].dvd=isdvd;
									deskIconsArray[deskIconsCnt].cdrom=iscd;
									deskIconsArray[deskIconsCnt].usb=isusb;
									deskIconsArray[deskIconsCnt].file=false;
									deskIconsArray[deskIconsCnt].iconhint=iconhint;
									
									if(ignores!=NULL)
										{
											if(strstr(ignores,deskIconsArray[deskIconsCnt].label)!=NULL)
												deskIconsArray[deskIconsCnt].ignore=true;
										}
									sprintf(buffer,"%s/%s",diskInfoPath,deskIconsArray[deskIconsCnt].uuid);
									
									if(loadVarsFromFile(buffer,globalFileData))
										{
											deskIconsArray[deskIconsCnt].x=fileDiskXPos;
											deskIconsArray[deskIconsCnt].y=fileDiskYPos;
											if(fileGotCustomIcon==true)
												{
													deskIconsArray[deskIconsCnt].icon=fileCustomIcon;
													deskIconsArray[deskIconsCnt].customicon=true;
													fileCustomIcon=NULL;
													fileGotCustomIcon=false;
												}
											else
												deskIconsArray[deskIconsCnt].icon=NULL;
										}
									else
										{
											getFreeSlot(&deskIconsArray[deskIconsCnt].x,&deskIconsArray[deskIconsCnt].y);
											saveInfofile(DISKFOLDER,deskIconsArray[deskIconsCnt].label,NULL,NULL,deskIconsArray[deskIconsCnt].uuid,(char*)iconDiskType[deskIconsArray[deskIconsCnt].iconhint],deskIconsArray[deskIconsCnt].x,deskIconsArray[deskIconsCnt].y,deskIconsCnt);
										}
									deskIconsArray[deskIconsCnt].installed=true;
									xySlot[deskIconsArray[deskIconsCnt].x][deskIconsArray[deskIconsCnt].y]=1;
									deskIconsCnt++;
								}
						}
				}
		}
	pclose(fp);

	holdfilename=NULL;

//desktop files
	sprintf(buffer,"find %s -mindepth 1 -maxdepth 1",desktopPath);
	char	buffer2[4096];
	char	*tptr;
	fp=popen(buffer,"r");
	while(fgets(buffer,BUFFERSIZE,fp))
		{
			char	*ptr;
			buffer[strlen(buffer)-1]=0;
			ptr=strrchr(buffer,'/');
			ptr++;
			sprintf(buffer2,"%s/%s",cachePath,ptr);
			addExtraIconSpace();

			if(strcmp(&buffer[strlen(buffer)-8],".desktop")==0)
				{
					holdfilename=strdup(buffer);
				}
				
			if(fileExists(buffer2)==-1)
				{
					clearDeskEntry(deskIconsCnt,false);
					deskIconsArray[deskIconsCnt].mountpoint=strdup(buffer);
					ptr=strrchr(buffer,'/');
					ptr++;
					deskIconsArray[deskIconsCnt].label=strdup(ptr);
					tptr=getMimeType(buffer);
					ptr=strchr(tptr,'/');
					while(ptr!=NULL)
						{
							*ptr='-';
							ptr=strchr(tptr,'/');
						}
					ptr=strstr(tptr,"text-x-shellscript");
					if(ptr==NULL)
						deskIconsArray[deskIconsCnt].mime=strdup(tptr);
					else
						deskIconsArray[deskIconsCnt].mime=strdup("application-x-shellscript");
					free(tptr);
					getFreeSlot(&deskIconsArray[deskIconsCnt].x,&deskIconsArray[deskIconsCnt].y);
					xySlot[deskIconsArray[deskIconsCnt].x][deskIconsArray[deskIconsCnt].y]=1;
					saveInfofile(CACHEFOLDER,deskIconsArray[deskIconsCnt].label,deskIconsArray[deskIconsCnt].mime,deskIconsArray[deskIconsCnt].mountpoint,NULL,NULL,deskIconsArray[deskIconsCnt].x,deskIconsArray[deskIconsCnt].y,deskIconsCnt);
					deskIconsArray[deskIconsCnt].installed=true;
					deskIconsArray[deskIconsCnt].file=true;
					deskIconsCnt++;
				}
			else
				{
					readDesktopFile(ptr);
				}
			
			if((holdfilename!=NULL) && (deskIconsArray[deskIconsCnt-1].customicon==false))
				{
					char	commandbuffer[MAXBUFFER];
					char	*icon;
					char	*pth;

					sprintf(commandbuffer,"awk -F= '/Icon=/{print $2}' \"%s\"",holdfilename);
					icon=oneLiner("%s",commandbuffer);
					pth=strrchr(icon,'.');
					if(pth!=NULL)
						*pth=0;
					if(icon!=NULL)
						{
							pth=pathToIcon(icon,"");
							deskIconsArray[deskIconsCnt-1].icon=strdup(pth);
						}
					free(holdfilename);
					holdfilename=NULL;
					fileCustomIcon=NULL;
				}
		}
	pclose(fp);
}