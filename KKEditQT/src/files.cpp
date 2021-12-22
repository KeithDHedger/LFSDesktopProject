/*
 *
 * ©K. D. Hedger. Sun  7 Nov 13:10:48 GMT 2021 keithdhedger@gmail.com

 * This file (files.cpp) is part of KKEdit.

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

#include "kkedit-includes.h"
//
//const char	*saveFileName=NULL;
//const char	*saveFilePath=NULL;
//bool		dropTextFile=false;
//
//void saveVarsToFile(char* filepath,args* dataptr)
//{
//	FILE*	fd=NULL;
//	int		cnt=0;
//	GSList*	list=NULL;
//
//	fd=fopen(filepath,"w");
//	if(fd!=NULL)
//		{
//			while(dataptr[cnt].name!=NULL)
//				{
//					switch(dataptr[cnt].type)
//						{
//							case TYPEINT:
////				printf("%s\n%s\n%i\n%i\n",filepath,dataptr[cnt].name,dataptr[cnt].type,*(int*)(dataptr[cnt].data));
//								fprintf(fd,"%s	%i\n",dataptr[cnt].name,*(int*)dataptr[cnt].data);
//								break;
//							case TYPESTRING:
////				printf("%s\n%s\n%i\n%s\n",filepath,dataptr[cnt].name,dataptr[cnt].type,*(char**)(dataptr[cnt].data));
//								fprintf(fd,"%s	%s\n",dataptr[cnt].name,*(char**)(dataptr[cnt].data));
//								break;
//							case TYPEBOOL:
////				printf("%s\n%s\n%i\n%i\n",filepath,dataptr[cnt].name,dataptr[cnt].type,(int)*(bool*)(dataptr[cnt].data));
//								fprintf(fd,"%s	%i\n",dataptr[cnt].name,(int)*(bool*)dataptr[cnt].data);
//								break;
//							case TYPELIST:
//								list=*(GSList**)((dataptr[cnt].data));
//								if(g_slist_length(list)>maxFRHistory)
//									list=g_slist_nth(list,g_slist_length(list)-maxFRHistory);
//								while(list!=NULL)
//									{
//										if(strlen((char*)list->data)>0)
//											{
//												fprintf(fd,"%s\t%s\n",dataptr[cnt].name,(char*)list->data);
//											}
//										list=list->next;
//									}
//								break;
//						}
//					cnt++;
//				}
//			fclose(fd);
//		}
//}
//
//void loadVarsFromFile(char* filepath,args* dataptr)
//{
//	FILE*	fd=NULL;
//	char	buffer[2048];
//	int		cnt;
//	char*	argname=NULL;
//	char*	strarg=NULL;
//	char*	liststr=NULL;
//
//	fd=fopen(filepath,"r");
//	if(fd!=NULL)
//		{
//			while(feof(fd)==0)
//				{
//					buffer[0]=0;
//					fgets(buffer,2048,fd);
//					sscanf(buffer,"%ms %ms",&argname,&strarg);
//					cnt=0;
//					while(dataptr[cnt].name!=NULL)
//						{
//							if((strarg!=NULL) && (argname!=NULL) && (strcmp(argname,dataptr[cnt].name)==0))
//								{
//									switch(dataptr[cnt].type)
//										{
//											case TYPEINT:
//												*(int*)dataptr[cnt].data=atoi(strarg);
//												break;
//											case TYPESTRING:
//												if(*(char**)(dataptr[cnt].data)!=NULL)
//													debugFree(&*(char**)(dataptr[cnt].data),"loadVarsFromFile dataptr[cnt].data");
//												sscanf(buffer,"%*s %m[^\n]s",(char**)dataptr[cnt].data);
//												break;
//											case TYPEBOOL:
//												*(bool*)dataptr[cnt].data=(bool)atoi(strarg);
//												break;
//											case TYPELIST:
//												sscanf(buffer,"%*s\t%m[^\n]s",&liststr);
//												*(GSList**)dataptr[cnt].data=g_slist_append(*(GSList**)dataptr[cnt].data,liststr);
//												break;
//										}
//								}
//							cnt++;
//						}
//					debugFree(&argname,"loadVarsFromFile argname");
//					debugFree(&strarg,"loadVarsFromFile strarg");
//				}
//			fclose(fd);
//		}
//}
//
//#ifndef _USEQT5_
//void dropText(GtkWidget *widget,GdkDragContext *context,gint x,gint y,GtkSelectionData *selection_data,guint info,guint32 time,gpointer user_data)
//#else
////TODO//
//void dropText(void)
//#endif
//{
//#ifndef _USEQT5_
//	gchar**			array=NULL;
//	int				cnt;
//	char*			filename;
//	FILE*			fp;
//	char*			command;
//	GString*		str;
//	char			line[1024];
//	pageStruct*		page=(pageStruct*)user_data;
//	GtkTextIter		iter;
//	GtkTextMark*	mark;
//
//	array=gtk_selection_data_get_uris(selection_data);
//	if(array==NULL)
//		{
//			gtk_drag_finish (context,true,true,time);
//			return;
//		}
//
//	cnt=g_strv_length(array);
//
//	if(dropTextFile==false)
//		{
//			dropTextFile=true;
//
//			for(int j=0; j<cnt; j++)
//				{
//					str=g_string_new(NULL);
//					filename=g_filename_from_uri(array[j],NULL,NULL);
//					asprintf(&command,"cat %s",filename);
//					fp=popen(command, "r");
//					while(fgets(line,1024,fp))
//						g_string_append_printf(str,"%s",line);
//					gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(page->buffer),str->str,str->len);
//
//					mark=gtk_text_buffer_get_insert((GtkTextBuffer*)page->buffer);
//					gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&iter,mark);
//					gtk_text_iter_forward_chars(&iter,str->len);
//					gtk_text_buffer_place_cursor((GtkTextBuffer*)page->buffer,&iter);
//
//					debugFree(&command,"dropText command");
//					g_string_free(str,true);
//					pclose(fp);
//				}
//
//			g_strfreev(array);
//		}
//	else
//		dropTextFile=false;
//
//	gtk_drag_finish (context,true,true,time);
//#endif
//}
//
//void saveSession(Widget* widget,uPtr data)
////TODO//
//{
//printf("saveSession %i\n",(int)(long)data);
//#ifndef _USEQT5_
//	pageStruct*		page;
//	FILE*			fd=NULL;
//	char*			filename;
//	GtkTextMark*	mark;
//	GtkTextIter		iter;
//	int				linenumber;
//	GtkTextIter		markiter;
//	GList*			ptr;
//
//	asprintf(&filename,"%s/" KKEDITFOLDER,getenv("HOME"));
//	g_mkdir_with_parents(filename,493);
//	debugFree(&filename,"saveSession filename");
//	asprintf(&filename,"%s/" KKEDITFOLDER "/session",getenv("HOME"));
//	fd=fopen(filename,"w");
//	if (fd!=NULL)
//		{
//			for(int loop=0; loop<gtk_notebook_get_n_pages((GtkNotebook*)mainNotebook); loop++)
//				{
//					page=kkedit->getDocumentForTab(loop);
//					mark=gtk_text_buffer_get_insert((GtkTextBuffer*)page->buffer);
//					gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&iter,mark);
//					linenumber=gtk_text_iter_get_line(&iter);
//					fprintf(fd,"%i %s\n",linenumber,page->filePath);
//
//					ptr=newBookMarksList;
//					while(ptr!=NULL)
//						{
//							if(((bookMarksNew*)ptr->data)->page==page)
//								{
//									gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&markiter,(GtkTextMark*)((bookMarksNew*)ptr->data)->mark);
//									fprintf(fd,"%i ",gtk_text_iter_get_line(&markiter));
//									fprintf(fd,"%s\n",((bookMarksNew*)ptr->data)->label);
//								}
//							ptr=g_list_next(ptr);
//						}
//					fprintf(fd,"-1 endmarks\n");
//				}
//
//			fclose(fd);
//			debugFree(&filename,"saveSession filename");
//		}
//#endif
//}
//
//void restoreSession(Widget* widget,uPtr data)
////TODO//
//{
//printf("restoreSession %i\n",(int)(long)data);
//#if 0
//#ifdef _USEQT5_
//	FILE*		fd=NULL;
//	char*		filename;
//	char		buffer[2048];
//	int			intarg;
//	char		strarg[2048];
//	int			currentline;
//
//	kkedit->sessionBusy=true;
//
//	asprintf(&filename,"%s/" KKEDITFOLDER "/session",getenv("HOME"));
//	fd=fopen(filename,"r");
//	if (fd!=NULL)
//		{
//			closeAllTabs(NULL,0);
//			while(fgets(buffer,2048,fd)!=NULL)
//				{
//					sscanf(buffer,"%i %[^\n]s",(int*)&currentline,(char*)&strarg);
//					if(kkedit->openFile(strarg,currentline,true)==false)
//						{
//							kkedit->sessionBusy=true;
//							intarg=999;
//							while(intarg!=-1)
//								{
//									fgets(buffer,2048,fd);
//									sscanf(buffer,"%i",(int*)&intarg);
//								}
//						}
//					else
//						{
//							kkedit->sessionBusy=true;
//							intarg=999;
//							fgets(buffer,2048,fd);
//							sscanf(buffer,"%i %s",(int*)&intarg,(char*)&strarg);
//							while(intarg!=-1)
//								{
//									fgets(buffer,2048,fd);
//									sscanf(buffer,"%i",(int*)&intarg);
//								}
///*
//do bookmarks
////TODO//
//							fgets(buffer,2048,fd);
//							sscanf(buffer,"%i %s",(int*)&intarg,(char*)&strarg);
//							page=kkedit->getDocumentForTab(currentPage-1);
//							gtk_notebook_set_current_page((GtkNotebook*)mainNotebook,currentPage-1);
//							while(intarg!=-1)
//								{
//									if((bool)data==true)
//										{
//											gtk_text_buffer_get_iter_at_line((GtkTextBuffer*)page->buffer,&markiter,intarg);
//											gtk_text_buffer_place_cursor((GtkTextBuffer*)page->buffer,&markiter);
//											toggleBookmark(NULL,&markiter);
//										}
//									fgets(buffer,2048,fd);
//									sscanf(buffer,"%i %s",(int*)&intarg,(char*)&strarg);
//								}
//							buf->textBuffer=(GtkTextBuffer*)page->buffer;
//							buf->scroll2Line((GtkTextView*)page->view,currentline);					
//*/
//						}
//				}
//			fclose(fd);
//			debugFree(&filename,"restoreSession filename");
//		}
//
//	kkedit->sessionBusy=false;
//#else
//	FILE*		fd=NULL;
//	char*		filename;
//	char		buffer[2048];
//	int			intarg;
//	char		strarg[2048];
//	pageStruct*	page;
//	GtkTextIter	markiter;
//	int			currentline;
//	TextBuffer*	buf=new TextBuffer;
//
//	closeAllTabs(NULL,NULL);
//	while(gtk_events_pending())
//		gtk_main_iteration_do(false);
//
//	kkedit->sessionBusy=true;
//	gtk_widget_freeze_child_notify((GtkWidget*)mainNotebook);
//
//	asprintf(&filename,"%s/" KKEDITFOLDER "/session",getenv("HOME"));
//	fd=fopen(filename,"r");
//	if (fd!=NULL)
//		{
//			closeAllTabs(NULL,NULL);
//			while(fgets(buffer,2048,fd)!=NULL)
//				{
//					sscanf(buffer,"%i %[^\n]s",(int*)&currentline,(char*)&strarg);
//					if(kkedit->openFile(strarg,currentline,true)==false)
//						{
//						kkedit->sessionBusy=true;
//							intarg=999;
//							while(intarg!=-1)
//								{
//									fgets(buffer,2048,fd);
//									sscanf(buffer,"%i",(int*)&intarg);
//								}
//						}
//					else
//						{
//							fgets(buffer,2048,fd);
//							sscanf(buffer,"%i %s",(int*)&intarg,(char*)&strarg);
//							page=kkedit->getDocumentForTab(currentPage-1);
//							gtk_notebook_set_current_page((GtkNotebook*)mainNotebook,currentPage-1);
//							while(intarg!=-1)
//								{
//									if((bool)data==true)
//										{
//											gtk_text_buffer_get_iter_at_line((GtkTextBuffer*)page->buffer,&markiter,intarg);
//											gtk_text_buffer_place_cursor((GtkTextBuffer*)page->buffer,&markiter);
//											toggleBookmark(NULL,&markiter);
//										}
//									fgets(buffer,2048,fd);
//									sscanf(buffer,"%i %s",(int*)&intarg,(char*)&strarg);
//								}
//							buf->textBuffer=(GtkTextBuffer*)page->buffer;
//							buf->scroll2Line((GtkTextView*)page->view,currentline);					
//						}
//				}
//			fclose(fd);
//			debugFree(&filename,"restoreSession filename");
//		}
//
//	gtk_widget_thaw_child_notify((GtkWidget*)mainNotebook);
//	kkedit->sessionBusy=false;
//	while(gtk_events_pending())
//		gtk_main_iteration_do(false);
//	delete buf;
//#endif
//#endif
//}
//
//int showFileChanged(char* filename)
//{
//	gint		result=0;
//#ifndef _USEQT5_
//	GtkWidget*	dialog;
//	char*		message;
//
//	asprintf(&message,"File %s Has Changed on disk\nDo you want to reload it?",filename);
//	dialog=gtk_message_dialog_new(GTK_WINDOW(mainWindow),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_WARNING,GTK_BUTTONS_NONE,message);
//
//	gtk_dialog_add_buttons((GtkDialog*)dialog,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_REFRESH,GTK_RESPONSE_YES,NULL);
//	gtk_window_set_title(GTK_WINDOW(dialog),"Warning file changed!");
//
//	result=gtk_dialog_run(GTK_DIALOG(dialog));
//
//	gtk_widget_destroy(dialog);
//	debugFree(&message,"showFileChanged message");
//#endif
//	return(result);
//}
//
//#ifndef _USEQT5_
//void fileChangedOnDisk(GFileMonitor *monitor,GFile *file,GFile *other_file,GFileMonitorEvent event_type,gpointer user_data)
//#else
////TODO
//void fileChangedOnDisk(void)
//#endif
//{
//#ifndef _USEQT5_
//	pageStruct*		page=(pageStruct*)user_data;
//	GtkTextIter		start;
//	GtkTextIter		end;
//	gchar*			buffer;
//	long			filelen;
//	int				answer;
//
//	if(G_FILE_MONITOR_EVENT_CHANGES_DONE_HINT==event_type)
//		{
//			if((page->itsMe==false) && (page->showingChanged==false))
//				{
//					page->showingChanged=true;
//
//					if(noWarnings==false)
//						answer=showFileChanged(page->fileName);
//					else
//						answer=GTK_RESPONSE_YES;
//
//					page->showingChanged=false;
//					if(answer==GTK_RESPONSE_YES)
//						{
//							g_file_get_contents(page->filePath,&buffer,(gsize*)&filelen,NULL);
//							gtk_text_buffer_get_bounds((GtkTextBuffer*)page->buffer,&start,&end);
//							gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&start,&end);
//							gtk_text_buffer_delete_selection((GtkTextBuffer*)page->buffer,true,true);
//							gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&start);
//							gtk_text_buffer_insert((GtkTextBuffer*)page->buffer,&start,buffer,filelen);
//							debugFree(&buffer,"fileChangedOnDisk buffer");
//						}
//				}
//			else
//				page->itsMe=false;
//		}
//#endif
//}
//

