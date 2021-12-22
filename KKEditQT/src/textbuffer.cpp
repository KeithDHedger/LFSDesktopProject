/******************************************************
*
*     ©keithhedger Thu  5 Jun 14:36:43 BST 2014
*     kdhedger68713@gmail.com
*
*     textbuffer.cpp
* 
******************************************************/

//#include "globals.h"
//
//TextBuffer::TextBuffer()
//{
//#ifndef _USEQT5_
//	this->textBuffer=NULL;
//#endif
//}
//
//TextBuffer::~TextBuffer()
//{
//#ifndef _USEQT5_
//#endif
//}
//
//#ifndef _USEQT5_
//TextBuffer::TextBuffer(GtkTextBuffer* buffer)
//#else
////TODO//
//TextBuffer::TextBuffer(int num)
//#endif
//{
//#ifndef _USEQT5_
//	this->textBuffer=buffer;
//	this->getLineData();
//	this->getVisibleLine();
//	this->getLine();
//#endif
//}
//
//void TextBuffer::getLineData()
//{
//#ifndef _USEQT5_
//	gtk_text_buffer_get_iter_at_mark(textBuffer,&lineStart,gtk_text_buffer_get_insert(textBuffer));
//	gtk_text_buffer_get_iter_at_mark(textBuffer,&lineEnd,gtk_text_buffer_get_insert(textBuffer));
//	gtk_text_iter_forward_visible_line(&lineEnd);
//	gtk_text_iter_set_line_offset(&lineStart,0);
//	gtk_text_buffer_get_iter_at_mark(textBuffer,&cursorPos,gtk_text_buffer_get_insert(textBuffer));
//	lineNum=gtk_text_iter_get_line(&cursorPos)+1;
//	column=gtk_text_iter_get_line_offset(&cursorPos)+1;
//	charPos=gtk_text_iter_get_offset(&cursorPos);
//#endif
//}
//
//void TextBuffer::getCursorIter()
//{
//#ifndef _USEQT5_
//	gtk_text_buffer_get_iter_at_mark(textBuffer,&cursorPos,gtk_text_buffer_get_insert(textBuffer));
//#endif
//}
//
//void TextBuffer::getToLineEnd()
//{
//#ifndef _USEQT5_
//	this->getCursorIter();
//	lineEnd=cursorPos;
//	gtk_text_iter_forward_to_line_end(&lineEnd);
//#endif
//}
//
//void TextBuffer::getToVisibleLineEnd()
//{
//#ifndef _USEQT5_
//	this->getCursorIter();
//	visibleLineEnd=cursorPos;
//	gtk_text_iter_forward_to_line_end(&visibleLineEnd);
//#endif
//}
//
//void TextBuffer::getToLineStart()
//{
//#ifndef _USEQT5_
//	this->getCursorIter();
//	lineStart=cursorPos;
//	gtk_text_iter_set_line_offset(&lineStart,0);
//#endif
//}
//
//#ifndef _USEQT5_
//void TextBuffer::deleteFromCursor(GtkTextIter* iter)
//#else
////TODO//
//void TextBuffer::deleteFromCursor(void)
//#endif
//{
//#ifndef _USEQT5_
//	this->getCursorIter();
//	if(!gtk_text_iter_ends_line(&cursorPos))
//		gtk_text_buffer_delete(textBuffer,&cursorPos,iter);
//#endif
//}
//
//#ifndef _USEQT5_
//void TextBuffer::deleteToCursor(GtkTextIter* iter)
//#else
////TODO//
//void TextBuffer::deleteToCursor(void)
//#endif
//{
//#ifndef _USEQT5_
//	this->getCursorIter();
//	if(!gtk_text_iter_starts_line(&cursorPos))
//		this->deleteFromCursor(iter);
//#endif
//}
//
//bool TextBuffer::selectWord()
//{
//#ifndef _USEQT5_
//	this->getCursorIter();
//	lineStart=cursorPos;
//	if(gtk_text_iter_inside_word(&cursorPos))
//		{
//			gtk_text_iter_backward_word_start(&lineStart);
//			gtk_text_iter_forward_word_end(&cursorPos);
//			return(true);
//		}
//#endif
//	return(false);
//}
//
//char* TextBuffer::getSelectedText()
//{
//#ifndef _USEQT5_
//	return(gtk_text_buffer_get_text(textBuffer,&lineStart,&lineEnd,true));
//#endif
//return(NULL);
//}
//
//#ifndef _USEQT5_
//void TextBuffer::selectRange(GtkTextIter* start,GtkTextIter* end)
//#else
////TODO//
//void TextBuffer::selectRange(void)
//#endif
//{
//#ifndef _USEQT5_
//	gtk_text_buffer_select_range(textBuffer,start,end);
//#endif
//}
//
//
//void TextBuffer::getVisibleLine()
//{
//#ifndef _USEQT5_
//	this->getCursorIter();
//	lineStart=cursorPos;
//	visibleLineEnd=cursorPos;
//	gtk_text_iter_set_line_offset(&lineStart,0);
//	gtk_text_iter_forward_to_line_end(&visibleLineEnd);
//#endif
//}
//
//void TextBuffer::selectVisibleLine()
//{
//#ifndef _USEQT5_
//	gtk_text_buffer_select_range(textBuffer,&lineStart,&visibleLineEnd);
//#endif
//}
//
//void TextBuffer::getLine()
//{
//#ifndef _USEQT5_
//	this->getCursorIter();
//	lineStart=cursorPos;
//	gtk_text_iter_set_line_offset(&lineStart,0);
//	lineEnd=cursorPos;
//	gtk_text_iter_forward_visible_line(&lineEnd);
//#endif
//}
//
//void TextBuffer::selectLine()
//{
//#ifndef _USEQT5_
//	gtk_text_buffer_select_range(textBuffer,&lineStart,&lineEnd);
//#endif
//}
//
//void TextBuffer::selectToLineStart()
//{
//#ifndef _USEQT5_
//	this->getToLineStart();
//	gtk_text_buffer_select_range(textBuffer,&lineStart,&cursorPos);
//#endif
//}
//
//void TextBuffer::selectToLineEnd()
//{
//#ifndef _USEQT5_
//	this->getToVisibleLineEnd();
//	gtk_text_buffer_select_range(textBuffer,&cursorPos,&visibleLineEnd);
//#endif
//}
//
//#ifndef _USEQT5_
//void TextBuffer::scroll2Line(GtkTextView* view,int linenum)
//#else
////TODO//
//void TextBuffer::scroll2Line(void)
//#endif
//{
//#ifndef _USEQT5_
//	GtkTextMark*	mark;
//	GtkTextIter		iter;
//
//	gtk_text_buffer_get_iter_at_line_offset(textBuffer,&iter,linenum,0);
//	gtk_text_buffer_place_cursor(textBuffer,&iter);
//	if(!gtk_text_view_scroll_to_iter(view,&iter,0,true,0,0.5))
//		{
//			mark=gtk_text_buffer_get_mark(textBuffer,"insert");
//			this->getCursorIter();
//			if(!gtk_text_view_scroll_to_iter(view,&cursorPos,0,true,0,0.5))
//				gtk_text_view_scroll_to_mark(view,mark,0,true,0,0.5);
//		}
//#endif
//}
//
//void TextBuffer::scroll2LineM(pageStruct* page,int linenum)
//{
//#ifndef _USEQT5_
//	GtkTextIter		iter;
//
//	gtk_text_buffer_get_iter_at_line_offset(textBuffer,&iter,linenum,0);
//	gtk_text_buffer_place_cursor(textBuffer,&iter);
//	gtk_text_view_scroll_to_iter((GtkTextView*)page->view,&iter,0,true,0,0.5);
//
//	for(int j=0;j<10;j++)
//		{
//			gtk_text_view_scroll_to_iter((GtkTextView*)page->view,&iter,0,true,0,0.5);
//			gtk_main_iteration_do(false);
//		}
//#endif
//}
//
//
//#ifndef _USEQT5_
//void TextBuffer::scroll2Mark(GtkTextView* view,GtkTextMark* mark)
//#else
////TODO//
//void TextBuffer::scroll2Mark(void)
//#endif
//{
//#ifndef _USEQT5_
//	gtk_text_buffer_get_iter_at_mark(textBuffer,&cursorPos,mark);
//	gtk_text_view_scroll_to_iter(view,&cursorPos,0,true,0,0.5);
//	gtk_text_view_scroll_to_mark(view,mark,0,true,0,0.5);
//#endif
//}
//
//#ifndef _USEQT5_
//void TextBuffer::scroll2Iter(GtkTextView* view,GtkTextIter iter)
//#else
////TODO//
//void TextBuffer::scroll2Iter(void)
//#endif
//{
//#ifndef _USEQT5_
//	gtk_text_view_scroll_to_iter(view,&iter,0,true,0,0.5);
//#endif
//}
//

