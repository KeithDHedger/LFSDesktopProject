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
char	defineText[1024];
bool	closingAll=false;

void releasePlugs(gpointer data,gpointer user_data)
{
#ifndef _USEQT5_
	if(((moduleData*)data)->module!=NULL)
		g_module_close((GModule*)((moduleData*)data)->module);
#endif
}

void setToobarSensitive(void)
{
#ifndef _USEQT5_
	pageStruct*	page=kkedit->getDocumentForTab(currentTabNumber);

	for(int j=0; j<(int)strlen(prefsToolBarLayout); j++)
		{
			switch(prefsToolBarLayout[j])
				{
				case 'N':
//newnewButton
					if(newButton!=NULL)
						gtk_widget_set_sensitive((GtkWidget*)newButton,true);
					break;
				case 'O':
//open+recent
					if(openButton!=NULL)
						gtk_widget_set_sensitive((GtkWidget*)openButton,true);
					break;
				case 'S':
//save
					if(saveButton!=NULL)
						{
							if(page!=NULL)
								gtk_widget_set_sensitive((GtkWidget*)saveButton,gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(page->buffer)));
							else
								gtk_widget_set_sensitive((GtkWidget*)saveButton,false);
						}
					break;
				case 'X':
//cut
					if(cutButton!=NULL)
						{
							if(page!=NULL)
								gtk_widget_set_sensitive((GtkWidget*)cutButton,true);
							else
								gtk_widget_set_sensitive((GtkWidget*)cutButton,false);
						}
					break;
				case 'C':
//copy
					if(copyButton!=NULL)
						{
							if(page!=NULL)
								gtk_widget_set_sensitive((GtkWidget*)copyButton,true);
							else
								gtk_widget_set_sensitive((GtkWidget*)copyButton,false);
						}
					break;
				case 'P':
//paste
					if(pasteButton!=NULL)
						{
							if(page!=NULL)
								gtk_widget_set_sensitive((GtkWidget*)pasteButton,true);
							else
								gtk_widget_set_sensitive((GtkWidget*)pasteButton,false);
						}
					break;
				case 'U':
//undo
					if(undoButton!=NULL)
						{
							if(page!=NULL)
								gtk_widget_set_sensitive((GtkWidget*)undoButton,gtk_source_buffer_can_undo(page->buffer));
							else
								gtk_widget_set_sensitive((GtkWidget*)undoButton,false);
						}
					break;
				case 'R':
//redo
					if(redoButton!=NULL)
						{
							if(page!=NULL)
								gtk_widget_set_sensitive((GtkWidget*)redoButton,gtk_source_buffer_can_redo(page->buffer));
							else
								gtk_widget_set_sensitive((GtkWidget*)redoButton,false);
						}
					break;
				case 'F':
//find
					if(findButton!=NULL)
						{
							if(page!=NULL)
								gtk_widget_set_sensitive((GtkWidget*)findButton,true);
							else
								gtk_widget_set_sensitive((GtkWidget*)findButton,false);
						}
					break;
				case 'G':
//navigation
					if(gotoDefButton!=NULL)
						{
							if(page!=NULL)
								gtk_widget_set_sensitive((GtkWidget*)gotoDefButton,true);
							else
								gtk_widget_set_sensitive((GtkWidget*)gotoDefButton,false);
						}
					break;

				case 'B':
//go back
					if(backButton!=NULL)
						{
							if(page!=NULL)
								gtk_widget_set_sensitive((GtkWidget*)backButton,true);
							else
								gtk_widget_set_sensitive((GtkWidget*)backButton,false);
						}
					break;


				case '9':
//find in gtkdoc
					if(gotoLineButton!=NULL)
						{
							if(page!=NULL)
								gtk_widget_set_sensitive((GtkWidget*)gotoLineButton,true);
							else
								gtk_widget_set_sensitive((GtkWidget*)gotoLineButton,false);
						}
					break;

				case 'A':
//find in gtkdoc
					if(findApiButton!=NULL)
						{
							if(page!=NULL)
								gtk_widget_set_sensitive((GtkWidget*)findApiButton,true);
							else
								gtk_widget_set_sensitive((GtkWidget*)findApiButton,false);
						}
					break;

				case 'D':
//find in function def
					if(findFuncDefButton!=NULL)
						{
							if(page!=NULL)
								gtk_widget_set_sensitive((GtkWidget*)findFuncDefButton,true);
							else
								gtk_widget_set_sensitive((GtkWidget*)findFuncDefButton,false);
						}
					break;
				case 'L':
//livesearch
					if(liveSearchButton!=NULL)
						{
							if(page!=NULL)
								gtk_widget_set_sensitive((GtkWidget*)liveSearchButton,true);
							else
								gtk_widget_set_sensitive((GtkWidget*)liveSearchButton,false);
						}
					break;
				}
		}
#endif
}

void refreshMainWindow(void)
{
#ifndef _USEQT5_
	gtk_widget_show(mainWindow);
	gtk_widget_show(mainWindowVBox);
	gtk_widget_show(mainVPane);
	gtk_widget_show(mainWindowHPane);
	gtk_widget_show(secondWindowHPane);
	gtk_widget_show_all(menuBar);
	gtk_widget_show_all(toolBarBox);
	gtk_widget_show_all(mainWindowHBox);

	if(showToolOutWin)
		gtk_widget_show(toolOutVBox);
	else
		gtk_widget_hide(toolOutVBox);

	if(showStatus)
		gtk_widget_show(statusWidget);
	else
		gtk_widget_hide(statusWidget);

	if(showToolBar)
		gtk_widget_show((GtkWidget*)toolBarBox);
	else
		gtk_widget_hide((GtkWidget*)toolBarBox);
#endif
}

