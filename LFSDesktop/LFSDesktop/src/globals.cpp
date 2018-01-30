/*
 *
 * ©K. D. Hedger. Wed Jan 10 11:33:09 GMT 2018 kdhedger68713@gmail.com

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

//list
diskLinkedList		*diskLL=NULL;

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
int					**xySlot;
geometryStruct		oldPos;

//info
int					errLine;
const char			*errFile;
const char			*errFunc;

//dialogs
LFSTK_windowClass	*diskWindow=NULL;
LFSTK_windowClass	*fileWindow=NULL;
LFSTK_windowClass	*iconChooser=NULL;
LFSTK_lineEditClass *iconChooserEdit=NULL;
LFSTK_buttonClass	*diskButtons[NOMOREBUTONS];
LFSTK_buttonClass	*fileButtons[3];
bool				dialogLoop=false;
int					dialogRetVal=DIALOGRETERROR;

void freeAndNull(char **data)
{
	if((data!=NULL) && (*data!=NULL))
		{
			free(*data);
			*data=NULL;
		}
}

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

void removeDeleted(void)
{
#if 0
	diskLinkedList	*list=diskLL;
	char			*command;
	FILE			*fd=NULL;
	char			buffer[2048];

	if(list==NULL)
		return;

	do
		{
			if(list->data==NULL)
				continue;

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
									buffer[0]=0;
								}
							pclose(fd);
						}
				}
			
			if(strcmp(list->data->devName,devname)==0)
				return(list);
			list=list->next;
		}
	while(list!=NULL);
	return(NULL);
#endif
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
					dnode->gadgetSize=iconSize;
				}
		}

}

//slots
void getFreeSlot(int *x,int *y)
{

	for(int yy=0;yy<maxYSlots;yy++)
		{
			for(int xx=0;xx<maxXSlots;xx++)
				{
					if(xySlot[xx][yy]==0)
						{
							*x=xx;
							*y=yy;
							xySlot[xx][yy]=1;
							return;
						}
				}
		}
}

void setSlotFromPos(int x,int y,int val)
{
	xySlot[x/gridSize][y/gridSize]=val;
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
	//dialog->LFSTK_setTransientFor(wc->window);
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

							case ConfigureNotify:
								//iconChooser->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height,false);
								//iconChooser->globalLib->LFSTK_setCairoSurface(iconChooser->display,iconChooser->window,iconChooser->visual,&iconChooser->sfc,&iconChooser->cr,event.xconfigurerequest.width,event.xconfigurerequest.height);
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
											dialog->droppedData.type=-1;
										}
								}
						}
				}
		}
	dialog->LFSTK_hideWindow();
}
