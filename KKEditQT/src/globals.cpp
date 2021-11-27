/*
 *
 * ©K. D. Hedger. Tue  2 Nov 15:58:58 GMT 2021 keithdhedger@gmail.com

 * This file (globals.cpp) is part of KKEdit.

 * KKEdit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * KKEdit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with KKEdit.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "globals.h"

QAction*		menuItemOpen=NULL;

GList*	newBookMarksList=NULL;
char*			highlightColour;
bool			showBMBar;
int				bmMarkNumber=0;

//app stuff
bool			busyFlag=false;
bool	sessionBusy=false;
bool			autoSelected=false;

//find replaceAll
GSList*			findList=NULL;
GSList*			replaceList=NULL;
unsigned int	maxFRHistory=5;

int				currentPage=0;
//nag
bool			nagScreen;

//docview size and position
int				docWindowWidth;
int				docWindowHeight;
int				docWindowX=-1;
int				docWindowY=-1;

//prefs





bool			noSyntax;
bool			singleUse;





bool			onExitSaveSession;
bool			restoreBookmarks;
char*			styleName=NULL;
bool			noDuplicates;
bool			noWarnings;
bool			readLinkFirst;

bool			autoCheck;
unsigned int	autoShowMinChars;


unsigned int	maxBMChars;


char*			browserCommand=strdup("firefox");


toolStruct*		selectedToolFromDrop=NULL;

GList*			toolsList=NULL;

int				currentTabNumber;
int 			untitledNumber=1;

QAction			*gotoLineButton=NULL;
QAction			*findApiButton=NULL;
QAction			*findQtApiButton=NULL;
QAction			*findFuncDefButton=NULL;
QAction			*liveSearchButton=NULL;




char*			functionSearchText=NULL;
char*			thePage=NULL;
char*			htmlFile=NULL;
char*			htmlURI=NULL;
char*			tmpFolderName=NULL;


//spellcheck

char*			badWord=NULL;
char*			goodWord=NULL;
#ifdef _ASPELL_
AspellConfig*	aspellConfig;
AspellSpeller*	spellChecker=0;
#endif

const char*		localeLang;

HistoryClass*	history;
StringSlice*	globalSlice=NULL;

unsigned int	shortCuts[NUMSHORTCUTS][2]=
{
	{121,0},
	{63,1},
	{107,2},
	{72,3},
	{104,4},
	{68,5},
	{108,6},
	{109,7},
	{77,8},
	{93,9},
	{91,10},
	{39,11},
	{64,12},
	{32,0}
};

char*			shortCutStrings[NUMSHORTCUTS]={NULL,};

args			keybindings_rc[]=
	{
//string
		{"deleteline",TYPESTRING,&shortCutStrings[0]},
		{"deletetoeol",TYPESTRING,&shortCutStrings[1]},
		{"deletetosol",TYPESTRING,&shortCutStrings[2]},
		{"selectword",TYPESTRING,&shortCutStrings[3]},
		{"deleteword",TYPESTRING,&shortCutStrings[4]},
		{"duplicateline",TYPESTRING,&shortCutStrings[5]},
		{"selectline",TYPESTRING,&shortCutStrings[6]},
		{"lineup",TYPESTRING,&shortCutStrings[7]},
		{"linedown",TYPESTRING,&shortCutStrings[8]},
		{"selecttoeol",TYPESTRING,&shortCutStrings[9]},
		{"selecttosol",TYPESTRING,&shortCutStrings[10]},
		{"selectionup",TYPESTRING,&shortCutStrings[11]},
		{"selectiondown",TYPESTRING,&shortCutStrings[12]},
		{"complete",TYPESTRING,&shortCutStrings[13]},
		{NULL,0,NULL}
	};

//121 0 ^y Delete Current Line
//63 1 ^? Delete To End Of Line
//107 2 ^k Delete To Beginning Of Line
//72 3 ^H Select Word Under Cursor
//104 4 ^h Delete Word Under Cursor
//68 5 ^D Duplicate Current Line
//108 6 ^l Select Current Line
//109 7 ^m Move Current Line Up
//77 8 ^M Move Current Line Down
//93 9 ^] Select From Cursor To End Of Line
//91 10 ^[ Select From Beginning Of Line To Cursor
//39 11 ^' Move Selection Up
//64 12 ^@ Move Selection Down

PluginClass*	globalPlugins=NULL;

//save and load var lists
char*			windowAllocData=NULL;
char*			docWindowAllocData=NULL;

int				intermarg=0;
int				flagsarg=0;
int				inpopup=0;
int				alwayspopup=0;
int				clearview=0;
char*			commandarg=NULL;
char*			commentarg=NULL;
char*			menuname=NULL;
int				rootarg=0;
int				keycode=0;
int				usebar=0;

//status bar message
char*			statusMessage=NULL;

//truncate tabname with elipses
QString truncateWithElipses(const QString str,unsigned int maxlen)
{
	QString newlabel;
	if(str.length()>maxlen)
		newlabel=str.left((maxlen-3)/2)+"..."+str.right((maxlen-3)/2);
	else
		newlabel=str;

	return(newlabel);
}

char* truncateWithElipses(char* str,unsigned int maxlen)
{
	char*	retstr;
	char	*front,*back;
	int		sides;

	if(g_utf8_validate(str,-1,NULL)==true)
		{
			if(g_utf8_strlen(str,-1)>maxlen)
				{
					sides=(maxlen-5)/2;
					front=g_utf8_substring(str,0,sides);
					back=g_utf8_substring(str,g_utf8_strlen(str,-1)-sides,g_utf8_strlen(str,-1));
					asprintf(&retstr,"%s ... %s",front,back);
					debugFree(&front,"truncateWithElipses front");
					debugFree(&back,"truncateWithElipses back");
				}
			else
				retstr=strdup(str);
		}
	else
		retstr=strdup(str);

	return(retstr);
}

void plugRunFunction(gpointer data,gpointer funcname)
{
#ifndef _USEQT5_
	globalPlugins->runPlugFunction((moduleData*)data,(const char*)funcname);
#endif
}
//
//DocumentClass* kkedit->getDocumentForTab(int pagenum)
//{
//fprintf(stderr,">>>>>>>>>>>>>>>>>>\n");
//	if(pagenum==-1)
//		return((DocumentClass*)qobject_cast<QTabWidget*>(kkedit->mainNotebook)->currentWidget());
//	else
//		return((DocumentClass*)qobject_cast<QTabWidget*>(kkedit->mainNotebook)->widget(pagenum));
//}

void getMimeType(char* filepath,void* ptr)
{
}

void setLanguage(pageStruct* page)
{
}

void runCommand(char* commandtorun,void* ptr,bool interm,int flags,int useroot,char* title)
{
}

functionData* getFunctionByName(const char* name,bool recurse,bool casesensitive)
{
	DocumentClass		*document=kkedit->getDocumentForTab(-1);
	pageStruct			*page;
	char				*functions=NULL;
	QString				str;
	char				*lineptr;
	int					gotmatch=-1;
	char				function[1024];
	functionData*		fdata;
	int					loop;
	int					startpage;
	int					holdlistfunction=kkedit->prefsFunctionMenuLayout;
	StringSlice			slice;
	bool				whileflag=true;
	bool				checkthispage=true;
	int					maxpage;;
	char				funcname[256];
	char				filepath[1024];
	int					linenumber;
	QStringList			strlist;
	Qt::CaseSensitivity	cs;

	if(document==NULL)
		return(NULL);

	if(casesensitive==true)
		cs=Qt::CaseSensitive;
	else
		cs=Qt::CaseInsensitive;

	loop=qobject_cast<QTabWidget*>(kkedit->mainNotebook)->currentIndex();
	startpage=loop;
	checkthispage=true;
	maxpage=qobject_cast<QTabWidget*>(kkedit->mainNotebook)->count();

	while(whileflag==true)
		{
			document=kkedit->getDocumentForTab(loop);
			if(document->filePath!=NULL)
				{
					kkedit->prefsFunctionMenuLayout=0;
					getRecursiveTagList((char*)document->filePath.toStdString().c_str(),&functions);//TODO//
					kkedit->prefsFunctionMenuLayout=holdlistfunction;
					if(functions!=NULL)
						{
							str=functions;
							strlist=str.split("\n",QString::SkipEmptyParts);
							gotmatch=-1;
							for(int i=0;i<strlist.size();i++)
								{
									if(strlist.at(i).startsWith(name,cs))
										{
											gotmatch=0;
											lineptr=strdup(strlist.at(i).toLocal8Bit().constData());
											break;
										}
								}

							debugFree(&functions,"functions getFunctionByName");
							if(gotmatch==0)
								{
									fdata=(functionData*)malloc(sizeof(functionData));
									sscanf (lineptr,"%" VALIDFUNCTIONCHARS "s",function);
									fdata->name=strdup(function);
									sscanf (lineptr,"%*s %" VALIDFUNCTIONCHARS "s",function);
									fdata->type=strdup(function);
									sscanf (lineptr,"%*s %*s %i",&fdata->line);
									sscanf (lineptr,"%*s %*s %*i %" VALIDFILENAMECHARS "s",function);
									fdata->file=strdup(function);
									sscanf (lineptr,"%*s %*s %*i %*s %[^\n]s",function);
									fdata->define=strdup(function);
									fdata->intab=loop;
									debugFree(&lineptr,"lineptr getFunctionByName");
									return(fdata);
								}
						}
				}

			if(checkthispage==true)
				{
					loop=-1;
					checkthispage=false;
				}

			loop++;
			if(loop==startpage)
				loop++;
			if(loop==maxpage)
				whileflag=false;

		}

//not in any open files
//check ./ from all files
//dont do this from popup for speed reasons
//	if(recurse==true)
		{
			if(document->getDirPath()!=NULL)
				{
					getRecursiveTagListFileName((char*)document->getDirPath().toStdString().c_str(),&functions);//TODO//
					if(functions!=NULL)
						{
							str=functions;
							strlist=str.split("\n",QString::SkipEmptyParts);
							gotmatch=-1;
							for(int i=0;i<strlist.size();i++)
								{
									if(strlist.at(i).startsWith(name,cs))
										{
											gotmatch=0;
											lineptr=strdup(strlist.at(i).toLocal8Bit().constData());
											break;
										}
								}

							if(gotmatch!=-1)
								{
									sscanf (lineptr, "%s\t%s\t%i",funcname,filepath,&linenumber);
									fdata=(functionData*)malloc(sizeof(functionData));
									fdata->name=strdup(funcname);
									fdata->file=strdup(filepath);
									fdata->line=linenumber+1;
									fdata->type=NULL;
									fdata->define=NULL;
									fdata->intab=-1;
									return(fdata);
								}
						}
					
				}
		}
	return(NULL);			
}

void destroyData(functionData* fdata)
{
	if(fdata!=NULL)
		{
			if(fdata->name!=NULL)
				debugFree(&fdata->name,"destroyData name");
			if(fdata->type!=NULL)
				debugFree(&fdata->type,"destroyData type");
			if(fdata->file!=NULL)
				debugFree(&fdata->file,"destroyData file");
			if(fdata->define!=NULL)
				debugFree(&fdata->define,"destroyData define");
			debugFree((char**)&fdata,"destroyData fdata");
		}
}

void getRecursiveTagListFileName(char* filepath,void* ptr)
{
	FILE*		fp;
	char		line[1024];
	GString*	str=g_string_new(NULL);
	char*		command;

	if(filepath==NULL)
		return;

	asprintf(&command,"find \"%s\" -maxdepth %i|ctags -L - --excmd=number --format=1 -f -",filepath,kkedit->prefsDepth);
	fp=popen(command, "r");
	while(fgets(line,1024,fp))
		{
			g_string_append_printf(str,"%s",line);
		}
	pclose(fp);

	*((char**)ptr)=str->str;
	g_string_free(str,false);

	debugFree(&command,"getRecursiveTagListFileName command");
}

void getRecursiveTagList(char* filepath,void* ptr)
{

	FILE*		fp;
	char		line[2048];
	GString*	str=g_string_new(NULL);
	char*		command;
	char*		newstr=NULL;
	char*		sort=NULL;

	if(filepath==NULL)
		return;

	switch (kkedit->prefsFunctionMenuLayout)
		{
		case 0:
			asprintf(&sort,"sort -k 2rb,2rb -k 1b,1b");
			break;
		case 1:
			asprintf(&sort,"sort -k 2rb,2rb -k 3n,3n");
			break;
		case 2:
			asprintf(&sort,"sort -k 3n");
			break;
		case 4:
			asprintf(&sort,"sort -k 2rb,2rb -k 1b,1b");
			break;
		default:
			asprintf(&sort,"sort");
			break;
		}

//TODO// filepath
	asprintf(&command,"find \"%s\" -maxdepth %i|ctags -L - -x|%s|sed 's@ \\+@ @g'",filepath,kkedit->prefsDepth,sort);
	fp=popen(command, "r");
	while(fgets(line,2048,fp))
		{
			newstr=globalSlice->deleteSlice(line,filepath);
			if(globalSlice->getResult()==NOERROR)
				{
					g_string_append_printf(str,"%s",newstr);
					debugFree(&newstr,"getRecursiveTagList newstr");
				}
		}
	pclose(fp);

	*((char**)ptr)=str->str;
	g_string_free(str,false);
	debugFree(&command,"getRecursiveTagList command");
	debugFree(&sort,"getRecursiveTagList sort");
}

void destroyTool(gpointer data)
{
	if(((toolStruct*)data)->menuName!=NULL)
		debugFree(&((toolStruct*)data)->menuName,"destroyTool menuName");
	if(((toolStruct*)data)->filePath!=NULL)
		debugFree(&((toolStruct*)data)->filePath,"destroyTool filePath");
	if(((toolStruct*)data)->command!=NULL)
		debugFree(&((toolStruct*)data)->command,"destroyTool command");
	debugFree((char**)&data,"destroyTool data");
}

gint sortTools(gconstpointer a,gconstpointer b)
{
	return(strcasecmp(((toolStruct*)a)->menuName,((toolStruct*)b)->menuName));
}

void buildToolsList(void)
{
}

//
//VISIBLE void goBack(QWidget* widget,uPtr data)
//{
//printf("goBack %i\n",(int)(long)data);
//}
//
//VISIBLE void goFoward(QWidget* widget,uPtr data)
//{
//printf("goFoward %i\n",(int)(long)data);
//}


gboolean idleScroll(gpointer data)
{
	return(false);
}

void showBarberPole(const char* title)
{
}

void killBarberPole(void)
{
}

void debugFree(char** ptr,const char* message)
{
#ifdef _DEBUG_FREE_
	fprintf(stderr,"free :%s\n",message);
#endif

	if (*ptr!=NULL)
		free(*ptr);

	*ptr=NULL;

}

void doBusy(bool busy,pageStruct* page)
{
}

char* toCharStar(QString *str)
{
	QByteArray ba;
	ba=str->toLocal8Bit();
	return(strdup(ba.data()));
}
KKEditClass  *kkedit;


 


