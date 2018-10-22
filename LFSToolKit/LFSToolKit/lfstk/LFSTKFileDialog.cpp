/*
 *
 * ©K. D. Hedger. Tue 13 Sep 20:00:45 BST 2016 keithdhedger@gmail.com

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
	if(this->currentPath!=NULL)
		free(this->currentPath);

	freeAndNull(&this->filter);
	this->freeFileList();
	delete this->fc;
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

void LFSTK_fileDialogClass::freeFileList(void)
{
	if(this->fileList!=NULL)
		{
			for(int j=0;j<this->fileListCnt;j++)
				free(this->fileList[j]);
			delete[] this->fileList;
			this->fileList=NULL;
		}
	if(this->fileImageList!=NULL)
		{
			for(int j=0;j<this->fileListCnt;j++)
				free(this->fileImageList[j]);
			delete[] this->fileImageList;
			this->fileImageList=NULL;
		}
}

void LFSTK_fileDialogClass::getFileList(void)
{
	char	*imagepath;

	this->fc->setDepth(1,1);
	if(this->dialogType==FOLDERDIALOG)
		this->fc->setFindType(FTW_D);
	else
		this->fc->setFindType(-1);
	this->fc->setFollowLinks(true);
	this->fc->setIncludeHidden(false);
	this->fc->setSort(false);
	this->fc->findFiles(this->currentDir);
	this->fc->sortByNameAndType();

	this->fileListCnt=this->fc->getDataCount()+1;
	this->fileList=new char*[this->fileListCnt];
	this->fileImageList=new char*[this->fileListCnt];

	this->fileList[0]=strdup("..");
	this->fileImageList[0]=strdup(folderImage);
	for(int j=1;j<this->fileListCnt;j++)
		{
			this->fileList[j]=strdup(this->fc->data.at(j-1).name.c_str());
			if((strcasecmp(&this->fileList[j][strlen(this->fileList[j])-4],".jpg")==0) || (strcasecmp(&this->fileList[j][strlen(this->fileList[j])-4],".png")==0))
				asprintf(&imagepath,"%s",(char*)this->imageImage);
			else if(this->fc->data.at(j-1).fileType==FTW_D)
				asprintf(&imagepath,"%s",(char*)this->folderImage);
			else
				asprintf(&imagepath,"%s",(char*)this->fileImage);
			this->fileImageList[j]=strdup(imagepath);
			free(imagepath);
		}
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

	this->freeFileList();
	this->getFileList();
	this->fileListGadget->LFSTK_setImageList(this->fileImageList,this->fileListCnt);
	this->fileListGadget->LFSTK_setList(this->fileList,this->fileListCnt);
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
* \return const char *filename.
* \note Don't free.
*/
const char	*LFSTK_fileDialogClass::LFSTK_getCurrentFile(void)
{
	return(this->currentFile);
}

/**
* Return the currently selected filepath.
*
* \return const char *path to file.
* \note Don't free.
*/
const char *LFSTK_fileDialogClass::LFSTK_getCurrentPath(void)
{
	if((this->currentDir!=NULL) && (this->currentFile!=NULL))
		{
			if(this->currentPath!=NULL)
				free(this->currentPath);
			asprintf(&this->currentPath,"%s/%s",this->currentDir,this->currentFile);
		}
	return(this->currentPath);
}

/**
* Set show preview of selected file.
* \param bool show
*/
void LFSTK_fileDialogClass::LFSTK_setShowPreview(bool show)
{
	this->showPreview=show;
}

