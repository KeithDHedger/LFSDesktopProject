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
