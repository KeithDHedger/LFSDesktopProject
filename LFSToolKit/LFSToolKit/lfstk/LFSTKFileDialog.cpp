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
	this->freeDirList();
	this->freeFileList();
	delete this->dialog;
	delete this->fc;
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

//static int LFSTK_fileDialogClass::getFileCnt(const char *fpath,const struct stat *sb,int tflag,struct FTW *ftwbuf)
//{
//	if(tflag==FTW_F)
//		{
//			if(strcasecmp(fpath+ftwbuf->base,chkfname)==0)
//				{
//					initEditor();
//					setTempEdFile(fpath);
//					page->filePath=strdup(fpath);
//					oneLiner(true,"cp %s %s/%s",page->filePath,tmpEdDir,tmpEdFile);
//					openTheFile(tmpEdFilePath,hilite);
//					page->topLine=0;
//					page->currentLine=0;
//					currentX=minX;
//					currentY=minY;
//					page->lineXCurs=0;
//				}
//		}
//	return(0);
//}

//nftw(includepath,openInclude,20,FTW_PHYS);
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

unsigned LFSTK_fileDialogClass::getFileListcnt(const char *dir)
{
//	nftw(dir,openInclude,20,FTW_PHYS);
	unsigned		cnt=0;
	DIR				*dirp=NULL;
	struct dirent	*entry=NULL;
	struct stat		statbuf;
	char			*cwd=NULL;
	if(dir==NULL)
		return(0);

	cwd=get_current_dir_name();
	chdir(dir);
	dirp=opendir(dir);

	if(dirp!=NULL)
		{
			while((entry=readdir(dirp)) != NULL)
				{
					if(entry->d_type != DT_DIR)
						{
							if(entry->d_type==DT_LNK)
								{
									stat(entry->d_name,&statbuf);
									if(((statbuf.st_mode & S_IFMT) !=S_IFDIR))
										{
											cnt++;
										}
								}
							else
								{
									cnt++;
								}
						}
				}
			closedir(dirp);
		}
	if(cwd!=NULL)
		{
			chdir(cwd);
			free(cwd);
		}
	return(cnt);
}

#if 0
void LFSTK_fileDialogClass::getFileList(void)
{
	unsigned	filecnt=0;
	unsigned	filecnt1=0;
	FILE		*fp;
	char		*command;
	char		*out=NULL;
	char		line[1024];
	unsigned	cnt=0;
	char		*imagepath;
	
	this->freeFileList();

	asprintf(&command,"(cd \"%s\" ;find  -maxdepth 1 -mindepth 1 -follow ! -type d -print0 | xargs -0 -n 1 basename 2>/dev/null|sort|wc -l)",this->currentDir);
	out=this->wc->globalLib->LFSTK_oneLiner("%s",command);
fprintf(stderr,"command=%s\n",command);
	if(out==NULL)
		filecnt=0;
	else
		{
			filecnt=atoi(out);
			free(out);
		}
	free(command);
//fprintf(stderr,">>filecount=%i\n",filecnt);
//filecnt1=this->getFileListcnt(this->currentDir);
//fprintf(stderr,"<<filecount=%i\n",filecnt1);
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
							if((strcasecmp(&line[strlen(line)-4],".jpg")==0) || (strcasecmp(&line[strlen(line)-4],".png")==0))
								asprintf(&imagepath,"%s/%s",this->currentDir,line);
							else
								asprintf(&imagepath,"%s",(char*)this->fileImage);
							this->fileImageList[cnt]=strdup(imagepath);
							free(imagepath);
							cnt++;
						}
					pclose(fp);
				}
			free(command);
		}
	this->fileListCnt=filecnt;
}
#else
void LFSTK_fileDialogClass::getFileList(void)
{
	char	*imagepath;

	this->fc->setDepth(1,1);
	this->fc->setFindType(FTW_F);
	this->fc->setFollowLinks(true);
	this->fc->setIncludeHidden(false);
	this->fc->findFiles(this->currentDir);
	this->fc->sortData();

	this->fileListCnt=this->fc->getDataCount();
	this->fileList=new char*[this->fileListCnt];
	this->fileImageList=new char*[this->fileListCnt];

	for(int j=0;j<this->fileListCnt;j++)
		{
			this->fileList[j]=strdup(this->fc->data.at(j).c_str());
			if((strcasecmp(&this->fileList[j][strlen(this->fileList[j])-4],".jpg")==0) || (strcasecmp(&this->fileList[j][strlen(this->fileList[j])-4],".png")==0))
				asprintf(&imagepath,"%s",(char*)this->imageImage);
//				asprintf(&imagepath,"%s/%s",this->currentDir,this->fileList[j]);
			else
				asprintf(&imagepath,"%s",(char*)this->fileImage);
			this->fileImageList[j]=strdup(imagepath);
			free(imagepath);
		}
return;
#if 0
	unsigned	filecnt=0;
	unsigned	filecnt1=0;
	FILE		*fp;
	char		*command;
	char		*out=NULL;
	char		line[1024];
	unsigned	cnt=0;
	char		*imagepath;
	
	this->freeFileList();

	asprintf(&command,"(cd \"%s\" ;find  -maxdepth 1 -mindepth 1 -follow ! -type d -print0 | xargs -0 -n 1 basename 2>/dev/null|sort|wc -l)",this->currentDir);
	out=this->wc->globalLib->LFSTK_oneLiner("%s",command);
fprintf(stderr,"command=%s\n",command);
	if(out==NULL)
		filecnt=0;
	else
		{
			filecnt=atoi(out);
			free(out);
		}
	free(command);
//fprintf(stderr,">>filecount=%i\n",filecnt);
//filecnt1=this->getFileListcnt(this->currentDir);
//fprintf(stderr,"<<filecount=%i\n",filecnt1);
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
							if((strcasecmp(&line[strlen(line)-4],".jpg")==0) || (strcasecmp(&line[strlen(line)-4],".png")==0))
								asprintf(&imagepath,"%s/%s",this->currentDir,line);
							else
								asprintf(&imagepath,"%s",(char*)this->fileImage);
							this->fileImageList[cnt]=strdup(imagepath);
							free(imagepath);
							cnt++;
						}
					pclose(fp);
				}
			free(command);
		}
	this->fileListCnt=filecnt;
