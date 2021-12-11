/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
 * functioncallbacks.cpp
 *
*/

#include "kkedit-includes.h"

int		currentFindPage=-1;
int		firstPage=-1;
int		pagesChecked=0;
int		itemsReplaced=-1;
struct	regexData
{
	int start;
	int	end;
};

char	*searchtext=NULL;
char	*replacetext=NULL;
bool	fromregexreplace=false;
bool	gotselection=false,fromregexsinglereplace=false;

#ifdef _BUILDDOCVIEWER_
void webKitGoBack(void)
{

	qobject_cast<QWebView*>(kkedit->webView)->page()->triggerAction(QWebPage::Back);
}

void webKitGoForward(void)
{
	qobject_cast<QWebView*>(kkedit->webView)->page()->triggerAction(QWebPage::Forward);
}

void webKitGoHome(void)
{
	if(g_file_test(htmlFile,G_FILE_TEST_EXISTS)==true)
		qobject_cast<QWebView*>(kkedit->webView)->load(QUrl(htmlURI));
}
#endif

PROTECTED void showDocView(int howtodisplay,char* text,const char* title)
{
#ifdef _BUILDDOCVIEWER_

	qobject_cast<QMainWindow*>(kkedit->docView)->setWindowTitle(title);

	if(howtodisplay==USEURI)
		{
			if(strcasecmp(thePage,"file://(null)")==0)
				{
					debugFree(&thePage,"showDocView thePage");
					asprintf(&thePage,"https://www.google.co.uk/search?q=%s",text);
				}

			qobject_cast<QWebView*>(kkedit->webView)->load(QUrl(thePage));
		}

	if(howtodisplay==USEFILE)
			qobject_cast<QWebView*>(kkedit->webView)->load(QUrl(htmlURI));

	qobject_cast<QAction*>(kkedit->toggleDocViewMenuItem)->setText("Hide Docviewer");
	kkedit->docView->show();
	kkedit->docviewerVisible=true;
#else
	char*		command;
	command=NULL;
	if(howtodisplay==USEURI)
		{
			if(strcasecmp(thePage,"file://(null)")!=0)
				asprintf(&command,"%s %s &",browserCommand,thePage);
			else
				asprintf(&command,"%s https://www.google.co.uk/search?q=%s",browserCommand,text);
		}

	if(howtodisplay==USEFILE)
		asprintf(&command,"%s %s",browserCommand,htmlURI);

	if(command!=NULL)
		{
			system(command);
			debugFree(&command,"showDocView command");
		}

#endif
	if(thePage!=NULL)
		debugFree(&thePage,"showDocView thePage");
	thePage=NULL;
		
	return;
}