/**
* Main constructor.
*
* \param Window parent Window transient for.
* \param const char *label Displayed name.
* \param const char *startdir Open dialog in this folder.
* \param type true=folder select, false= file select.
*/
LFSTK_fileDialogClass::LFSTK_fileDialogClass(LFSTK_windowClass* parentwc,const char *label,const char *startdir,bool type,const char *recentname)
{
	int					hite=600;
	LFSTK_labelClass	*spacer;
	int					midprev;
	int					yoffset=BORDER+PREVIEWWIDTH;
	int					yspacing=0;

	this->wc=parentwc;
	this->dialog=NULL;
	this->fileList=NULL;
	this->fileImageList=NULL;
	this->fileListCnt=0;
	this->currentFile=NULL;
	this->currentPath=NULL;
	this->fileImage=LFSTKPIXMAPSDIR "/documents.png";
	this->folderImage=LFSTKPIXMAPSDIR "/folder.png";
	this->imageImage=LFSTKPIXMAPSDIR "/image.png";

	if(recentname!=NULL)
		this->recentsName=recentname;

	if(startdir==NULL)
		this->LFSTK_getLastFolder();
	else
		this->currentDir=strdup(startdir);

	this->dialogType=type;
	int dwidth=DIALOGWIDTH+PREVIEWWIDTH;

	this->dialog=new LFSTK_windowClass(0,0,dwidth,hite,label,false,true,true);

//find files
	this->fc=new FindClass;
	this->dialog->closeDisplayOnExit=false;
//TODO//?
	XSizeHints sh;
	sh.flags=PMinSize|PMaxSize|PSize|PResizeInc;
	sh.min_width=dwidth;
	sh.min_height=hite;
	sh.max_width=dwidth;
	sh.max_height=hite;
	sh.width_inc=0;
	sh.height_inc=0;
	sh.base_width=dwidth;
	sh.base_width=hite;

	XSetWMNormalHints(this->wc->display,dialog->window,&sh);

	this->getFileList();
//file list
	this->fileListGadget=new LFSTK_listGadgetClass(this->dialog,"",BORDER,BORDER,DIALOGWIDTH-FNAVBUTTONWID,(GADGETHITE*FFILEHITE),NorthWestGravity,NULL,0);
	this->fileListGadget->LFSTK_setImageList(this->fileImageList,this->fileListCnt);
	this->fileListGadget->LFSTK_setList(this->fileList,this->fileListCnt);

//edit box
	yspacing+=GADGETHITE*(FFILEHITE+1);
	this->dirEdit=new LFSTK_lineEditClass(this->dialog,this->currentDir,BORDER,yspacing,DIALOGWIDTH-(BORDER*2)+PREVIEWWIDTH,GADGETHITE,NorthWestGravity);
	yspacing+=GADGETHITE+FGAP;

//butons
	spacer=new LFSTK_labelClass(this->dialog,"--",0,yspacing,DIALOGWIDTH+PREVIEWWIDTH,8,NorthWestGravity);
	yspacing+=GADGETHITE;
	this->buttonCancel=new LFSTK_buttonClass(this->dialog,"Cancel",BORDER,yspacing,GADGETWIDTH,GADGETHITE,SouthWestGravity);
	this->buttonApply=new LFSTK_buttonClass(this->dialog,"Apply",DIALOGWIDTH-BORDER-GADGETWIDTH+PREVIEWWIDTH,yspacing,GADGETWIDTH,GADGETHITE,NorthWestGravity);

	midprev=DIALOGWIDTH+PREVIEWWIDTH-(PREVIEWWIDTH/2);
	this->tux=new LFSTK_imageClass(this->dialog,NULL,midprev-(PREVIEWWIDTH/2)-(BORDER/4),BORDER,PREVIEWWIDTH,PREVIEWWIDTH,PRESERVEASPECT,true);
//filename
	this->previewFileName=new LFSTK_labelClass(this->dialog,"",DIALOGWIDTH,yoffset,PREVIEWWIDTH,16,NorthWestGravity);
	this->previewFileName->LFSTK_setFontString(this->previewFileName->monoFontString);
	this->previewFileName->LFSTK_setCairoFontData();
	yoffset+=GADGETHITE;
//mime type
	this->previewMimeType=new LFSTK_labelClass(this->dialog,"",DIALOGWIDTH,yoffset,PREVIEWWIDTH,16,NorthWestGravity);
	this->previewMimeType->LFSTK_setFontString(this->previewMimeType->monoFontString);
	this->previewMimeType->LFSTK_setCairoFontData();
	yoffset+=GADGETHITE;
//size
	this->previewSize=new LFSTK_labelClass(this->dialog,"",DIALOGWIDTH,yoffset,PREVIEWWIDTH,16,NorthWestGravity);
	this->previewSize->LFSTK_setFontString(this->previewSize->monoFontString);
	this->previewSize->LFSTK_setCairoFontData();
	yoffset+=GADGETHITE;
//mode
	this->previewMode=new LFSTK_labelClass(this->dialog,"",DIALOGWIDTH,yoffset,PREVIEWWIDTH,16,NorthWestGravity);
	this->previewMode->LFSTK_setFontString(this->previewMode->monoFontString);
	this->previewMode->LFSTK_setCairoFontData();
}

