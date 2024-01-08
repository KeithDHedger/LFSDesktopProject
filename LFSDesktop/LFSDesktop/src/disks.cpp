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

const char	*diskModel=NULL;
const char	*diskVendor=NULL;
char			diskUUID[256]={0,};
char			diskLabel[256]={0,};
struct udev	*udev;

int diskTypeData(const char *devname)
{
	udev_device *device;
	const char	*tprop=NULL;
	const char	*ptr;

	ptr=strrchr(devname,'/');
	ptr++;
	device=udev_device_new_from_subsystem_sysname(udev,"block",ptr);

	tprop=udev_device_get_property_value(device,"ID_MODEL");
	if(tprop!=NULL)
		{
			if(strcasestr(tprop,"ipod")!=NULL)
				{
					udev_device_unref(device);
					return(ISIPOD);
				}
			if(strcasestr(tprop,"SD_MMC")!=NULL)
				{
					udev_device_unref(device);
					return(ISSSD);
				}			
		}

	if(udev_device_get_property_value(device,"ID_CDROM_MEDIA_DVD")!=NULL)
		{
			udev_device_unref(device);
			return(ISDVDROM);
		}

	if(udev_device_get_property_value(device,"ID_CDROM")!=NULL)
		{
			udev_device_unref(device);
			return(ISCDROM);
		}

	if(udev_device_get_property_value(device,"ID_DRIVE_THUMB")!=NULL)
		{
			udev_device_unref(device);
			return(ISTHUMBDISK);
		}

	tprop=udev_device_get_property_value(device,"DEVPATH");
	if(tprop!=NULL)
		{
			if(strstr(tprop,"usb")!=NULL)
				{
					udev_device_unref(device);
					return(ISUSBHDD);
				}
		}


	udev_device_unref(device);
	return(ISHDDDISK);
}

int setDiskType(const char *devname)
{
	return(diskTypeData(devname));
}

void updateMounted(void)
{
	char	*command;
	int		retval=-1;
	bool	mounted=false;
	bool	dosync=false;

	for(unsigned j=0;j<desktopItems.size();j++)
		{
			if(desktopItems.at(j).type<ISDESKTOPFILE)
				{
					asprintf(&command,"grep -i \"%s\" /proc/mounts &>/dev/null",desktopItems.at(j).itemPath);
					retval=system(command);
					free(command);

					mounted=(bool)!retval;
					if(desktopItems.at(j).mounted!=mounted)
						{
							desktopItems.at(j).mounted=mounted;
							desktopItems.at(j).dirty=true;
						}

					if(desktopItems.at(j).dirty==true)
						{
							dosync=true;
							desktopItems.at(j).dirty=false;
							if(desktopItems.at(j).mounted==true)
								desktopItems.at(j).item->LFSTK_setAlpha(1.0);
							else
								desktopItems.at(j).item->LFSTK_setAlpha(0.5);
							desktopItems.at(j).item->LFSTK_clearWindow();
						}
					getLabel(j);
				}
		}
	if(dosync==true)
		{
			XSync(apc->display,false);
		}
}

void getLabel(int item)
{
	char		disk[256]={0,};
	const char	*ptr;
	udev_device *device;
	const char	*tuuid=NULL;
	const char *dev=NULL;

	dev=desktopItems.at(item).itemPath;
	sprintf(disk,"%s",dev);
	ptr=strrchr(disk,'/');
	ptr++;
	device=udev_device_new_from_subsystem_sysname(udev,"block",ptr);
	if(device!=NULL)
	{
	tuuid=udev_device_get_property_value(device,"ID_FS_UUID");
	if(tuuid!=NULL)
		{
			if(udev_device_get_property_value(device,"ID_FS_LABEL")!=NULL)
				{
					if(strcmp(desktopItems.at(item).label,udev_device_get_property_value(device,"ID_FS_LABEL"))!=0)
						{
							desktopItems.at(item).item-> LFSTK_setLabel(udev_device_get_property_value(device,"ID_FS_LABEL"));
							free(desktopItems.at(item).label);
							desktopItems.at(item).label=strdup(udev_device_get_property_value(device,"ID_FS_LABEL"));
							setItemSize(&desktopItems.at(item));
							desktopItems.at(item).item->LFSTK_clearWindow();
						}
				}
		}
	udev_device_unref(device);
	}
}