void searchGtkDocs(Widget* widget,uPtr data)
{
	char*		selection=NULL;
	char*		searchdata[2048][2];
	char		line[1024];
	FILE*		fp;
	FILE*		fd;
	char*		command=NULL;
	char*		ptr=NULL;
	char*		funcname;
	char*		foldername;
	char*		tempstr;
	char*		link;
	int			cnt=0;

	DocumentClass	*document=kkedit->getDocumentForTab(-1);

	if(document==NULL)
		return;

	for(int loop=0;loop<2048;loop++)
		{
			searchdata[loop][0]=NULL;
			searchdata[loop][1]=NULL;
		}

	if((gpointer)data!=NULL)
		selection=strdup((char*)data);
	else
		{
			selection=strdup(document->textCursor().selectedText().toUtf8().constData());
		}

	if(selection!=NULL)
		{
			asprintf(&command,"find /usr/share/gtk-doc/html -iname \"*.devhelp2\" -exec grep -iHe %s '{}' \\;",selection);
			fp=popen(command,"r");
			while(fgets(line,1024,fp))
				{
					ptr=strstr(line,"name=\"");
					if(ptr!=NULL)
						{
							funcname=globalSlice->sliceBetween(line,(char*)"name=\"",(char*)"\" link=");
							if(globalSlice->getResult()==0)
								{
									if(strstr(funcname,selection)!=NULL)
										{
											if(cnt<2048)
												{
													tempstr=globalSlice->sliceBetween(line,(char*)"",(char*)":");
													if(tempstr!=NULL)
														{
															foldername=g_path_get_dirname(tempstr);
															link=globalSlice->sliceBetween(line,(char*)"link=\"",(char*)"\"");
															if((foldername!=NULL) && (link!=NULL))
																{
																	searchdata[cnt][0]=strdup(funcname);
																	asprintf(&searchdata[cnt][1],"%s/%s",foldername,link);
																	debugFree(&foldername,"seachGtkDocs foldername");
																	debugFree(&link,"seachGtkDocs link");
																	cnt++;
																}
															debugFree(&tempstr,"seachGtkDocs tempstr");
														}
												}
										}
									debugFree(&funcname,"seachGtkDocs funcname");
									funcname=NULL;
								}
						}
				}

			if(cnt>1)
				{
					fd=fopen(htmlFile,"w");
					if(fd!=NULL)
						{								
							fprintf(fd,"<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n");
							fprintf(fd,"<html>\n");
							fprintf(fd,"<body>\n");

							for(int loop=0;loop<cnt;loop++)
								{
									fprintf(fd,"<a href=\"%s\">%s</a><br>\n",searchdata[loop][1],searchdata[loop][0]);
								}
							fprintf(fd,"</body>\n");
							fprintf(fd,"</html>\n");
							fclose(fd);
							thePage=strdup(htmlURI);
						}
				}
			else
				{
					asprintf(&thePage,"file://%s",searchdata[0][1]);
				}

			showDocView(USEURI,selection,"Gtk Docs");
		}

	for(int loop=0;loop<cnt;loop++)
		{
			if(searchdata[loop][0]!=NULL)
				debugFree(&searchdata[loop][0],"seachGtkDocs searchdata[loop][0]");
			if(searchdata[loop][1]!=NULL)
				debugFree(&searchdata[loop][1],"seachGtkDocs searchdata[loop][1]");
		}
	if((selection!=NULL) && ((gpointer)data==NULL))
		debugFree(&selection,"seachGtkDocs selection");
}


//find in doxy docs
void doxyDocs(Widget* widget,uPtr data)
//TODO//
{
printf("doxyDocs %i\n",(int)(long)data);

#ifndef _USEQT5_
	pageStruct*	page=kkedit->getDocumentForTab(-1);
	GtkTextIter	start;
	GtkTextIter	end;
	char*		selection=NULL;
	char*		findcommand=NULL;
	char*		headcommand=NULL;
	char		line[1024];
	char		titleline[1024];
	FILE*		findfile;
	FILE*		headfile;

	if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
		selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);

	if(selection!=NULL)
		{
			showBarberPole("Searching ...");

			asprintf(&findcommand,"find %s/html -name \"*.html\"",page->dirName);
			findfile=popen(findcommand,"r");
			while(fgets(line,1024,findfile))
				{
					gtk_main_iteration_do(false);
					line[strlen(line)-1]=0;
					asprintf(&headcommand,"head \"%s\"|grep title",line);
					headfile=popen(headcommand,"r");
					while(fgets(titleline,1024,headfile))
						{
							if(strstr(titleline,selection)!=NULL)
								{
									if(thePage!=NULL)
										debugFree(&thePage,"doxyDocs thePage");
									asprintf(&thePage,"file://%s",line);
									while(fgets(titleline,1024,headfile));
									while(fgets(titleline,1024,findfile));
									pclose(headfile);
									pclose(findfile);
									debugFree(&headcommand,"doxyDocs headcommand 1");
									showDocView(USEURI,thePage,"Doxygen Documentation");
									killBarberPole();
									return;
								}
						}
					pclose(headfile);
				}
			pclose(findfile);
			if(headcommand!=NULL)
				debugFree(&headcommand,"doxyDocs headcommand 2");
			debugFree(&findcommand,"doxyDocs findcommand 2");
			killBarberPole();
		}
#endif
}

