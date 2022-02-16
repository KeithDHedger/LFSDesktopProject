/*
 *
 * ©K. D. Hedger. Thu 23 Dec 20:38:56 GMT 2021 keithdhedger@gmail.com

 * This file (KKEditClassDocuments.cpp) is part of KKEditQT.

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

#include "KKEditClass.h"

DocumentClass* KKEditClass::getDocumentForTab(int tabnum)
{
	if(tabnum==-1)
		return(qobject_cast<DocumentClass*>(this->mainNotebook->currentWidget()));
	else
		return(qobject_cast<DocumentClass*>(this->mainNotebook->widget(tabnum)));
}

void KKEditClass::resetAllFilePrefs(void)
{
	DocumentClass	*doc;

	for(int loop=0;loop<this->mainNotebook->count();loop++)
		{
			doc=this->getDocumentForTab(loop);
			doc->setFilePrefs();
		}
}

void KKEditClass::goToDefinition(const QString txt)
{
	DocumentClass	*doc=this->getDocumentForTab(-1);
	DocumentClass	*dochold=this->getDocumentForTab(-1);
	QString			searchfor;
	bool			whileflag=false;
	int				linenumber;
	QString			label="";
	QStringList		sl;

	if((txt.isEmpty()==true) && (doc==NULL))
		return;

	if((txt.isEmpty()==true) && (doc->textCursor().hasSelection()==false))
		return;
	else
		{
			if(txt.isEmpty()==true)
				searchfor=doc->textCursor().selectedText();
			else
				searchfor=txt;
		}
DEBUGSTR("goToDefinition")

	Qt::CaseSensitivity casesens=Qt::CaseSensitive;
	for(int sens=0;sens<3;sens++)
		{
//optimized for speed
//exact match case sensitive this file
			doc=this->getDocumentForTab(-1);
			if(doc!=NULL)
				{
					sl=this->getNewRecursiveTagList(doc->getFilePath());
					if(sl.isEmpty()==false)
						{
							for(int loop=0;loop<sl.count();loop++)
								{
									label=sl.at(loop).section(" ",0,0);
									if( ((sens<2) && (label.compare(searchfor,casesens)==0)) || ((sens==2) && (label.contains(searchfor,casesens)==true)))
										{
											linenumber=sl.at(loop).section(" ",2,2).toInt();
											this->mainNotebook->setCurrentIndex(loop);
											this->history->pushToBackList(dochold->getCurrentLineNumber(),dochold->getFilePath());
											this->gotoLine(linenumber);
											return;
										}
								}
						}
				}
	
//check open files
//exact match case sensitive
			sl.clear();
			for(int tabs=0;tabs<this->mainNotebook->count();tabs++)
				{
					doc=this->getDocumentForTab(tabs);
					if(doc->filePath!=NULL)
						sl=this->getNewRecursiveTagList(doc->getFilePath());
//exact match case sensitive
					if(sl.isEmpty()==false)
						{
							for(int loop=0;loop<sl.count();loop++)
								{
									label=sl.at(loop).section(" ",0,0);
									if( ((sens<2) && (label.compare(searchfor,casesens)==0)) || ((sens==2) && (label.contains(searchfor,casesens)==true)))
										{
											linenumber=sl.at(loop).section(" ",2,2).toInt();
											label=sl.at(loop).section(" ",3,3);
											this->history->pushToBackList(dochold->getCurrentLineNumber(),dochold->getFilePath());
											this->mainNotebook->setCurrentIndex(tabs);
											this->gotoLine(linenumber);
											return;
										}
								}
						}
				}

			for(int loop=0;loop<this->mainNotebook->count();loop++)
				{
					doc=this->getDocumentForTab(loop);
					if(doc->filePath!=NULL)
						{
							QDir			dir(doc->getDirPath());
							QStringList		flist=dir.entryList(QDir::Files);
							for(int k=0;k<flist.count();k++)
								{
									if(!flist.at(k).endsWith(".o"))
										{
											sl=this->getNewRecursiveTagList(doc->getDirPath() + "/" + flist.at(k));
											if(sl.isEmpty()==true)
												continue;
//exact match
											for(int j=0;j<sl.count();j++)
												{
													label=sl.at(j).section(" ",0,0);
													if( ((sens<2) && (label.compare(searchfor,casesens)==0)) || ((sens==2) && (label.contains(searchfor,casesens)==true)))
														{
															this->history->pushToBackList(dochold->getCurrentLineNumber(),dochold->getFilePath());
															linenumber=sl.at(j).section(" ",2,2).toInt();
															this->openFile(doc->getDirPath() + "/" + flist.at(k),linenumber);
															return;
														}
												}									
										}
								}
						}
				}
			casesens=Qt::CaseInsensitive;
		}
	this->statusBar->showMessage(QString("Couldn't find definition for %1").arg(searchfor),STATUSBARTIMEOUT);
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
			cursor.setPosition(docstart+token.offset);
			cursor.movePosition(QTextCursor::EndOfWord,QTextCursor::KeepAnchor);
			doc->setTextCursor(cursor);
			this->setUpSpellGUI(badword,NULL);

			if(this->cancelCheck==true)
				{
					delete_aspell_document_checker(checker);
					this->cancelCheck=false;
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
					doc->setPlainText(line);
					docstart=diff;
				}
			}

	delete_aspell_document_checker(checker);
//replace all text in check document

	doc->setPlainText(line);
#endif
}

