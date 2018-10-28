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
#include <libgen.h>

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
	char	*rp;

	//rp=realpath(this->currentDir,NULL);
	rp=this->wc->globalLib->LFSTK_oneLiner("realpath -s '%s'",this->currentDir);
	if(this->currentDir!=NULL)
		free(this->currentDir);
	this->currentDir=rp;
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
		this->fc->setFindType(FOLDERTYPE);
	else
		this->fc->setFindType(ANYTYPE);
	this->fc->setIgnoreBroken(false);
	this->fc->setIncludeHidden(this->showHidden);
	this->fc->findFiles(this->currentDir);
	this->fc->setSort(false);
	this->fc->sortByTypeAndName();

	this->fileListCnt=this->fc->getDataCount();
	this->fileList=new char*[this->fileListCnt];
	this->fileImageList=new char*[this->fileListCnt];

	for(int j=0;j<this->fileListCnt;j++)
		{
			this->fileList[j]=strdup(this->fc->data.at(j).name.c_str());
			switch(this->fc->data.at(j).fileType)
				{
							case FOLDERTYPE:
								asprintf(&imagepath,"%s",(char*)this->folderImage);
								break;
							case FILETYPE:
								if((strcasecmp(&this->fileList[j][strlen(this->fileList[j])-4],".jpg")==0) || (strcasecmp(&this->fileList[j][strlen(this->fileList[j])-4],".png")==0))
									asprintf(&imagepath,"%s",(char*)this->imageImage);
									//asprintf(&imagepath,"%s/%s",this->currentDir,this->fileList[j]);
								else
									asprintf(&imagepath,"%s",(char*)this->fileImage);
								break;
							case FOLDERLINKTYPE:
								asprintf(&imagepath,"%s",(char*)this->folderImageLink);
								break;
							case FILELINKTYPE:
								if((strcasecmp(&this->fileList[j][strlen(this->fileList[j])-4],".jpg")==0) || (strcasecmp(&this->fileList[j][strlen(this->fileList[j])-4],".png")==0))
									asprintf(&imagepath,"%s",(char*)this->imageImageLink);
									//asprintf(&imagepath,"%s",this->fileList[j]);
								else
									asprintf(&imagepath,"%s",(char*)this->fileImageLink);
								break;
							case BROKENLINKTYPE:
								asprintf(&imagepath,"%s",(char*)this->imageBrokenLink);
								break;
							default:
								asprintf(&imagepath,"%s",(char*)this->fileImage);
								break;
						}
				//}
			//fprintf(stderr,"%s\n",imagepath);
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
*/
void LFSTK_fileDialogClass::LFSTK_setWorkingDir(const char *dir)
{
	char	*holddir;
//in case dir doesnt exist
	if(access(dir,F_OK)!=0)
		holddir=strdup(this->wc->userHome);
//in case dir=this->currentDir
	else
		holddir=strdup(dir);

	if(this->currentDir!=NULL)
		free(this->currentDir);

	this->currentDir=holddir;
	this->cleanDirPath();

	this->freeFileList();
	this->getFileList();
	this->fileListGadget->LFSTK_setImageList(this->fileImageList,this->fileListCnt);
	this->fileListGadget->LFSTK_setList(this->fileList,this->fileListCnt);
	this->fileListGadget->LFSTK_clearWindow();
	this->dirEdit->LFSTK_setBuffer(this->currentDir);
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
	return(this->currentPath);
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
	int					dwidth=DIALOGWIDTH+PREVIEWWIDTH;

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
	this->fileImageLink=LFSTKPIXMAPSDIR "/document.link.png";
	this->folderImageLink=LFSTKPIXMAPSDIR "/folder.link.png";
	this->imageImageLink=LFSTKPIXMAPSDIR "/image.link.png";
	this->imageBrokenLink=LFSTKPIXMAPSDIR "/document.link.broken.png";

	if(recentname!=NULL)
		this->recentsName=recentname;

	if(startdir==NULL)
		this->LFSTK_getLastFolder();
	else
		this->currentDir=strdup(startdir);

	this->dialogType=type;
	if(type==FOLDERDIALOG)
		dwidth=DIALOGWIDTH;

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
	this->dirEdit=new LFSTK_lineEditClass(this->dialog,this->currentDir,BORDER,yspacing,dwidth-(BORDER*2),GADGETHITE,NorthWestGravity);
	yspacing+=GADGETHITE+FGAP;

//butons
	spacer=new LFSTK_labelClass(this->dialog,"--",0,yspacing,DIALOGWIDTH+PREVIEWWIDTH,8,NorthWestGravity);
	yspacing+=GADGETHITE;
	this->buttonCancel=new LFSTK_buttonClass(this->dialog,"Cancel",BORDER,yspacing,GADGETWIDTH,GADGETHITE,SouthWestGravity);
	if(this->dialogType==FOLDERDIALOG)
		this->buttonHidden=new LFSTK_buttonClass(this->dialog,"Hidden",DIALOGMIDDLE-HALFGADGETWIDTH,yspacing,GADGETWIDTH,GADGETHITE,NorthWestGravity);
	else
		this->buttonHidden=new LFSTK_buttonClass(this->dialog,"Hidden",DIALOGMIDDLE+(PREVIEWWIDTH/2)-HALFGADGETWIDTH,yspacing,GADGETWIDTH,GADGETHITE,NorthWestGravity);
	this->buttonApply=new LFSTK_buttonClass(this->dialog,"Apply",dwidth-BORDER-GADGETWIDTH,yspacing,GADGETWIDTH,GADGETHITE,NorthWestGravity);

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

bool LFSTK_fileDialogClass::doOpenDir(const char *dir)
{
	struct stat	sb;
	stat(dir,&sb);

	if((sb.st_mode & S_IFMT)==S_IFDIR)
		{
			if(this->currentDir!=NULL)
				free(this->currentDir);
			this->currentDir=strdup(dir);
			this->openDir();
			return(true);
		}
	else
		return(false);
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
				this->LFSTK_getLastFolder();
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
void LFSTK_fileDialogClass::setPreviewData(bool fromlist)
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

	if(fromlist==true)
		asprintf(&mt,"%s/%s",this->currentDir,this->LFSTK_getCurrentFileSelection());
	else
		asprintf(&mt,"%s",this->dirEdit->LFSTK_getCStr());

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
	this->currentDir=this->wc->globalLib->LFSTK_oneLiner("grep -i '%s' '%s/dialoglast.rc'|awk -F= '{print $NF}'",this->recentsName,this->wc->configDir);
	if(strlen(this->currentDir)<2)
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
			this->dirEdit->LFSTK_setBuffer(this->currentDir);
			this->LFSTK_setWorkingDir(this->currentDir);
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

					if(ml==NULL)
						continue;

					if(ml->gadget==this->dirEdit)
						{
							switch(event.type)
								{
									char	c[4];
									case KeyRelease:
										KeySym	keysym_return;
										XLookupString(&event.xkey,(char*)&c,255,&keysym_return,NULL);
										if(keysym_return==XK_Return)
											{
												if(this->isADir(this->dirEdit->LFSTK_getCStr()))
													{
														this->apply=false;
														this->LFSTK_setWorkingDir(this->dirEdit->LFSTK_getCStr());
														continue;
													}
											}
										break;
								}
							continue;
						}

					switch(event.type)
						{
							case ButtonRelease:
								if(ml->gadget==this->buttonHidden)
									{
										this->showHidden=!this->showHidden;
										this->LFSTK_setWorkingDir(this->currentDir);
										break;
									}
								if(ml->gadget==this->buttonApply)
									{
										char	*buf;
										char	*rp;
											
										asprintf(&buf,"%s/%s",this->dirEdit->LFSTK_getCStr(),this->fileListGadget->LFSTK_getListString(this->fileListGadget->LFSTK_getCurrentListItem()));
										if((this->isADir(buf)==true) && (this->dialogType==FILEDIALOG))
											{
												this->apply=false;
												this->LFSTK_setWorkingDir(buf);
												free(buf);
												break;
											}
										
										if(this->dialogType==FOLDERDIALOG)
											{
												this->apply=true;
												mainLoop=false;
												this->setFileData();
												free(buf);
												break;
											}
										this->apply=true;
										this->setFileData();
										mainLoop=false;
										free(buf);
										break;
									}

								if(ml->gadget==this->buttonCancel)
									{
										mainLoop=false;
										this->apply=false;
										break;
									}
								
								this->fileListGadget->LFSTK_getGlobalGeom(&geomfile);
								pt={event.xbutton.x_root,event.xbutton.y_root};
								if(this->wc->globalLib->LFSTK_pointInRect(&pt,&geomfile)==true)
									{
										char	*buf;
										char	*rp;
											
										asprintf(&buf,"%s/%s",this->currentDir,this->fileListGadget->LFSTK_getListString(this->fileListGadget->LFSTK_getCurrentListItem()));
										//rp=canonicalize_file_name(buf);
										rp=this->wc->globalLib->LFSTK_oneLiner("realpath -s '%s'",buf);
										if(this->isADir(buf)==false)
											this->dirEdit->LFSTK_setBuffer(rp);
										free(buf);
										this->setPreviewData(true);
//double click
										if((event.xbutton.time-lasttime<1000) && (event.xbutton.state & Button1Mask))
											{												
												this->dirEdit->LFSTK_setBuffer(rp);
												lasttime=event.xbutton.time-1000;
												if(this->isADir(this->dirEdit->LFSTK_getCStr())==true)
													{
														this->apply=false;
														this->LFSTK_setWorkingDir(this->dirEdit->LFSTK_getCStr());
													}
												else
													{
														this->apply=true;
														this->setFileData();
														mainLoop=false;
													}
											}
										else
											lasttime=event.xbutton.time;
										free(rp);
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

/**
* Set file data.
*/
void LFSTK_fileDialogClass::setFileData(void)
{
	char	*filestr;
	char	*lastdir;

	if(this->apply==true)
		{
//dir
			filestr=strdup(this->dirEdit->LFSTK_getCStr());
			if(this->currentDir!=NULL)
				freeAndNull(&this->currentDir);
			if(this->dialogType==FILEDIALOG)
				this->currentDir=strdup(dirname(filestr));
			else
				this->currentDir=strdup(filestr);
			free(filestr);
//file
			filestr=strdup(this->dirEdit->LFSTK_getCStr());
			if(this->currentFile!=NULL)
				freeAndNull(&this->currentFile);
			this->currentFile=strdup(basename(filestr));
			free(filestr);
//path
			if(this->currentPath!=NULL)
				freeAndNull(&this->currentPath);
			this->currentPath=strdup(this->dirEdit->LFSTK_getCStr());

			asprintf(&lastdir,"sed -i '/%s=/d' '%s/dialoglast.rc';echo '%s=%s'|cat - '%s/dialoglast.rc'|sort -uo '%s/dialoglast.rc'",this->recentsName,this->wc->configDir,this->recentsName,this->currentDir,this->wc->configDir,this->wc->configDir);
			system(lastdir);
			free(lastdir);
		}
}

/**
* Check if folder.
*/
bool LFSTK_fileDialogClass::isADir(const char *path)
{
	struct stat	sb;
	int			retval;
	retval=stat(path,&sb);

	if(retval!=0)
		return(false);
	if((sb.st_mode & S_IFMT)==S_IFDIR)
		return(true);
	else
		return(false);
}






