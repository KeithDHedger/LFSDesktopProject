/*
 *
 * ©K. D. Hedger. Thu 23 Dec 20:37:27 GMT 2021 keithdhedger@gmail.com

 * This file (QT_document.h) is part of KKEditQT.

 * KKEditQT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * KKEditQT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with KKEditQT.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _QT_DOCUMENT_
#define _QT_DOCUMENT_

#include "KKEditClass.h"

using namespace QSourceHighlite;

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
		bool								gotUndo=false;
		bool								gotRedo=false;
		int									pageIndex;

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
		void								setUndo(bool avail);
		void								setRedo(bool avail);

	private:
		void								clearXtraSelections(void);
		QWidget 							*lineNumberArea;
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
