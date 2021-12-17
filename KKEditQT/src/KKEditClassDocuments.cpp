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

/*
functionData* getFunctionByName(const char* name,bool recurse,bool casesensitive)
{
	DocumentClass		*document=kkedit->getDocumentForTab(-1);
	pageStruct			*page;
	char				*functions=NULL;
	QString				str;
	char				*lineptr;
	int					gotmatch=-1;
	char				function[1024];
	functionData*		fdata;
	int					loop;
	int					startpage;
	int					holdlistfunction=kkedit->prefsFunctionMenuLayout;
	StringSlice			slice;
	bool				whileflag=true;
	bool				checkthispage=true;
	int					maxpage;;
	char				funcname[256];
	char				filepath[1024];
	int					linenumber;
	QStringList			strlist;
	Qt::CaseSensitivity	cs;

	if(document==NULL)
		return(NULL);

	if(casesensitive==true)
		cs=Qt::CaseSensitive;
	else
		cs=Qt::CaseInsensitive;

	loop=qobject_cast<QTabWidget*>(kkedit->mainNotebook)->currentIndex();
	startpage=loop;
	checkthispage=true;
	maxpage=qobject_cast<QTabWidget*>(kkedit->mainNotebook)->count();

	while(whileflag==true)
		{
			document=kkedit->getDocumentForTab(loop);
			if(document->filePath!=NULL)
				{
					kkedit->prefsFunctionMenuLayout=0;
					getRecursiveTagList((char*)document->filePath.toStdString().c_str(),&functions);//TODO//
					kkedit->prefsFunctionMenuLayout=holdlistfunction;
					if(functions!=NULL)
						{
							str=functions;
							strlist=str.split("\n",Qt::SkipEmptyParts);
							gotmatch=-1;
							for(int i=0;i<strlist.size();i++)
								{
									if(strlist.at(i).startsWith(name,cs))
										{
											gotmatch=0;
											lineptr=strdup(strlist.at(i).toLocal8Bit().constData());
											break;
										}
								}

							debugFree(&functions,"functions getFunctionByName");
							if(gotmatch==0)
								{
									fdata=(functionData*)malloc(sizeof(functionData));
									sscanf (lineptr,"%" VALIDFUNCTIONCHARS "s",function);
									fdata->name=strdup(function);
									sscanf (lineptr,"%*s %" VALIDFUNCTIONCHARS "s",function);
									fdata->type=strdup(function);
									sscanf (lineptr,"%*s %*s %i",&fdata->line);
									sscanf (lineptr,"%*s %*s %*i %" VALIDFILENAMECHARS "s",function);
									fdata->file=strdup(function);
									sscanf (lineptr,"%*s %*s %*i %*s %[^\n]s",function);
									fdata->define=strdup(function);
									fdata->intab=loop;
									debugFree(&lineptr,"lineptr getFunctionByName");
									return(fdata);
								}
						}
				}

			if(checkthispage==true)
				{
					loop=-1;
					checkthispage=false;
				}

			loop++;
			if(loop==startpage)
				loop++;
			if(loop==maxpage)
				whileflag=false;

		}

//not in any open files
//check ./ from all files
//dont do this from popup for speed reasons
//	if(recurse==true)
		{
			if(document->getDirPath()!=NULL)
				{
					getRecursiveTagListFileName((char*)document->getDirPath().toStdString().c_str(),&functions);//TODO//
					if(functions!=NULL)
						{
							str=functions;
							strlist=str.split("\n",Qt::SkipEmptyParts);
							gotmatch=-1;
							for(int i=0;i<strlist.size();i++)
								{
									if(strlist.at(i).startsWith(name,cs))
										{
											gotmatch=0;
											lineptr=strdup(strlist.at(i).toLocal8Bit().constData());
											break;
										}
								}

							if(gotmatch!=-1)
								{
									sscanf (lineptr, "%s\t%s\t%i",funcname,filepath,&linenumber);
									fdata=(functionData*)malloc(sizeof(functionData));
									fdata->name=strdup(funcname);
									fdata->file=strdup(filepath);
									fdata->line=linenumber+1;
									fdata->type=NULL;
									fdata->define=NULL;
									fdata->intab=-1;
									return(fdata);
								}
						}
					
				}
		}
	return(NULL);			
}					getRecursiveTagList((char*)document->filePath.toStdString().c_str(),&functions);//TODO//
*/
void KKEditClass::goToDefinition(const QString txt)
{
	DocumentClass	*doc=this->getDocumentForTab(-1);
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
#if 0
void KKEditClass::goToDefinition(const QString txt)
{
	DocumentClass	*doc=this->getDocumentForTab(-1);
	functionData	*fdata=NULL;
	char			*selection;
	const char		*selectionptr;
	QString			searchfor;

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

//	if(document==NULL)
//		return;

	//selection=strdup(document->textCursor().selectedText().toUtf8().constData());
	selection=strdup(searchfor.toUtf8().constData());
	selectionptr=selection;
fprintf(stderr,">>>>>>>>>>>>>>>\n");
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
#endif

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