bool LFSTK_fileDialogClass::LFSTK_isValid(void)
{
	if(this->apply==false)
		return(false);
	if(this->dialogType==FILEDIALOG)
		{
			if((this->currentFile!=NULL) && (this->currentDir!=NULL))
				return(true);
		}
	else
		{
			if(this->currentDir!=NULL)
				return(true);
		}
	return(false);
}

void LFSTK_fileDialogClass::openDir(void)
{
	this->cleanDirPath();
	this->freeFileList();
	this->getFileList();
	this->fileListGadget->LFSTK_setImageList(this->fileImageList,this->fileListCnt);
	this->fileListGadget->LFSTK_setList(this->fileList,this->fileListCnt);
}

bool LFSTK_fileDialogClass::doOpenDir(void)
{
	char	*newdir;
	struct stat	sb;
	asprintf(&newdir,"%s/%s",this->currentDir,this->fileListGadget->LFSTK_getListString(-1));
	stat(newdir,&sb);

	if((sb.st_mode & S_IFMT)==S_IFDIR)
		{
			if(this->currentDir!=NULL)
				free(this->currentDir);
			this->currentDir=newdir;
			this->openDir();
			this->dirEdit->LFSTK_setBuffer(this->currentDir);
			return(true);
		}
	else
		free(newdir);
	return(false);
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
			if(dir!=NULL)
				this->LFSTK_setWorkingDir(dir);
			else
				{
					this->LFSTK_getLastFolder();
				}
			this->LFSTK_showFileDialog();
		}
}

/**
* Find themed icon from mimetype
* \param const char *mimetype
* \return char*
* \note Caller owns return value and must free.
*/
char* LFSTK_fileDialogClass::findThemedIconFromMime(const char *mimetype)
{
	char		*theme=NULL;
	char		*iconpath=NULL;
	const char	*iconthemes[3];
	const char	*iconfolders[2];

	theme=this->wc->globalLib->LFSTK_oneLiner("head ~/.config/LFS/lfsdesktop.rc|grep -i icontheme|awk '{print $2}'");
	if(theme==NULL)
		asprintf(&theme,"gnome");

	iconthemes[0]=theme;
	iconthemes[1]="hicolor";
	iconthemes[2]="gnome";

	iconfolders[0]="~/.icons";
	iconfolders[1]="/usr/share/icons";
	iconpath=NULL;
	for(int j=0;j<2;j++)
		{
			for(int k=0;k<3;k++)
				{
					iconpath=this->wc->globalLib->LFSTK_oneLiner("find %s/\"%s\"/*/mime* -iname \"*%s.*\" 2>/dev/null|sort -Vr|head -n1 2>/dev/null",iconfolders[j],iconthemes[k],mimetype);
					if((iconpath!=NULL) && (strlen(iconpath)>1))
						goto breakReturn;
					if(iconpath!=NULL)
						free(iconpath);
					iconpath=NULL;
				}
		}
breakReturn:
	free(theme);
	return(iconpath);
}

