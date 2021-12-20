/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
 * callbacks.cpp
 *
*/

#include "kkedit-includes.h"


#ifdef _BUILDDOCVIEWER_

struct docFileData
{
	char*	fileName;
	char*	filePath;
	int		lineNum;
	char*	hashTag;
	char*	sourceFile;
};

bool readFile(char *name)
{
#ifndef _USEQT5_
	FILE*			file;
	unsigned long	fileLen;

	if(filebuffer!=NULL)
		debugFree(&filebuffer,"readFile filebuffer");
	//Open file
	file=fopen(name,"rb");
	if (!file)
		{
			fprintf(stderr,"Unable to open file %s",name);
			return(false);
		}

	//Get file length
	fseek(file,0,SEEK_END);
	fileLen=ftell(file);
	fseek(file,0,SEEK_SET);

	//Allocate memory
	filebuffer=(char*)malloc(fileLen+1);
	if (!filebuffer)
		{
			fprintf(stderr,"Memory error!");
			fclose(file);
			return(false);
		}

	//Read file contents into buffer
	fread(filebuffer,fileLen,1,file);
	fclose(file);
#endif
	return(true);
}

int getLineFromXML(char* xml)
{
	int				retline=1;
#ifndef _USEQT5_
	StringSlice*	slice=new StringSlice;

	char*			data;
	char*			xmldata=xml;

	data=slice->sliceBetween(xmldata,(char*)"Definition at line",(char*)"\">");
	if(slice->getResult()==0)
		retline=atoi(slice->sliceBetween(data,(char*)"#l",NULL));
	delete slice;
#endif
	return(retline);
}

char* getPathFromXML(char* xml)
{
#ifndef _USEQT5_
	StringSlice*	slice=new StringSlice;
	bool			done=false;
	char*			data;
	unsigned int	bufferlen=1024;
	char*			buffer=(char*)calloc(bufferlen,1);
	char*			xmldata=xml;
	char*			portion;

	buffer[0]=0;
	mustBeAClass=false;

	xmldata=strstr(xml,(char*)"<li class=\"navelem\"");
	while(done==false)
		{
			data=slice->sliceInclude(xmldata,(char*)"<li class=\"navelem\"",(char*)"</a>");
			if(slice->getResult()==0)
				{
					portion=slice->sliceBetween(data,(char*)"html\">",(char*)"</a>");
					if(strlen(buffer)+strlen(portion)+1<bufferlen)
						{
							bufferlen=bufferlen+strlen(portion)+1024;
							buffer=(char*)realloc(buffer,bufferlen);
						}
					strcat(buffer,"/");
					strcat(buffer,portion);
					xmldata=strstr(xmldata,portion);
					xmldata=(char*)(long)xmldata+strlen(data);
				}
			else
				done=true;
		}

	xmldata=xml;
	done=false;
// Class Reference
	data=slice->sliceBetween(xmldata,(char*)"\"title\">",(char*)" Class Reference");
	if(slice->getResult()==0)
		{
			xmldata=strstr(xml,(char*)"<p>Definition at line ");
			if(xmldata!=NULL)
				{
					char*	tfile;
					asprintf(&tfile,"/%s",slice->sliceBetween(xmldata,(char*)".html\">",(char*)"</a>"));					
					delete slice;
					return(tfile);
				}
		}

	xmldata=xml;
	data=slice->sliceBetween(xmldata,(char*)"\"title\">",(char*)" File Reference");
	if(slice->getResult()==0)
		{
			if(strlen(buffer)+strlen(data)+1<bufferlen)
				{
					bufferlen=bufferlen+strlen(data)+1024;
					buffer=(char*)realloc(buffer,bufferlen);
				}
			strcat(buffer,"/");
			strcat(buffer,data);
		}
	else
		{
			data=slice->sliceBetween(xmldata,(char*)"\"title\">",(char*)"</div>");
			if(slice->getResult()==0)
				{
					if(strlen(buffer)+strlen(data)+1<bufferlen)
						{
							bufferlen=bufferlen+strlen(data)+1024;
							buffer=(char*)realloc(buffer,bufferlen);
						}
					strcat(buffer,"/");
					strcat(buffer,data);
				}
			else
				{
					data=slice->sliceBetween(xmldata,(char*)"<title>",(char*)"</title>");
				if(slice->getResult()==0)
						{
						if(strstr(data,"::")!=NULL)
							{
								xmldata=strstr(xml,(char*)"<p>Definition at line ");
								if(xmldata!=NULL)
									{
										mustBeAClass=false;
										classLineNumber=atoi(slice->sliceBetween(xmldata,(char*)"#l",(char*)"\">"));
										asprintf(&classFileName,"/%s",slice->sliceBetween(xmldata,(char*)".html\">",(char*)"</a>"));
										delete slice;
										return(classFileName);
									}
							}
						}
				}
		}
	delete slice;
	return(buffer);
#endif
	return(NULL);
}

