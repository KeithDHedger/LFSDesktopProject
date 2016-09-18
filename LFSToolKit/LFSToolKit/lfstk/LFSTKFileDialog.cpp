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
	if(this->currentDir!=NULL)
		free(this->currentDir);
	if(this->currentFile!=NULL)
		free(this->currentFile);
	this->freeDirList();
	this->freeFileList();
}

void LFSTK_fileDialogClass::cleanDirPath(void)
{
	char		*command;

	asprintf(&command,"cd \"%s\" 2>/dev/null;pwd",this->currentDir);

	if(this->currentDir!=NULL)
		free(this->currentDir);
	this->currentDir=this->wc->globalLib->LFSTK_oneLiner("%s",command);
	free(command);
}

void LFSTK_fileDialogClass::freeDirList(void)
{
	if(this->dirList!=NULL)
		{
			for(int j=0;j<this->dirListCnt;j++)
				free(this->dirList[j]);
			free(this->dirList);
		}
	if(this->dirImageList!=NULL)
		free(this->dirImageList);
}

void LFSTK_fileDialogClass::freeFileList(void)
{
	if(this->fileList!=NULL)
		{
			for(int j=0;j<this->fileListCnt;j++)
				free(this->fileList[j]);
			free(this->fileList);
		}
	if(this->fileImageList!=NULL)
		free(this->fileImageList);
}

void LFSTK_fileDialogClass::getFileList(void)
{
	unsigned	filecnt=0;
	FILE		*fp;
	char		*command;
	char		*out=NULL;
	char		line[1024];
	unsigned	cnt=0;

	asprintf(&command,"(cd %s ;find  -maxdepth 1 -mindepth 1 -follow ! -type d| xargs -n 1 basename 2>/dev/null|wc -l)",this->currentDir);
	out=this->wc->globalLib->LFSTK_oneLiner("%s",command);

	if(out==NULL)
		filecnt=0;
	else
		{
			filecnt=atoi(out);
			free(out);
		}
	free(command);

	if(filecnt>0)
		{
			this->fileList=new char*[filecnt];
			this->fileImageList=new char*[filecnt];
			asprintf(&command,"(cd %s ;find  -maxdepth 1 -mindepth 1 -follow ! -type d | xargs -n 1 basename 2>/dev/null)",this->currentDir);
			fp=popen(command, "r");
			if(fp!=NULL)
				{
					while(fgets(line,1024,fp))
						{
							if(strlen(line)>0)
								line[strlen(line)-1]=0;
							this->fileList[cnt]=strdup(line);
							this->fileImageList[cnt]=(char*)this->fileImage;
							cnt++;
						}
					pclose(fp);
				}
			free(command);
		}
	this->fileListCnt=filecnt;
}

void LFSTK_fileDialogClass::getDirList(void)
{
	unsigned	dircnt=0;
	FILE		*fp;
	char		*command;
	char		*out=NULL;
	char		line[1024];
	unsigned	cnt=0;

	asprintf(&command,"(cd %s ;find  -maxdepth 1 -mindepth 1 -follow -type d| xargs -n 1 basename 2>/dev/null|wc -l)",this->currentDir);
//printf("command=%s\n",command);
	out=this->wc->globalLib->LFSTK_oneLiner("%s",command);
	if(out==NULL)
		dircnt=0;
	else
		{
			dircnt=atoi(out);
			free(out);
		}
	free(command);
	dircnt++;
	this->freeDirList();
	this->dirList=new char*[dircnt];
	this->dirImageList=new char*[dircnt];
	this->dirList[cnt]=strdup("..");
	this->dirImageList[cnt]=(char*)this->folderImage;
	cnt++;
	asprintf(&command,"(cd %s ;find  -maxdepth 1 -mindepth 1 -follow -type d| xargs -n 1 basename 2>/dev/null)",this->currentDir);
	fp=popen(command, "r");
	if(fp!=NULL)
		{
			while(fgets(line,1024,fp))
				{
					if(strlen(line)>0)
						line[strlen(line)-1]=0;
					this->dirList[cnt]=strdup(line);
					this->dirImageList[cnt]=(char*)this->folderImage;
					cnt++;
				}
			pclose(fp);
		}
	free(command);
	this->dirListCnt=dircnt;
}

/**
* Return the currently selected folder.
*
* \return const char *path to folder.
* \note Don't free.
*/
const char	*LFSTK_fileDialogClass::LFSTK_getCurrentDir(void)
{
	return(this->currentDir);
}