/**
* Get current selection
* \return const char*
* \note Return is owned by TK don't free.
*/
const char* LFSTK_fileDialogClass::LFSTK_getCurrentFileSelection(void)
{
	return(this->fileListGadget->LFSTK_getListString(this->fileListGadget->LFSTK_getCurrentListItem()));
}

/**
* Set preview data
*/
void LFSTK_fileDialogClass::setPreviewData(void)
{
	char		*mt=NULL;
	char		*convmt=NULL;
	char		*iconpath=NULL;
	char		*testmime=NULL;
	char		*ptr=NULL;
	struct stat	buf;
	char		*statdata=NULL;

	if(this->dialogType!=FILEDIALOG)
		return;

	if(this->fileListCnt==0)
		{
			this->previewMimeType->LFSTK_setLabel("");
			this->previewSize->LFSTK_setLabel("");
			this->previewFileName->LFSTK_setLabel("");
			this->tux->useImage=false;
			this->previewMimeType->LFSTK_clearWindow();
			this->previewSize->LFSTK_clearWindow();
			this->previewFileName->LFSTK_clearWindow();
			this->tux->LFSTK_clearWindow();
			return;
		}

	asprintf(&mt,"%s/%s",this->currentDir,this->LFSTK_getCurrentFileSelection());
	convmt=this->wc->globalLib->LFSTK_getMimeType(mt);
	this->previewMimeType->LFSTK_setLabel(convmt);
	this->previewFileName->LFSTK_setLabel(this->LFSTK_getCurrentFileSelection());
	if(stat(mt,&buf)==0)
		{
			asprintf(&statdata,"Size:%i",buf.st_size);
			this->previewSize->LFSTK_setLabel(statdata);
			free(statdata);
			asprintf(&statdata,"Mode:%o",buf.st_mode & 07777);
			this->previewMode->LFSTK_setLabel(statdata);
			free(statdata);
			if((buf.st_mode & S_IFMT)!=S_IFDIR)
				this->dirEdit->LFSTK_setBuffer(mt);
		}

	for(int j=0;j<strlen(convmt);j++)
		{
			if(convmt[j]=='/')
				convmt[j]='-';
		}

	if((strcmp(convmt,"image-jpeg")==0) || (strcmp(convmt,"image-png")==0))
		iconpath=strdup(mt);
	else
		{
			iconpath=this->findThemedIconFromMime(convmt);

			if((iconpath!=NULL) && (strlen(iconpath)<2) || (iconpath==NULL))
				{
					asprintf(&testmime,"gnome-mime-%s",convmt);
					ptr=strrchr(testmime,'-');
					ptr[0]=0;
					iconpath=this->findThemedIconFromMime(testmime);
					free(testmime);
				}

			if((iconpath!=NULL) && (strlen(iconpath)<2) || (iconpath==NULL))
				{
					ptr=strchr(convmt,'-');
					ptr++;
					asprintf(&testmime,"gnome-mime-application-%s",ptr);
					iconpath=this->findThemedIconFromMime(testmime);
					free(testmime);
				}

			if((iconpath!=NULL) && (strlen(iconpath)<2) || (iconpath==NULL))
				iconpath=strdup("/usr/share/icons/gnome/256x256/mimetypes/text-x-generic.png");
		}

	this->tux->LFSTK_setImageFromPath(iconpath,PRESERVEASPECT,true);
	this->wc->LFSTK_clearWindow();
	this->tux->LFSTK_clearWindow();
	this->wc->LFSTK_clearWindow();

	free(mt);
	free(convmt);
	free(iconpath);
}

