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
LFSTK_windowClass	*wc=NULL;
bool				mainLoop=true;
Display				*display;
bool				isDragging=false;

//list
diskLinkedList		*diskLL=NULL;
int					nextXPos=2;
int					nextYPos=2;

//paths
char				*diskInfoPath;
char				*cacheDisksPath;
char				*cacheDeskPath;
char				*prefsPath;
char				*desktopPath;

//save data
char				*diskUUID=NULL;
char				*iconPath=NULL;
int					xPos=-1;
int					yPos=-1;
bool				customIcon=false;
int					dataType=TYPENONE;

args				diskData[]=
{
	{"uuid",TYPESTRING,&diskUUID},
	{"icon",TYPESTRING,&iconPath},
	{"xpos",TYPEINT,&xPos},
	{"ypos",TYPEINT,&yPos},
	{"datatype",TYPEINT,&dataType},
	{"customicon",TYPEBOOL,&customIcon},
	{NULL,0,NULL}
};

//prefs
bool				showSuffix=false;
int					maxXSlots;
int					maxYSlots;
geometryStruct		oldPos;

//dialogs
LFSTK_windowClass	*diskWindow=NULL;
LFSTK_windowClass	*fileWindow=NULL;
LFSTK_windowClass	*iconChooser=NULL;
LFSTK_lineEditClass *iconChooserEdit=NULL;
LFSTK_buttonClass	*diskButtons[NOMOREBUTONS];
LFSTK_buttonClass	*fileButtons[3];
bool				dialogLoop=false;
int					dialogRetVal=DIALOGRETERROR;

diskLinkedList* isInList(const char *devname)
{
	diskLinkedList	*list=diskLL;

	if(list==NULL)
		return(NULL);

	do
		{
			if(list->data==NULL)
				continue;
			if(strcmp(list->data->devName,devname)==0)
				return(list);
			list=list->next;
		}
	while(list!=NULL);
	return(NULL);
}

void newNode(void)
{
	diskLinkedList	*node;

	if(diskLL==NULL)
		{
			diskLL=(diskLinkedList*)malloc(sizeof(diskLinkedList));
			diskLL->data=(diskDataStruct*)calloc(1,sizeof(diskDataStruct));
			diskLL->next=NULL;
			diskLL->prev=NULL;
			return;
		}

	node=(diskLinkedList*)malloc(sizeof(diskLinkedList));
	diskLL->prev=node;
	node->next=diskLL;
	node->prev=NULL;
	node->data=(diskDataStruct*)calloc(1,sizeof(diskDataStruct));
	diskLL=node;
}

void printDiskData(diskDataStruct *diskstruct)
{
	if(diskstruct->label!=NULL)
		printf("diskstruct->label=%s\n",diskstruct->label);
	if(diskstruct->devName!=NULL)
		printf("diskstruct->devName=%s\n",diskstruct->devName);
	if(diskstruct->uuid!=NULL)
		printf("diskstruct->uuid=%s\n",diskstruct->uuid);
	if(diskstruct->pathToIcon!=NULL)
		printf("diskstruct->pathToIcon=%s\n",diskstruct->pathToIcon);

	printf("diskstruct->diskType=%i\n",diskstruct->diskType);
	printf("diskstruct->posx=%i\n",diskstruct->posx);
	printf("diskstruct->posy=%i\n",diskstruct->posy);
	printf("diskstruct->mounted=%i\n",diskstruct->mounted);
	printf("diskstruct->driveHasMedia=%i\n",diskstruct->driveHasMedia);
	printf("diskstruct->hasCustomIcon=%i\n",diskstruct->hasCustomIcon);
	printf("\n");
}

//round to nearest int
int toNearestInt(int left,int rite)
{
	return((int)(((float)left/(float)rite)+0.5));
}

