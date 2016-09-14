/*
 *
 * ©K. D. Hedger. Tue 13 Sep 20:00:45 BST 2016 kdhedger68713@gmail.com

 * This file (LFSTKFileDialog.cpp) is part of LFSToolKit.

 * LFSToolKit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSToolKit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSToolKit.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include <unistd.h>
#include <stdlib.h>

#include "LFSTKGlobals.h"

LFSTK_fileDialogClass::~LFSTK_fileDialogClass(void)
{
}

void LFSTK_fileDialogClass::getDirList(void)
{
	unsigned	dircnt=0;
	FILE		*fp;
	char		*command;
	char		*out=NULL;
	char		line[1024];
	unsigned	cnt=0;

	asprintf(&command,"(cd %s ;find  -maxdepth 1 -mindepth 1 -type d| xargs -n 1 basename|wc -l)",this->currentDir);
	out=this->wc->globalLib->LFSTK_oneLiner("%s",command);

	if(out==NULL)
		dircnt=0;
	else
		{
			dircnt=atoi(out);
			free(out);
		}
	free(command);

	if(dircnt>0)
		{
			this->dirList=new char*[dircnt];
			asprintf(&command,"(cd %s ;find  -maxdepth 1 -mindepth 1 -type d| xargs -n 1 basename)",this->currentDir);
			fp=popen(command, "r");
			if(fp!=NULL)
				{
					while(fgets(line,1024,fp))
						{
							if(strlen(line)>0)
								line[strlen(line)-1]=0;
							this->dirList[cnt]=strdup(line);
							cnt++;
						}
					pclose(fp);
				}
			free(command);
			this->dirListCnt=dircnt;
		}
}

/**
* Main constructor.
*
* \param Window parent Window transient for.
* \param const char *label Displayed name.
* \param const char *startdir Open dialog in this folder.
*/
LFSTK_fileDialogClass::LFSTK_fileDialogClass(Window parent,const char *label,const char *startdir)
{
	LFSTK_labelClass	*spacer;

printf(">>>PDIR=%s<<<\n",LFSTKPIXMAPSDIR);
	this->dirList=NULL;
	this->currentDir=startdir;
	this->list=NULL;
	this->dirListCnt=0;

	this->dialog=new LFSTK_windowClass(0,0,DWIDTH,DHITE,label,false);
	this->getDirList();

	this->list=new LFSTK_listGadgetClass(this->dialog,"list",FGAP,FGAP,FWIDTH-(FGAP*2)-FNAVBUTTONWID,FHITE-(FGAP*4)-(BUTTONHITE*2),NorthWestGravity,NULL,0);
	this->list->LFSTK_setList(this->dirList,this->dirListCnt);
	spacer=new LFSTK_labelClass(this->dialog,"--",0,FHITE-(BUTTONHITE*2),FWIDTH,8,NorthWestGravity);

//dialog buttons
	this->buttonCancel=new LFSTK_buttonClass(this->dialog,"Cancel",FBORDER,FHITE-FBORDER-BUTTONHITE,BUTTONWIDTH,BUTTONHITE,SouthWestGravity);
	this->buttonApply=new LFSTK_buttonClass(this->dialog,"Apply",FWIDTH-FBORDER-BUTTONWIDTH,FHITE-FBORDER-BUTTONHITE,BUTTONWIDTH,BUTTONHITE,SouthEastGravity);
}

bool LFSTK_fileDialogClass::LFSTK_isValid(void)
{
	if(this->list->LFSTK_getCurrentListItem()!=-1)
		return(this->apply);
	return(false);
}

void LFSTK_fileDialogClass::LFSTK_showDialog(void)
{
	XEvent	event;
	this->apply=false;
	if(this->dialog!=NULL)
		{
			this->dialog->LFSTK_showWindow();
			this->dialog->LFSTK_setKeepAbove(true);
			this->dialog->LFSTK_setTransientFor(this->wc->window);
			this->list->currentItem=-1;

	this->mainLoop=true;
	while(this->mainLoop==true)
		{
			XNextEvent(this->dialog->display,&event);
			mappedListener *ml=this->dialog->LFSTK_getMappedListener(event.xany.window);
			if(ml!=NULL)
				{
					ml->function(ml->gadget,&event,ml->type);
				}

			switch(event.type)
				{
				case ButtonRelease:
					if(ml!=NULL)
						{
							if(ml->gadget==this->buttonApply)
								{
									this->apply=true;
									this->mainLoop=false;
								}
							if(ml->gadget==this->buttonCancel)
								{
									this->apply=false;
									this->mainLoop=false;
								}
						}
					break;

					case Expose:
						this->dialog->LFSTK_clearWindow();
						break;

					case ConfigureNotify:
						this->dialog->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height);
						break;

					case ClientMessage:
					case SelectionNotify:
						{
							if(event.xclient.message_type==XInternAtom(this->dialog->display, "WM_PROTOCOLS", 1) && (Atom)event.xclient.data.l[0] == XInternAtom(this->dialog->display, "WM_DELETE_WINDOW", 1))
							this->mainLoop=false;
						}
						break;
				}
		}

	this->dialog->LFSTK_hideWindow();
//	printf(">>selected num=%i<<<\n",this->list->LFSTK_getCurrentListItem());
//	printf(">>selected label=%s<<<\n",this->list->LFSTK_getListString(this->list->LFSTK_getCurrentListItem()));
		}

}