docFileData* getDoxyFileData(char* uri)
{
#ifndef _USEQT5_
	char*	linetag=NULL;
	bool	gotline=false;

	char*	unhashedline=NULL;
	char*	filepath=NULL;

	StringSlice*	slice=new StringSlice;

	slice->setReturnDupString(true);
	docFileData* doxydata=(docFileData*)malloc(sizeof(docFileData));

	doxydata->lineNum=1;
	doxydata->fileName=(char*)"";
	doxydata->filePath=(char*)"";
	doxydata->hashTag=(char*)"";
	doxydata->sourceFile=(char*)"";

	linetag=slice->sliceBetween(uri,(char*)"#",NULL);
	if(slice->getResult()==0)
		{
			if(linetag[0]=='l')
				{
					doxydata->lineNum=atoi(&linetag[1]);
					gotline=true;
				}
			unhashedline=slice->sliceBetween(uri,NULL,(char*)"#");
		}
	else
		{
			unhashedline=strdup(uri);
			linetag=strdup("");
		}

	filepath=g_filename_from_uri(unhashedline,NULL,NULL);
	if(readFile(filepath)==true)
		{
			doxydata->filePath=slice->sliceBetween(filepath,NULL,(char*)"/html/");
			doxydata->fileName=getPathFromXML(filebuffer);
			if(mustBeAClass==false)
				{
					asprintf(&doxydata->sourceFile,"%s%s",doxydata->filePath,doxydata->fileName);
					if(gotline==false)
						{
							doxydata->lineNum=getLineFromXML(filebuffer);
						}
				}
			else
				{
					doxydata->lineNum=classLineNumber;
					asprintf(&doxydata->sourceFile,"%s%s",doxydata->filePath,classFileName);
					
				}
		}
	else
		{
			debugFree((char**)&doxydata,"getDoxyFileData doxydata");
			doxydata=NULL;
		}
	debugFree(&unhashedline,"getDoxyFileData unhashedline");
	debugFree(&linetag,"getDoxyFileData linetag");
	debugFree(&filepath,"getDoxyFileData filepath");
	return(doxydata);
#endif
	return(NULL);
}

#ifndef _USEQT5_
gboolean docLinkTrap(WebKitWebView* web_view,WebKitWebFrame* frame,WebKitNetworkRequest* request,WebKitWebNavigationAction* navigationAction,WebKitWebPolicyDecision* policy_decision, gpointer user_data)
{
#ifndef _USEQT5_
	int				mod=-1;
	const char*		uri;
	pageStruct*		page;
	TextBuffer*		buf;
	docFileData*	doxydata;

	mod=webkit_web_navigation_action_get_modifier_state(navigationAction);
	if(mod&GDK_SHIFT_MASK)
		{
			uri=webkit_network_request_get_uri(request);
			doxydata=getDoxyFileData((char*)uri);
			if(doxydata==NULL)
				return(false);

//check in open tabs
			buf=new TextBuffer;
			for(int j=0; j<gtk_notebook_get_n_pages((GtkNotebook*)mainNotebook); j++)
				{
					page=kkedit->getDocumentForTab(j);
					if((strcmp(page->realFilePath,doxydata->sourceFile)==0) || (strcmp(page->filePath,doxydata->sourceFile)==0))
						{
							gtk_notebook_set_current_page((GtkNotebook*)mainNotebook,j);
							buf->textBuffer=(GtkTextBuffer*)page->buffer;
							buf->scroll2LineM(page,doxydata->lineNum-1);
							delete buf;
							debugFree((char**)&doxydata,"docLinkTrap doxydata");
							return(false);
						}
				}
//try to open file f not in tabs
			kkedit->openFile(doxydata->sourceFile,doxydata->lineNum,false);
			debugFree((char**)&doxydata,"docLinkTrap doxydata");
		}
	return(false);
#endif
}
#endif

#endif