//search QT5 Documentaiion
void searchQT5Docs(Widget* widget,uPtr data)
{
	char*		selection=NULL;
	char		line[1024];
	FILE*		fp;
	FILE*		fd;
	char*		command=NULL;
	int			cnt=0;
	GString*	str;
	char*		func=NULL;

	DocumentClass	*document=kkedit->getDocumentForTab(-1);

	if(document==NULL)
		return;

	if((gpointer)data!=NULL)
		selection=strdup((char*)data);
	else
		{
			selection=strdup(document->textCursor().selectedText().toUtf8().constData());
		}

	if(selection!=NULL)
		{
			str=g_string_new(selection);
			str=g_string_ascii_down(str);

			asprintf(&command,"find %s -iname \"%s*.html\"|sed 's/.html$//'|sort",QT5DOCSDIR,str->str);

			fd=fopen(htmlFile,"w");
			if(fd!=NULL)
				{								
					fprintf(fd,"<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n");
					fprintf(fd,"<html>\n");
					fprintf(fd,"<body>\n");

					fp=popen(command,"r");
					while(fgets(line,1024,fp))
						{
							func=g_path_get_basename(line);
							if((func !=NULL) && (strcasecmp(func,"")!=0))
								{
									cnt++;
									fprintf(fd,"<a href=\"%s.html\">%s</a><br>\n",line,func);
								}
						}
					fprintf(fd,"</body>\n");
					fprintf(fd,"</html>\n");
					fclose(fd);
					fclose(fp);
					debugFree(&command,"searchQT5Docs command");
				}
			if(cnt==0)
				asprintf(&thePage,"file://(null)");
			else
				thePage=strdup(htmlURI);

			showDocView(USEURI,(char*)str->str,"Qt5 Docs");

			g_string_free(str,true);
			debugFree(&selection,"searchQT5Docs selection");
		}
}




void defSearchFromBar(void)
{
fprintf(stderr,"void defSearchFromBar(void)\n");
#if 0
	functionData* fdata;

	functionSearchText=strdup(kkedit->findDefWidget->property("text").toByteArray().constData());

	if(functionSearchText!=NULL)
		{
			printf("defSearchFromBar %s\n",functionSearchText);
			fdata=getFunctionByName(functionSearchText,true,true);
			if(fdata!=NULL)
				{
					goToDefine(fdata);
					destroyData(fdata);
				}
			debugFree(&functionSearchText,"defSearchFromBar functionSearchText");
		}
#endif
}

#ifndef _USEQT5_
void docSearchFromBar(Widget* widget,uPtr data)
#else
void docSearchFromBar(void)
#endif
{
#ifdef _USEQT5_
	const char* text=kkedit->findApiWidget->property("text").toByteArray().constData();
#else
	const char* text=gtk_entry_get_text((GtkEntry*)data);
#endif

	if(text!=NULL && strlen(text)>0)
		searchGtkDocs(NULL,(uPtr)text);
}

#ifdef _BUILDDOCVIEWER_
void docSearchInPageFoward(Widget* widget,uPtr data)
//TODO//
{
#ifndef _USEQT5_
	const char* text=gtk_entry_get_text((GtkEntry*)data);

	if(text!=NULL && strlen(text)>0)
		webkit_web_view_search_text((WebKitWebView*)webView,text,false,true,true);
#endif
}

void docSearchInPageBack(Widget* widget,uPtr data)
//TODO//
{
#ifndef _USEQT5_
	const char* text=gtk_entry_get_text((GtkEntry*)data);

	if(text!=NULL && strlen(text)>0)
		webkit_web_view_search_text((WebKitWebView*)webView,text,false,false,true);
#endif
}
#endif

#ifndef _USEQT5_
void qt5DocSearchFromBar(Widget* widget,uPtr data)
#else
void qt5DocSearchFromBar(void)
#endif
{
#ifndef _USEQT5_
	const char* text=gtk_entry_get_text((GtkEntry*)data);
#else
	const char* text=kkedit->findQtApiWidget->property("text").toByteArray().constData();
#endif

	if(text!=NULL && strlen(text)>0)
		searchQT5Docs(NULL,(uPtr)text);
}

