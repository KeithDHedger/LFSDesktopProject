/*
 *
 * ©K. D. Hedger. Sun 22 Nov 18:38:48 GMT 2020 keithdhedger@gmail.com

 * This file (lfsmimeeditor.cpp) is part of LFSApplications.

 * LFSApplications is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSApplications is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSApplications.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include <lfstk/LFSTKGlobals.h>

#include <fcntl.h>
#include <unistd.h>

#define LISTHITE		GADGETHITE * 16
#define MIMETYPESLABEL	"Mime Type"
#define APPLABEL		"Default Application"

LFSTK_applicationClass	*apc=NULL;
LFSTK_windowClass		*wc=NULL;
LFSTK_labelClass		*label=NULL;
LFSTK_labelClass		*personal=NULL;
LFSTK_labelClass		*copyrite=NULL;
LFSTK_buttonClass		*seperator=NULL;
LFSTK_buttonClass		*cancel=NULL;
LFSTK_buttonClass		*insert=NULL;
LFSTK_buttonClass		*delentry=NULL;
LFSTK_buttonClass		*apply=NULL;
LFSTK_listGadgetClass	*mimeList=NULL;
LFSTK_listGadgetClass	*appsList=NULL;
LFSTK_lineEditClass		*editLine=NULL;
LFSTK_lineEditClass		*appLine=NULL;

char					*mimeTypesFile=NULL;
char					*lastBitPath;
char					*mimeTypesPath;
char					*appsPath;
char					*tmpOutFile;
infoDataStruct			**labelLst=NULL;
char					*workDir;

bool doQuit(void *p,void* ud)
{
	apc->exitValue=0;
	apc->mainLoop=false;
	return(false);
}

void setMimeTypesList(char *filepath)
{
	FILE	*file=NULL;
	char	*buffer;
	int		cnt=0;
	size_t	linelen=0;
	ssize_t	read=0;
	int		linecnt=0;
	char	*lines=NULL;
	infoDataStruct listit;

	mimeList->LFSTK_freeList();
	if(filepath!=NULL)
		{
			file=fopen(mimeTypesPath,"r");
			if(file!=NULL)
				{
					while(!feof(file))
						{
							buffer=NULL;
							linelen=0;
							read=getline(&buffer,&linelen,file);
							if(read>0)
								{
									buffer[strlen(buffer)-1]=0;
									if(strlen(buffer)>0)
										{
											listit.label=strdup(buffer);
											listit.imageType=NOTHUMB;
											if(listit.label[listit.label.length()-1]==';')
												listit.label[listit.label.length()-1]=0;
											char	 *ptr=strchr((char*)listit.label.c_str(),'=');
											*ptr=0;
											ptr++;
											listit.userData=ptr;
											mimeList->LFSTK_appendToList(listit);
											//thelist.push_back(listit);
										}
								}
							free(buffer);
						}
					mimeList->LFSTK_updateList();
					fclose(file);
				}
		}
}

void splitFile(char *path)
{
	char	*lastbit;
	char	*firstbit;
	char	*appslist;

	asprintf(&lastbit,"sed -n -e '/\\[Added Associations]/,$p' \"%s\" > \"%s\"",path,lastBitPath);
	system(lastbit);
	asprintf(&firstbit,"sed '/\\[Added Associations]/Q;s/\\[Default Applications]//' \"%s\"|sort -u  > \"%s\"",path,mimeTypesPath);
	system(firstbit);
	asprintf(&appslist,"find /usr/share/applications ~/.local/share/applications -iname \"*.desktop\"|sed 's@.*/@@;s@\\.desktop$@@'|sort -u > \"%s\"",appsPath);
	system(appslist);
	free(lastbit);
	free(firstbit);
	free(appslist);
}

void reWriteMimeFile(void)
{
	char	*lab;
	FILE	*file=NULL;
	char	*command;
	char	*ptr;

	file=fopen(tmpOutFile,"w");
	if(file!=NULL)
		{
			fprintf(file,"[Default Applications]\n");
			for(int j=0;j<mimeList->listCnt;j++)
				{
					if(j!=mimeList->currentItem)
						{
							ptr=(char*)mimeList->listDataArray->at(j).label.c_str();
							ptr+=mimeList->listDataArray->at(j).label.length();
							ptr++;
							fprintf(file,"%s=%s;\n",mimeList->listDataArray->at(j).label.c_str(),ptr);
						}
				}

			fclose(file);
			asprintf(&command,"cat \"%s\" \"%s\" > \"%s\"",tmpOutFile,lastBitPath,mimeTypesFile);
			system(command);
			free(command);
			asprintf(&command,"awk -i inplace '/^$/ {print; next} {if ($1 in a) next; a[$1]=$0; print}' \"%s\"",mimeTypesFile);
			system(command);
			free(command);
		}
	splitFile(mimeTypesFile);
	setMimeTypesList(mimeTypesFile);
}

