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



//find replaceAll
//GSList*			findList=NULL;
//GSList*			replaceList=NULL;
unsigned int	maxFRHistory=5;


char*			browserCommand=strdup("firefox");


toolStruct*		selectedToolFromDrop=NULL;

GList*			toolsList=NULL;

char*			functionSearchText=NULL;


HistoryClass*	history;

PluginClass*	globalPlugins=NULL;

void plugRunFunction(gpointer data,gpointer funcname)
{
#ifndef _USEQT5_
	globalPlugins->runPlugFunction((moduleData*)data,(const char*)funcname);
#endif
}

void getMimeType(char* filepath,void* ptr)
{
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


void debugFree(char** ptr,const char* message)
{
#ifdef _DEBUG_FREE_
	fprintf(stderr,"free :%s\n",message);
#endif

	if (*ptr!=NULL)
		free(*ptr);

	*ptr=NULL;

}

KKEditClass  *kkedit;


 