void doAllFiles(int dowhat,bool found)
{
#ifndef _USEQT5_
	pageStruct*	page=NULL;

	if((findInAllFiles==true) && (found==false))
		{
			if(dowhat==FINDNEXT)
				{
					currentFindPage=gtk_notebook_get_current_page((GtkNotebook*)mainNotebook)+1;
					if(currentFindPage==gtk_notebook_get_n_pages((GtkNotebook*)mainNotebook))
						currentFindPage=0;
				}
			else
				{
					currentFindPage=gtk_notebook_get_current_page((GtkNotebook*)mainNotebook)-1;
					if(currentFindPage==-1)
						currentFindPage=gtk_notebook_get_n_pages((GtkNotebook*)mainNotebook)-1;
				}

			pagesChecked++;
			if(pagesChecked>gtk_notebook_get_n_pages((GtkNotebook*)mainNotebook))
				{
					currentFindPage=-1;
					gtk_notebook_set_current_page((GtkNotebook*)mainNotebook,firstPage);
					pagesChecked=0;
					return;
				}
		}

	gtk_notebook_set_current_page((GtkNotebook*)mainNotebook,currentFindPage);
	page=kkedit->getDocumentForTab(currentFindPage);

	if(dowhat==FINDNEXT)
		{
			gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&page->iter);
			gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&page->match_start);
			gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&page->match_end);
			gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
		}
	else
		{
			gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&page->iter);
			gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&page->match_start);
			gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&page->match_end);
			gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
		}

	if(useRegex==false)
		basicFind(dowhat);
	else
		regexFind(dowhat);
#endif
}

int findNextRegex(pageStruct* page,int charpos,int thisregxnum)
{
	int retval=-1;
	regexData*				xdata;

	for(int j=thisregxnum;j<(int)g_slist_length(page->regexList);j++)
		{
			xdata=(regexData*)g_slist_nth_data(page->regexList,j);
			if(xdata->start>charpos)
				{
					retval=j;
					return(j);
				}
		}
	return(retval);
}

int findPrevRegex(pageStruct* page,int charpos)
{
	int retval=-1;
	regexData*				xdata;

	for(int j=g_slist_length(page->regexList)-1;j>=0;j--)
		{
			xdata=(regexData*)g_slist_nth_data(page->regexList,j);
			if(xdata->start<charpos)
				{
					retval=j;
					return(j);
				}
		}
	return(retval);
}

int findThisRegex(pageStruct* page,int charpos)
{
	int			retval=-1;
	regexData*	xdata;

	for(int j=0;j<(int)g_slist_length(page->regexList);j++)
		{
			xdata=(regexData*)g_slist_nth_data(page->regexList,j);
			if(xdata->start==charpos)
				{
					retval=j;
					return(j);
				}
		}
	return(retval);
}

