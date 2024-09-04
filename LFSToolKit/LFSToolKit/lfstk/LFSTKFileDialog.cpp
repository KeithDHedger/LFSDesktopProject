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
	cairo_surface_destroy(this->fileImage);
	cairo_surface_destroy(this->folderImage);
	cairo_surface_destroy(this->imageImage);
	cairo_surface_destroy(this->fileImageLink);
	cairo_surface_destroy(this->folderImageLink);
	cairo_surface_destroy(this->imageImageLink);
	cairo_surface_destroy(this->brokenLink);
	delete this->fc;
	delete this->dialog;
}

void LFSTK_fileDialogClass::cleanDirPath(void)
{
	char		*rp;

	rp=realpath(this->currentDir.c_str(),NULL);
	if(rp!=NULL)
		{
			this->currentDir=rp;
			freeAndNull(&rp);
		}
}

void LFSTK_fileDialogClass::getFileList(void)
{
	char				imagepath[PATH_MAX];
	infoDataStruct	ls;
	dataStruct		nav;

	this->fc->LFSTK_setDepth(1,1);
	if(this->dialogType==FOLDERDIALOG)
		this->fc->LFSTK_setFindType(FOLDERTYPE);
	else
		this->fc->LFSTK_setFindType(FILETYPE);
	this->fc->LFSTK_setFullPath(true);
	this->fc->LFSTK_setIgnoreBroken(false);
	this->fc->LFSTK_setIncludeHidden(this->showHidden);
	this->fc->LFSTK_findFiles(this->currentDir.c_str(),false);

	this->fc->LFSTK_setSortDecending(false);
	this->fc->LFSTK_sortByTypeAndName();

	this->fileListCnt=this->fc->LFSTK_getDataCount();
	this->fileListGadget->freeCairoImages=false;
	this->fileListGadget->LFSTK_freeList();
	if(fileListGadget->maxShowing>this->fileListCnt)
		this->fileListGadget->scrollBar->LFSTK_setActive(false);
	else
		this->fileListGadget->scrollBar->LFSTK_setActive(true);

	for(int j=0;j<this->fileListCnt;j++)
		{
			ls.label=this->fc->data.at(j).name.c_str();
			ls.imageType=CAIROTHUMB;
			switch(this->fc->data.at(j).fileType)
				{
					case FOLDERTYPE:
						ls.surface=this->folderImage;
						break;

					case FILETYPE:
						if((LFSTK_UtilityClass::LFSTK_strStr(ls.label,".jpg",true).length()>0) || (LFSTK_UtilityClass::LFSTK_strStr(ls.label,".png",true).length()>0))
							{
								if(this->useThumbs==true)
									{
										ls.imageType=FILETHUMB;
										ls.imagePath=this->fc->data.at(j).path;
									}
								else
									ls.surface=this->imageImage;
							}
						else
							ls.surface=this->fileImage;
						break;

					case FOLDERLINKTYPE:
						ls.surface=this->folderImageLink;
						break;

					case FILELINKTYPE:
						if((LFSTK_UtilityClass::LFSTK_strStr(ls.label,".jpg",true).length()>0) || (LFSTK_UtilityClass::LFSTK_strStr(ls.label,".png",true).length()>0))
							{
								if(this->useThumbs==true)
									{
										ls.imageType=FILETHUMB;
										ls.imagePath=this->fc->data.at(j).path;
									}
								else
									ls.surface=this->imageImageLink;
							}
						else
							ls.surface=this->fileImageLink;
						break;

					case BROKENLINKTYPE:
						ls.surface=this->brokenLink;
						break;
				
					default:
						ls.surface=this->fileImage;
						break;
				}
			fileListGadget->LFSTK_appendToList(ls);
		}
	this->fileListGadget->LFSTK_updateList();
	this->fileListGadget->LFSTK_selectByIndex(0);
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
void LFSTK_fileDialogClass::LFSTK_setWorkingDir(std::string dir)
{
	std::string	holddir;
//in case dir doesnt exist
	if(!std::filesystem::exists(std::filesystem::status(dir)))
		holddir=this->wc->app->userHome;
//in case dir=this->currentDir
	else
		holddir=dir;

	this->currentDir=holddir;
	this->cleanDirPath();

	this->getFileList();
	this->fileListGadget->LFSTK_clearWindow();
	this->dirEdit->LFSTK_setBuffer(this->currentDir);
}

/**
* Return the currently selected folder.
*
* \return const char *path to folder.
* \note Don't free.
*/
std::string LFSTK_fileDialogClass::LFSTK_getCurrentDir(void)
{
	return(this->currentDir);
}

/**
* Return the currently selected file.
*
* \return const char *filename.
* \note Don't free.
*/
std::string LFSTK_fileDialogClass::LFSTK_getCurrentFile(void)
{
	return(this->currentFile);
}

/**
* Return the currently selected filepath.
*
* \return const char *path to file.
* \note Don't free.
*/
std::string LFSTK_fileDialogClass::LFSTK_getCurrentPath(void)
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
	LFSTK_labelClass		*spacer;
	int					midprev;
	windowInitStruct		*win;
	int					hite=600;
	int					yspacing=0;
	int					yoffset=BORDER+PREVIEWWIDTH;
	unsigned				dwidth=DIALOGWIDTH+PREVIEWWIDTH;

	this->wc=parentwc;
	this->dialog=NULL;
	this->fileListCnt=0;
	this->fileImage=parentwc->globalLib->LFSTK_createSurfaceFromPath(DOCSPNG);
	this->folderImage=parentwc->globalLib->LFSTK_createSurfaceFromPath(FOLDERPNG);
	this->imageImage=parentwc->globalLib->LFSTK_createSurfaceFromPath(IMAGEPNG);
	this->fileImageLink=parentwc->globalLib->LFSTK_createSurfaceFromPath(DOCLINKPNG);
	this->folderImageLink=parentwc->globalLib->LFSTK_createSurfaceFromPath(FOLDERLINKPNG);
	this->imageImageLink=parentwc->globalLib->LFSTK_createSurfaceFromPath(IMAGELINKPNG);
	this->brokenLink=parentwc->globalLib->LFSTK_createSurfaceFromPath(BOKENLINKPNG);

	if(recentname!=NULL)
		this->recentsName=recentname;

	if(startdir==NULL)
		this->LFSTK_getLastFolder();
	else
		this->currentDir=startdir;

	this->dialogType=type;
	if(type==FOLDERDIALOG)
		dwidth=DIALOGWIDTH;

	win=this->wc->app->LFSTK_getDefaultWInit();
	win->windowName=label;
	win->w=dwidth;
	win->h=hite;
	win->wc=parentwc;
	win->windowType=win->app->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_DIALOG"));
	this->dialog=new LFSTK_windowClass(win,parentwc->app);

	this->dialogGeom={0,0,dwidth,600,0};
	delete win;

	this->dialog->autoLabelColour=false;
	this->dialog->LFSTK_reloadGlobals();
	this->dialog->LFSTK_clearWindow();

//hack
	LFSTK_labelClass *label1=new LFSTK_labelClass(this->dialog,"XX",0,0,dwidth,600,NorthGravity);

//find files
	this->fc=new LFSTK_findClass;

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

	XSetWMNormalHints(this->wc->app->display,dialog->window,&sh);

//file list
	this->fileListGadget=new LFSTK_listGadgetClass(this->dialog,"",BORDER,BORDER,DIALOGWIDTH-FNAVBUTTONWID,(GADGETHITE*FFILEHITE));
	this->fileListGadget->LFSTK_setMouseCallBack(NULL,this->select,this);
	this->getFileList();

//edit box
	yspacing+=GADGETHITE*(FFILEHITE+1);
	this->dirEdit=new LFSTK_lineEditClass(this->dialog,this->currentDir.c_str(),BORDER,yspacing,dwidth-(BORDER*2),GADGETHITE,NorthWestGravity);
	yspacing+=GADGETHITE+FGAP;

//butons
	spacer=new LFSTK_labelClass(this->dialog,"--",0,yspacing,DIALOGWIDTH+PREVIEWWIDTH,8,NorthWestGravity);
	yspacing+=GADGETHITE;
	this->buttonCancel=new LFSTK_buttonClass(this->dialog,"Cancel",BORDER,yspacing,GADGETWIDTH,GADGETHITE,NorthWestGravity);
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
	yoffset+=GADGETHITE;
//link
	this->previewIsLink=new LFSTK_labelClass(this->dialog,"",DIALOGWIDTH,yoffset,PREVIEWWIDTH,16,NorthWestGravity);
	this->previewIsLink->LFSTK_setFontString(this->previewMode->monoFontString);
	this->previewIsLink->LFSTK_setCairoFontData();
}