int yesNo(char* question,char* file)
{
	int			result=0;
#ifndef _USEQT5_
	GtkWidget*	dialog;

	dialog=gtk_message_dialog_new(GTK_WINDOW(mainWindow),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_WARNING,GTK_BUTTONS_NONE,"%s %s",question,file);

	gtk_dialog_add_buttons((GtkDialog*)dialog,GTK_STOCK_YES,GTK_RESPONSE_YES,GTK_STOCK_NO,GTK_RESPONSE_CANCEL,NULL);
	gtk_window_set_title(GTK_WINDOW(dialog),"What Do You Want To Do?");

	result=gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);

#endif
	return(result);
}

int askSaveDialog(const QString filename)
{
	QMessageBox msgBox;

	msgBox.setIcon(QMessageBox::Warning);
	msgBox.setText(filename+" has been modified.");
	msgBox.setInformativeText("Do you want to save your changes?");
	msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Cancel);
	return(msgBox.exec());
}

void setSensitive(void)
{
#ifndef _USEQT5_
	pageStruct*		page=kkedit->getDocumentForTab(-1);
	const gchar*	text;
	char*			newlabel;
	int				offset=0;
	GtkTextIter	start_find,end_find;

	if(sessionBusy==true)
		return;

	setToobarSensitive();

	if(page==NULL)
		{
//menu
			gtk_widget_set_sensitive((GtkWidget*)undoMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)redoMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)undoAllMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)redoAllMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)cutMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)copyMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)pasteMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)saveMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)saveAsMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)bookMarkMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)funcMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)navMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)printMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)closeMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)closeAllMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)saveAllMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)revertMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)goBackMenu,false);
			gtk_statusbar_remove_all((GtkStatusbar*)statusWidget,0);
		}
	else
		{
			gtk_widget_set_sensitive((GtkWidget*)goBackMenu,history->canGoBack());
			text=gtk_label_get_text((GtkLabel*)page->tabName);
//menu
			gtk_widget_set_sensitive((GtkWidget*)undoMenu,gtk_source_buffer_can_undo(page->buffer));
			gtk_widget_set_sensitive((GtkWidget*)redoMenu,gtk_source_buffer_can_redo(page->buffer));
			gtk_widget_set_sensitive((GtkWidget*)undoAllMenu,gtk_source_buffer_can_undo(page->buffer));
			gtk_widget_set_sensitive((GtkWidget*)redoAllMenu,gtk_source_buffer_can_redo(page->buffer));
			gtk_widget_set_sensitive((GtkWidget*)saveMenu,gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(page->buffer)));
			gtk_widget_set_sensitive((GtkWidget*)goBackMenu,history->canGoBack());
//tab
			if(text[0]=='*')
				offset=1;

			if(gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(page->buffer))==true)
				asprintf(&newlabel,"*%s",&text[offset]);
			else
				newlabel=strdup(&text[offset]);

			gtk_label_set_text((GtkLabel*)page->tabName,(const gchar*)newlabel);
			debugFree(&newlabel,"setSensitive newlabel");
			gtk_widget_set_sensitive((GtkWidget*)cutMenu,true);
			gtk_widget_set_sensitive((GtkWidget*)copyMenu,true);
			gtk_widget_set_sensitive((GtkWidget*)pasteMenu,true);
			gtk_widget_set_sensitive((GtkWidget*)bookMarkMenu,true);
			gtk_widget_set_sensitive((GtkWidget*)navMenu,true);
			gtk_widget_set_sensitive((GtkWidget*)saveAsMenu,true);
			gtk_widget_set_sensitive((GtkWidget*)printMenu,true);
			gtk_widget_set_sensitive((GtkWidget*)closeMenu,true);
			gtk_widget_set_sensitive((GtkWidget*)closeAllMenu,true);
			gtk_widget_set_sensitive((GtkWidget*)saveAllMenu,true);
			gtk_widget_set_sensitive((GtkWidget*)revertMenu,true);
			gtk_widget_show_all(page->tabName);
			updateStatusBar((GtkTextBuffer*)page->buffer,NULL,NULL,page);
			gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&start_find);
			gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&end_find);
			gtk_text_buffer_remove_tag_by_name((GtkTextBuffer*)page->buffer,"highlighttag",&start_find,&end_find);
		}
//do plugin sensitive
	globalPlugins->globalPlugData->page=page;
	g_list_foreach(globalPlugins->plugins,plugRunFunction,(gpointer)"setSensitive");
#endif
}

bool closeTab(Widget* widget,uPtr data)
//TODO//
{
fprintf(stderr,"VISIBLE void closeTab(Widget* widget,uPtr data) TODO\n");
	DocumentClass	*doc=NULL;
	long			thispage=(long)data;

	busyFlag=true;	
	if(closingAll==true)
		thispage=0;
	else
		{
			if(data==0)
				thispage=((QTabWidget*)kkedit->mainNotebook)->currentIndex();
		}

	doc=(DocumentClass*)((QTabWidget*)kkedit->mainNotebook)->widget(thispage);
	if(doc!=0)
		{
			if(doc->dirty==true)
				{
					int result=askSaveDialog(doc->fileName);
					switch(result)
						{
							case QMessageBox::Save:
								fprintf(stderr,"save\n");
       // Save was clicked
								break;
							case QMessageBox::Discard:
       // Don't Save was clicked
								fprintf(stderr,"Discard\n");
								break;
							case QMessageBox::Cancel:
								return(false);
       // Cancel was clicked
								fprintf(stderr,"Cancel\n");
								break;
						}
				}

			((QTabWidget*)kkedit->mainNotebook)->removeTab(thispage);
			delete doc;
		}
	return(true);
}