/**
* Set current folder to last used folder, universal.
*/
void LFSTK_fileDialogClass::LFSTK_getLastFolder(void)
{
	if(this->currentDir!=NULL)
		free(this->currentDir);
	DEBUGFUNC("grep -i '%s' '%s/dialoglast.rc'|awk -F= '{print $NF}'",this->recentsName,this->wc->configDir);
	this->currentDir=this->wc->globalLib->LFSTK_oneLiner("grep -i '%s' '%s/dialoglast.rc'|awk -F= '{print $NF}'",this->recentsName,this->wc->configDir);
	if(strlen(this->currentDir)==0)
		{
			free(this->currentDir);
			this->currentDir=strdup(this->wc->userHome);
		}
}

/**
* Resize window.
* \param w New width.
* \param h New height.
* \param tellx Inform X (default=true).
* \note Should only be called from child gadget constructor.
*/
void LFSTK_fileDialogClass::resizeWindow(int w,int h)
{
//	this->setWindowGeom(0,0,w,h,WINDSETWH);
//	if(tellx==true)
		XResizeWindow(this->dialog->display,this->dialog->window,w,h);
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
	char			*lastdir=NULL;

	this->apply=false;
	if(this->dialog!=NULL)
		{
//			if(this->showPreview==true)
//				this->resizeWindow(DIALOGWIDTH+PREVIEWWIDTH,-1);
//			else
//				this->resizeWindow(DIALOGWIDTH,-1);
			this->setPreviewData();
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

					if(this->dialog->LFSTK_handleWindowEvents(&event)<0)
						mainLoop=false;

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
												this->doOpenDir(this->dirEdit->LFSTK_getCStr());
												lasttime=event.xkey.time-1000;
											}
									}
								break;

							case ButtonRelease:
								if((event.xbutton.time-lasttime<1000) && (event.xbutton.state & Button1Mask))
									{
										pt={event.xbutton.x_root,event.xbutton.y_root};
										lasttime=event.xbutton.time-1000;

										this->fileListGadget->LFSTK_getGlobalGeom(&geomfile);
										if(this->wc->globalLib->LFSTK_pointInRect(&pt,&geomfile)==true)
											{
												if(this->doOpenDir()==false)
													{
														this->apply=true;
														if(this->currentFile!=NULL)
															free(this->currentFile);
														asprintf(&this->currentFile,"%s",this->fileListGadget->LFSTK_getListString(this->fileListGadget->LFSTK_getCurrentListItem()));
														this->mainLoop=false;
													}
											}
									}
								else
									{
										lasttime=event.xbutton.time;
									}

								setPreviewData();

								if(ml!=NULL)
									{
										if(ml->gadget==this->buttonApply)
											{
												this->apply=true;
												this->mainLoop=false;
												if(currentDir!=NULL)
													{
														asprintf(&lastdir,"sed -i '/%s=/d' '%s/dialoglast.rc';echo '%s=%s'|cat - '%s/dialoglast.rc'|sort -uo '%s/dialoglast.rc'",this->recentsName,this->wc->configDir,this->recentsName,this->currentDir,this->wc->configDir,this->wc->configDir);
														system(lastdir);
														free(lastdir);
													}
												if(this->dialogType==FILEDIALOG)
													{
														if(this->currentFile!=NULL)
															free(this->currentFile);
														asprintf(&this->currentFile,"%s",this->fileListGadget->LFSTK_getListString(this->fileListGadget->LFSTK_getCurrentListItem()));
													}
											}
										if(ml->gadget==this->buttonCancel)
											{
												this->apply=false;
												this->mainLoop=false;
											}
									}
								break;

							case Expose:
								if (event.xexpose.count==0)
									this->dialog->LFSTK_clearWindow();
								break;

							case ConfigureNotify:
								if(this->dialogType==FILEDIALOG)
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

/**
* Show the file selector dialog.
*/
void LFSTK_fileDialogClass::LFSTK_setNameFilter(const char *filt)
{
	freeAndNull(&this->filter);
	if(filt!=NULL)
		this->filter=strdup(filt);
}