//set icon
void setIconImage(diskDataStruct *dnode)
{
	char			*out=NULL;
	char			*ticon=NULL;
	struct stat		st;
	const char		*dot=NULL;

	if(dnode->hasCustomIcon==true)
		{
			stat(dnode->pathToIcon,&st);
			if(!S_ISREG(st.st_mode))
				{
					freeAndNull(&dnode->pathToIcon);
					dnode->hasCustomIcon=false;
				}
		}

	if(dnode->hasCustomIcon==false)
		{
			dot=strrchr(dnode->devName,'.');
			if(dot!=0)
				{
					if((strcasecmp(dot+1,"jpg")==0) || (strcasecmp(dot+1,"png")==0))
						{
							asprintf(&dnode->pathToIcon,"%s/Desktop/%s",wc->userHome,dnode->devName);
							return;
						}
				}
		}

	switch(dnode->diskType)
		{
			case HDDDISK:
				ticon=wc->globalLib->LFSTK_findThemedIcon(iconTheme,"-harddisk","devices");
				break;
			case CDROM:
				ticon=wc->globalLib->LFSTK_findThemedIcon(iconTheme,"-cdrom","devices");
				break;
			case THUMBDISK:
				ticon=wc->globalLib->LFSTK_findThemedIcon(iconTheme,"-removable","devices");
				break;
			case USBHDD:
				ticon=wc->globalLib->LFSTK_findThemedIcon(iconTheme,"-usb","devices");
				break;
			case DESKTOPFILE:
				out=wc->globalLib->LFSTK_oneLiner("sed -n 's/^icon=\\(.*\\)$/\\1/Ip' '%s/%s'",desktopPath,dnode->devName);
				if(strlen(out)>0)
					ticon=wc->globalLib->LFSTK_findThemedIcon(iconTheme,out,"");
				free(out);
				break;
			case DESKFILE:
				out=wc->globalLib->LFSTK_oneLiner("file -bL --mime-type '%s/%s'|awk -F/ '{print \"-\" $2}'",desktopPath,dnode->devName);
				if(strcmp(out,"-directory")==0)
					ticon=wc->globalLib->LFSTK_findThemedIcon(iconTheme,out,"places");
				else
					ticon=wc->globalLib->LFSTK_findThemedIcon(iconTheme,out,"mimetypes");
				free(out);
				break;
		}

	if(ticon==NULL)
		ticon=wc->globalLib->LFSTK_findThemedIcon(iconTheme,"text-x-generic","mimetypes");

	if((ticon!=NULL) && (dnode->hasCustomIcon==false))
		{
			freeAndNull(&dnode->pathToIcon);
			dnode->pathToIcon=strdup(ticon);
		}

	freeAndNull(&ticon);
}

//set icon size
void setImageSize(diskDataStruct *dnode)
{
	int textwid=0;

	dnode->gadgetSize=iconSize;
	if(dnode->diskImage!=NULL)
		{
			textwid=dnode->diskImage->LFSTK_getTextWidth(dnode->label);
			if(textwid>iconSize)
				{
					dnode->diskImage->LFSTK_setGadgetSize(textwid,iconSize);
					dnode->gadgetSize=textwid;
				}
			else
				dnode->gadgetSize=iconSize;
		}

}

//slots
void setGridXY(diskDataStruct *dnode,int x,int y)
{
	int	addx=0;

	if(dnode->gadgetSize>gridSize)
		addx=1;
	dnode->posx=addx+toNearestInt(x,gridSize);
	dnode->posy=toNearestInt(y,gridSize);
}

void getRealXY(diskDataStruct *dnode,int *x,int *y)
{
	*x=(dnode->posx*gridSize)-(dnode->gadgetSize/2);
	*y=(dnode->posy*gridSize)-(iconSize/2)+1;
}

void getFreeGridXY(int *px,int *py)
{
	diskLinkedList	*list=diskLL;

	if(list==NULL)
		{
			*px=gridBorderLeft;
			*py=gridBorderRight;
			return;
		}

	do
		{
			if((list->data->posx==nextXPos) && (list->data->posy==nextYPos))
				{
					nextXPos++;
					if(nextXPos>(maxXSlots-gridBorderRight))
						{
							nextXPos=gridBorderLeft;
							nextYPos++;
						}
				}
			list=list->next;
		}			
		while(list!=NULL);
	*px=nextXPos;
	*py=nextYPos;
}

bool dialogCB(void *p,void* ud)
{
	dialogRetVal=(int)(long)ud;
	dialogLoop=false;
	return(true);
}

void dialogRun(LFSTK_windowClass *dialog)
{
	XEvent				event;

	dialog->LFSTK_showWindow();
	dialog->LFSTK_setKeepAbove(true);
	dialog->LFSTK_setTransientFor(wc->window);
	while(dialogLoop==true)
		{
			while (XPending(display) && (dialogLoop==true))
				{
					XNextEvent(dialog->display,&event);
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
									if (event.xclient.message_type == XInternAtom(dialog->display, "WM_PROTOCOLS", 1) && (Atom)event.xclient.data.l[0] == XInternAtom(dialog->display, "WM_DELETE_WINDOW", 1))
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
}