void closeAllTabs(Widget* widget,uPtr data)//TOGO//
{
	bool retval;
printf("closeAllTabs %i\n",(int)(long)data);
	int	numtabs=((QTabWidget*)kkedit->mainNotebook)->count();
	for(int loop=0; loop<numtabs; loop++)
		{
			closingAll=true;
			retval=closeTab(NULL,0);
			if(retval==false)
				return;
		}
//rebuild bookmark menu
//TODO//
//	rebuildBookMarkMenu();
//	sessionBusy=shold;
}

void closeTabQT(int tabnum)
{
	closingAll=false;
	bool ret=closeTab(NULL,tabnum);
}

void sortTabs(Widget* widget,uPtr data)
{
#ifndef _USEQT5_
	bool		flag=true;
	pageStruct	*page1=NULL;
	pageStruct	*page2=NULL;

	while(flag==true)
		{
			flag=false;
			for (int j=0;j<gtk_notebook_get_n_pages((GtkNotebook*)mainNotebook)-1;j++)
				{
					page1=kkedit->getDocumentForTab(j);
					page2=kkedit->getDocumentForTab(j+1);
					if(strcmp(page2->fileName,page1->fileName)<0)
						{
							flag=true;
							gtk_notebook_reorder_child((GtkNotebook*)mainNotebook,page2->tabVbox,j);
						}
				}
		}
#endif
}

//void switchPage(int thispage)//TODO//move ?
//{
//	char*			functions=NULL;
//	char*			lineptr;
//	DocumentClass	*doc=NULL;
//	int				linenum;
//	char			tmpstr[1024];
//	char*			correctedstr;
//	char*			typenames[50]= {NULL,};
//	char*			newstr=NULL;
//	bool			flag;
//	int				numtypes=0;
//	QMenu			*whattypemenu;
//	QMenu			*typesubmenus[50]= {NULL,};
//	bool			onefunc=false;
//	MenuItemClass	*menuitem;
//
//	//if(sessionBusy==true)
//	//	return;
//
//	doc=(DocumentClass*)((QTabWidget*)kkedit->mainNotebook)->widget(thispage);
//	if(doc==0)
//		return;
//	if(doc==NULL)
//		return;
//
//	doc->setStatusBarText();
//	qobject_cast<QMenu*>(kkedit->funcMenu)->clear();
//	getRecursiveTagList((char*)doc->getFilePath().toStdString().c_str(),&functions);//TODO//
//
//	lineptr=functions;
//	while (lineptr!=NULL)
//		{
//			tmpstr[0]=0;
//			sscanf (lineptr,"%*s %*s %i %[^\n]s",&linenum,tmpstr);
//			correctedstr=truncateWithElipses(tmpstr,kkedit->prefsMaxFuncChars);
//			sprintf(tmpstr,"%s",correctedstr);
//			debugFree(&correctedstr,"switchPage correctedstr");
//
//			if(strlen(tmpstr)>0)
//				{
//					if(kkedit->prefsFunctionMenuLayout==4)
//						{
//							newstr=NULL;
//							newstr=globalSlice->sliceBetween(lineptr,(char*)" ",(char*)" ");
//							if(newstr!=NULL)
//								{
//									flag=false;
//									for(int j=0; j<numtypes; j++)
//										{
//											if (strcmp(newstr,typenames[j])==0)
//												{
//													whattypemenu=typesubmenus[j];
//													flag=true;
//													break;
//												}
//										}
//
//									if(flag==false)
//										{
//											typenames[numtypes]=strdup(newstr);
//											debugFree(&newstr,"switchPage newstr");
//											if(typenames[numtypes][strlen(typenames[numtypes])-1]=='s')
//												asprintf(&newstr,"%s's",typenames[numtypes]);
//											else
//												asprintf(&newstr,"%ss",typenames[numtypes]);
//											newstr[0]=toupper(newstr[0]);
//											typesubmenus[numtypes]=new QMenu(newstr);
//											qobject_cast<QMenu*>(kkedit->funcMenu)->addMenu(qobject_cast<QMenu*>(typesubmenus[numtypes]));
//											whattypemenu=typesubmenus[numtypes];
//											numtypes++;
//										}
//
//									debugFree(&newstr,"switchPage newstr");
//
//									onefunc=true;
//									menuitem=new MenuItemClass(tmpstr);
//									//menuitem->setCallBackVoid(gotoLine);
//									menuitem->setMenuID(linenum);
//									menuitem->mainKKEditClass=kkedit;
//									qobject_cast<QMenu*>(whattypemenu)->addAction(menuitem);
//							//		QObject::connect(menuitem,SIGNAL(triggered()),menuitem,SLOT(menuClickedGotoLine()));
//									QObject::connect(menuitem,SIGNAL(triggered()),menuitem,SLOT(menuClickedGotoLine()));
//							}
//						}
//					else
//						{
//							onefunc=true;
//							menuitem=new MenuItemClass(tmpstr);
//							//menuitem->setCallBackVoid(gotoLine);
//							menuitem->setMenuID(linenum);
//							menuitem->mainKKEditClass=kkedit;
//							qobject_cast<QMenu*>(kkedit->funcMenu)->addAction(menuitem);
//						//	QObject::connect(menuitem,SIGNAL(triggered()),menuitem,SLOT(menuClickedGotoLine()));
//							QObject::connect(menuitem,SIGNAL(triggered()),menuitem,SLOT(menuClickedGotoLine()));
//						}
//				}
//
//			lineptr=strchr(lineptr,'\n');
//			if (lineptr!=NULL)
//				lineptr++;
//		}
//	kkedit->funcMenu->setEnabled(onefunc);
//}

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
#ifndef _USEQT5_
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

	if(strncmp(localeLang,"en",2)==0)
		lang="en";
	else
		lang="fr";

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
void copyToClipboard(GtkWidget* widget,gpointer data)
#else
//TODO//
void copyToClipboard(void)
#endif
{
#ifndef _USEQT5_
	GtkClipboard*	clipboard=gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	gtk_clipboard_set_text(clipboard,(char*)data,-1);
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
	debugFree(&command,"addtoCustomWordList command");
	debugFree(&selection,"addtoCustomWordList selection");
	createCompletion(page);
#endif
}