#endif
}
#endif

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
	this->dirListCnt=cnt;
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
	if(this->dialogType==FILEDIALOG)
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
//	if(this->currentDir==NULL)
//		printf("this->apply=%i\n",this->apply);
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
//	if(this->currentFile==NULL)
//		this->apply=false;
//		printf("this->apply=%i\n",this->apply);
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
	int					hite=DIALOGHITE;
	LFSTK_labelClass	*spacer;
	int					midprev;
	int					yoffset=BORDER+PREVIEWWIDTH;

	this->wc=parentwc;
	this->dialog=NULL;
	this->dirList=NULL;
	this->dirImageList=NULL;
	this->fileList=NULL;
	this->fileImageList=NULL;
	this->fileListCnt=0;
	this->currentFile=NULL;
	this->currentPath=NULL;
	this->dirListGadget=NULL;
	this->dirListCnt=0;
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
	//int dirlisthite=GADGETHITE*FDIRHITE;
	int dirlistwid=DIALOGWIDTH-(FGAP*2)-FNAVBUTTONWID;
	int dwidth=DIALOGWIDTH+PREVIEWWIDTH;

	if(this->dialogType==FOLDERDIALOG)
		{
			this->dirlisthite=GADGETHITE*FDIRHITE;
			hite=dirlisthite+(FGAP*7)+(GADGETHITE*2);
			dwidth=DIALOGWIDTH;
		}
	else
		{
			this->dirlisthite=GADGETHITE*(FDIRHITE/2);
			hite=dirlisthite+(FGAP*6)+GADGETHITE+(FFILEHITE*GADGETHITE)+FGAP+GADGETHITE+FGAP;
		}

	this->dialog=new LFSTK_windowClass(0,0,dwidth,hite,label,false,true,true);

