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
		return(qobject_cast<DocumentClass*>(this->mainNotebook->currentWidget()));
	else
		return(qobject_cast<DocumentClass*>(this->mainNotebook->widget(tabnum)));
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

	for(int loop=0;loop<this->mainNotebook->count();loop++)
		{
			doc=this->getDocumentForTab(loop);
			doc->setFilePrefs();
		}
}

void KKEditClass::goToDefinition(void)//TODO//clean
{
	DocumentClass	*document=this->getDocumentForTab(-1);
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
					this->mainNotebook->setCurrentIndex(fdata->intab);
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

	doc=this->getDocumentForTab(-1);
	if(doc==NULL)
		return;
	block=doc->document()->findBlockByNumber(linenumber-1);
	cursor=doc->textCursor();
	cursor.setPosition(block.position());
	doc->setTextCursor(cursor);
}


void KKEditClass::reloadDocument(void)
{
printf("void KKEditClass::reloadFile(void)\n");

	DocumentClass	*doc=this->getDocumentForTab(-1);
	bool			retval;
	int				calctabnum;

	if(doc==NULL)
		return;

	if(doc->getFilePath().isEmpty()==true)
		return;

	calctabnum=this->mainNotebook->indexOf(doc);

	QFile			file(doc->getFilePath());
	QFileInfo		fileinfo(file);

	this->sessionBusy=true;

	retval=file.open(QIODevice::Text | QIODevice::ReadOnly);
	if(retval==true)
		{
			QString	content=QString::fromUtf8(file.readAll());
			doc->setPlainText(content);
			doc->highlighter->rehighlight();
			doc->dirty=false;
			file.close();
			doc->setTabName(this->truncateWithElipses(doc->getFileName(),this->prefsMaxTabChars));
		}

	this->sessionBusy=false;
	this->switchPage(calctabnum);
}