#ifndef _USEQT5_
void populatePopupMenu(GtkTextView *entry,GtkMenu *menu,gpointer user_data)
#else
//TODO//
void populatePopupMenu(void)
#endif
{
#ifndef _USEQT5_
	pageStruct*		page=kkedit->getDocumentForTab(-1);
	GtkTextIter		start;
	GtkTextIter		end;
	char*			selection=NULL;
	GtkWidget*		menuitem;
	GtkWidget*		image;
	GList*			ptr;
	functionData*	fdata;
	char*			temptext=NULL;

	if(sessionBusy==true)
		return;

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_prepend(GTK_MENU_SHELL(menu),menuitem);

	if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
		{
			selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
			if(selection!=NULL)
				{
				//TODO//
					fdata=getFunctionByName(selection,false,true);
					if(fdata!=NULL)
						{
							temptext=truncateWithElipses(fdata->define,kkedit->prefsMaxFuncChars);
							menuitem=gtk_menu_item_new_with_label(temptext);
							debugFree(&temptext,"populatePopupMenu temptext");
							sprintf((char*)&defineText,"%s",fdata->define);
							gtk_menu_shell_prepend(GTK_MENU_SHELL(menu),menuitem);
							g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(copyToClipboard),(void*)defineText);
							destroyData(fdata);

							image=gtk_image_new_from_stock(GTK_STOCK_DIALOG_QUESTION,GTK_ICON_SIZE_MENU);
							menuitem=gtk_image_menu_item_new_with_label("Go To Definition");
							gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
							gtk_menu_shell_prepend(GTK_MENU_SHELL(menu),menuitem);
							g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(goToDefinition),NULL);
						}

					if(gotDoxygen==0)
						{
							menuitem=gtk_image_menu_item_new_with_label("Find In Documentation");
							image=gtk_image_new_from_stock(GTK_STOCK_FIND,GTK_ICON_SIZE_MENU);
							gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
							gtk_menu_shell_prepend(GTK_MENU_SHELL(menu),menuitem);
							g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(doxyDocs),NULL);
						}
					menuitem=gtk_image_menu_item_new_with_label("Search In Qt5 Docs");
					image=gtk_image_new_from_stock(GTK_STOCK_FIND,GTK_ICON_SIZE_MENU);
					gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
					gtk_menu_shell_prepend(GTK_MENU_SHELL(menu),menuitem);
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(searchQT5Docs),NULL);

					menuitem=gtk_image_menu_item_new_with_label("Search In Gtk Docs");
					image=gtk_image_new_from_stock(GTK_STOCK_FIND,GTK_ICON_SIZE_MENU);
					gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
					gtk_menu_shell_prepend(GTK_MENU_SHELL(menu),menuitem);
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(searchGtkDocs),NULL);

					menuitem=gtk_image_menu_item_new_with_label("Add To Custom Word List");
					image=gtk_image_new_from_stock(GTK_STOCK_EDIT,GTK_ICON_SIZE_MENU);
					gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
					gtk_menu_shell_prepend(GTK_MENU_SHELL(menu),menuitem);
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(addtoCustomWordList),(void*)page);

#ifdef _ASPELL_
//spell check
					if((spellChecker!=NULL) && (aspellConfig!=NULL))
						{
							menuitem=gtk_image_menu_item_new_with_label("Check Spellling");
							image=gtk_image_new_from_stock(GTK_STOCK_SPELL_CHECK,GTK_ICON_SIZE_MENU);
							gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
							gtk_menu_shell_prepend(GTK_MENU_SHELL(menu),menuitem);
							g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(checkWord),NULL);
						}
