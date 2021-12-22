/*
 *
 * ©K. D. Hedger. Fri  5 Nov 14:05:52 GMT 2021 keithdhedger@gmail.com

 * This file (callbacks.cpp) is part of KKEdit.

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

#include "callbacks.h"



void releasePlugs(gpointer data,gpointer user_data)
{
#ifndef _USEQT5_
	if(((moduleData*)data)->module!=NULL)
		g_module_close((GModule*)((moduleData*)data)->module);
#endif
}


#ifndef _USEQT5_
void dropUri(GtkWidget *widget,GdkDragContext *context,gint x,gint y,GtkSelectionData *selection_data,guint info,guint32 time,gpointer user_data)
#else
//TODO
void dropUri(void)
#endif
{
#ifndef _USEQT5_
	gchar**	array=gtk_selection_data_get_uris(selection_data);
	int		cnt=g_strv_length(array);
	char*	filename;

	for(int j=0; j<cnt; j++)
		{
			filename=g_filename_from_uri(array[j],NULL,NULL);
			openFile(filename,0,true);
		}

	g_strfreev(array);
#endif
}

#ifndef _USEQT5_
void externalTool(GtkWidget* widget,gpointer data)
#else
//TODO//
void externalTool(void)
#endif
{
#if 0
	toolStruct*		tool=(toolStruct*)data;
	pageStruct*		page=kkedit->getDocumentForTab(-1);
	char*			docdirname=NULL;
	char*			tooldirname=NULL;
	char*			text=NULL;
	GtkTextIter		start;
	GtkTextIter		end;
	char*			selection=NULL;
	const char*		vars[]= {"%t","%f","%d","%i","%h","%l",NULL};
	char*			ptr;
	long			pos;
	int				loop=0;
	GString*		tempCommand;
	bool			continueflag;
	char*			barcontrol;
	StringSlice*	slice=new StringSlice;
	char*			barcommand=NULL;

	if(page==NULL || tool==NULL)
		return;

	const char*		varData[]= {NULL,page->filePath,NULL,DATADIR,htmlFile,page->lang};

	tempCommand=g_string_new(tool->command);

	if(page->filePath!=NULL)
		docdirname=g_path_get_dirname(page->filePath);
	else
		docdirname=strdup(getenv("HOME"));

	tooldirname=g_path_get_dirname(tool->filePath);
	chdir(tooldirname);

	setenv("KKEDIT_CURRENTFILE",page->filePath,1);
	setenv("KKEDIT_HTMLFILE",htmlFile,1);
	setenv("KKEDIT_CURRENTDIR",docdirname,1);
	setenv("KKEDIT_DATADIR",DATADIR,1);
	setenv("KKEDIT_SOURCE_LANG",page->lang,1);

	asprintf(&barcontrol,"%s/BarControl-%s",tmpFolderName,slice->randomName(6));

	if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
		{
			selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
			setenv("KKEDIT_SELECTION",selection,1);
		}
	else
		selection=strdup("");

	varData[0]=selection;
	varData[2]=docdirname;

	continueflag=false;
	while(continueflag==false)
		{
			continueflag=true;
			loop=0;
			while(vars[loop]!=NULL)
				{
					ptr=strstr(tempCommand->str,vars[loop]);
					if(ptr!=NULL)
						{
							pos=(long)ptr-(long)tempCommand->str;
							tempCommand=g_string_erase(tempCommand,pos,2);
							tempCommand=g_string_insert(tempCommand,pos,varData[loop]);
							continueflag=false;
						}
					loop++;
				}
		}

	if(tool->clearView==true)
		{
			gtk_text_buffer_set_text(toolOutputBuffer,"",0);
		}

	if(tool->useBar==true)
		{
			setenv("KKEDIT_BAR_CONTROL",barcontrol,1);
			asprintf(&barcommand,POLEPATH " \"%s\" \"%s\" &",tool->menuName,barcontrol);
			system(barcommand);
		}

	runCommand(tempCommand->str,&text,tool->inTerminal,tool->flags,tool->runAsRoot,tool->menuName);
	debugFree(&selection,"externalTool selection");

	if(text!=NULL)
		{
			if(tool->flags & TOOL_REPLACE_OP)
				{
					gtk_text_buffer_get_bounds((GtkTextBuffer*)page->buffer,&start,&end);
					gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&start,&end);
					gtk_text_buffer_delete_selection((GtkTextBuffer*)page->buffer,true,true);
					gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER (page->buffer),&start);
					gtk_text_buffer_insert(GTK_TEXT_BUFFER(page->buffer),&start,text,strlen(text));
				}

			if(tool->flags & TOOL_PASTE_OP)
				{
					if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
						gtk_text_buffer_delete_selection((GtkTextBuffer*)page->buffer,true,true);
					gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(page->buffer),text,strlen(text));
				}
		}

	unsetenv("KKEDIT_CURRENTFILE");
	unsetenv("KKEDIT_CURRENTDIR");
	unsetenv("KKEDIT_DATADIR");
	unsetenv("KKEDIT_SELECTION");
	unsetenv("KKEDIT_HTMLFILE");
	unsetenv("KKEDIT_BAR_CONTROL");

	debugFree(&text,"externalTool text");
	debugFree(&docdirname,"externalTool docdirname");
	debugFree(&tooldirname,"externalTool tooldirname");
	if(barcommand!=NULL)
		{
			debugFree(&barcommand,"externalTool barcommand");
			asprintf(&barcommand,"echo quit>%s",barcontrol);
			system(barcommand);
			debugFree(&barcommand,"externalTool barcommand");
		}
	debugFree(&barcontrol,"externalTool barcontrol");
	delete slice;
#endif
}

void openHelp(Widget* widget,uPtr data)
//TODO//
{
printf("openHelp %i\n",(int)(long)data);

#ifndef _USEQT5_
	const char* lang;


		lang="en";

	asprintf(&thePage,"file://%s/help/help.%s.html",DATADIR,lang);
#ifdef _BUILDDOCVIEWER_
	showDocView(USEURI,(char*)"KKEdit","KKEdit Help");
#else
	asprintf(&thePage,"%s %s/help/help.%s.shtml",browserCommand,DATADIR,lang);
	runCommand(thePage,NULL,false,8,0,(char*)"KKEdit Help");
	debugFree((char**)&thePage,"openHelp thePage");
	thePage=NULL;
#endif
#endif
}

#ifndef _USEQT5_
void addtoCustomWordList(GtkWidget* widget,gpointer data)
#else
//TODO//
void addtoCustomWordList(void)
#endif
{
#ifndef _USEQT5_
	pageStruct*		page=(pageStruct*)data;
	GtkTextIter		start;
	GtkTextIter		end;
	char*			selection=NULL;
	char*			command;

	if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
		{
			selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
			if(selection==NULL)
				return;
		}
	else
		return;

	asprintf(&command,"echo '%s'|cat - %s/%s|sort -u -o %s/%s.tmp;mv %s/%s.tmp %s/%s",selection,getenv("HOME"),CUSTOMWORDFILE,getenv("HOME"),CUSTOMWORDFILE,getenv("HOME"),CUSTOMWORDFILE,getenv("HOME"),CUSTOMWORDFILE);
	system(command);
	if (command!=NULL) free(command);command=NULL;
	if (selection!=NULL) free(selection);selection=NULL;
	createCompletion(page);
#endif
}

#ifndef _USEQT5_
void setToolOptions(GtkWidget* widget,gpointer data)
#else
//TODO//
void setToolOptions(void)
#endif
{
#ifndef _USEQT5_
	char*		dirname;
	char*		text;
	char*		toolpath;
	int			flags;

	asprintf(&dirname,"%s/.KKEdit/tools",getenv("HOME"));
	text=strdup(gtk_entry_get_text((GtkEntry*)toolNameWidget));
	text=g_strdelimit(text," ",'-');
	asprintf(&toolpath,"%s/.KKEdit/tools/%s",getenv("HOME"),text);

	if((gtk_toggle_button_get_active((GtkToggleButton*)syncWidget)==false) || (gtk_toggle_button_get_active((GtkToggleButton*)inTermWidget)==true))
		{
			gtk_widget_set_sensitive(ignoreWidget,false);
			gtk_widget_set_sensitive(pasteWidget,false);
			gtk_widget_set_sensitive(replaceWidget,false);
			gtk_widget_set_sensitive(outputWidget,false);
		}
	else
		{
			gtk_widget_set_sensitive(ignoreWidget,true);
			gtk_widget_set_sensitive(pasteWidget,true);
			gtk_widget_set_sensitive(replaceWidget,true);
			gtk_widget_set_sensitive(outputWidget,true);
		}

	if(strcmp(gtk_widget_get_name(widget),"apply")==0)
		{
			if(gtk_toggle_button_get_active((GtkToggleButton*)syncWidget)==false)
				flags=TOOL_ASYNC;
			else
				{
					if(gtk_toggle_button_get_active((GtkToggleButton*)ignoreWidget)==true)
						flags=TOOL_IGNORE_OP;
					if(gtk_toggle_button_get_active((GtkToggleButton*)pasteWidget)==true)
						flags=TOOL_PASTE_OP;
					if(gtk_toggle_button_get_active((GtkToggleButton*)replaceWidget)==true)
						flags=TOOL_REPLACE_OP;
					if(gtk_toggle_button_get_active((GtkToggleButton*)outputWidget)==true)
						flags=TOOL_VIEW_OP;
				}
			if(gtk_toggle_button_get_active((GtkToggleButton*)showDocWidget)==true)
				flags=flags+TOOL_SHOW_DOC;


			intermarg=(int)gtk_toggle_button_get_active((GtkToggleButton*)inTermWidget);
			flagsarg=flags;
			inpopup=(int)gtk_toggle_button_get_active((GtkToggleButton*)inPopupWidget);
			alwayspopup=(int)gtk_toggle_button_get_active((GtkToggleButton*)alwaysPopupWidget);
			clearview=(int)gtk_toggle_button_get_active((GtkToggleButton*)clearViewWidget);
			commandarg=(char*)gtk_entry_get_text((GtkEntry*)commandLineWidget);
			commentarg=(char*)gtk_entry_get_text((GtkEntry*)commentWidget);
			menuname=(char*)gtk_entry_get_text((GtkEntry*)toolNameWidget);
			rootarg=(int)gtk_toggle_button_get_active((GtkToggleButton*)runAsRootWidget);
			keycode=(int)gdk_keyval_from_name(gtk_entry_get_text((GtkEntry*)keyWidget));
			usebar=(int)gtk_toggle_button_get_active((GtkToggleButton*)useBarWidget);

			saveVarsToFile(toolpath,tool_vars);

			gtk_widget_hide((GtkWidget*)data);
			gtk_widget_destroy((GtkWidget*)data);
			buildTools();
			gtk_widget_show_all(toolsMenu);
		}

	if(strcmp(gtk_widget_get_name(widget),"cancel")==0)
		{
			gtk_widget_hide((GtkWidget*)data);
			gtk_widget_destroy((GtkWidget*)data);
		}

	if(strcmp(gtk_widget_get_name(widget),"delete")==0)
		{
			if((gtk_entry_get_text((GtkEntry*)toolNameWidget)!=NULL) && (yesNo((char*)"Are you sure you want to delete",(char*)gtk_entry_get_text((GtkEntry*)toolNameWidget))==GTK_RESPONSE_YES))
				{
					asprintf(&dirname,"rm \"%s\"",toolpath);
					system(dirname);
					gtk_widget_hide((GtkWidget*)data);
					gtk_widget_destroy((GtkWidget*)data);
					buildTools();
					gtk_widget_show_all(toolsMenu);
				}
		}

	if (toolpath!=NULL) free(toolpath);toolpath=NULL;
	if (text!=NULL) free(text);text=NULL;
	if (dirname!=NULL) free(dirname);dirname=NULL;
#endif
}

void doAbout(Widget* widget,uPtr data)
//TODO//
{
printf("doAbout %i\n",(int)(long)data);

#ifndef _USEQT5_
	const char*	authors[]= {"K.D.Hedger <" MYEMAIL ">",MYWEBSITE,"\nMore by the same author\n","Xfce-Theme-Manager\nhttp://xfce-look.org/content/show.php?content=149647\n","Xfce4-Composite-Editor\nhttp://gtk-apps.org/content/show.php/Xfce4-Composite-Editor?content=149523\n","Manpage Editor\nhttp://gtk-apps.org/content/show.php?content=160219\n","GtkSu\nhttp://gtk-apps.org/content/show.php?content=158974\n","ASpell GUI\nhttp://gtk-apps.org/content/show.php/?content=161353\n","Clipboard Viewer\nhttp://gtk-apps.org/content/show.php/?content=121667",NULL};
	const char	copyright[] ="Copyright \xc2\xa9 2013 K.D.Hedger";
	const char*	aboutboxstring="KKEdit Code Text Editor";
	char*		licence;
	char*		translators;

	asprintf(&translators,"%s:\nNguyen Thanh Tung <thngtong@gmail.com>","French Translation");
	g_file_get_contents(DATADIR"/docs/gpl-3.0.txt",&licence,NULL,NULL);

	gtk_show_about_dialog(NULL,"authors",authors,"translator-credits",translators,"comments",aboutboxstring,"copyright",copyright,"version",VERSION,"website",MYWEBSITE,"program-name","KKEdit","logo-icon-name","KKEdit","license",licence,NULL);

	if (licence!=NULL) free(licence);licence=NULL;
	if (translators!=NULL) free(translators);translators=NULL;
#endif
}

