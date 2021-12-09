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

bool KKEditClass::checkSelection(QString selection)
{
#ifdef _ASPELL_
	int							correct;
	AspellWordList*				suggestions;
	AspellStringEnumeration*	elements;
	const char*					suggestedword;
	int							wordcnt=0;
	char*						wordlist[100];
	const char					*word=selection.toStdString().c_str();

	correct=aspell_speller_check(spellChecker,word,-1);
	this->wordDropBox->clear();

	if(!correct)
		{
			suggestions=(AspellWordList*)aspell_speller_suggest(spellChecker,word,-1);
			elements=aspell_word_list_elements(suggestions);
			while((suggestedword=aspell_string_enumeration_next(elements))!=NULL)
				{
					wordlist[wordcnt]=strdup(suggestedword);
					this->wordDropBox->addItem(wordlist[wordcnt],QVariant(wordcnt));
					wordcnt++;
				}
			delete_aspell_string_enumeration(elements);
			return(true);
		}
#endif
	return(false);
}

void KKEditClass::setUpSpellGUI(QString word,DocumentClass *doc)
{
#ifdef _ASPELL_
	if(doc==NULL)
		doc=this->getDocumentForTab(-1);

	if(this->checkSelection(word)==true)
		{
			this->infoLabel->setText(QString("Change %1 to:").arg(word));
			this->spellCheckGUI->exec();
		}
#endif
}

void KKEditClass::checkDoc(DocumentClass *doc)
{
#ifdef _ASPELL_
	AspellCanHaveError*		ret;
	AspellDocumentChecker*	checker;
	AspellToken				token;
	int						diff=0;
	unsigned int			goodwordlen;
	char*					word_begin;
	char*					badword;
	char*					line;
	QString					qstr;
	QTextCursor				cursor;
	int						docstart=0;
	QByteArray				bytearray;

	if(doc==NULL)
		return;

	cursor=doc->textCursor();
	qstr=doc->toPlainText();
	bytearray=qstr.toUtf8();
	line=(char*)bytearray.constData();

	/* Set up the document checker */
	ret=new_aspell_document_checker(this->spellChecker);
	if (aspell_error(ret)!=0)
		{
			printf("Error: %s\n",aspell_error_message(ret));
			return;
		}

	checker=to_aspell_document_checker(ret);
	  /* First process the line */
	aspell_document_checker_process(checker,line,-1);
	  /* Now find the misspellings in the line */
	while(token=aspell_document_checker_next_misspelling(checker),token.len!=0)
		{
	    /* Pay particular attention to how token.offset and diff is used */
			asprintf(&badword,"%.*s",token.len,(char*)&line[token.offset+diff]);
			this->goodWord="";
			this->cancelCheck=false;
			this->returnWord=true;
			this->badWord=badword;
			cursor.setPosition(token.offset);
			cursor.movePosition(QTextCursor::EndOfWord,QTextCursor::KeepAnchor);
			doc->setTextCursor(cursor);
			this->setUpSpellGUI(badword,NULL);

			if(this->cancelCheck==true)
				{
					delete_aspell_document_checker(checker);
					this->cancelCheck=false;
					doc->setPlainText(line);
					return;
				}
			word_begin=line+token.offset+diff;

			if(this->goodWord.isEmpty()==false)
				{
					goodwordlen=this->goodWord.length();
	    /* Replace the misspelled word with the replacement */
					diff+=goodwordlen-token.len;
					memmove(word_begin+goodwordlen,word_begin+token.len,strlen(word_begin+token.len)+1);
					memcpy(word_begin,this->goodWord.toStdString().c_str(),goodwordlen);
					doc->setPlainText(line);//TODO//HMMMmmmmmm
				cursor.setPosition(docstart);
					docstart+=goodwordlen;
			cursor.movePosition(QTextCursor::EndOfWord,QTextCursor::KeepAnchor);
			doc->setTextCursor(cursor);
				
				}
			}

	delete_aspell_document_checker(checker);
//replace all text in check document

	doc->setPlainText(line);
#endif
}