bool doUpdate(void *p,void* ud)
{
	char	*lab;
	FILE	*file=NULL;
	char	*command;
	char	*ptr;

	file=fopen(tmpOutFile,"w");
	if(file!=NULL)
		{
			fprintf(file,"[Default Applications]\n");
			for(int j=0;j<mimeList->listDataArray->size();j++)
				{
					if(j==mimeList->currentItem)
						{
							fprintf(file,"%s=%s.desktop;\n",mimeList->listDataArray->at(mimeList->currentItem).label,appLine->LFSTK_getCStr());
						}
					else
						{
							ptr=(char*)mimeList->listDataArray->at(j).label.c_str();
							ptr+=mimeList->listDataArray->at(j).label.length();
							ptr++;
							fprintf(file,"%s=%s;\n",mimeList->listDataArray->at(j).label.c_str(),ptr);
						}
				}
			fprintf(file,"%s=%s.desktop;\n",editLine->LFSTK_getCStr(),appLine->LFSTK_getCStr());
			fclose(file);
			asprintf(&command,"cat \"%s\" \"%s\" > \"%s\"",tmpOutFile,lastBitPath,mimeTypesFile);
			system(command);
			free(command);
			asprintf(&command,"awk -i inplace '/^$/ {print; next} {if ($1 in a) next; a[$1]=$0; print}' \"%s\"",mimeTypesFile);
			system(command);
			free(command);
		}
	splitFile(mimeTypesFile);
	setMimeTypesList(mimeTypesFile);

	return(true);
}

bool doInsert(void *p,void* ud)
{
	char		*command;
	const char	*data=mimeList->listDataArray->at(mimeList->currentItem).label.c_str();

	asprintf(&command,"sed -i 's@\\(%s.*\\)@\\1\\n%s=Custom.desktop@' \"%s\"",data,data,mimeTypesFile);
	system(command);
	splitFile(mimeTypesFile);
	setMimeTypesList(mimeTypesFile);
	free(command);
	return(true);
}

bool doDelete(void *p,void* ud)
{
	char	*command;
	int		holdnum=mimeList->currentItem;

	if(holdnum>=mimeList->listCnt)
		return(true);
	if(mimeList->LFSTK_getSelectedLabel()[0]=='[')
		return(true);

	asprintf(&command,"sed -i '%id' \"%s\"",mimeList->LFSTK_getCurrentListItem()+2,mimeTypesFile);
	system(command);
	reWriteMimeFile();
	fflush(NULL);
	splitFile(mimeTypesFile);
	setMimeTypesList(mimeTypesFile);

	mimeList->currentItem=holdnum;
	free(command);
	return(true);
}

bool doApply(void *p,void* ud)
{
	char		*command;
	const char	*data=mimeList->listDataArray->at(mimeList->currentItem).label.c_str();

	asprintf(&command,"cp \"%s\" \"%s/.config\"",mimeTypesFile,getenv("HOME"));
	system(command);
	free(command);
	splitFile(mimeTypesFile);
	setMimeTypesList(mimeTypesFile);
	return(true);
}

bool selectMime(void *object,void* ud)
{
	char	*app;
	LFSTK_listGadgetClass	*list=static_cast<LFSTK_listGadgetClass*>(object);

	if(list->LFSTK_getSelectedLabel()[0]=='[')
		return(true);

	asprintf(&app,"%s",list->listDataArray->at(list->LFSTK_getCurrentListItem()).userData);
	*(strrchr(app,'.'))=0;
	editLine->LFSTK_setBuffer(list->LFSTK_getSelectedLabel());
	appLine->LFSTK_setBuffer((const char*)app);
	appsList->LFSTK_findByLabel(app);
	free(app);
	return(true);
}

bool selectApp(void *object,void* ud)
{
	LFSTK_listGadgetClass	*list=static_cast<LFSTK_listGadgetClass*>(object);

	if(list->LFSTK_getSelectedLabel()[0]=='[')
		return(true);
	appLine->LFSTK_setBuffer(list->LFSTK_getSelectedLabel());
	doUpdate(NULL,NULL);
	return(true);
}

bool returnKeyPressed(void *p,void* ud)
{
	doUpdate(NULL,NULL);
	return(true);
}