void regexFind(int dowhat)
{
#ifndef _USEQT5_
	GRegex*					regex;
	GMatchInfo*				match_info=NULL;
	GRegexCompileFlags		compileflags=(GRegexCompileFlags)(G_REGEX_MULTILINE|G_REGEX_EXTENDED);
	GRegexMatchFlags		matchflags=(GRegexMatchFlags)(G_REGEX_MATCH_NOTBOL|G_REGEX_MATCH_NOTEOL);
	pageStruct*				page=NULL;
	regexData*				xdata;
	GtkTextIter				startiter,enditer;
	int						charstartpos;
	int						charendpos;
	int						startpos,endpos;
	char*					text=NULL;
	char*					reptext=NULL;
	TextBuffer*				textbuffer=NULL;
	bool					dofindnext=false;
	bool					dofindprev=false;
	int						testformatch;
	GtkTextIter				hastart,haend;

	page=kkedit->getDocumentForTab(currentFindPage);
	if(gtk_entry_get_text_length((GtkEntry*)findBox)==0)
		return;

	gotselection=gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&startiter,&enditer);

	if(gotselection==false || ((fromregexsinglereplace==true) && (gotselection==true)))
		{
			gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&startiter);
			gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&enditer);
		}

	if(replaceAll==true)
		{
			if(gotselection==true)
				{
					if(fromregexsinglereplace==true)
						{
							gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&startiter);
							gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&enditer);
						}
				}
		}

	textbuffer=new TextBuffer((GtkTextBuffer*)page->buffer);

	if(searchtext!=NULL)
		{
			if((strcmp(searchtext,gtk_entry_get_text((GtkEntry*)findBox))!=0) || (page->regexMatchNumber==-1))
				page->regexMatchNumber=-1;
			debugFree(&searchtext,"regexFind searchtext");
		}

	if(replacetext!=NULL)
		{
			if((strcmp(replacetext,gtk_entry_get_text((GtkEntry*)replaceBox))!=0) || (page->regexMatchNumber==-1))
				page->regexMatchNumber=-1;
			debugFree(&replacetext,"regexFind replacetext");
		}

	searchtext=strdup((char*)gtk_entry_get_text((GtkEntry*)findBox));
	replacetext=strdup((char*)gtk_entry_get_text((GtkEntry*)replaceBox));

	if(insensitiveSearch==true)
		compileflags=(GRegexCompileFlags)(compileflags|G_REGEX_CASELESS);

	regex=g_regex_new(searchtext,(GRegexCompileFlags)compileflags,matchflags,NULL);
	gtk_text_buffer_begin_user_action((GtkTextBuffer*)page->buffer);

	text=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&startiter,&enditer,false);

	if(page->regexMatchNumber==-1)
		{
			if(page->regexList!=NULL)
				{
					g_slist_free_full(page->regexList,free);
					page->regexList=NULL;
				}

			gtk_text_buffer_remove_tag_by_name((GtkTextBuffer*)page->buffer,"highlighttag",&startiter,&enditer);
			g_regex_match_full(regex,text,-1,0,matchflags,&match_info,NULL);
			while (g_match_info_matches(match_info))
				{
					if(g_match_info_fetch_pos(match_info,0,&startpos,&endpos))
						{
							charstartpos=g_utf8_pointer_to_offset(text,&text[startpos]);
							charendpos=g_utf8_pointer_to_offset(text,&text[endpos]);
							xdata=(regexData*)malloc(sizeof(regexData));
							xdata->start=charstartpos;
							xdata->end=charendpos;
							page->regexList=g_slist_append(page->regexList,xdata);
							if(hightlightAll==true)
								{	
									gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&hastart);
									gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&haend);
									gtk_text_iter_set_offset(&hastart,charstartpos);
									gtk_text_iter_set_offset(&haend,charendpos);
									gtk_text_buffer_apply_tag_by_name((GtkTextBuffer*)page->buffer,"highlighttag",&hastart,&haend);
								}
						}
					g_match_info_next(match_info,NULL);
				}
			page->regexMatchNumber=0;
		}

	switch (dowhat)
		{
//forward search
			case FINDNEXT:
				fromregexreplace=false;
				page->regexMatchNumber=findNextRegex(page,textbuffer->charPos,page->regexMatchNumber);
				if(page->regexMatchNumber!=-1)
					{
						xdata=(regexData*)g_slist_nth_data(page->regexList,page->regexMatchNumber);
						if(xdata!=NULL)
							{
								gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&startiter);
								gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&enditer);
								gtk_text_iter_set_offset(&startiter,xdata->start);
								gtk_text_iter_set_offset(&enditer,xdata->end);
								gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&startiter,&enditer);
								scrollToIterInPane(page,&startiter);
								page->regexMatchNumber++;
							}
					}
				else
					{
						if(findInAllFiles==true)
							{
								page->regexMatchNumber=-1;
								currentFindPage++;
								pagesChecked++;
								if(currentFindPage>=gtk_notebook_get_n_pages((GtkNotebook*)mainNotebook))
									currentFindPage=0;
								gtk_notebook_set_current_page((GtkNotebook*)mainNotebook,currentFindPage);
								page=kkedit->getDocumentForTab(currentFindPage);
								page->regexMatchNumber=-1;
								dofindnext=true;
								gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&startiter);
								gtk_text_buffer_place_cursor((GtkTextBuffer*)page->buffer,&startiter);

								if(pagesChecked>gtk_notebook_get_n_pages((GtkNotebook*)mainNotebook))
									{
										dofindnext=false;
										pagesChecked=0;
									}
							}
						else if(wrapSearch==true)
							{
								testformatch=findNextRegex(page,0,0);
								if(testformatch!=-1)
									dofindnext=true;
								gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&startiter);
								gtk_text_buffer_place_cursor((GtkTextBuffer*)page->buffer,&startiter);
							}
					}
				break;

			case FINDPREV:
				fromregexreplace=false;
				page->regexMatchNumber=findPrevRegex(page,textbuffer->charPos);
				if(page->regexMatchNumber!=-1)
					{
						xdata=(regexData*)g_slist_nth_data(page->regexList,page->regexMatchNumber);
						if(xdata!=NULL)
							{
								gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&startiter);
								gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&enditer);
								gtk_text_iter_set_offset(&startiter,xdata->start);
								gtk_text_iter_set_offset(&enditer,xdata->end);
								gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&startiter,&enditer);
								scrollToIterInPane(page,&startiter);
								page->regexMatchNumber--;
								if(page->regexMatchNumber==-1)
									page->regexMatchNumber=0;
							}
					}
				else
					{
						if(findInAllFiles==true)
							{
								page->regexMatchNumber=-1;
								currentFindPage--;
								if(currentFindPage<0)
									currentFindPage=gtk_notebook_get_n_pages((GtkNotebook*)mainNotebook)-1;
								gtk_notebook_set_current_page((GtkNotebook*)mainNotebook,currentFindPage);
								page=kkedit->getDocumentForTab(currentFindPage);
								page->regexMatchNumber=-1;
								dofindprev=true;
								gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&enditer);
								gtk_text_buffer_place_cursor((GtkTextBuffer*)page->buffer,&enditer);
								scrollToIterInPane(page,&startiter);
							}
						else if(wrapSearch==true)
							{
								testformatch=findNextRegex(page,0,0);
								if(testformatch!=-1)
									dofindprev=true;
								gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&enditer);
								gtk_text_buffer_place_cursor((GtkTextBuffer*)page->buffer,&enditer);
								scrollToIterInPane(page,&startiter);
							}
					}
				break;

			case FINDREPLACE:
								fromregexreplace=false;
