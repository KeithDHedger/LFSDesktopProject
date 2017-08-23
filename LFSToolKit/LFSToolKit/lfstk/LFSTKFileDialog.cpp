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
#include <sys/stat.h>
#include <sys/types.h>

#include "lfstk/LFSTKGlobals.h"

LFSTK_fileDialogClass::~LFSTK_fileDialogClass(void)
{
	if(this->currentDir!=NULL)
		free(this->currentDir);
	if(this->currentFile!=NULL)
		free(this->currentFile);
	this->freeDirList();
	this->freeFileList();
	delete this->dialog;
}

void LFSTK_fileDialogClass::cleanDirPath(void)
{
	char	*command;

	asprintf(&command,"cd \"%s\" 2>/dev/null;realpath .",this->currentDir);
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
			delete[] this->dirList;
		}
	delete[] this->dirImageList;
}

void LFSTK_fileDialogClass::freeFileList(void)
{
	if(this->fileList!=NULL)
		{
			for(int j=0;j<this->fileListCnt;j++)
				free(this->fileList[j]);
			delete[] this->fileList;
		}
	if(this->fileImageList!=NULL)
		{
			for(int j=0;j<this->fileListCnt;j++)
				free(this->fileImageList[j]);
			delete[] this->fileImageList;
		}
}