#endif
					menuitem=gtk_separator_menu_item_new();
					gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

					ptr=toolsList;
					while(ptr!=NULL)
						{
							if((((toolStruct*)ptr->data)->inPopUp==true) && (((toolStruct*)ptr->data)->alwaysPopup==false))
								{
									menuitem=gtk_image_menu_item_new_with_label(((toolStruct*)ptr->data)->menuName);
									if(((toolStruct*)ptr->data)->comment!=NULL)
										gtk_widget_set_tooltip_text((GtkWidget*)menuitem,((toolStruct*)ptr->data)->comment);

									gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
									g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(externalTool),(void*)ptr->data);
								}
							ptr=g_list_next(ptr);
						}
				}
		}

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

	ptr=toolsList;
	while(ptr!=NULL)
		{
			if((((toolStruct*)ptr->data)->alwaysPopup==true))
				{
					menuitem=gtk_image_menu_item_new_with_label(((toolStruct*)ptr->data)->menuName);
					if(((toolStruct*)ptr->data)->comment!=NULL)
						gtk_widget_set_tooltip_text((GtkWidget*)menuitem,((toolStruct*)ptr->data)->comment);
					gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(externalTool),(void*)ptr->data);
				}
			ptr=g_list_next(ptr);
		}

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	menuitem=gtk_menu_item_new_with_label("Toggle Bookmark");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(toggleBookmark),NULL);

	globalPlugins->globalPlugData->contextPopUpMenu=(GtkWidget*)menu;

	g_list_foreach(globalPlugins->plugins,plugRunFunction,(gpointer)"addToContext");

	gtk_widget_show_all((GtkWidget*)menu);
#endif
}

#ifndef _USEQT5_
void doTabMenu(GtkWidget *widget,gpointer user_data)
#else
//TODO//
void doTabMenu(void)
#endif
{
#ifndef _USEQT5_
	GtkClipboard*	clipboard=gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);

	gtk_clipboard_set_text(clipboard,(char*)user_data,-1);
	gtk_widget_destroy(tabMenu);
#endif
}

#ifndef _USEQT5_
gboolean whatPane(GtkWidget *widget,GdkEvent *event,gpointer data)
#else
//TODO//
gboolean whatPane(void)
#endif
{
#ifndef _USEQT5_
	pageStruct* page=(pageStruct*)kkedit->getDocumentForTab(-1);

	if((long)data==1)
		page->inTop=true;
	else
		page->inTop=false;

#endif
	return(false);
}

#ifndef _USEQT5_
void doSplitView(GtkWidget *widget,gpointer user_data)
#else
//TODO//
void doSplitView(void)
#endif
{
#ifndef _USEQT5_
	pageStruct* page=(pageStruct*)user_data;

	if(gtk_paned_get_child2((GtkPaned*)page->pane)==NULL)
		{
			page->pageWindow2=(GtkScrolledWindow*)gtk_scrolled_window_new(NULL, NULL);
			gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(page->pageWindow2),GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

			page->view2 = (GtkSourceView*)gtk_source_view_new_with_buffer (page->buffer);
			g_signal_connect(G_OBJECT(page->view2),"populate-popup",G_CALLBACK(populatePopupMenu),NULL);
			setFilePrefs(page);
//	g_signal_connect(G_OBJECT(page->view2),"paste-clipboard",G_CALLBACK(testcallback),NULL);


			gtk_paned_add2(GTK_PANED(page->pane),(GtkWidget*)page->pageWindow2);
			gtk_container_add(GTK_CONTAINER((GtkWidget*)page->pageWindow2),(GtkWidget*)page->view2);
			g_signal_connect(G_OBJECT(page->view2),"button-release-event",G_CALLBACK(whatPane),(void*)2);
			page->isSplit=true;
		}
	else
		{
			page->isSplit=false;
			gtk_widget_destroy(gtk_paned_get_child2((GtkPaned*)page->pane));
		}

	gtk_widget_show_all(page->pane);
#endif
}

#ifndef _USEQT5_
void changeSourceStyle(GtkWidget* widget,gpointer data)
#else
//TODO//
void changeSourceStyle(void)
#endif
{
#ifndef _USEQT5_
	pageStruct*					page=kkedit->getDocumentForTab(-1);
	GtkSourceLanguageManager*	lm=gtk_source_language_manager_get_default();
	const gchar* const*			ids=gtk_source_language_manager_get_language_ids(lm);
	GtkSourceLanguage*			lang=gtk_source_language_manager_get_language(lm,ids[(long)data]);
	const char*					langname=gtk_source_language_get_name(lang);

	gtk_source_buffer_set_language(page->buffer,lang);
	page->lang=langname;
#endif
}

#ifndef _USEQT5_
void openFromTab(GtkMenuItem* widget,pageStruct* page)
#else
//TODO//
void openFromTab(void)
#endif
{
#ifndef _USEQT5_
	char*		filepath=NULL;

	asprintf(&filepath,"%s/%s",page->dirName,gtk_menu_item_get_label(widget));
	kkedit->openFile(filepath,0,true);
	debugFree(&filepath,"openFromTab filepath");
#endif
}