/**
* Return the currently selected file.
*
* \return const char *path to file.
* \note Don't free.
*/
const char	*LFSTK_fileDialogClass::LFSTK_getCurrentFile(void)
{
	return(this->fileListGadget->LFSTK_getListString(this->fileListGadget->LFSTK_getCurrentListItem()));
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

	this->dialog=NULL;
	this->dirList=NULL;
	this->dirImageList=NULL;
	this->currentDir=strdup(startdir);
	this->currentFile=NULL;
	this->dirListGadget=NULL;
	this->dirListCnt=0;
	this->fileImage=LFSTKPIXMAPSDIR "/documents.png";
	this->folderImage=LFSTKPIXMAPSDIR "/folder.png";
	this->dialog=new LFSTK_windowClass(0,0,DWIDTH,DHITE,label,false);
	this->getDirList();
	this->getFileList();

//folder list
	this->dirListGadget=new LFSTK_listGadgetClass(this->dialog,"list",FGAP,FGAP,FWIDTH-(FGAP*2)-FNAVBUTTONWID,(BUTTONHITE*4),NorthWestGravity,NULL,0);
	this->dirListGadget->LFSTK_setImageList(this->dirImageList,this->dirListCnt);
	this->dirListGadget->LFSTK_setList(this->dirList,this->dirListCnt);
	this->dirEdit=new LFSTK_lineEditClass(this->dialog,startdir,FGAP,(BUTTONHITE*4)+(2*FGAP),FWIDTH-(FGAP*2),BUTTONHITE,NorthWestGravity);

//file list
	this->fileListGadget=new LFSTK_listGadgetClass(this->dialog,"list",FGAP,FGAP+(BUTTONHITE*5)+(2*FGAP),FWIDTH-(FGAP*2)-FNAVBUTTONWID,(BUTTONHITE*4),NorthWestGravity,NULL,0);
	this->fileListGadget->LFSTK_setImageList(this->fileImageList,this->fileListCnt);
	this->fileListGadget->LFSTK_setList(this->fileList,this->fileListCnt);

	spacer=new LFSTK_labelClass(this->dialog,"--",0,FHITE-(BUTTONHITE*2),FWIDTH,8,NorthWestGravity);

//dialog buttons
	this->buttonCancel=new LFSTK_buttonClass(this->dialog,"Cancel",FBORDER,FHITE-FBORDER-BUTTONHITE,BUTTONWIDTH,BUTTONHITE,SouthWestGravity);
	this->buttonApply=new LFSTK_buttonClass(this->dialog,"Apply",FWIDTH-FBORDER-BUTTONWIDTH,FHITE-FBORDER-BUTTONHITE,BUTTONWIDTH,BUTTONHITE,SouthEastGravity);
}

bool LFSTK_fileDialogClass::LFSTK_isValid(void)
{
	if(this->dirListGadget->LFSTK_getCurrentListItem()!=-1)
		return(this->apply);
	return(false);
}

void LFSTK_fileDialogClass::doOpenDir(void)
{
	char	*newdir;
	asprintf(&newdir,"%s/%s",this->currentDir,this->dirListGadget->LFSTK_getListString(-1));
	if(this->currentDir!=NULL)
	free(this->currentDir);
	this->currentDir=newdir;
	this->cleanDirPath();
//folders
	this->getDirList();
	this->dirListGadget->LFSTK_setImageList(this->dirImageList,this->dirListCnt);
	this->dirListGadget->LFSTK_setList(this->dirList,this->dirListCnt);
	this->dirEdit->LFSTK_setBuffer(this->currentDir);
//files
	this->getFileList();
	this->fileListGadget->LFSTK_setImageList(this->fileImageList,this->fileListCnt);
	this->fileListGadget->LFSTK_setList(this->fileList,this->fileListCnt);
}

void LFSTK_fileDialogClass::LFSTK_showDialog(void)
{
	XEvent	event;
	unsigned	lasttime=0;
	unsigned	lastdiritem=0;
	unsigned	lastfileitem=0;
	geometryStruct	*geomdir=NULL;
	geometryStruct	*geomfile=NULL;

	this->apply=false;
	if(this->dialog!=NULL)
		{
			this->dialog->LFSTK_showWindow();
			this->dialog->LFSTK_setKeepAbove(true);
			this->dialog->LFSTK_setTransientFor(this->wc->window);

			this->mainLoop=true;
			while(this->mainLoop==true)
				{
					XNextEvent(this->dialog->display,&event);
					mappedListener *ml=this->dialog->LFSTK_getMappedListener(event.xany.window);
					if(ml!=NULL)
						ml->function(ml->gadget,&event,ml->type);

					switch(event.type)
						{
							case ButtonRelease:
								if((event.xbutton.x_root>geomdir->x) && (event.xbutton.x_root<(geomdir->x+geomdir->w)) && (event.xbutton.y_root>geomdir->y) && (event.xbutton.y_root<(geomdir->y+geomdir->h)))
									{
										if((event.xbutton.time-lasttime<1000) && (event.xbutton.state & Button1Mask))
											{
												this->doOpenDir();
												lasttime=event.xbutton.time-1000;
											}
										else
											lasttime=event.xbutton.time;
									}

								if((event.xbutton.x_root>geomfile->x) && (event.xbutton.x_root<(geomfile->x+geomfile->w)) && (event.xbutton.y_root>geomfile->y) && (event.xbutton.y_root<(geomfile->y+geomfile->h)))
									{
										if((event.xbutton.time-lasttime<1000) && (event.xbutton.state & Button1Mask))
											{
												lasttime=event.xbutton.time-1000;
												this->apply=true;
												this->mainLoop=false;
											}
										else
											lasttime=event.xbutton.time;
									}

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
								if(geomdir!=NULL)
									delete geomdir;
								geomdir=this->dirListGadget->LFSTK_getGeom();
								if(geomfile!=NULL)
									delete geomfile;
								geomfile=this->fileListGadget->LFSTK_getGeom();
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
		}
}