int main(int argc, char **argv)
{
	XEvent				event;
	int					sy=BORDER;
	char					*command;
	char					*tempfolder=(char*)malloc(256);
	LFSTK_labelClass		*infolabel;

	sprintf(tempfolder,"/tmp/lfsmimedir-XXXXXX");
	workDir=mkdtemp(tempfolder);

	apc=new LFSTK_applicationClass();
	apc->LFSTK_addWindow(NULL,"Mime Editor");
	wc=apc->mainWindow;

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,2*DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,2*DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

	infolabel=new LFSTK_labelClass(wc,MIMETYPESLABEL,BORDER,sy,GADGETWIDTH*4,GADGETHITE,LEFT);
	infolabel->LFSTK_setCairoFontDataParts("sB",16);

	infolabel=new LFSTK_labelClass(wc,APPLABEL,BORDER+DIALOGWIDTH,sy,GADGETWIDTH*4,GADGETHITE,LEFT);
	infolabel->LFSTK_setCairoFontDataParts("sB",16);
	sy+=YSPACING;

	wc->globalLib->LFSTK_oneLiner("cp %s/.config/mimeapps.list %s",getenv("HOME"),workDir);//TODO//

	asprintf(&mimeTypesFile,"%s/mimeapps.list",workDir);
	if(access(mimeTypesFile,F_OK)!=0)
		{
			asprintf(&command,"echo -e \"[Default Applications]\n[Added Associations]\" > \"%s\"",mimeTypesFile);
			system(command);
			free(command);
		}

	asprintf(&lastBitPath,"%s/lfslastbit",workDir);
	asprintf(&mimeTypesPath,"%s/lfsmimetypeslist",workDir);
	asprintf(&appsPath,"%s/lfsappslist",workDir);
	asprintf(&tmpOutFile,"%s/tmpoutfile",workDir);

//split file
	splitFile(mimeTypesFile);

//mime type list
	mimeList=new LFSTK_listGadgetClass(wc,"",BORDER,sy,DIALOGWIDTH-(BORDER*2)-LGAP,LISTHITE);
	setMimeTypesList(mimeTypesFile);
	mimeList->LFSTK_setMouseCallBack(NULL,selectMime,NULL);
	
//apps list
	appsList=new LFSTK_listGadgetClass(wc,"",BORDER+DIALOGWIDTH,sy,DIALOGWIDTH-(BORDER*2)-LGAP,LISTHITE);
	appsList->LFSTK_setListFromFile(appsPath,false);
	appsList->LFSTK_setMouseCallBack(NULL,selectApp,NULL);
	sy+=LISTHITE+8;

//command
	editLine=new LFSTK_lineEditClass(wc,"",BORDER,sy,DIALOGWIDTH-(BORDER*2),GADGETHITE);
	editLine->LFSTK_setMouseCallBack(returnKeyPressed,NULL,NULL);
//app to use
	appLine=new LFSTK_lineEditClass(wc,"",BORDER+DIALOGWIDTH,sy,DIALOGWIDTH-(BORDER*2),GADGETHITE);
	appLine->LFSTK_setMouseCallBack(returnKeyPressed,NULL,NULL);
	sy+=YSPACING;

//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,2*DIALOGWIDTH,GADGETHITE);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=YSPACING;

	insert=new LFSTK_buttonClass(wc,"Duplicate",BORDER,sy,GADGETWIDTH,GADGETHITE);
	insert->LFSTK_setMouseCallBack(NULL,doInsert,NULL);

	delentry=new LFSTK_buttonClass(wc,"Remove",2*BORDER+GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE);
	delentry->LFSTK_setMouseCallBack(NULL,doDelete,NULL);

//aply changes
	apply=new LFSTK_buttonClass(wc,"Apply",2*DIALOGWIDTH-2*GADGETWIDTH-2*BORDER,sy,GADGETWIDTH,GADGETHITE);
	apply->LFSTK_setMouseCallBack(NULL,doApply,NULL);

//cancel/quit
	cancel=new LFSTK_buttonClass(wc,"Quit",2*DIALOGWIDTH-GADGETWIDTH-BORDER,sy,GADGETWIDTH,GADGETHITE);
	cancel->LFSTK_setMouseCallBack(NULL,doQuit,NULL);

	sy+=YSPACING;
	sy+=(YSPACING/2);

	wc->LFSTK_resizeWindow(DIALOGWIDTH*2,sy,true);
	wc->LFSTK_showWindow();

	int retval=apc->LFSTK_runApp();

	wc->globalLib->LFSTK_oneLiner("rm -r %s",workDir);//TODO//
	delete apc;
	free(tmpOutFile);
	free(lastBitPath);
	free(appsPath);
	free(mimeTypesPath);
	free(mimeTypesFile);
	free(tempfolder);
	return(retval);
}