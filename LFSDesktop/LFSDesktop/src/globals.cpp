/*
 *
 * ©K. D. Hedger. Wed Jan 10 11:33:09 GMT 2018 keithdhedger@gmail.com

 * This file (globals.cpp) is part of LFSDesktop.

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

//main window
LFSTK_applicationClass			*apc=NULL;
LFSTK_windowClass				*wc=NULL;

//paths
char							*cachePath;
char							*prefsPath;
char							*desktopPath;

std::vector<desktopItemStruct>	desktopItems;
desktopItemStruct				cacheFileData;
char							*documentsPath=NULL;
char							*homePath=NULL;
char							*diskWatch=NULL;
char							*desktopWatch=NULL;

//prefs
bool							showSuffix=false;//TODO//

//dialogs
LFSTK_windowClass				*diskWindow=NULL;
LFSTK_windowClass				*fileWindow=NULL;

void setItemSize(desktopItemStruct	*cf)
{
	int textwid=0;

	if(cf->item!=NULL)
		{
			textwid=cf->item->LFSTK_getTextWidth(cf->label);
			if(textwid>iconSize)
				cf->item->LFSTK_setGadgetSize(textwid,iconSize);
		}
}

void createDesktopGadget(LFSTK_windowClass *window)
{
	desktopItemStruct	cf;

	cf=cacheFileData;
	setIconImage(&cf);
	cf.item=new LFSTK_buttonClass(wc,cacheFileData.label,cacheFileData.posx,cacheFileData.posy,iconSize,iconSize);
	cf.item->LFSTK_setCanDrag(true);
	cf.item->LFSTK_moveGadget(cacheFileData.posx,cacheFileData.posy);
	cf.item->LFSTK_setStyle(BEVELNONE);
	if(cf.hasCustomIcon==false)
		cf.item->LFSTK_setImageFromPath(cf.iconPath,TOOLBAR,true);
	else
		{
			cf.item->LFSTK_setImageFromPath(cf.pathToCustomIcon,TOOLBAR,true);
		}
	cf.item->LFSTK_setUseWindowPixmap(true);
	cf.item->LFSTK_setMouseCallBack(NULL,mouseUpCB,USERDATA(desktopItems.size()));
	cf.item->userData=USERDATA(desktopItems.size());
	cf.item->LFSTK_setContextWindow(window);

	cf.item->LFSTK_setLabelAutoColour(false);
	//cf.item->LFSTK_setFontColourName(NORMALCOLOUR,foreCol,false);
	cf.item->newGadgetFGColours[NORMALCOLOUR]=cf.item->LFSTK_setColour(foreCol);
	cf.item->LFSTK_setLabelBGColour(backCol,strtod(backAlpha,NULL));
	cf.item->LFSTK_setFontString(fontFace,true);//TODO//URGENT
	cf.item->drawLabelBG=true;
	cf.item->autoLabelBGColour=false;
	setItemSize(&cf);

	cf.item->gadgetDetails.showLink=cf.isSymLink;
	if(access(cf.itemPath,F_OK)!=F_OK)
		cf.item->gadgetDetails.showBroken=true;
	desktopItems.push_back(cf);
	asprintf(&desktopItems.back().uuid,"%s",cacheFileData.uuid);
	asprintf(&desktopItems.back().itemPath,"%s",cacheFileData.itemPath);
	asprintf(&desktopItems.back().label,"%s",cacheFileData.label);
}

//set icon
void setIconImage(desktopItemStruct	*cf)
{
	if(cf->hasCustomIcon==true)
		{
			return;
		}

	switch(cf->type)
		{
			case ISHDDDISK:
				cf->isSymLink=false;
				cf->iconPath=apc->globalLib->LFSTK_findThemedIcon(iconTheme,"-harddisk","devices");
				break;
			case ISUSBHDD:
				cf->isSymLink=false;
				cf->iconPath=apc->globalLib->LFSTK_findThemedIcon(iconTheme,"-usb","devices");
				break;
			case ISTHUMBDISK:
				cf->isSymLink=false;
				cf->iconPath=apc->globalLib->LFSTK_findThemedIcon(iconTheme,"-removable","devices");
				break;
			case ISCDROM:
				cf->iconPath=apc->globalLib->LFSTK_findThemedIcon(iconTheme,"-cdrom","devices");
				cf->isSymLink=false;
				break;
			case ISDVDROM:
				cf->isSymLink=false;
				cf->iconPath=apc->globalLib->LFSTK_findThemedIcon(iconTheme,"-dvd","devices");
				break;
			case ISIPOD:
				cf->isSymLink=false;
				cf->iconPath=apc->globalLib->LFSTK_findThemedIcon(iconTheme,"-ipod","devices");
				break;
			case ISSSD:
				cf->isSymLink=false;
				cf->iconPath=apc->globalLib->LFSTK_findThemedIcon(iconTheme,"-flash","devices");
				break;
			case ISDESKTOPFILE:
				{
					fileInformation fileinfo;
					apc->globalLib->LFSTK_getFileInfo(cf->itemPath,&fileinfo);
					cf->isSymLink=fileinfo.isLink;
					cf->iconPath=strdup(fileinfo.iconPath.c_str());
				break;
					GKeyFile		*gkf=g_key_file_new();
					char			*icon=NULL;
					if(g_key_file_load_from_file(gkf,cf->itemPath,G_KEY_FILE_NONE,NULL)==true)
						{
							icon=g_key_file_get_string(gkf,"Desktop Entry",G_KEY_FILE_DESKTOP_KEY_ICON,NULL);
						}
					g_key_file_free(gkf);
					cf->iconPath=wc->app->globalLib->LFSTK_findThemedIcon(iconTheme,icon,"");
					freeAndNull(&icon);
				}
				break;
			case ISDOCUMENTSFOLDER:
				cf->iconPath=apc->globalLib->LFSTK_findThemedIcon(iconTheme,"-documents","places");
				break;
			case ISHOMEFOLDER:
				cf->iconPath=apc->globalLib->LFSTK_findThemedIcon(iconTheme,"user-home","places");
				break;
			case ISCOMPUTER:
				cf->iconPath=apc->globalLib->LFSTK_findThemedIcon(iconTheme,"computer","");
				break;
			case ISIMAGEFILE:
				cf->iconPath=strdup(cf->itemPath);
				break;
			default:
				{
					fileInformation fileinfo;
					apc->globalLib->LFSTK_getFileInfo(cf->itemPath,&fileinfo);
					cf->isSymLink=fileinfo.isLink;
					cf->iconPath=strdup(fileinfo.iconPath.c_str());
				}
				break;
		}
}

bool dialogCB(void *p,void* ud)//TODO//
{
//	dialogRetVal=(int)(long)ud;
//	dialogLoop=false;

	return(true);
}

void dialogRun(LFSTK_windowClass *dialog)//TODO//
{
#if 0
	XEvent				event;

	dialog->LFSTK_showWindow();
	dialog->LFSTK_setKeepAbove(true);
	dialog->LFSTK_setTransientFor(wc->window);
	while(dialogLoop==true)
		{
			while (XPending(dialog->app->display) && (dialogLoop==true))
				{
					XNextEvent(dialog->app->display,&event);
					mappedListener *ml=dialog->LFSTK_getMappedListener(event.xany.window);
					if(ml!=NULL)
						ml->function(ml->gadget,&event,ml->type);

					switch(event.type)
						{
							case Expose:
								dialog->LFSTK_clearWindow();
								break;

							case ClientMessage:
							case SelectionNotify:
								{
									if (event.xclient.message_type == XInternAtom(dialog->app->display, "WM_PROTOCOLS", 1) && (Atom)event.xclient.data.l[0] == XInternAtom(dialog->app->display, "WM_DELETE_WINDOW", 1))
										{
											dialog->LFSTK_hideWindow();
											dialogLoop=false;
											dialogRetVal=0;
										}
//dnd for edit box
									if(dialog->acceptDnd==true)
										{
											dialog->LFSTK_handleDnD(&event);
											dialog->droppedData.type=DROPINVALID;
										}
								}
						}
				}
		}
	dialog->LFSTK_hideWindow();
#endif
}
