/*
 *
 * ©K. D. Hedger. Fri 19 Nov 11:35:40 GMT 2021 keithdhedger@gmail.com

 * This file (KKEditClassDocuments.cpp) is part of KKEdit.

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

#include "KKEditClass.h"

DocumentClass* KKEditClass::getDocumentForTab(int tabnum)
{
	if(tabnum==-1)
		return(qobject_cast<DocumentClass*>(qobject_cast<QTabWidget*>(kkedit->mainNotebook)->currentWidget()));
	else
		return(qobject_cast<DocumentClass*>(qobject_cast<QTabWidget*>(kkedit->mainNotebook)->widget(tabnum)));
}

/*
 QHashIterator<QString, int> i(hash);
 while (i.hasNext()) {
     i.next();
     cout << i.key() << ": " << i.value() << Qt::endl;
 }
Here's the same code, but using an STL-style iterator:

 QHash<QString, int>::const_iterator i = hash.constBegin();
 while (i != hash.constEnd()) {
     cout << i.key() << ": " << i.value() << Qt::endl;
     ++i;
 }
*/
void KKEditClass::resetAllFilePrefs(void)
{
	DocumentClass	*doc;

	for(int loop=0;loop<qobject_cast<QTabWidget*>(this->mainNotebook)->count();loop++)
		{
			doc=getDocumentData(loop);
			doc->setFilePrefs();
		}
}

void KKEditClass::goToDefinition(void)//TODO//clean
{
	DocumentClass	*document=getDocumentData(-1);
	functionData	*fdata=NULL;
	char			*selection;
	const char		*selectionptr;

	if(document==NULL)
		return;

	selection=strdup(document->textCursor().selectedText().toUtf8().constData());
	selectionptr=selection;

	fdata=getFunctionByName(selectionptr,true,true);
	if(fdata!=NULL)
		{
			//TODO//
//			history->savePosition();
			if(fdata->intab!=-1)
				{
					qobject_cast<QTabWidget*>(this->mainNotebook)->setCurrentIndex(fdata->intab);
					this->gotoLine(fdata->line);
				}
			else
				{
					this->openFile(fdata->file,fdata->line-1,true);
				}
			destroyData(fdata);
		}

	free(selection);
}

void KKEditClass::gotoLine(int linenumber)
{
	DocumentClass	*doc=NULL;
	QTextBlock		block;
	QTextCursor		cursor;

	doc=getDocumentData(-1);
	if(doc==NULL)
		return;
	block=doc->document()->findBlockByNumber(linenumber-1);
	cursor=doc->textCursor();
	cursor.setPosition(block.position());
	doc->setTextCursor(cursor);
}