void LFSTK_fileDialogClass::getFileList(void)
{
	unsigned	filecnt=0;
	FILE		*fp;
	char		*command;
	char		*out=NULL;
	char		line[1024];
	unsigned	cnt=0;
	char		*imagepath;

	asprintf(&command,"(cd \"%s\" ;find  -maxdepth 1 -mindepth 1 -follow ! -type d -print0 | xargs -0 -n 1 basename 2>/dev/null|sort|wc -l)",this->currentDir);
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
			asprintf(&command,"(cd \"%s\" ;find  -maxdepth 1 -mindepth 1 -follow ! -type d -print0 | xargs -0 -n 1 basename 2>/dev/null|sort)",this->currentDir);
			fp=popen(command, "r");
			if(fp!=NULL)
				{
					while(fgets(line,1024,fp))
						{
							if(strlen(line)>0)
								line[strlen(line)-1]=0;
							this->fileList[cnt]=strdup(line);
							if	(
									(strcasecmp(&line[strlen(line)-4],".jpg")==0) ||
									(strcasecmp(&line[strlen(line)-4],".png")==0) ||
									(strcasecmp(&line[strlen(line)-4],".gif")==0) ||
									(strcasecmp(&line[strlen(line)-5],".tiff")==0)
								)
								asprintf(&imagepath,"%s/%s",this->currentDir,line);
							else
								asprintf(&imagepath,"%s",(char*)this->fileImage);
							this->fileImageList[cnt]=strdup(imagepath);
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
	int			retval=0;
	struct stat		buf;

	retval=stat(this->currentDir,&buf);
	if(retval==-1)
		{
			if(this->currentDir!=NULL)
				free(this->currentDir);
			this->currentDir=strdup(getenv("HOME"));
		}
	asprintf(&command,"(cd \"%s\" 2>/dev/null;find  -maxdepth 1 -mindepth 1 -follow -type d -print0| xargs -0 -n 1 basename 2>/dev/null|sort|wc -l)",this->currentDir);
	out=this->wc->globalLib->LFSTK_oneLiner("%s",command);
	if(out==NULL)
		{
			dircnt=0;
		}
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
	asprintf(&command,"(cd \"%s\" 2>/dev/null ;find  -maxdepth 1 -mindepth 1 -follow -type d -print0| xargs -0 -n 1 basename 2>/dev/null|sort)",this->currentDir);
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
* Set select dialog type.
*
* \param bool true=folder, false=file.
* \note defaults to select file.
*/
void LFSTK_fileDialogClass::LFSTK_setRequestType(bool type)
{
	this->dialogType=type;
}

/**
* Get select dialog type.
*
* \return bool true=folder, false=file.
* \note defaults to select file.
*/
bool LFSTK_fileDialogClass::LFSTK_getRequestType(void)
{
	return(this->dialogType);
}

/**
* Set the working directory.
*
* \param const char *path to folder.
* \note Don't free.
*/
void LFSTK_fileDialogClass::LFSTK_setWorkingDir(const char *dir)
{
	if(this->currentDir!=NULL)
		free(this->currentDir);
	this->currentDir=strdup(dir);
	this->cleanDirPath();
//folders
	this->getDirList();
	this->dirListGadget->LFSTK_setImageList(this->dirImageList,this->dirListCnt);
	this->dirListGadget->LFSTK_setList(this->dirList,this->dirListCnt);
	this->dirEdit->LFSTK_setBuffer(this->currentDir);
//files
	if(this->dialogType==false)
		{
			this->getFileList();
			this->fileListGadget->LFSTK_setImageList(this->fileImageList,this->fileListCnt);
			this->fileListGadget->LFSTK_setList(this->fileList,this->fileListCnt);
		}
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
* \param type true=folder select, false= file select.
*/
LFSTK_fileDialogClass::LFSTK_fileDialogClass(LFSTK_windowClass* parentwc,const char *label,const char *startdir,bool type)
{
	int					hite=DIALOGHITE;
	LFSTK_labelClass	*spacer;

	this->wc=parentwc;
	this->dialog=NULL;
	this->dirList=NULL;
	this->dirImageList=NULL;
	this->fileList=NULL;
	this->fileImageList=NULL;
	this->fileListCnt=0;
	this->currentDir=strdup(startdir);
	this->currentFile=NULL;
	this->dirListGadget=NULL;
	this->dirListCnt=0;
	this->fileImage=LFSTKPIXMAPSDIR "/documents.png";
	this->folderImage=LFSTKPIXMAPSDIR "/folder.png";


	this->dialogType=type;
	int dirlisthite=GADGETHITE*FDIRHITE;
	int dirlistwid=DIALOGWIDTH-(FGAP*2)-FNAVBUTTONWID;

	if(this->dialogType==true)
		{
			dirlisthite=GADGETHITE*FDIRHITE;
			hite=dirlisthite+(FGAP*7)+(GADGETHITE*2);
		}
	else
		{
			dirlisthite=GADGETHITE*(FDIRHITE/2);
			hite=dirlisthite+(FGAP*6)+GADGETHITE+(FFILEHITE*GADGETHITE)+FGAP+GADGETHITE+FGAP;
		}

	this->dialog=new LFSTK_windowClass(0,0,DIALOGWIDTH,hite,label,false);
//TODO//?
	XSizeHints sh;
	sh.flags=PMinSize|PMaxSize|PSize|USSize;
	sh.min_width=DIALOGWIDTH;
	sh.min_height=hite;
	sh.max_width=DIALOGWIDTH;
	sh.max_height=hite;

	XSetWMNormalHints(this->wc->display,dialog->window,&sh);

	this->getDirList();
	this->getFileList();

	if(this->dialogType==true)
		{
			this->dirListGadget=new LFSTK_listGadgetClass(this->dialog,"",FGAP,FGAP,dirlistwid,dirlisthite,NorthWestGravity,NULL,0);
			this->dirEdit=new LFSTK_lineEditClass(this->dialog,this->currentDir,FGAP,dirlisthite+(FGAP*2),dirlistwid,GADGETHITE,NorthWestGravity);
			this->dirListGadget->LFSTK_setImageList(this->dirImageList,this->dirListCnt);
			this->dirListGadget->LFSTK_setList(this->dirList,this->dirListCnt);

			spacer=new LFSTK_labelClass(this->dialog,"--",0,dirlisthite+(FGAP*3)+GADGETHITE,DIALOGWIDTH,8,NorthWestGravity);
			
			this->buttonCancel=new LFSTK_buttonClass(this->dialog,"Cancel",BORDER,dirlisthite+(FGAP*6)+GADGETHITE,GADGETWIDTH,GADGETHITE,SouthWestGravity);
			this->buttonApply=new LFSTK_buttonClass(this->dialog,"Apply",DIALOGWIDTH-BORDER-GADGETWIDTH,dirlisthite+(FGAP*6)+GADGETHITE,GADGETWIDTH,GADGETHITE,SouthEastGravity);
		}
	else
		{
			this->dirListGadget=new LFSTK_listGadgetClass(this->dialog,"",FGAP,FGAP,dirlistwid,dirlisthite,NorthWestGravity,NULL,0);
			this->dirEdit=new LFSTK_lineEditClass(this->dialog,this->currentDir,FGAP,dirlisthite+(FGAP*2),dirlistwid,GADGETHITE,NorthWestGravity);
			this->dirListGadget->LFSTK_setImageList(this->dirImageList,this->dirListCnt);
			this->dirListGadget->LFSTK_setList(this->dirList,this->dirListCnt);

			this->fileListGadget=new LFSTK_listGadgetClass(this->dialog,"",FGAP,dirlisthite+(FGAP*3)+GADGETHITE,DIALOGWIDTH-(FGAP*2)-FNAVBUTTONWID,(GADGETHITE*FFILEHITE),NorthWestGravity,NULL,0);
			this->fileListGadget->LFSTK_setImageList(this->fileImageList,this->fileListCnt);
			this->fileListGadget->LFSTK_setList(this->fileList,this->fileListCnt);

			spacer=new LFSTK_labelClass(this->dialog,"--",0,dirlisthite+(FGAP*3)+GADGETHITE+(FFILEHITE*GADGETHITE)+FGAP,DIALOGWIDTH,8,NorthWestGravity);
			
			this->buttonCancel=new LFSTK_buttonClass(this->dialog,"Cancel",BORDER,dirlisthite+(FGAP*6)+GADGETHITE+(FFILEHITE*GADGETHITE)+FGAP,GADGETWIDTH,GADGETHITE,SouthWestGravity);
			this->buttonApply=new LFSTK_buttonClass(this->dialog,"Apply",DIALOGWIDTH-BORDER-GADGETWIDTH,dirlisthite+(FGAP*6)+GADGETHITE+(FFILEHITE*GADGETHITE)+FGAP,GADGETWIDTH,GADGETHITE,SouthEastGravity);
		}
}

bool LFSTK_fileDialogClass::LFSTK_isValid(void)
{
	if(this->dirListGadget->LFSTK_getCurrentListItem()!=-1)
		return(this->apply);
	return(false);
}

void LFSTK_fileDialogClass::openDir(void)
{
	this->cleanDirPath();
//folders
	this->getDirList();
	this->dirListGadget->LFSTK_setImageList(this->dirImageList,this->dirListCnt);
	this->dirListGadget->LFSTK_setList(this->dirList,this->dirListCnt);
	this->dirEdit->LFSTK_setBuffer(this->currentDir);
//files
	if(this->dialogType==false)
		{
			this->getFileList();
			this->fileListGadget->LFSTK_setImageList(this->fileImageList,this->fileListCnt);
			this->fileListGadget->LFSTK_setList(this->fileList,this->fileListCnt);
		}
}

void LFSTK_fileDialogClass::doOpenDir(void)
{
	char	*newdir;
	asprintf(&newdir,"%s/%s",this->currentDir,this->dirListGadget->LFSTK_getListString(-1));
	if(this->currentDir!=NULL)
		free(this->currentDir);
	this->currentDir=newdir;
	this->openDir();
}

void LFSTK_fileDialogClass::doOpenDir(const char *dir)
{
	if(this->currentDir!=NULL)
		free(this->currentDir);
	this->currentDir=strdup(dir);
	this->openDir();
}

/**
* Set the working directory and dialog title on show.
*
* \param const char *start dir.
* \param const char *title.
*/
void LFSTK_fileDialogClass::LFSTK_showFileDialog(const char *dir,const char *title)
{
	if(this->dialog!=NULL)
		{
			this->dialog->LFSTK_setWindowTitle(title);
			this->LFSTK_setWorkingDir(dir);
			this->LFSTK_showFileDialog();
		}
}

/**
* Show the file selector dialog.
*/
void LFSTK_fileDialogClass::LFSTK_showFileDialog(void)
{
	XEvent			event;
	unsigned		lasttime=0;
	unsigned		lastdiritem=0;
	unsigned		lastfileitem=0;
	geometryStruct	geomdir;
	geometryStruct	geomfile;
	pointStruct		pt;

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
							case KeyRelease:
								if(ml->gadget==this->dirEdit)
									{
										char	c[4];
										KeySym	keysym_return;
										XLookupString(&event.xkey,(char*)&c,255,&keysym_return,NULL);
										if(keysym_return==XK_Return)
											{
												this->doOpenDir(this->dirEdit->LFSTK_getBuffer()->c_str());
												lasttime=event.xkey.time-1000;
											}
									}
								break;

							case ButtonRelease:
								if((event.xbutton.time-lasttime<1000) && (event.xbutton.state & Button1Mask))
									{
										pt={event.xbutton.x_root,event.xbutton.y_root};
										lasttime=event.xbutton.time-1000;

										if(this->wc->globalLib->LFSTK_pointInRect(&pt,&geomdir)==true)
											this->doOpenDir();

										if(this->wc->globalLib->LFSTK_pointInRect(&pt,&geomfile)==true)
											{
												this->apply=true;
												this->mainLoop=false;
											}
									}
								else
									lasttime=event.xbutton.time;

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
								this->dirListGadget->LFSTK_getGlobalGeom(&geomdir);
								if(this->dialogType==false)
									{
										this->fileListGadget->LFSTK_getGlobalGeom(&geomfile);
									}
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