#ifndef _USEQT5_
bool tabPopUp(GtkWidget *widget, GdkEventButton *event,gpointer user_data)
#else
//TODO//
bool tabPopUp(void)
#endif
{
#ifndef _USEQT5_
	pageStruct*					page;
	GtkWidget*					menuitem;
	GtkWidget*					image;
	GtkWidget*					submenu;
	GtkWidget*					menuids;
	GtkSourceLanguageManager*	lm;
	const gchar* const*			ids;
	int							cnt=0;
	char*						idsort[1000];
	int							idnum[1000];
	char*						holdstr=NULL;
	int							holdidnum;
	bool						flag=true;
	GtkSourceLanguage*			lang;
	const char*					langname;
	FILE*						fp=NULL;
	char*						command;
	char						line[2048];
	char*						name;

	if(sessionBusy==true)
		return(false);

	if(event->button==3 && event->type==GDK_BUTTON_PRESS)
		{
			tabMenu=gtk_menu_new();
			page=(pageStruct*)user_data;

//copy dirname
			image=gtk_image_new_from_stock(GTK_STOCK_COPY,GTK_ICON_SIZE_MENU);
			menuitem=gtk_image_menu_item_new_with_label("Copy Folder Path");
			gtk_image_menu_item_set_image((GtkImageMenuItem*)menuitem,image);
			gtk_menu_shell_append(GTK_MENU_SHELL(tabMenu),menuitem);
			g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(doTabMenu),(void*)page->dirName);
//copy filepath
			image=gtk_image_new_from_stock(GTK_STOCK_COPY,GTK_ICON_SIZE_MENU);
			menuitem=gtk_image_menu_item_new_with_label("Copy Filepath");
			gtk_image_menu_item_set_image((GtkImageMenuItem*)menuitem,image);
			gtk_menu_shell_append(GTK_MENU_SHELL(tabMenu),menuitem);
			g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(doTabMenu),(void*)page->filePath);
//copy filename
			image=gtk_image_new_from_stock(GTK_STOCK_COPY,GTK_ICON_SIZE_MENU);
			menuitem=gtk_image_menu_item_new_with_label("Copy FileName");
			gtk_image_menu_item_set_image((GtkImageMenuItem*)menuitem,image);
			gtk_menu_shell_append(GTK_MENU_SHELL(tabMenu),menuitem);
			g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(doTabMenu),(void*)page->fileName);

#ifdef _ASPELL_
//check document
			if((spellChecker!=NULL) && (aspellConfig!=NULL) && (gtk_text_buffer_get_modified((GtkTextBuffer*)page->buffer)==false))
				{
					image=gtk_image_new_from_stock(GTK_STOCK_SPELL_CHECK,GTK_ICON_SIZE_MENU);
					menuitem=gtk_image_menu_item_new_with_label("Spell Check Document");
					gtk_image_menu_item_set_image((GtkImageMenuItem*)menuitem,image);
					gtk_menu_shell_append(GTK_MENU_SHELL(tabMenu),menuitem);
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(doSpellCheckDoc),(void*)page->filePath);
				}
#endif

//paned view
			image=gtk_image_new_from_stock(GTK_STOCK_NEW,GTK_ICON_SIZE_MENU);
			if(page->isSplit==true)
				menuitem=gtk_image_menu_item_new_with_label("Un-Split View");
			else
				menuitem=gtk_image_menu_item_new_with_label("Split View");

			gtk_image_menu_item_set_image((GtkImageMenuItem*)menuitem,image);
			gtk_menu_shell_append(GTK_MENU_SHELL(tabMenu),menuitem);
			g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(doSplitView),(void*)page);

//source highlighting
			lm=gtk_source_language_manager_get_default();
			ids=gtk_source_language_manager_get_language_ids(lm);

			image=gtk_image_new_from_stock(GTK_STOCK_SELECT_COLOR,GTK_ICON_SIZE_MENU);
			menuitem=gtk_image_menu_item_new_with_label("Source Highlight");
			gtk_image_menu_item_set_image((GtkImageMenuItem*)menuitem,image);
			gtk_menu_shell_append(GTK_MENU_SHELL(tabMenu),menuitem);

			submenu=gtk_menu_new();
			gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),submenu);

			cnt=0;
			while(ids[cnt]!=NULL)
				{
					idsort[cnt]=strdup(ids[cnt]);
					idnum[cnt]=cnt;
					cnt++;
				}
			idsort[cnt]=NULL;

			flag=true;
			while(flag==true)
				{
					flag=false;
					for(int j=0; j<cnt; j++)
						{
							if((idsort[j+1]!=NULL) && (strcmp(idsort[j],idsort[j+1])>0))
								{
									flag=true;
									holdstr=idsort[j];
									idsort[j]=idsort[j+1];
									idsort[j+1]=holdstr;
									holdidnum=idnum[j];
									idnum[j]=idnum[j+1];
									idnum[j+1]=holdidnum;
								}
						}
				}

			for(int j=0; j<cnt; j++)
				{
					lang=gtk_source_language_manager_get_language(lm,idsort[j]);
					if(gtk_source_language_get_hidden(lang)!=true)
						{
							langname=gtk_source_language_get_name(lang);

							if((page->lang!=NULL) && (strcmp(page->lang,langname)==0))
								{
									image=gtk_image_new_from_stock(GTK_STOCK_APPLY,GTK_ICON_SIZE_MENU);
									menuids=gtk_image_menu_item_new_with_label(langname);
									gtk_image_menu_item_set_image((GtkImageMenuItem *)menuids,image);
								}
							else
								{
									menuids=gtk_menu_item_new_with_label(langname);
								}

							g_signal_connect(G_OBJECT(menuids),"activate",G_CALLBACK(changeSourceStyle),(void*)(long)idnum[j]);
							gtk_menu_shell_append(GTK_MENU_SHELL(submenu),menuids);
						}
				}