fromregexsinglereplace=false;
				if(replaceAll==true)
					{
	fromregexreplace=true;					
						int startloop,endloop;
						if(findInAllFiles==true)
							{
								if(yesNo((char*)"Do you want to replace in ALL open files?"),(char*)""==GTK_RESPONSE_CANCEL)
									return;
								startloop=0;
								endloop=gtk_notebook_get_n_pages((GtkNotebook*)mainNotebook);
							}
						else
							{
								startloop=gtk_notebook_get_current_page((GtkNotebook*)mainNotebook);
								endloop=gtk_notebook_get_current_page((GtkNotebook*)mainNotebook)+1;
							}

						for(int j=startloop;j<endloop;j++)
							{
								page=kkedit->getDocumentForTab(j);
								if(findInAllFiles==true)
									{
										gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&startiter);
										gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&enditer);
									}
								text=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&startiter,&enditer,false);
								regex=g_regex_new(searchtext,(GRegexCompileFlags)(G_REGEX_CASELESS|G_REGEX_EXTENDED),(GRegexMatchFlags)0,NULL);
								g_regex_match_full(regex,text,-1,0,matchflags,&match_info,NULL);
								while (g_match_info_matches(match_info))
									{
										itemsReplaced++;
										g_match_info_next(match_info,NULL);
									}

								reptext=g_regex_replace(regex,text,-1,0,replacetext,(GRegexMatchFlags)0,NULL);
								if((reptext!=NULL) && (strcmp(reptext,text)!=0))
									{
										gtk_text_buffer_delete((GtkTextBuffer*)page->buffer,&startiter,&enditer);
										gtk_text_buffer_insert((GtkTextBuffer*)page->buffer,&startiter,reptext,-1);
										page->regexMatchNumber=-1;
										debugFree(&reptext,"regexfind reptext");
									}
								debugFree(&text,"replace all regex text");
							}
					}
				else
					{
						page->regexMatchNumber=findThisRegex(page,textbuffer->charPos);
						xdata=(regexData*)g_slist_nth_data(page->regexList,page->regexMatchNumber);
						if(xdata!=NULL)
							{
								gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&startiter);
								gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&enditer);
								gtk_text_iter_set_offset(&startiter,xdata->start);
								gtk_text_iter_set_offset(&enditer,xdata->end);

								regex=g_regex_new(searchtext,(GRegexCompileFlags)(G_REGEX_CASELESS|G_REGEX_EXTENDED),(GRegexMatchFlags)0,NULL);
								text=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&startiter,&enditer,false);
								reptext=g_regex_replace(regex,text,-1,0,replacetext,(GRegexMatchFlags)0,NULL);
								if((reptext!=NULL) && (strcmp(reptext,text)!=0))
									{
										gtk_text_buffer_delete((GtkTextBuffer*)page->buffer,&startiter,&enditer);
										gtk_text_buffer_insert((GtkTextBuffer*)page->buffer,&startiter,reptext,-1);
										page->regexMatchNumber=-1;
										debugFree(&reptext,"regexfind reptext");
										dofindnext=true;
										fromregexsinglereplace=true;
									}
							}
					}				
				break;
		}

	debugFree(&text,"findregex text");
	delete textbuffer;

	if(dofindnext==true)
		regexFind(FINDNEXT);
	if(dofindprev==true)
		regexFind(FINDPREV);
