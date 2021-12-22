/******************************************************
*
*     ©keithhedger Wed 18 Jun 17:14:17 BST 2014
*     kdhedger68713@gmail.com
*
*     backclass.cpp
* 
******************************************************/

//#include "kkedit-includes.h"
#include "backclass.h"

HistoryClass::HistoryClass()
{
//#ifndef _USEQT5_
//	buf=new TextBuffer;
//	this->savedPage=NULL;
//#endif
}

HistoryClass::~HistoryClass()
{
//#ifndef _USEQT5_
//	delete buf;
//#endif
}
//
//void HistoryClass::getThisPoint(void)
//{
//#ifndef _USEQT5_
//	GtkWidget*	pageBox;
//	int			tabNum=gtk_notebook_get_current_page((GtkNotebook*)mainNotebook);
//
//	this->goBack=true;
//	pageBox=gtk_notebook_get_nth_page((GtkNotebook*)mainNotebook,tabNum);
//	if(pageBox==NULL)
//		savedPage=NULL;
//	else
//		savedPage=(pageStruct*)g_object_get_data((GObject*)pageBox,"pagedata");
//
//	if(this->savedPage==NULL)
//		this->goBack=false;
//#endif
//}
//
//pageStruct* HistoryClass::getPage(void)
//{
//#ifndef _USEQT5_
//	return(savedPage);
//#endif
//return(NULL);
//}
//
//bool HistoryClass::canGoBack(void)
//{
//	return(this->goBack);
//}
//
//
//void HistoryClass::setPage(pageStruct* page)
//{
//#ifndef _USEQT5_
//	savedPage=page;
//#endif
//}
//
//TextBuffer* HistoryClass::getTextBuffer(void)
//{
//#ifndef _USEQT5_
//	return(buf);
//#endif
//return(NULL);
//}
//
//bool HistoryClass::savePosition(void)
//{
//#ifndef _USEQT5_
//	getThisPoint();
//
//	if(this->savedPage!=NULL)
//		{
//			buf->textBuffer=(GtkTextBuffer*)savedPage->buffer;
//			buf->getLineData();
//			gtk_text_buffer_move_mark_by_name((GtkTextBuffer*)savedPage->buffer,"back-mark",&buf->cursorPos);
//			this->goBack=true;
//			setSensitive();
//			return(true);
//		}
//
//	this->goBack=false;
//	setSensitive();
//#endif
//	return(false);
//}
//
//int HistoryClass::getTabNumForPage(void)
//{
//#ifndef _USEQT5_
//	pageStruct*	page;
//	int			numpages=gtk_notebook_get_n_pages((GtkNotebook*)mainNotebook);
//
//	for(int loop=0;loop<numpages;loop++)
//		{
//			page=getDocumentData(loop);
//			if (page==savedPage)
//				{
//					this->goBack=true;
//					return(loop);
//				}
//		}
//
//	this->savedPage=NULL;
//	this->goBack=false;
//#endif
//	return(-1);
//}
