//add files to tab
			menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN,NULL);
			gtk_menu_shell_append(GTK_MENU_SHELL(tabMenu),menuitem);

			submenu=gtk_menu_new();
			gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),submenu);

			asprintf(&command,"ls -1 \"%s\"|sort|sed -n '/^.*[^\\.o]$/p'",page->dirName);
			fp=popen(command,"r");
			if(fp!=NULL)
				{
					while(fgets(line,2048,fp))
						{
							line[strlen(line)-1]=0;
							name=basename(line);
							menuids=gtk_image_menu_item_new_with_label(strdup(name));
							gtk_menu_shell_append(GTK_MENU_SHELL(submenu),menuids);
							g_signal_connect(G_OBJECT(menuids),"activate",G_CALLBACK(openFromTab),(void*)page);
						}
					fclose(fp);
				}
			debugFree(&command,"tabpopup command");

			gtk_widget_show_all(menuitem);

//do plugs
			globalPlugins->globalPlugData->tabPopUpMenu=tabMenu;
			globalPlugins->globalPlugData->page=page;
			g_list_foreach(globalPlugins->plugins,plugRunFunction,(gpointer)"addToTab");

			gtk_menu_attach_to_widget(GTK_MENU(tabMenu),widget,NULL);
			gtk_menu_popup(GTK_MENU(tabMenu),NULL,NULL,NULL,NULL,event->button,event->time);
			gtk_widget_show_all((GtkWidget*)tabMenu);

			return(true);
		}
	else
		return(false);
#endif
	return(false);
}


void doShutdown(Widget* widget,uPtr data)
//TODO//save modified files
{
#if 1
	//if(kkedit->forcedMultInst==false)
	if(kkedit->forcedMultInst==false)
		kkedit->writeExitData();
#ifdef _ASPELL_
	delete_aspell_config(aspellConfig);
	delete_aspell_speller(spellChecker);
#endif
	//if(onExitSaveSession)
	//	saveSession(NULL,0);
	//if(doSaveAll(widget,(uPtr)true)==false)
	//	return;
	//g_list_foreach(globalPlugins->plugins,releasePlugs,NULL);
#endif

	QApplication::quit();
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

	debugFree(&toolpath,"setToolOptions toolpath");
	debugFree(&text,"setToolOptions text");
	debugFree(&dirname,"setToolOptions dirname");
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

	debugFree(&licence,"doAbout licence");
	debugFree(&translators,"doAbout licence");
#endif
}

//printing
void drawPage(void)
{
}

void beginPrint(void)
{
}

void printFile(Widget* widget,uPtr data)
{
printf("printFile %i\n",(int)(long)data);
}

void recentFileMenu(void)
{
}