#endif
}

void pasteFRClip(Widget* widget,uPtr data)
//TODO//
{
#ifndef _USEQT5_
	gtk_entry_set_text((GtkEntry*)data,gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget));
#endif
}

void doSearchPrefs(int state)
{
	for(int j=FRCASE;j<FRMAXSWITCHES;j++)
		{
			switch(j)
				{
					case FRCASE:
						kkedit->insensitiveSearch=reinterpret_cast<QCheckBox*>(kkedit->frSwitches[FRCASE])->isChecked();
						break;
					case FRUSEREGEX:
						kkedit->useRegex=reinterpret_cast<QCheckBox*>(kkedit->frSwitches[FRUSEREGEX])->isChecked();
						break;
					case FRWRAP:
						kkedit->wrapSearch=reinterpret_cast<QCheckBox*>(kkedit->frSwitches[FRWRAP])->isChecked();
						break;
					case FRALLFILES:
						kkedit->findInAllFiles=reinterpret_cast<QCheckBox*>(kkedit->frSwitches[FRALLFILES])->isChecked();
						break;
					case FRHIGHLIGHTALL:
						kkedit->hightlightAll=reinterpret_cast<QCheckBox*>(kkedit->frSwitches[FRHIGHLIGHTALL])->isChecked();
						break;
					case FRREPLACEALL:
						kkedit->replaceAll=reinterpret_cast<QCheckBox*>(kkedit->frSwitches[FRREPLACEALL])->isChecked();
						if(kkedit->replaceAll==false)
							reinterpret_cast<QPushButton*>(kkedit->frReplace)->setText("Replace");
						else
							reinterpret_cast<QPushButton*>(kkedit->frReplace)->setText("Replace All");
						break;
				}
		}
}

#ifndef _USEQT5_
void doLiveSearch(GtkWidget* widget,GdkEvent *event,gpointer data)
#else
//TODO//
void doLiveSearch(void)
#endif
{
printf("doLiveSearch\n");
#ifndef _USEQT5_
	pageStruct* 			page=kkedit->getDocumentForTab(-1);
	GtkSourceSearchFlags	flags;
	char*					searchtext;
	int						modkey=((GdkEventKey*)event)->state;

	gtk_text_buffer_begin_user_action((GtkTextBuffer*)page->buffer);

	flags=(GtkSourceSearchFlags)(GTK_SOURCE_SEARCH_TEXT_ONLY|GTK_SOURCE_SEARCH_CASE_INSENSITIVE);

	searchtext=g_strcompress(gtk_entry_get_text((GtkEntry*)widget));

	if(modkey==0)
		{
			if(!gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end))
				gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&page->iter,gtk_text_buffer_get_insert((GtkTextBuffer*)page->buffer));

			if(gtk_source_iter_forward_search(&page->match_end,searchtext,flags,&page->match_start,&page->match_end,NULL))
				{
					gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
					scrollToIterInPane(page,&page->match_start);
					page->iter=page->match_end;
				}
			else
				{
					gtk_text_buffer_get_start_iter((GtkTextBuffer*)page->buffer,&page->iter);
					if(gtk_source_iter_forward_search(&page->iter,searchtext,flags,&page->match_start,&page->match_end,NULL))
						{
							gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
							scrollToIterInPane(page,&page->match_start);
							page->iter=page->match_end;
						}
				}
		}
	else
		{
			if(!gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end))
				gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&page->iter,gtk_text_buffer_get_insert((GtkTextBuffer*)page->buffer));
			if(gtk_source_iter_backward_search(&page->match_start,searchtext,flags,&page->match_start,&page->match_end,NULL))
				{
					gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
					scrollToIterInPane(page,&page->match_start);
					page->iter=page->match_start;
				}
			else
				{
					gtk_text_buffer_get_end_iter((GtkTextBuffer*)page->buffer,&page->iter);
					if(gtk_source_iter_backward_search(&page->iter,searchtext,flags,&page->match_start,&page->match_end,NULL))
						{
							gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&page->match_start,&page->match_end);
							scrollToIterInPane(page,&page->match_start);
							page->iter=page->match_start;
						}
				}
		}
	gtk_text_buffer_end_user_action((GtkTextBuffer*)page->buffer);
#endif
}