//find files
	this->fc=new FindClass;
	//this->dialog->closeDisplayOnExit=true;
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

	this->getDirList();
	this->getFileList();

	if(this->dialogType==FOLDERDIALOG)
		{
			this->dirListGadget=new LFSTK_listGadgetClass(this->dialog,"",FGAP,FGAP,dirlistwid,dirlisthite,NorthWestGravity,NULL,0);
			this->dirEdit=new LFSTK_lineEditClass(this->dialog,this->currentDir,FGAP,dirlisthite+(FGAP*2),dirlistwid,GADGETHITE,NorthWestGravity);
			this->dirListGadget->LFSTK_setImageList(this->dirImageList,this->dirListCnt);
			this->dirListGadget->LFSTK_setList(this->dirList,this->dirListCnt);

			spacer=new LFSTK_labelClass(this->dialog,"--",0,dirlisthite+(FGAP*3)+GADGETHITE,DIALOGWIDTH,8,NorthWestGravity);
			
			this->buttonCancel=new LFSTK_buttonClass(this->dialog,"Cancel",BORDER,dirlisthite+(FGAP*6)+GADGETHITE,GADGETWIDTH,GADGETHITE,SouthWestGravity);
			this->buttonApply=new LFSTK_buttonClass(this->dialog,"Apply",DIALOGWIDTH-BORDER-GADGETWIDTH,dirlisthite+(FGAP*6)+GADGETHITE,GADGETWIDTH,GADGETHITE,SouthEastGravity);
			this->dirlisthite=dirlisthite+(FGAP*7)+GADGETHITE*2;
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

			spacer=new LFSTK_labelClass(this->dialog,"--",0,dirlisthite+(FGAP*3)+GADGETHITE+(FFILEHITE*GADGETHITE)+FGAP,DIALOGWIDTH+PREVIEWWIDTH,8,NorthWestGravity);
			
			this->buttonCancel=new LFSTK_buttonClass(this->dialog,"Cancel",BORDER,dirlisthite+(FGAP*6)+GADGETHITE+(FFILEHITE*GADGETHITE)+FGAP,GADGETWIDTH,GADGETHITE,SouthWestGravity);
			this->buttonApply=new LFSTK_buttonClass(this->dialog,"Apply",DIALOGWIDTH-BORDER-GADGETWIDTH+PREVIEWWIDTH,dirlisthite+(FGAP*6)+GADGETHITE+(FFILEHITE*GADGETHITE)+FGAP,GADGETWIDTH,GADGETHITE,NorthWestGravity);

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
			this->dirlisthite=dirlisthite+(FGAP*8)+GADGETHITE*2+(FFILEHITE*GADGETHITE);
		}
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
//folders
	this->getDirList();
	this->dirListGadget->LFSTK_setImageList(this->dirImageList,this->dirListCnt);
	this->dirListGadget->LFSTK_setList(this->dirList,this->dirListCnt);
	this->dirEdit->LFSTK_setBuffer(this->currentDir);
//files
	if(this->dialogType==FILEDIALOG)
		{
			this->getFileList();
			this->fileListGadget->LFSTK_setImageList(this->fileImageList,this->fileListCnt);
			this->fileListGadget->LFSTK_setList(this->fileList,this->fileListCnt);
			this->freeFileList();
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
			if(dir!=NULL)
				this->LFSTK_setWorkingDir(dir);
			else
				{
					this->LFSTK_getLastFolder();
					//DEBUGFUNC("this->LFSTK_getLastFolder();=%s",this->currentDir);
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
				//printf(">>>find %s/\"%s\"/*/mime* -iname \"*%s.*\" 2>/dev/null|head -n1 2>/dev/null<<\n",iconfolders[j],iconthemes[k],mimetype);
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
		XResizeWindow(this->dialog->display,this->dialog->window,w,this->dirlisthite);

//	this->globalLib->LFSTK_setCairoSurface(this->display,this->window,this->visual,&this->sfc,&this->cr,w,this->dirlisthite);
//	this->LFSTK_clearWindow();
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
//	int dwidth=DIALOGWIDTH+PREVIEWWIDTH;

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

										if(this->wc->globalLib->LFSTK_pointInRect(&pt,&geomdir)==true)
											this->doOpenDir();

										if(this->wc->globalLib->LFSTK_pointInRect(&pt,&geomfile)==true)
											{
												this->apply=true;
												if(this->currentFile!=NULL)
													free(this->currentFile);
												asprintf(&this->currentFile,"%s",this->fileListGadget->LFSTK_getListString(this->fileListGadget->LFSTK_getCurrentListItem()));
												this->mainLoop=false;
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
								this->dirListGadget->LFSTK_getGlobalGeom(&geomdir);
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
