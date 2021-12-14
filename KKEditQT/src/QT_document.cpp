/*
 *
 * ©K. D. Hedger. Fri  5 Nov 14:04:15 GMT 2021 keithdhedger@gmail.com

 * This file (QT_document.cpp) is part of KKEdit.

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

#include "QT_document.h"

bool DocumentClass::realShowLineNumbers(void)
{
	return(this->mainKKEditClass->lineNumbersVisible);
}

bool DocumentClass::realHiliteLine(void)
{
	return(this->mainKKEditClass->hiliteCurrentLine);
}

bool DocumentClass::realSyntaxHighlighting(void)
{
	return(this->mainKKEditClass->showHighLighting);
}

void DocumentClass::lineNumberAreaPaintEvent(QPaintEvent *event)
{
	if(this->realShowLineNumbers()==false)
		return;

	QPainter painter(this->lineNumberArea);
	painter.fillRect(event->rect(),Qt::lightGray);

	QTextBlock block=this->firstVisibleBlock();
	int blockNumber=block.blockNumber();
	int top=(int) blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom=top+(int) blockBoundingRect(block).height();

	while(block.isValid() && top<=event->rect().bottom())
		{
			if(block.isVisible() && bottom>=event->rect().top())
				{
					painter.setPen(Qt::black);
					foreach(bookMarkStruct value,this->mainKKEditClass->bookMarks)
						{
							if((value.docIndex==this->pageIndex) && (value.line==blockNumber+1))
								{
									painter.fillRect(QRect(0,top,lineNumberArea->width(),fontMetrics().height()),Qt::green);
								}
						}
					QString number=QString::number(blockNumber+1);
					painter.drawText(0,top,lineNumberArea->width(),fontMetrics().height(),Qt::AlignRight,number);
				}

			block=block.next();
			top=bottom;
			bottom=top+(int) blockBoundingRect(block).height();
			blockNumber++;
		}
}

int DocumentClass::lineNumberAreaWidth(void)
{
	int digits=1;
	int max=qMax(1,blockCount());
	while (max>=10)
		{
			max/=10;
			digits++;
		}
	int space=3+fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

	return space;
}

void DocumentClass::resizeEvent(QResizeEvent *e)
{
	QPlainTextEdit::resizeEvent(e);

	if(this->realShowLineNumbers()==false)
		lineNumberArea->setGeometry(0,0,0,0);

	QRect cr=contentsRect();
	lineNumberArea->setGeometry(QRect(cr.left(),cr.top(),lineNumberAreaWidth(),cr.height()));
}

DocumentClass::~DocumentClass()
{
	this->mainKKEditClass->pages.remove(this->pageIndex);
}

void DocumentClass::updateLineNumberAreaWidth(void)
{
	if(this->realShowLineNumbers()==true)
		setViewportMargins(lineNumberAreaWidth(),0,0,0);
	else
		setViewportMargins(0,0,0,0);
}

void DocumentClass::modified()
{
	//this->mainKKEditClass->setToobarSensitive();
	if((this->mainKKEditClass->sessionBusy==true) || (this->dirty==true))
		return;

	this->dirty=true;
	this->setTabName(this->getTabName()+"*");
}

void DocumentClass::setStatusBarText(void)
{
	QString text=QString("Line %1\tCol %2\tSessionId 0x%3\t\tFilePath %4").arg(this->textCursor().blockNumber()+1).arg(this->textCursor().positionInBlock()+1).arg(this->mainKKEditClass->sessionID,0,16).arg(this->filePath);
	this->mainKKEditClass->statusText->setText(text);
	this->mainKKEditClass->setToobarSensitive();
}

void DocumentClass::highlightCurrentLine()
{
	this->setStatusBarText();
	foreach(bookMarkStruct value,this->mainKKEditClass->bookMarks)
		{
			if(value.docIndex==this->pageIndex)
				{
					this->mainKKEditClass->sessionBusy=true;
					QTextBlock block=this->document()->findBlockByNumber(value.line-1);
					QTextBlockFormat bf=block.blockFormat();
					QTextCursor cursor(block);
					bf.setBackground(this->bookmarkLineColor);
					cursor.setBlockFormat(bf);
					this->mainKKEditClass->sessionBusy=false;
				}
		}

	this->selectedLine.format.setBackground(this->prefsHiLiteLineColor);
	this->selectedLine.format.setProperty(QTextFormat::FullWidthSelection, true);
	this->selectedLine.cursor=textCursor();

	this->selectedLine.cursor.movePosition(QTextCursor::StartOfBlock,QTextCursor::MoveAnchor);
	this->selectedLine.cursor.movePosition(QTextCursor::NextBlock,QTextCursor::KeepAnchor);

	if(this->realHiliteLine()==false)
		{
			this->extraSelections.clear();
			this->setExtraSelections(this->extraSelections);
		}
	else
		this->setXtraSelections();
	this->mainKKEditClass->setToobarSensitive();
}

void DocumentClass::setXtraSelections()
{
	this->extraSelections.clear();
	this->extraSelections=this->extraSelections+this->hilightSelections;
	this->extraSelections.append(this->selectedLine);
	this->setExtraSelections(this->extraSelections);
}

void DocumentClass::addXtraSelections()
{
}

void DocumentClass::clearXtraSelections()
{
	this->extraSelections.clear();
	this->setXtraSelections();
}

void DocumentClass::clearHilites()
{
	this->hilightSelections.clear();
	this->setXtraSelections();
}

void DocumentClass::updateLineNumberArea(const QRect &rect,int dy)
{
	if(this->realShowLineNumbers()==false)
		return;

	if(dy)
		lineNumberArea->scroll(0,dy);
	else
		lineNumberArea->update(0,rect.y(),lineNumberArea->width(),rect.height());

	if(rect.contains(viewport()->rect()))
		updateLineNumberAreaWidth();
}

void DocumentClass::keyPressEvent(QKeyEvent *event)
{
	//this->dirty=true;

//fix for vnc/linuxfb tab key
	if(((this->mainKKEditClass->application->platformName().compare("vnc")==0) || (this->mainKKEditClass->application->platformName().compare("linuxfb")==0)) && (event->key()==Qt::Key_Tab))
		{
			event->accept();
			this->insertPlainText("\t");
			return;
		}

	if((event->key()==Qt::Key_Return) && (this->mainKKEditClass->prefsIndent==true))
		{
			int j=0;
			QTextCursor cursor=this->textCursor();
			QTextBlock tb=cursor.block();
			this->indentPad="";
			QString data=tb.text();
			while((j<data.length()) && ((data.at(j)=='\t') || (data.at(j)==' ')))
				{
						this->indentPad+=data.at(j++);
				}
		}
	QPlainTextEdit::keyPressEvent(event);
}

void DocumentClass::keyReleaseEvent(QKeyEvent *event)
{
	if((event->key()==Qt::Key_Return)&& (this->mainKKEditClass->prefsIndent==true))
		{
			QTextCursor cursor=this->textCursor();
			this->insertPlainText(this->indentPad);
		}
	this->dirty=true;
	this->mainKKEditClass->setToobarSensitive();
	QPlainTextEdit::keyReleaseEvent(event);
}

DocumentClass::DocumentClass(KKEditClass *kk,QWidget *parent): QPlainTextEdit(parent)
{
	this->regexList=NULL;
	this->doneHighlightAll=false;
	this->lastFind=NULL;
	this->reserved4=0;
	this->mainKKEditClass=kk;

#ifdef _USEMINE_
	this->highlighter=new Highlighter(this->document());
#else
	this->highlighter=new QSourceHighlite::QSourceHighliter(this->document());
#endif
	this->setCenterOnScroll(true);
	lineNumberArea=new LineNumberArea(this);

	connect(this,SIGNAL(blockCountChanged(int)),this,SLOT(updateLineNumberAreaWidth()));
	connect(this,SIGNAL(updateRequest(QRect,int)),this,SLOT(updateLineNumberArea(QRect,int)));
	connect(this,SIGNAL(cursorPositionChanged()),this,SLOT(highlightCurrentLine()));
	connect(this,SIGNAL(textChanged()),this,SLOT(modified()));

	connect(this,SIGNAL(undoAvailable(bool)),this,SLOT(setUndo(bool)));
	connect(this,SIGNAL(redoAvailable(bool)),this,SLOT(setRedo(bool)));

	//connect(this,SIGNAL(keyPressEvent()),this,SLOT(keyp(QKeyEvent * event)));
	//QObject::connect(qobject_cast<QWidget*>(this->mainNotebook),&QWidget::keyPressEvent,keyp);

	updateLineNumberAreaWidth();
	highlightCurrentLine();
}

void DocumentClass::setFileName(const QString filename)
{
	this->fileName=filename;
}

const QString DocumentClass::getFileName(void)
{
	return(this->fileName);
}

void DocumentClass::setFilePath(const QString pathname)
{
	this->filePath=pathname;
}

const QString DocumentClass::getFilePath(void)
{
	return(this->filePath);
}

void DocumentClass::setDirPath(const QString dirname)
{
	this->dirPath=dirname;
}

const QString DocumentClass::getDirPath(void)
{
	return(this->dirPath);
}

void DocumentClass::setTabName(QString tabname)
{
	int tabnum=this->mainKKEditClass->mainNotebook->indexOf(this);
	this->tabName=tabname;
	this->mainKKEditClass->mainNotebook->setTabText(tabnum,tabname);
}

const QString DocumentClass::getTabName(void)
{
	return(this->tabName);
}

void DocumentClass::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu				menu(this);
	QList<QAction*>		menuactions;

	menuactions=this->mainKKEditClass->bookMarkMenu->actions();
	menu.addAction(menuactions.at(TOGGLEBOOKMARKMENUITE-REMOVEALLBOOKMARKSMENUITEM));
	menu.addSeparator();

	menuactions=qobject_cast<QMenu*>(this->mainKKEditClass->editMenu)->actions();
	menu.addAction(menuactions.at(UNDOMENUITEM-UNDOMENUITEM));
	menu.addAction(menuactions.at(REDOMENUITEM-UNDOMENUITEM));
	menu.addSeparator();
	menu.addAction(menuactions.at(CUTMENUITEM-UNDOMENUITEM));
	menu.addAction(menuactions.at(COPYMENUITEM-UNDOMENUITEM));
	menu.addAction(menuactions.at(PASTEMENUITEM-UNDOMENUITEM));
	menu.addSeparator();

	menuactions=qobject_cast<QMenu*>(this->mainKKEditClass->navMenu)->actions();

	menu.addAction(menuactions.at(SEARCHGTKDOCS-GOTODEFINEMENUITEM));
	menu.addAction(menuactions.at(SEARCHQT5DOCS-GOTODEFINEMENUITEM));
	menu.addAction(menuactions.at(SEARCHDOXYDOCS-GOTODEFINEMENUITEM));
	menu.addAction(menuactions.at(GOTODEFINEMENUITEM-GOTODEFINEMENUITEM));
	menu.addSeparator();

#ifdef _ASPELL_
	menu.addAction(this->mainKKEditClass->spellCheckMenuItem);
#endif
	menu.addSeparator();

	menu.exec(event->globalPos());
}

void DocumentClass::setFilePrefs(void)
{
	bool						holddirty=this->dirty;
	QSourceHighliter::Themes	theme=(QSourceHighliter::Themes)-1;
	QTextOption					opts;
	this->setTabStopDistance(1.0);
	this->dirty=true;

	theme=(QSourceHighliter::Themes)2;//TODO//get theme from file

	this->highlighter->setTheme(theme);

	this->dirty=holddirty;
	this->updateLineNumberAreaWidth();
	
	this->setFont(this->mainKKEditClass->prefsDocumentFont);
	this->setLineWrapMode(static_cast<QPlainTextEdit::LineWrapMode>(this->mainKKEditClass->wrapLine));
	this->prefsHiLiteLineColor=this->mainKKEditClass->prefsHiLiteLineColor;
	this->bookmarkLineColor=this->mainKKEditClass->prefsBookmarkHiLiteColor;
	this->highlightCurrentLine();

	if(this->realSyntaxHighlighting()==false)
		opts.setFlags(QTextOption::SuppressColors);

	if(this->mainKKEditClass->showWhiteSpace==true)
		opts.setFlags(opts.flags()|QTextOption::ShowLineAndParagraphSeparators | QTextOption::ShowTabsAndSpaces|QTextOption::ShowDocumentTerminator);

	this->document()->setDefaultTextOption(opts);

	QFontMetrics fm(this->mainKKEditClass->prefsDocumentFont);
	this->setTabStopDistance(fm.horizontalAdvance(" ")*this->mainKKEditClass->prefsTabWidth);
}

/*
Mime type: "text/x-c++src"
Mime type: "application/x-shellscript"
Mime type: "text/x-csrc"
Mime type: "text/x-c++src"
Mime type: "text/x-c++src"
Mime type: "text/x-c++src"
Mime type: "text/plain"

*/
void DocumentClass::setHiliteLanguage(void)
{
//fprintf(stderr,"%s=%s\n",this->filePath.toStdString().c_str(),this->mimeType.toStdString().c_str());
	QSourceHighliter::Themes	theme=(QSourceHighliter::Themes)2;

	if(this->mainKKEditClass->prefsSyntaxHilighting==false)
		{
			theme=(QSourceHighliter::Themes)-1;
			return;
		}
	if(this->mimeType.compare("text/plain",Qt::CaseInsensitive)==0)
		theme=(QSourceHighliter::Themes)-1;
	if(this->mimeType.compare("text/x-c++src",Qt::CaseInsensitive)==0)
		this->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeCpp);
	else if(this->mimeType.compare("text/x-c++hdr",Qt::CaseInsensitive)==0)
			this->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeCpp);
	else if(this->mimeType.compare("text/x-csrc",Qt::CaseInsensitive)==0)
			this->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeC);
	else if(this->mimeType.compare("text/x-chdr",Qt::CaseInsensitive)==0)
			this->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeC);
	else if(this->mimeType.compare("application/x-shellscript",Qt::CaseInsensitive)==0)
			this->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeBash);
	else if(this->mimeType.compare("text/x-python",Qt::CaseInsensitive)==0)
			this->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodePython);
	else if(this->mimeType.compare("text/x-go",Qt::CaseInsensitive)==0)
			this->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeGo);
	else if(this->mimeType.compare("text/x-lua",Qt::CaseInsensitive)==0)
			this->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeLua);
	else if(this->mimeType.compare("application/x-yaml",Qt::CaseInsensitive)==0)
			this->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeYAML);
	else if(this->mimeType.compare("application/x-php",Qt::CaseInsensitive)==0)
			this->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodePHP);
	else if(this->mimeType.compare("application/xhtml+xml",Qt::CaseInsensitive)==0)
			this->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeXML);
	else if(this->mimeType.compare("text/css",Qt::CaseInsensitive)==0)
			this->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeCSS);
	else if(this->mimeType.compare("application/javascript",Qt::CaseInsensitive)==0)
			this->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeJs);
	else if(this->mimeType.compare("text/x-makefile",Qt::CaseInsensitive)==0)
			this->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeMake);
	else
		theme=(QSourceHighliter::Themes)-1;

	this->highlighter->setTheme(theme);
}

void DocumentClass::setUndo(bool avail)
{
	this->gotUndo=avail;
}

void DocumentClass::setRedo(bool avail)
{
	this->gotRedo=avail;
}