bool LFSTK_fileDialogClass::LFSTK_isValid(void)
{
	if(this->apply==false)
		return(false);

	if(this->dialogType==FILEDIALOG)
		{
			if((this->currentFile.empty()==false) && (this->currentDir.empty()==false))
				return(true);
		}
	else
		{
			if(this->currentDir.empty()==false)
				return(true);
		}
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
* Get current selection
* \return const char*
* \note Return is owned by TK don't free.
*/
std::string LFSTK_fileDialogClass::LFSTK_getCurrentFileSelection(void)
{
	return(this->fileListGadget->LFSTK_getSelectedLabel());
}

/**
* Set preview data
*/
void LFSTK_fileDialogClass::setPreviewData(bool fromlist)
{
	fileInformation						info;
	std::string							previewlabel="";
	std::experimental::filesystem::path	filepath;
	std::stringstream					infostream;

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
		filepath=this->currentDir+"/"+this->LFSTK_getCurrentFileSelection();
	else
		filepath=this->dirEdit->LFSTK_getBuffer();

	this->wc->globalLib->LFSTK_getFileInfo(filepath.c_str(),&info);
	this->previewMimeType->LFSTK_setLabel(info.mimeType.c_str());

	if((info.mimeType.compare("image/jpeg")==0) || (info.mimeType.compare("image/png")==0))
		this->tux->LFSTK_setImageFromPath(filepath,PRESERVEASPECT,true);
	else
		this->tux->LFSTK_setImageFromPath(info.iconPath,PRESERVEASPECT,true);
	this->previewFileName->LFSTK_setLabel(this->LFSTK_getCurrentFileSelection().c_str());
	previewlabel+="Size:"+std::to_string(info.fileSize);
	this->previewSize->LFSTK_setLabel(previewlabel.c_str());
    infostream << "Mode:"<<std::setfill('0') << std::setw(3)<<std::oct << info.fileMode;
    this->previewMode->LFSTK_setLabel(infostream.str());

	if(info.isLink==true)
		this->previewIsLink->LFSTK_setLabel("Symlink");
	else
		this->previewIsLink->LFSTK_setLabel("");

//TODO//HMMMmmmmmmm
  	this->dialog->globalLib->LFSTK_setCairoSurface(this->wc->app->display,this->dialog->window,this->dialog->visual,&this->dialog->sfc,&this->dialog->cr,this->dialogGeom.w,this->dialogGeom.h);
	this->tux->LFSTK_clearWindow();
}

/**
* Set current folder to last used folder, universal.
*/
void LFSTK_fileDialogClass::LFSTK_getLastFolder(void)
{
	this->currentDir=this->wc->app->globalLib->LFSTK_oneLiner("grep -i '%S' '%S/dialoglast.rc' 2>/dev/null|awk -F= '{print $NF}'",this->recentsName,this->wc->app->configDir);
	if(this->currentDir.length()<2)
		this->currentDir=this->wc->app->userHome;
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
	XResizeWindow(this->dialog->app->display,this->dialog->window,w,h);
}

/**
* Show the file selector dialog.
*/
void LFSTK_fileDialogClass::LFSTK_showFileDialog(void)
{
	XEvent			event;
	unsigned			lasttime=0;
	unsigned			lastdiritem=0;
	unsigned			lastfileitem=0;
	geometryStruct	geomdir;
	geometryStruct	geomfile;
	pointStruct		pt;
	char				*lastdir=NULL;

	this->apply=false;
	if(this->dialog!=NULL)
		{
			this->LFSTK_setWorkingDir(this->currentDir);
			this->dialog->LFSTK_showWindow();
			this->dialog->LFSTK_setKeepAbove(true);
			this->dialog->LFSTK_setTransientFor(this->wc->window);
			this->mainLoop=true;
			while(this->mainLoop==true)
				{
					XNextEvent(this->dialog->app->display,&event);
					mappedListener *ml=this->dialog->LFSTK_getMappedListener(event.xany.window);
					if(ml!=NULL)
						ml->function(ml->gadget,&event,ml->type);

					if(this->dialog->LFSTK_handleWindowEvents(&event)<0)
						this->mainLoop=false;
					
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
										this->LFSTK_setWorkingDir(this->currentDir.c_str());
										break;
									}

								if(ml->gadget==this->buttonApply)
									{
										std::stringstream	infostream;
										infostream << this->dirEdit->LFSTK_getBuffer()<<"/"<<this->fileListGadget->LFSTK_getSelectedLabel();
										if((this->isADir(infostream.str().c_str())==true) && (this->dialogType==FILEDIALOG))
											{
												this->apply=false;
												this->LFSTK_setWorkingDir(infostream.str().c_str());
												break;
											}
										
										if(this->dialogType==FOLDERDIALOG)
											{
												this->apply=true;
												this->mainLoop=false;
												this->setFileData();
												break;
											}
										this->apply=true;
										this->setFileData();
										this->mainLoop=false;
										break;
									}

								if(ml->gadget==this->buttonCancel)
									{
										this->mainLoop=false;
										this->apply=false;
										break;
									}
								break;
						}
				}
			this->dialog->LFSTK_hideWindow();
		}
}

