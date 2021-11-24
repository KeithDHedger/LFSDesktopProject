/******************************************************
*
*     ©keithhedger Sat 11 Oct 11:51:45 BST 2014
*     kdhedger68713@gmail.com
*
*     completion.cpp
* 
******************************************************/

//#include "kkedit-includes.h"
#include "completion.h"

#ifndef _USEQT5_
#include <gtksourceview/completion-providers/words/gtksourcecompletionwords.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcecompletion.h>
#include <gtksourceview/gtksourcecompletioninfo.h>
#include <gtksourceview/gtksourcecompletionitem.h>
#include <gtksourceview/gtksourcelanguagemanager.h>
#else
//TODO//
#endif

struct _FunctionProviderClass
{
#ifndef _USEQT5_
	GObjectClass			parent_class;
#else
//TODO//
#endif
};

#ifndef _USEQT5_
G_DEFINE_TYPE_WITH_CODE(FunctionProvider,function_provider,G_TYPE_OBJECT,G_IMPLEMENT_INTERFACE (GTK_TYPE_SOURCE_COMPLETION_PROVIDER,function_provider_iface_init))

FunctionProvider*			funcProv;
FunctionProvider*			varsProv;
FunctionProvider*			customProv;

#else
//TODO//
#endif


bool						forcePopup=false;
#ifndef _USEQT5_
GtkSourceCompletionWords*	docWordsProv;
#else
//TODO//
#endif

#ifndef _USEQT5_
gchar* function_provider_get_name(GtkSourceCompletionProvider* provider)
#else
//TODO//
gchar* function_provider_get_name(void)
#endif
{
#ifndef _USEQT5_
	return g_strdup(((FunctionProvider *)provider)->name);
#endif
return(NULL);
}

#ifndef _USEQT5_
gint function_provider_get_priority(GtkSourceCompletionProvider* provider)
#else
//TODO//
gint function_provider_get_priority(void)
#endif
{
#ifndef _USEQT5_
	return((FunctionProvider*)provider)->priority;
#endif
return(0);
}

#ifndef _USEQT5_
gboolean function_provider_match(GtkSourceCompletionProvider* provider,GtkSourceCompletionContext*context)
#else
//TODO//
gboolean function_provider_match(void)
#endif
{
	return true;
}

#ifndef _USEQT5_
char* get_word_at_iter(GtkTextIter* iter,GtkTextBuffer *buffer)
#else
//TODO//
char* get_word_at_iter(void)
#endif
{
#ifndef _USEQT5_
	GtkTextIter*	startiter;
	char*			word;

	if(gtk_text_iter_ends_word(iter)==true)
		{
			startiter=gtk_text_iter_copy(iter);
			gtk_text_iter_backward_word_start(startiter);
			word=gtk_text_buffer_get_text(buffer,startiter,iter,true);
			if(strlen(word)>=autoShowMinChars)
				return(word);
		}
#endif
	return(NULL);
}

#ifndef _USEQT5_
GdkPixbuf* function_provider_get_icon(GtkSourceCompletionProvider* provider)
#else
//TODO//
void function_provider_get_icon(void)
#endif
{
#ifndef _USEQT5_
	FunctionProvider* tp=(FunctionProvider*)provider;

//	if (tp->icon==NULL)
//		{
//			GtkIconTheme* theme=gtk_icon_theme_get_default();
//			tp->icon=gtk_icon_theme_load_icon(theme,GTK_STOCK_DIALOG_INFO,16,(GtkIconLookupFlags)0,NULL);
//		}
	return tp->icon;
#endif
}

//completion
#ifndef _USEQT5_
GList* addPropsFromWord(pageStruct* page,char* theword,FunctionProvider* prov)
#else
//TODO//
GList* addPropsFromWord(void)
#endif
{
#ifndef _USEQT5_
	char*	infostr;
	GList*	newlist=NULL;
	GList*	customlist=customProv->proposals;
	char*	text;

	if(page->filePath==NULL)
		return(newlist);

	customlist=prov->proposals;
	while(customlist!=NULL)
		{
			text=gtk_source_completion_proposal_get_text((GtkSourceCompletionProposal*)customlist->data);
			infostr=gtk_source_completion_proposal_get_info((GtkSourceCompletionProposal*)customlist->data);
			if(text!=NULL)
				{
					if(strncasecmp(text,theword,strlen(theword))==0)
						newlist=g_list_append(newlist,gtk_source_completion_item_new(text,text,NULL,infostr));
					free(text);
				}
			customlist=customlist->next;
		}
	return(newlist);
#endif
return(NULL);
}

