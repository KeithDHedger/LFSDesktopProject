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
