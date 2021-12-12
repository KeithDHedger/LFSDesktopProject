/*
 *
 * ©K. D. Hedger. Fri  5 Nov 14:04:29 GMT 2021 keithdhedger@gmail.com

 * This file (QT_document.h) is part of KKEdit.

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

#ifndef _QT_DOCUMENT_
#define _QT_DOCUMENT_

#include "kkedit-includes.h"

#include <QPlainTextEdit>
#include <QObject>
#include "qsourcehighliter.h"

using namespace QSourceHighlite;

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;

class LineNumberArea;

class DocumentClass : public QPlainTextEdit
{
	Q_OBJECT

	public:
		explicit DocumentClass(KKEditClass *kk,QWidget *parent=0);
		~DocumentClass();

		void								setFileName(const QString filename);
		const QString						getFileName(void);
		void								setFilePath(const QString filename);
		const QString						getFilePath(void);
		void								setDirPath(const QString dirname);
		const QString						getDirPath(void);
		void								setTabName(QString tabname);
		const QString						getTabName(void);

		void								setFilePrefs(void);
		void								setHiliteLanguage(void);

		void								lineNumberAreaPaintEvent(QPaintEvent *event);
		int									lineNumberAreaWidth(void);
		void								setStatusBarText(void);

		QList<QTextEdit::ExtraSelection>	extraSelections;
		QList<QTextEdit::ExtraSelection>	hilightSelections;
		
		QTextEdit::ExtraSelection			selection;
		QTextEdit::ExtraSelection			selectedLine;

		void								setXtraSelections(void);
		void								addXtraSelections(void);
		void								clearHilites(void);

#ifdef _USEMINE_
		Highlighter							*highlighter;
#else
		QSourceHighlite::QSourceHighliter	*highlighter;
#endif
		KKEditClass							*mainKKEditClass=NULL;
		QColor								prefsHiLiteLineColor;
		QColor								bookmarkLineColor;

//pageStruct
		QString								fileName;
		QString								filePath;
		QString								dirPath;
		QString								tabName;
		bool								doneHighlightAll;
		char								*lastFind;
		bool								dirty=false;
		QString								mimeType="text/plain";
		GSList*								regexList;
		int									regexMatchNumber;
		uPtr								reserved4;

		int									pageIndex;

	//signals:
	//	void								cursorPositionChanged(void);

//	public slots:
	protected:
	    void								resizeEvent(QResizeEvent *event);
		void								keyPressEvent(QKeyEvent *event);
		void								keyReleaseEvent(QKeyEvent *event);
		void								contextMenuEvent(QContextMenuEvent *event) Q_DECL_OVERRIDE;

	private slots:
		void								highlightCurrentLine();
		void								updateLineNumberAreaWidth(void);
		void								updateLineNumberArea(const QRect &, int);
		void								modified();

	private:
		void								clearXtraSelections(void);
		QWidget 							*lineNumberArea;
		QTextCursor							oldCursor;
		QTextBlockFormat					oldBlockFormat;
		QString								indentPad;
		bool								realShowLineNumbers(void);
		bool								realHiliteLine(void);
		bool								realSyntaxHighlighting(void);
};

class LineNumberArea : public QWidget
{
	Q_OBJECT

	public:
	    LineNumberArea(DocumentClass *editor) : QWidget(editor)
			{
				codeEditor=editor;
			}

		QSize sizeHint() const
			{
				return QSize(codeEditor->lineNumberAreaWidth(),0);
			}

	protected:
		void paintEvent(QPaintEvent *event)
			{
				codeEditor->lineNumberAreaPaintEvent(event);
			}
 
	private:
		DocumentClass *codeEditor;
};

#endif