#ifndef _USEQT5_
void function_provider_populate(GtkSourceCompletionProvider* provider,GtkSourceCompletionContext* context)
#else
//TODO//
void function_provider_populate(void)
#endif
{
#ifndef _USEQT5_
	GtkTextIter 	iter;
	gchar*			word=NULL;
	GtkTextBuffer*	buffer;
	GList*			wordlist;
	pageStruct*		page;

	if(forcePopup==true)
		{
			gtk_source_completion_context_add_proposals(context,provider,((FunctionProvider *)provider)->proposals,true);
			return;
		}

	gtk_source_completion_context_get_iter(context,&iter);
	buffer=gtk_text_iter_get_buffer(&iter);

	word=get_word_at_iter(&iter,buffer);

	if(word==NULL || strlen(word)<autoShowMinChars)
		{
			g_free (word);
			gtk_source_completion_context_add_proposals(context,provider,NULL,true);
			return;
		}

	if(word!=NULL)
		{
			page=getDocumentData(-1);
			if(page!=NULL)
				{
					wordlist=addPropsFromWord(page,word,(FunctionProvider*)provider);
					gtk_source_completion_context_add_proposals(context,provider,wordlist,true);
					if(wordlist!=NULL)
						g_list_free_full(wordlist,g_object_unref);
				}
		}
	if(word!=NULL)
		free(word);
#endif
}

#ifndef _USEQT5_
void function_provider_iface_init(GtkSourceCompletionProviderIface* iface)
#else
//TODO//
void function_provider_iface_init(void)
#endif
{
#ifndef _USEQT5_
	iface->get_name=function_provider_get_name;
	iface->populate=function_provider_populate;
	iface->match=function_provider_match;
	iface->get_priority=function_provider_get_priority;
	iface->get_icon=function_provider_get_icon;
#endif
}

#ifndef _USEQT5_
void function_provider_class_init(FunctionProviderClass* klass)
#else
//TODO//
void function_provider_class_init(void)
#endif
{
}

#ifndef _USEQT5_
void function_provider_init(FunctionProvider* self)
#else
void function_provider_init(void)
#endif
{
#ifndef _USEQT5_
	self->icon=function_provider_get_icon(GTK_SOURCE_COMPLETION_PROVIDER(self));
#endif
}

void addProp(pageStruct* page)
{
#ifndef _USEQT5_
	char*		functions=NULL;
	char		tmpstr[1024];
	char*		lineptr;
	char*		correctedstr;
	char		functype[63];
	char		infostr[1024];
	char*		holdstr;
	FILE*		fd=NULL;
	char		buffer[2048];
	char*		customfile;

	if(page->filePath==NULL)
		return;

//custom
	asprintf(&customfile,"%s/%s",getenv("HOME"),CUSTOMWORDFILE);
	fd=fopen(customfile,"r");
	if(fd!=NULL)
		{
			while(feof(fd)==0)
				{
					buffer[0]=0;
					fgets(buffer,2048,fd);
					if(strlen(buffer)>2)
						{
							buffer[strlen(buffer)-1]=0;
							customProv->proposals=g_list_append(customProv->proposals,gtk_source_completion_item_new(buffer,buffer,NULL,NULL));
						}
				}
			fclose(fd);
		}

//function and variable compitions
	getRecursiveTagList(page->filePath,&functions);
	lineptr=functions;

	while (lineptr!=NULL)
		{
			tmpstr[0]=0;
			sscanf (lineptr,"%s",tmpstr);
			sscanf (lineptr,"%*s %*s %*i %[^\n]s",infostr);
			holdstr=strdup(tmpstr);
			correctedstr=truncateWithElipses(tmpstr,kkedit->prefsMaxFuncChars);
			sprintf(tmpstr,"%s",correctedstr);
			if(strlen(tmpstr)>0)
				{
					sscanf (lineptr,"%*s %s",functype);	
					if(strcasecmp(functype,"function")==0)
						funcProv->proposals=g_list_append(funcProv->proposals,gtk_source_completion_item_new(tmpstr,holdstr,NULL,infostr));
					if(strcasecmp(functype,"variable")==0)
						varsProv->proposals=g_list_append(varsProv->proposals,gtk_source_completion_item_new(tmpstr,holdstr,NULL,infostr));
				}
			debugFree(&correctedstr,"addProp correctedstr");
			debugFree(&holdstr,"addProp holdstr");

			lineptr=strchr(lineptr,'\n');
			if (lineptr!=NULL)
				lineptr++;
		}
	if(functions!=NULL)
		debugFree(&functions,"switchPage functions");
#endif
}

