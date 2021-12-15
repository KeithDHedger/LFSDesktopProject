/*
 *
 * ©K. D. Hedger. Thu 18 Nov 11:50:33 GMT 2021 keithdhedger@gmail.com

 * This file (KKEditClassCallbacks.cpp) is part of KKEdit.

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
#include <QClipboard>

void KKEditClass::doSelectTab()
{
	MenuItemClass	*mc=qobject_cast<MenuItemClass*>(sender());
	QTabBar			*bar=this->mainNotebook->tabBar();

	bar->setTabVisible(mc->getMenuID(),true);
	bar->setCurrentIndex(mc->getMenuID());
}

void KKEditClass::doBookmarkMenuItems()
{
	MenuItemClass	*mc=qobject_cast<MenuItemClass*>(sender());
	DocumentClass	*document=this->getDocumentForTab(-1);

	fprintf(stderr,"bookmarks -> menu id=%i, menu name=%s\n",mc->getMenuID(),mc->text().toStdString().c_str());
	switch(mc->getMenuID())
		{
			case REMOVEALLBOOKMARKSMENUITEM:
				this->rebuildBookMarkMenu();
				break;
			case TOGGLEBOOKMARKMENUITE:
				this->handleBMMenu(this->mainNotebook->currentWidget(),TOGGLEBMCLICKED);
				break;
			default:
				this->handleBMMenu(this->mainNotebook->currentWidget(),mc->getMenuID());
				break;
		}
}

void KKEditClass::doToolsMenuItems()
{
	MenuItemClass	*mc=qobject_cast<MenuItemClass*>(sender());
	DocumentClass	*document=this->getDocumentForTab(-1);

	fprintf(stderr,"tools -> menu id=%i, menu name=%s\n",mc->getMenuID(),mc->text().toStdString().c_str());
	switch(mc->getMenuID())
		{
			 case MANAGETOOLSMENUITEM:
			 	break;
		}
}

void KKEditClass::doHelpMenuItems()
{
	MenuItemClass	*mc=qobject_cast<MenuItemClass*>(sender());
	DocumentClass	*document=this->getDocumentForTab(-1);

	fprintf(stderr,"help -> menu id=%i, menu name=%s\n",mc->getMenuID(),mc->text().toStdString().c_str());
	switch(mc->getMenuID())
		{
			 case ABOUTMENUITEM:
			 	doAbout(NULL,0);
			 	break;
			 case HELPMENUITEM:
			 	openHelp(NULL,0);
			 	break;
			 case ONLINEHELPMENUITEM:
			 	openHelp(NULL,0);
			 	break;
			 case GETPLUGSMENUITEM:
				 getPlugins(NULL,0);
			 	break;
		}
}

void KKEditClass::doNavMenuItems()
{
	MenuItemClass	*mc=qobject_cast<MenuItemClass*>(sender());
	DocumentClass	*document=this->getDocumentForTab(-1);

	fprintf(stderr,"nav -> menu id=%i, menu name=%s\n",mc->getMenuID(),mc->text().toStdString().c_str());
	switch(mc->getMenuID())
		{
			case GOBACKMENUITEM:
				break;
			case GOFORWARDMENUITEM:
				break;
			case GOTODEFINEMENUITEM:
				this->goToDefinition("");
				break;
			case OPENINCLUDEMENUITEM:
				this->findFile();
				break;
			case GOTOLINEMENUITEM:
				this->showLineEntry();
				break;
			case SEARCHFORDEFINEMENUITEM:
				functionSearch(NULL,0);
				break;
			case SEARCHGTKDOCS:
				this->searchGtkDocs("",0);
				fprintf(stderr,"SEARCHGTKDOCS\n");
				break;
			case SEARCHQT5DOCS:
				fprintf(stderr,"SEARCHQT5DOCS >>%s<<\n",QT5DOCSDIR);
				this->searchGtkDocs("",1);
				break;
		}
}

void KKEditClass::doViewMenuItems()
{
	MenuItemClass	*mc=qobject_cast<MenuItemClass*>(sender());
	DocumentClass	*document=this->getDocumentForTab(-1);

	fprintf(stderr,"view -> menu id=%i, menu name=%s\n",mc->getMenuID(),mc->text().toStdString().c_str());
	switch(mc->getMenuID())
		{
			case DOCSMENUITEM:
				this->showDocs();
				break;
			case TOGGLETOOLBARMENUITEM:
				this->toolbarVisible=!this->toolbarVisible;
				if(this->toolbarVisible)
					{
						this->toggleToolBarMenuItem->setText("Hide Tool Bar");
						this->toolBar->show();
					}
				else
					{
						this->toggleToolBarMenuItem->setText("Show Tool Bar");
						this->toolBar->hide();
					}
				break;
			case TOGGLETOOLWINDOWMENUITEM:
				this->toolWindowVisible=!this->toolWindowVisible;
				if(this->toolWindowVisible)
					this->toggleToolWindowMenuItem->setText("Hide Tool Output");
				else
					this->toggleToolWindowMenuItem->setText("Show Tool Output");
				break;
			case TOGGLESTATUSBARMENUITEM:
				this->statusBarVisible=!this->statusBarVisible;
				if(this->statusBarVisible)
					{
						this->toggleStatusBarMenuItem->setText("Hide Status Bar");
						this->statusBar->show();
					}
				else
					{
						this->toggleStatusBarMenuItem->setText("Show Status Bar");
						this->statusBar->hide();
					}
				break;
			case TOGGLEDOCVIEWMENUITEM:
				this->docviewerVisible=!this->docviewerVisible;
				if(this->docviewerVisible)
					{
						this->toggleDocViewMenuItem->setText("Hide Docviewer");
						this->docView->show();
					}
				else
					{
						this->toggleDocViewMenuItem->setText("Show Docviewer");
						this->docView->hide();
					}
				break;
			case TOGGLELINENUMBERSMENUITEM:
				this->lineNumbersVisible=!this->lineNumbersVisible;
				this->resetAllFilePrefs();
				break;
			case TOGGLELINEWRAPMENUITEM:
				this->wrapLine=!this->wrapLine;
				this->resetAllFilePrefs();
				break;
			case TOGGLEHILITELINEMENUITEM:
				this->hiliteCurrentLine=!this->hiliteCurrentLine;
				this->resetAllFilePrefs();
				break;
			case TOGGLEWHITESPACEMENUITEM:
				this->showWhiteSpace=!this->showWhiteSpace;
				this->resetAllFilePrefs();
				break;
			case TOGGLESYNTAXHILITINGMENUITEM:
				this->showHighLighting=!this->showHighLighting;
				this->resetAllFilePrefs();
				break;
			case TOGGLECOMPLETIONSMENUITEM:
				this->showCompletions=!this->showCompletions;
				break;
		}
}

void KKEditClass::doEditMenuItems()
{
	MenuItemClass	*mc=qobject_cast<MenuItemClass*>(sender());
	DocumentClass	*document=this->getDocumentForTab(-1);

	fprintf(stderr,"edit -> menu id=%i, menu name=%s\n",mc->getMenuID(),mc->text().toStdString().c_str());

	switch(mc->getMenuID())
		{
			case UNDOMENUITEM:
				if(document!=NULL)
					document->undo();
				break;
			case REDOMENUITEM:
				if(document!=NULL)
					document->redo();
				break;
			case UNDOALLMENUITEM:
				if(document!=NULL)
					while(document->document()->availableUndoSteps()>0)
						document->undo();
				break;
			case REDOALLMENUITEM:
				if(document!=NULL)
					{
						while(document->document()->availableRedoSteps()>0)
							document->redo();
						document->redo();
					}
				break;
			case CUTMENUITEM:
				if(document!=NULL)
					document->cut();
				break;
			case COPYMENUITEM:
				if(document!=NULL)
					document->copy();
				break;
			case PASTEMENUITEM:
				if(document!=NULL)
					document->paste();
				break;
			case FINDMENUITEM:
				this->findReplaceDialog->show();
			case FINDNEXTMENUITEM:
				doFindReplace(FINDNEXT);
				break;
			case SORTTABSMENUITEM:
				this->sortTabs();
				break;
			case SHOWALLTABSMENUITEM:
				for(int j=0;j<this->mainNotebook->count();j++)
					this->mainNotebook->setTabVisible(j,true);
				break;
			case PREFSMENUITEM:
				this->doPrefs(NULL,0);
				break;
			case PLUGPREFSMENUITEM:
				doPlugPrefs(NULL,0);
				break;
		}
}

void KKEditClass::doFileMenuItems()
{
	MenuItemClass	*mc=qobject_cast<MenuItemClass*>(sender());
	bool			retval;
	fprintf(stderr,"file -> menu id=%i, menu name=%s\n",mc->getMenuID(),mc->text().toStdString().c_str());

	switch(mc->getMenuID())
		{
//file menu
			case NEWMENUITEM:
				this->newFile();
				break;
			case OPENMENUITEM:
				this->openFileDialog();
				break;
			case HEXDUMPMENUITEM:
				this->openAsHexDump();
				break;
			case NEWADMINEDMENUITEM:
				this->newEditor(NEWADMINEDMENUITEM);
				break;
			case NEWEDMENUITEM:
				this->newEditor(NEWEDMENUITEM);
				break;
			case
				MANPAGEEDMENUITEM:
				this->newEditor(MANPAGEEDMENUITEM);
				break;
			case BUILDDOCSMENUITEM:
				this->buildDocs();
				break;
			case SAVEMENUITEM:
				this->saveFile(-1,false);
				this->setToobarSensitive();
				break;
			case SAVEASMENUITEM:
				this->saveFileAs(-1);
				break;
			case SAVEALLMENUITEM:
				this->saveAllFiles();
				break;
			case SAVESESSIONMENUITEM:
				saveSession(NULL,SAVESESSIONMENUITEM);
				break;
			case RESTORESESSIONMENUITEM:
				restoreSession(NULL,RESTORESESSIONMENUITEM);
				break;
			case PRINTMENUITEM:
				printFile(NULL,PRINTMENUITEM);
				break;
			case CLOSEMENUITEM:
				this->closingAllTabs=false;
				retval=this->closeTab(-1);
				break;
			case CLOSEALLMENUITEM:
				this->closeAllTabs();
				break;
			case REVERTMENUITEM:
				this->reloadDocument();
				break;
			case QUITMENUITEM:
				this->shutDownApp();
				break;
		}
}

void KKEditClass::doTimer(void)
{
	int			retcode=0;
	msgStruct	buffer;

	while(retcode!=-1)
		{
			buffer.mText[0]=0;
			retcode=msgrcv(this->queueID,&buffer,MAXMSGSIZE,MSGANY,IPC_NOWAIT);
			buffer.mText[retcode]=0;
			if(retcode!=-1)
				{
					switch(buffer.mType)
						{
							case OPENFILE:
								this->openFile(buffer.mText);
								break;
							case SAVEFILE:
								break;
							case QUITAPP:
								//doShutdown(NULL,0);
								//this->
								break;
							case ACTIVATEAPP://TODO//minimized
								this->application->setActiveWindow(this->mainWindow);
								this->mainWindow->activateWindow();
								break;
						}
				}
		}
}

void KKEditClass::doDoubleClickPrefs(QListWidgetItem *item)
{
	const char		*data="NOSXCPURFG9ADLsEQBW";
	int				row=listWidget->currentRow();
	QListWidgetItem	*titem=this->listWidget->takeItem(row);

	for(unsigned int j=0;j<strlen(data);j++)
		{
			if(item->type()==data[j])
				{
					this->tool[j]->setEnabled(true);
					break;
				}
		}
	delete titem;
}

void KKEditClass::setPreferences(void)
{
	this->prefsIndent=qobject_cast<QCheckBox*>(this->prefsWidgets[AUTOINDENT])->checkState();
	this->prefsLineWrap=qobject_cast<QCheckBox*>(this->prefsWidgets[WRAP])->checkState();
	this->prefsShowLineNumbers=qobject_cast<QCheckBox*>(this->prefsWidgets[SHOWNUMS])->checkState();
	this->prefsHighLightline=qobject_cast<QCheckBox*>(this->prefsWidgets[HIGHLIGHT])->checkState();
	this->prefsDepth=qobject_cast<QCheckBox*>(this->prefsWidgets[MAXFUNCDEPTH])->checkState();
	this->prefsTabWidth=qobject_cast<QSpinBox*>(this->prefsIntWidgets[TABWIDTH])->value();
	this->prefsMaxTabChars=qobject_cast<QSpinBox*>(this->prefsIntWidgets[MAXTABCHARS])->value();
	this->prefsMaxFuncChars=qobject_cast<QSpinBox*>(this->prefsIntWidgets[MENUWIDTH])->value();
	this->prefsFunctionMenuLayout=qobject_cast<QComboBox*>(this->prefsOtherWidgets[FUNCTIONCOMBO])->currentIndex();
	this->prefsSyntaxHilighting=qobject_cast<QCheckBox*>(this->prefsWidgets[SYNTAXHILITE])->checkState();
	this->prefsAutoShowCompletions=qobject_cast<QCheckBox*>(this->prefsWidgets[AUTOSHOW])->checkState();

//TODO//
	this->prefsWindow->hide();

	this->prefsToolBarLayout="";
	for (int j=0;j<this->listWidget->count();j++)
		this->prefsToolBarLayout+=this->listWidget->item(j)->type();

	this->setUpToolBar();
//show line numbers
	this->lineNumbersVisible=this->prefsShowLineNumbers;
	this->toggleLineNumbersMenuItem->setChecked(this->lineNumbersVisible);
//wrap lines
	this->wrapLine=this->prefsLineWrap;
	this->toggleWrapLineMenuItem->setChecked(this->wrapLine);
//hilite current line
	this->hiliteCurrentLine=this->prefsHighLightline;
	this->toggleHiliteCurrentLineMenuItem->setChecked(this->hiliteCurrentLine);
//syntax hiliting
	this->showHighLighting=this->prefsSyntaxHilighting;
	this->toggleSyntaxHilightingMenuItem->setChecked(this->showHighLighting);
//completions
	this->showCompletions=this->prefsAutoShowCompletions;
	this->toggleCompletionsMenuItem->setChecked(this->showCompletions);

	this->resetAllFilePrefs();
	this->writeExitData();
	this->setAppShortcuts();
	switchPage(this->mainNotebook->currentIndex());
}

void KKEditClass::setBMColour(void)
{
	QColor	tmpcolour;
	tmpcolour=QColorDialog::getColor(QColor(this->prefsBookmarkHiLiteColor),0,"Select Color",QColorDialog::ShowAlphaChannel);
	if(tmpcolour.isValid())
		{
			this->prefsOtherWidgets[BMHIGHLIGHTCOLOUR]->setProperty("palette",QPalette(tmpcolour));
			this->prefsOtherWidgets[BMHIGHLIGHTCOLOUR]->setProperty("autoFillBackground",true);
			this->prefsOtherWidgets[BMHIGHLIGHTCOLOUR]->setProperty("text",tmpcolour.name());
			this->prefsBookmarkHiLiteColor=tmpcolour;
		}
}

void KKEditClass::setLineColour(void)
{
	QColor	tmpcolour;
	tmpcolour=QColorDialog::getColor(QColor(this->prefsHiLiteLineColor),0,"Select Color",QColorDialog::ShowAlphaChannel);
	if(tmpcolour.isValid())
		{
			this->prefsOtherWidgets[CURRENTLINECOLOUR]->setProperty("palette",QPalette(tmpcolour));
			this->prefsOtherWidgets[CURRENTLINECOLOUR]->setProperty("autoFillBackground",true);
			this->prefsOtherWidgets[CURRENTLINECOLOUR]->setProperty("text",tmpcolour.name());
			this->prefsHiLiteLineColor=tmpcolour;
		}
}

void KKEditClass::setFont(void)
{
	bool	ok;
	QFont	tmpfont;

	tmpfont=QFontDialog::getFont(&ok,this->prefsDocumentFont,this->mainWindow);
	if(ok==true)
		{
			this->prefsDocumentFont=tmpfont;
			qobject_cast<QLabel*>(this->prefsOtherWidgets[PREFSCURRENTFONT])->setText(QString("%1 %2").arg(tmpfont.family()).arg(tmpfont.pointSize()));
		}
}

void KKEditClass::addToToolBar(void)
{
	MenuItemClass	*mc=qobject_cast<MenuItemClass*>(sender());

	this->prefsToolBarLayout=(char*)qobject_cast<MenuItemClass*>(mc)->objectName().constData();
	this->populateStore();
}

void KKEditClass::doTabBarContextMenu(void)
{
	MenuItemClass	*mc=qobject_cast<MenuItemClass*>(sender());
	bool			retval;
	QClipboard		*clipboard=this->application->clipboard();
	DocumentClass	*doc;

	doc=this->getDocumentForTab((mc->getMenuID() & 0xff));
	fprintf(stderr,"doTabBarContextMenu -> menu id=%x, menu name=%s\n",mc->getMenuID(),mc->text().toStdString().c_str());
	switch(mc->getMenuID() & 0xfff00)
		{
			case COPYFOLDERPATH:
				clipboard->setText(doc->getDirPath());
				break;
			case COPYFILEPATH:
				clipboard->setText(doc->getFilePath());
				break;
			case COPYFILENAME:
				clipboard->setText(doc->getFileName());
				break;
			case SPELLCHECKDOC:
				fprintf(stderr,"SPELLCHECKDOC\n");
				this->checkDoc(doc);
				break;
			case OPENFROMHERE:
				this->openFile(QString("%1/%2").arg(doc->getDirPath()).arg(mc->text()));
				break;
			case HIDETAB:
				this->mainNotebook->setTabVisible(mc->getMenuID() & 0xff,false);
				break;
			case LOCKCONTENTS:
				doc->setReadOnly(!doc->isReadOnly());
				if(doc->isReadOnly()==true)
						doc->setTabName(QString("+%1").arg(doc->getTabName()));
				else
					if(doc->dirty==true)
						doc->setTabName(this->truncateWithElipses(doc->getFileName(),this->prefsMaxTabChars)+"*");
					else
						doc->setTabName(this->truncateWithElipses(doc->getFileName(),this->prefsMaxTabChars));
				break;
		}
	this->setToobarSensitive();
}

void KKEditClass::doTabBarContextMenuSetHilite(void)
{
	MenuItemClass	*mc=qobject_cast<MenuItemClass*>(sender());
	DocumentClass	*doc;
	int				themenum=2;

	doc=this->getDocumentForTab((mc->getMenuID() & 0xff));
	if(doc==NULL)
		return;

	this->sessionBusy=true;
	switch(mc->getMenuID() & 0xfff00)
		{
			case NONESRCCODE:
				themenum=-1;
				break;
			case CPPSRCCODE:
				doc->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeCpp);
				break;
			case CSRCCODE:
				doc->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeC);
				break;
			case BASHSRCCODE:
				doc->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeBash);
				break;
			case PYTHONSRCCODE:
				doc->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodePython);
				break;
			case GOSRCCODE:
				doc->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeGo);
				break;
			case LUASRCCODE:
				doc->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeLua);
				break;
			case YAMLSRCCODE:
				doc->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeYAML);
				break;
			case PHPSRCCODE:
				doc->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodePHP);
				break;
			case XMLSRCCODE:
				doc->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeXML);
				break;
			case CSSSRCCODE:
				doc->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeCSS);
				break;
			case JSSRCCODE:
				doc->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeJs);
				break;
			case MAKESRCCODE:
				doc->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeMake);
				break;
		}
	doc->highlighter->setTheme((QSourceHighliter::Themes)themenum);
	this->sessionBusy=false;
}

void KKEditClass::doOddMenuItems(void)
{
	MenuItemClass	*mc=qobject_cast<MenuItemClass*>(sender());
	bool			retval;
	QClipboard		*clipboard=this->application->clipboard();
	DocumentClass	*doc=this->getDocumentForTab(-1);

		switch(mc->getMenuID())
			{
				case SPELLCHECKMENUITEM:
#ifdef _ASPELL_
					if(doc->textCursor().selectedText().isEmpty()==false)
						{
							this->returnWord=false;
							this->setUpSpellGUI(doc->textCursor().selectedText(),doc);
						}
#endif
					break;
			}
}

void KKEditClass::doOddButtons(void)
{
	DocumentClass	*doc=this->getDocumentForTab(-1);
	switch(sender()->objectName().toInt())
		{
#ifdef _ASPELL_
			case APPLYWORDBUTTON:
				fprintf(stderr,"APPLYWORD\n");
				this->spellCheckGUI->hide();
				if(this->returnWord==false)
					{
						doc->textCursor().removeSelectedText();
						doc->textCursor().insertText(this->wordDropBox->currentText());
					}
				else
					this->goodWord=this->wordDropBox->currentText();
				this->returnWord=false;
				break;
			case IGNOREWORDBUTTON:
				fprintf(stderr,"IGNOREWORDBUTTON\n");
				if(this->returnWord==false)
					aspell_speller_add_to_session(this->spellChecker,doc->textCursor().selectedText().toStdString().c_str(),-1);
				else
					aspell_speller_add_to_session(this->spellChecker,this->badWord.toStdString().c_str(),-1);
				this->spellCheckGUI->hide();
				this->returnWord=false;
				break;
			case ADDWORDBUTTON:
				fprintf(stderr,"ADDWORDBUTTON\n");
				aspell_speller_add_to_personal(this->spellChecker,this->badWord.toStdString().c_str(),-1);
				aspell_speller_save_all_word_lists(this->spellChecker);
				this->spellCheckGUI->hide();
				break;
			case CANCELSPELLCHECK:
				fprintf(stderr,"CANCELSPELLCHECK\n");
				this->cancelCheck=true;
				this->spellCheckGUI->hide();
				break;
#endif
			case CANCELPREFS:
				this->prefsWindow->hide();
				this->readConfigs();
				this->resetAllFilePrefs();
				this->switchPage(this->mainNotebook->currentIndex());
				break;
			case DOLIVESEARCH:
				fprintf(stderr,"DOLIVESEARCH\n");
				break;
			case DOAPISEARCH:
				fprintf(stderr,"DOAPISEARCH\n");
				this->goToDefinition(this->findDefWidget->text());
				break;
			case DOQT5SEARCH:
				fprintf(stderr,"DOQT5SEARCH\n");
				this->searchGtkDocs(this->findQtApiWidget->text(),1);
				break; 
			case DOGTKSEARCH:
				fprintf(stderr,"DOGTKSEARCH gtkwidget QLineEdit\n");
				this->searchGtkDocs(this->findGtkApiWidget->text(),0);
				break;
			default:
				break;
		}
}