void doKeyShortCut(int what)
{
#ifndef _USEQT5_
	TextBuffer*		buf;
	char*			text;
	pageStruct*		page=kkedit->getDocumentForTab(-1);
	GtkTextMark*	mark;

	if(page==NULL)
		return;

	buf=new TextBuffer((GtkTextBuffer*)page->buffer);

	switch(what)
		{
//delete line ^Y
		case 0:
			gtk_text_buffer_delete(buf->textBuffer,&buf->lineStart,&buf->lineEnd);
			break;
//delete from cursor to end of line ^k
		case 1:
			buf->getToVisibleLineEnd();
			buf->deleteFromCursor(&buf->visibleLineEnd);
			break;
//delete from cursor to beginning of line ^?
		case 2:
			buf->getToLineStart();
			buf->deleteToCursor(&buf->lineStart);
			break;
//Select Word Under Cursor
		case 3:
			if(buf->selectWord())
				buf->selectRange(&buf->lineStart,&buf->cursorPos);
			break;
//delete word under cursor ^h
		case 4:
			if(buf->selectWord())
				gtk_text_buffer_delete(buf->textBuffer,&buf->lineStart,&buf->cursorPos);
			break;
//duplictae line ^D
		case 5:
			buf->getCursorIter();
			text=buf->getSelectedText();
			gtk_text_iter_backward_lines(&buf->cursorPos,-1);
			gtk_text_buffer_begin_user_action(buf->textBuffer);
			gtk_text_buffer_insert(buf->textBuffer,&buf->cursorPos,text,-1);
			debugFree(&text,"doKeyShortCut text");
			gtk_text_buffer_end_user_action(buf->textBuffer);
			break;
//select line ^l
		case 6:
			buf->selectVisibleLine();
			break;
//Move Current Line Up ^m
		case 7:
			text=buf->getSelectedText();
			gtk_text_buffer_begin_user_action(buf->textBuffer);
			gtk_text_buffer_delete(buf->textBuffer,&buf->lineStart,&buf->lineEnd);
			buf->getCursorIter();
			gtk_text_iter_backward_visible_line(&buf->cursorPos);
			gtk_text_buffer_insert(buf->textBuffer,&buf->cursorPos,text,-1);
			gtk_text_iter_backward_visible_line(&buf->cursorPos);
			gtk_text_buffer_place_cursor(buf->textBuffer,&buf->cursorPos);
			gtk_text_buffer_end_user_action(buf->textBuffer);
			break;
//Move Current Line Down
		case 8:
			text=buf->getSelectedText();
			gtk_text_buffer_begin_user_action(buf->textBuffer);
			gtk_text_buffer_delete(buf->textBuffer,&buf->lineStart,&buf->lineEnd);
			buf->getCursorIter();
			gtk_text_iter_forward_visible_line(&buf->cursorPos);
			gtk_text_buffer_insert(buf->textBuffer,&buf->cursorPos,text,-1);
			buf->getCursorIter();
			gtk_text_iter_forward_visible_line(&buf->cursorPos);
			gtk_text_buffer_place_cursor(buf->textBuffer,&buf->cursorPos);
			gtk_text_buffer_end_user_action(buf->textBuffer);
			break;
//Select From Cursor To End Of Line
		case 9:
			buf->selectToLineEnd();
			break;
//Select From Beginning Of Line To Cursor
		case 10:
			buf->selectToLineStart();
			break;
//Move Selection Up
		case 11:
			if(gtk_text_buffer_get_selection_bounds(buf->textBuffer,&buf->lineStart,&buf->lineEnd))
				{
					gtk_text_buffer_begin_user_action(buf->textBuffer);
					gtk_text_iter_set_line_offset(&buf->lineStart,0);
					if(!gtk_text_iter_starts_line(&buf->lineEnd))
						gtk_text_iter_forward_visible_line(&buf->lineEnd);
					buf->selectRange(&buf->lineStart,&buf->lineEnd);
					text=gtk_text_buffer_get_text(buf->textBuffer,&buf->lineStart,&buf->lineEnd,true);
					gtk_text_buffer_delete(buf->textBuffer,&buf->lineStart,&buf->lineEnd);
					buf->getCursorIter();
					gtk_text_iter_backward_visible_line(&buf->cursorPos);
					mark=gtk_text_buffer_create_mark(buf->textBuffer,"moveup",&buf->cursorPos,true);
					gtk_text_buffer_insert(buf->textBuffer,&buf->cursorPos,text,-1);
					gtk_text_buffer_get_iter_at_mark(buf->textBuffer,&buf->cursorPos,mark);
					buf->lineEnd=buf->cursorPos;
					gtk_text_buffer_place_cursor(buf->textBuffer,&buf->cursorPos);
					gtk_text_iter_forward_chars(&buf->lineEnd,strlen(text));
					buf->selectRange(&buf->cursorPos,&buf->lineEnd);
					gtk_text_buffer_delete_mark(buf->textBuffer,mark);
					gtk_text_buffer_end_user_action(buf->textBuffer);
				}
			break;
//Move Selection Down
		case 12:
			if(gtk_text_buffer_get_selection_bounds(buf->textBuffer,&buf->lineStart,&buf->lineEnd))
				{
					gtk_text_buffer_begin_user_action(buf->textBuffer);
					gtk_text_iter_set_line_offset(&buf->lineStart,0);
					if(!gtk_text_iter_starts_line(&buf->lineEnd))
						gtk_text_iter_forward_visible_line(&buf->lineEnd);
					buf->selectRange(&buf->lineStart,&buf->lineEnd);
					text=gtk_text_buffer_get_text(buf->textBuffer,&buf->lineStart,&buf->lineEnd,true);
					gtk_text_buffer_delete(buf->textBuffer,&buf->lineStart,&buf->lineEnd);
					buf->getCursorIter();
					gtk_text_iter_forward_visible_line(&buf->cursorPos);
					mark=gtk_text_buffer_create_mark(buf->textBuffer,"movedown",&buf->cursorPos,false);
					gtk_text_buffer_insert(buf->textBuffer,&buf->cursorPos,text,-1);
					gtk_text_buffer_get_iter_at_mark(buf->textBuffer,&buf->lineEnd,mark);
					buf->cursorPos=buf->lineEnd;
					gtk_text_iter_backward_chars(&buf->cursorPos,strlen(text));
					buf->selectRange(&buf->cursorPos,&buf->lineEnd);
					gtk_text_buffer_delete_mark(buf->textBuffer,mark);
					gtk_text_buffer_end_user_action(buf->textBuffer);
				}
			break;
//completion
		case 13:
			forcePopup=true;
			doCompletionPopUp(page);
			forcePopup=false;
			break;
		}
	delete buf;
#endif
}

void loadKeybindings(void)
{
#ifndef _USEQT5_
	char*	filename;

	asprintf(&filename,"%s/.KKEdit/keybindings.rc",getenv("HOME"));

	loadVarsFromFile(filename,keybindings_rc);
	for(int j=0;j<NUMSHORTCUTS;j++)
		{
			if(shortCutStrings[j]!=NULL)
				sscanf(shortCutStrings[j],"%i %i",(int*)&shortCuts[j][0],(int*)&shortCuts[j][1]);
		}
	debugFree(&filename,"readConfig filename");
#endif
}

#ifndef _USEQT5_
gboolean keyShortCut(GtkWidget* window,GdkEventKey* event,gpointer data)
#else
//TODO//
gboolean keyShortCut(void)
#endif
{
#ifndef _USEQT5_
	int		loop;
	bool	gotKey=false;


	if ((event->type==GDK_KEY_PRESS)&& (event->state & GDK_CONTROL_MASK))
		{
			for(loop=0; loop<NUMSHORTCUTS; loop++)
				{
					if(event->keyval==shortCuts[loop][0])
						{
							gotKey=true;
							break;
						}
				}

			if(gotKey==true)
				{
					doKeyShortCut(loop);
					return(true);
				}

			return(gtk_window_propagate_key_event((GtkWindow*)mainWindow, event));
		}

#endif
	return(false);
}

void getPlugins(Widget* widget,uPtr data)
//TODO//
{
printf("getPlugins %i\n",(int)(long)data);

#ifndef _USEQT5_
	char*	command;

	asprintf(&command,"%s \"%s\" &",browserCommand,PLUGINSLINK);
	system(command);
	free(command);
#endif
}