//TODO//
#if 0
//completion
void addPropXXX(pageStruct* page)
{
#ifndef _USEQT5_
	char*		functions=NULL;
	char		tmpstr[1024];
	char*		lineptr;
	char*		correctedstr;
	char		functype[63];
	char		infostr[1024];
	char*		holdstr;
	FILE*		fd=NULL;
	char		buffer[2048];
	char*		customfile;

	if(page->filePath==NULL)
		return;

//custom
	asprintf(&customfile,"%s/%s",getenv("HOME"),CUSTOMWORDFILE);
	fd=fopen(customfile,"r");
	if(fd!=NULL)
		{
			while(feof(fd)==0)
				{
					buffer[0]=0;
					fgets(buffer,2048,fd);
					if(strlen(buffer)>2)
						{
							buffer[strlen(buffer)-1]=0;
							customProv->proposals=g_list_append(customProv->proposals,gtk_source_completion_item_new(buffer,buffer,NULL,NULL));
						}
				}
			fclose(fd);
		}

//function and variable compitions
	getRecursiveTagList(page->filePath,&functions);
	lineptr=functions;


	while (lineptr!=NULL)
		{
			tmpstr[0]=0;
			sscanf (lineptr,"%s",tmpstr);
			sscanf (lineptr,"%*s %*s %*i %[^\n]s",infostr);
			holdstr=strdup(tmpstr);
			correctedstr=truncateWithElipses(tmpstr,MAXMENUFUNCLEN);
			sprintf(tmpstr,"%s",correctedstr);
			if(strlen(tmpstr)>0)
				{
					sscanf (lineptr,"%*s %s",functype);	
					if(strcasecmp(functype,"function")==0)
						funcProv->proposals=g_list_append(funcProv->proposals,gtk_source_completion_item_new(tmpstr,holdstr,NULL,infostr));
					if(strcasecmp(functype,"variable")==0)
						varsProv->proposals=g_list_append(varsProv->proposals,gtk_source_completion_item_new(tmpstr,holdstr,NULL,infostr));
				}
			free(correctedstr);
			free(holdstr);

			lineptr=strchr(lineptr,'\n');
			if (lineptr!=NULL)
				lineptr++;
		}
	if(functions!=NULL)
		debugFree(&functions,"switchPage functions");
#endif
}
#endif
void removeProps(void)
{
#ifndef _USEQT5_
	g_list_free_full(funcProv->proposals,g_object_unref);
	g_list_free_full(varsProv->proposals,g_object_unref);
	g_list_free_full(customProv->proposals,g_object_unref);
	funcProv->proposals=NULL;
	varsProv->proposals=NULL;
	customProv->proposals=NULL;
#endif
}

void createCompletion(pageStruct* page)
{
#ifndef _USEQT5_
	removeProps();

	gtk_source_completion_words_register(docWordsProv,gtk_text_view_get_buffer(GTK_TEXT_VIEW(page->view)));
	g_object_set(docWordsProv,"minimum-word-size",autoShowMinChars,NULL);

	gtk_source_completion_add_provider(page->completion,GTK_SOURCE_COMPLETION_PROVIDER(docWordsProv),NULL);
	gtk_source_completion_add_provider(page->completion,GTK_SOURCE_COMPLETION_PROVIDER(funcProv),NULL);
	gtk_source_completion_add_provider(page->completion,GTK_SOURCE_COMPLETION_PROVIDER(varsProv),NULL);
	gtk_source_completion_add_provider(page->completion,GTK_SOURCE_COMPLETION_PROVIDER(customProv),NULL);

	addProp(page);
#endif
}

void doCompletionPopUp(pageStruct* page)
{
#ifndef _USEQT5_
	GtkSourceCompletionContext*	context;
	GList*						list;

	context=gtk_source_completion_create_context(page->completion,NULL);
	list=gtk_source_completion_get_providers(page->completion);
	gtk_source_completion_show(page->completion,list,context);
#endif
}