/**
* Set name filter.
* \param const char *filt List of filters seperated by smi-colon ( eg "png;jpg" );
*/
void LFSTK_fileDialogClass::LFSTK_setNameFilter(const char *filt)
{
	if(filt!=NULL)
		{
			this->filter=filt;
			this->fc->LFSTK_setFileTypes(filt);
		}
}

/**
* Set file data.
*/
void LFSTK_fileDialogClass::setFileData(void)
{
	std::experimental::filesystem::path	filepath;
	std::string							command;
	if(this->apply==true)
		{
//dir
			filepath=this->dirEdit->LFSTK_getBuffer();

			if(this->dialogType==FILEDIALOG)
				this->currentDir=filepath.parent_path();
			else
				this->currentDir=filepath.string();
//file
			this->currentFile=filepath.filename();
//path
			this->currentPath=this->dirEdit->LFSTK_getBuffer();

			command="sed -i '/"+this->recentsName+"=/d' '"+this->wc->app->configDir+"/dialoglast.rc';echo '"+this->recentsName+"="+this->currentDir+"'|cat - '"+this->wc->app->configDir+"/dialoglast.rc'|sort -uo '"+this->wc->app->configDir+"/dialoglast.rc'";//TODO//
			system(command.c_str());
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

bool LFSTK_fileDialogClass::select(void *object,void* userdata)
{
	std::string				realpath;
	LFSTK_fileDialogClass	*fd=static_cast<LFSTK_fileDialogClass*>(userdata);
	LFSTK_listGadgetClass	*list=static_cast<LFSTK_listGadgetClass*>(object);

	realpath=fd->wc->globalLib->LFSTK_getRealPath(fd->LFSTK_getCurrentDir()+"/"+list->LFSTK_getSelectedLabel());

	if(fd->isADir(realpath.c_str())==false)
		fd->dirEdit->LFSTK_setBuffer(realpath);
	fd->setPreviewData(true);

	if(list->isDoubleClick==true)
		{											
			fd->dirEdit->LFSTK_setBuffer(realpath.c_str());
			if(fd->isADir(fd->dirEdit->LFSTK_getCStr())==true)
				{
					fd->apply=false;
					fd->LFSTK_setWorkingDir(fd->dirEdit->LFSTK_getCStr());
				}
			else
				{
					fd->apply=true;
					fd->setFileData();
					fd->mainLoop=false;
				}
		}
	return(true);
}