bool getUUID(const char *dev)
{
	char		disk[256]={0,};
	const char	*ptr;
	udev_device *device;
	const char	*tuuid=NULL;
	const char	*sep="-";
	const char	*partnum="";

	sprintf(disk,"%s",dev);
	ptr=strrchr(disk,'/');
	ptr++;
	diskLabel[0]=0;

	device=udev_device_new_from_subsystem_sysname(udev,"block",ptr);
	tuuid=udev_device_get_property_value(device,"ID_FS_UUID");

	if(tuuid!=NULL)
		{
			sprintf(diskUUID,"%s",tuuid);
			if(udev_device_get_property_value(device,"ID_FS_LABEL")==NULL)
				{
					diskModel=udev_device_get_property_value(device,"ID_MODEL");
					diskVendor=udev_device_get_property_value(device,"ID_VENDOR");
					partnum=udev_device_get_property_value(device,"PARTN");
					if(diskModel==NULL)
						{
							sep="";
							diskModel="";
						}
					if(diskVendor==NULL)
						{
							sep="";
							diskVendor="";
						}
					sprintf(diskLabel,"%s%s%s-%s",diskModel,sep,diskVendor,partnum);
				}
			else
				{
					sprintf(diskLabel,"%s",udev_device_get_property_value(device,"ID_FS_LABEL"));
				}

			udev_device_unref(device);
			return(true);
		}
	udev_device_unref(device);
	return(false);
}

void makeDiskButtons(void)
{
	char	*command;
	FILE	*fd=NULL;
	char	buffer[2048];
	char	*cachefilepath;

	asprintf(&command,"find /dev -maxdepth 1 -mindepth 1  -regextype sed -regex \"%s\"|grep -v \"%s\"|sort --version-sort",includeList,excludeList);
	fd=popen(command,"r");
	free(command);
	if(fd!=NULL)
		{
			while(feof(fd)==0)
				{
continueWithLoop:
					buffer[0]=0;
					fgets(buffer,2048,fd);
					if(strlen(buffer)>0)
						{
							buffer[strlen(buffer)-1]=0;
							if(getUUID(buffer)==false)
								goto continueWithLoop;

							for(unsigned j=0;j<desktopItems.size();j++)
								{
									if((diskUUID[0]!=0) && (desktopItems.at(j).item!=NULL))//TODO//
										{
											if(strcmp(diskUUID,desktopItems.at(j).uuid)==0)
												{
													desktopItems.at(j).item->LFSTK_moveGadget(desktopItems.at(j).posx,desktopItems.at(j).posy);
													goto continueWithLoop;
												}
										}
								}

							asprintf(&cachefilepath,"%s/%s",cachePath,diskUUID);
							if(access(cachefilepath,F_OK)==0)
								loadCacheFile(cachefilepath,&cacheFileData);
							else
								{
									cacheFileData.uuid=strdup(diskUUID);
									cacheFileData.posx=100;
									cacheFileData.posy=100;
									cacheFileData.hasCustomIcon=false;
									cacheFileData.pathToCustomIcon=NULL;
									saveCacheFile(cachefilepath,&cacheFileData);
								}

							cacheFileData.label=strdup(diskLabel);													
							cacheFileData.itemPath=strdup(buffer);
							cacheFileData.type=setDiskType(buffer);
							cacheFileData.dirty=true;
							cacheFileData.isADisk=true;

							createDesktopGadget(diskWindow);
							freeAndNull(&cacheFileData.uuid);
							freeAndNull(&cacheFileData.itemPath);
							freeAndNull(&cacheFileData.label);
							freeAndNull(&cachefilepath);										
						}
				}
		}
}
