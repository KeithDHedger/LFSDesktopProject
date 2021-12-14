/*
 *
 * ©K. D. Hedger. Sun  5 Dec 16:00:07 GMT 2021 keithdhedger@gmail.com

 * This file (KKEditClassSearch.cpp) is part of KKEditQT.

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

void KKEditClass::doFindButton(void)
{
	this->doFindReplace(sender()->objectName().toInt());
}

void KKEditClass::doFindReplace(int response_id)
{
	int									flags=0;
	DocumentClass						*document=this->getDocumentForTab(-1);
	char								*currentfindtext;
	char								*currentreplacetext;
	const char							*thetext;
	GSList								*tlist;
	GSList								*list;
	QComboBox							*combo;
	bool								flag=false;
	int									cnt;
	QString								str;
	bool								gotresult;
	QColor								lineColor=QColor(Qt::green);
	QTextDocument						*doc;

	if(document==NULL)
		return;

	doc=document->document();
	QTextCursor							newCursor(doc);

	currentfindtext=strdup(this->findDropBox->currentText().toUtf8().constData());
	currentreplacetext=strdup(this->replaceDropBox->currentText().toUtf8().constData());

	QRegExp								rx(currentfindtext);

	if(this->insensitiveSearch==true)
		rx.setCaseSensitivity(Qt::CaseInsensitive);
	else
		{
			flags+=QTextDocument::FindCaseSensitively;
			rx.setCaseSensitivity(Qt::CaseSensitive);
		}

	flags+=(((response_id==FINDPREV)<<((QTextDocument::FindBackward)-1)));

	if((response_id==FINDNEXT) && (this->hightlightAll==true))
		{
			document->clearHilites();
			document->selection.format.setBackground(lineColor);
			while(!newCursor.isNull() && !newCursor.atEnd())
				{
					if(this->useRegex==false)
						newCursor=doc->find(currentfindtext,newCursor,(QTextDocument::FindFlags)flags);
					else
						newCursor=doc->find(rx,newCursor,(QTextDocument::FindFlags)flags);
					
					if(!newCursor.isNull())
						{
							document->selection.cursor=newCursor;
							document->hilightSelections.append(document->selection);
						}
				}
				document->setXtraSelections();
		}

	if(this->hightlightAll==false)
		{
			document->clearHilites();
		}

	if(response_id!=FINDREPLACE)
		{
			combo=this->findDropBox;
			list=findList;
			thetext=currentfindtext;
			if(this->useRegex==false)
				gotresult=document->find(thetext,(QTextDocument::FindFlags)flags);
			else
				gotresult=document->find(rx,(QTextDocument::FindFlags)flags);

			if((this->wrapSearch==true) && (gotresult==false))
				{
					int ln=document->textCursor().blockNumber()+1;
					this->gotoLine(0);
					if(this->useRegex==false)
						gotresult=document->find(thetext,(QTextDocument::FindFlags)flags);
					else
						gotresult=document->find(rx,(QTextDocument::FindFlags)flags);
					if(gotresult==false)
						this->gotoLine(ln);			
				}
		}
	else
		{
			combo=this->replaceDropBox;
			list=replaceList;
			thetext=currentreplacetext;
			if(this->replaceAll==false)
				{
					if(this->useRegex==false)
						{
							if(document->textCursor().hasSelection())
								document->textCursor().insertText(thetext);
							document->find(currentfindtext,(QTextDocument::FindFlags)flags);
						}
					else
						{
							if(document->textCursor().hasSelection())
								{
									str=document->textCursor().selectedText();
									str.replace(rx,thetext);
									document->textCursor().insertText(str);
								}
							document->find(rx,(QTextDocument::FindFlags)flags);
						}
				}
			else
				{
					cnt=0;
					this->gotoLine(0);
					if(this->useRegex==false)
						document->find(currentfindtext,(QTextDocument::FindFlags)flags);
					else
						document->find(rx,(QTextDocument::FindFlags)flags);

					while(!newCursor.isNull() && !newCursor.atEnd())
						{
							if(this->useRegex==false)
								newCursor=doc->find(currentfindtext,newCursor,(QTextDocument::FindFlags)flags);
							else
								newCursor=doc->find(rx,newCursor,(QTextDocument::FindFlags)flags);
					
							if(!newCursor.isNull())
								{
									if(this->useRegex==false)
										{
											newCursor.insertText(thetext);
											cnt++;
										}
									else
										{
											str=newCursor.selectedText();
											str.replace(rx,thetext);
											newCursor.insertText(str);
											cnt++;
										}
								}
						}
					this->statusBar->showMessage(QString("Replaced %1 occurrances of %2 with %3").arg(cnt).arg(currentfindtext).arg(thetext),STATUSBARTIMEOUT);
				}
		}

	if(list==NULL)
		{
			list=g_slist_append(list,strdup(thetext));
			combo->addItem(thetext);
		}
	else
		{
			tlist=list;
			flag=false;
			do
				{
					if(strcmp((const gchar*)tlist->data,thetext)==0)
						flag=true;
					tlist=tlist->next;
				}
			while(tlist!=NULL);

			if(flag==false)
				{
					list=g_slist_append(list,strdup(thetext));
					combo->addItem(thetext);
				}
		}

	debugFree(&currentfindtext,"doFindReplace currentfindtext");
	debugFree(&currentreplacetext,"doFindReplace currentreplacetext");
}

void KKEditClass::searchGtkDocs(const QString txt)
{
//	char*		selection=NULL;
//	char*		searchdata[2048][2];
//	char		line[1024];
//	FILE*		fp;
//	FILE*		fd;
//	char*		command=NULL;
//	char*		ptr=NULL;
//	char*		funcname;
//	char*		foldername;
//	char*		tempstr;
//	char*		link;
//	int			cnt=0;

	QString		searchfor;
	QString		funcname;
	QString		link;

	DocumentClass	*doc=this->getDocumentForTab(-1);

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
	fprintf(stderr,"txt=%s\n",searchfor.toStdString().c_str());
	QString	results=this->runPipeAndCapture(QString("find /usr/share/gtk-doc/html -iname \"*.devhelp2\" -exec grep -iHe %1 '{}' \\;").arg(searchfor));
//runPipeAndCapture
//			asprintf(&command,"find /usr/share/gtk-doc/html -iname \"*.devhelp2\" -exec grep -iHe %s '{}' \\;",selection);
	fprintf(stderr,"results=%s\n",results.toStdString().c_str());
	QStringList	reslist=results.split("\n",Qt::SkipEmptyParts);
	for(int j=0;j<reslist.count();j++)
	{
		//fprintf(stderr,"results=%s\n",reslist.at(j).toStdString().c_str());
	//funcname=globalSlice->sliceBetween(line,(char*)"name=\"",(char*)"\" link=");
	funcname=reslist.at(j).section("\" link=",0,0).section("name=\"",1,1);
	link=reslist.at(j).section("link=\"",1,1).section("\"",0,0);
QTextStream(stderr) << funcname << " " << link << Qt::endl;

							fprintf(stderr,"<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n");
							fprintf(stderr,"<html>\n");
							fprintf(stderr,"<body>\n");

							//for(int loop=0;loop<cnt;loop++)
							//	{
									fprintf(stderr,"<a href=\"%s\">%s</a><br>\n",link.toStdString().c_str(),funcname.toStdString().c_str());
							//	}
							fprintf(stderr,"</body>\n");
							fprintf(stderr,"</html>\n");


}
return;
#if 0
	if(document==NULL)
		return;

	for(int loop=0;loop<2048;loop++)
		{
			searchdata[loop][0]=NULL;
			searchdata[loop][1]=NULL;
		}

	if((gpointer)data!=NULL)
		selection=strdup((char*)data);
	else
		{
			selection=strdup(document->textCursor().selectedText().toUtf8().constData());
		}

	if(selection!=NULL)
		{
			asprintf(&command,"find /usr/share/gtk-doc/html -iname \"*.devhelp2\" -exec grep -iHe %s '{}' \\;",selection);
			fp=popen(command,"r");
			while(fgets(line,1024,fp))
				{
					ptr=strstr(line,"name=\"");
					if(ptr!=NULL)
						{
							funcname=globalSlice->sliceBetween(line,(char*)"name=\"",(char*)"\" link=");
							if(globalSlice->getResult()==0)
								{
									if(strstr(funcname,selection)!=NULL)
										{
											if(cnt<2048)
												{
													tempstr=globalSlice->sliceBetween(line,(char*)"",(char*)":");
													if(tempstr!=NULL)
														{
															foldername=g_path_get_dirname(tempstr);
															link=globalSlice->sliceBetween(line,(char*)"link=\"",(char*)"\"");
															if((foldername!=NULL) && (link!=NULL))
																{
																	searchdata[cnt][0]=strdup(funcname);
																	asprintf(&searchdata[cnt][1],"%s/%s",foldername,link);
																	debugFree(&foldername,"seachGtkDocs foldername");
																	debugFree(&link,"seachGtkDocs link");
																	cnt++;
																}
															debugFree(&tempstr,"seachGtkDocs tempstr");
														}
												}
										}
									debugFree(&funcname,"seachGtkDocs funcname");
									funcname=NULL;
								}
						}
				}

			if(cnt>1)
				{
					fd=fopen(htmlFile,"w");
					if(fd!=NULL)
						{								
							fprintf(fd,"<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n");
							fprintf(fd,"<html>\n");
							fprintf(fd,"<body>\n");

							for(int loop=0;loop<cnt;loop++)
								{
									fprintf(fd,"<a href=\"%s\">%s</a><br>\n",searchdata[loop][1],searchdata[loop][0]);
								}
							fprintf(fd,"</body>\n");
							fprintf(fd,"</html>\n");
							fclose(fd);
							thePage=strdup(htmlURI);
						}
				}
			else
				{
					asprintf(&thePage,"file://%s",searchdata[0][1]);
				}

			showDocView(USEURI,selection,"Gtk Docs");
		}

	for(int loop=0;loop<cnt;loop++)
		{
			if(searchdata[loop][0]!=NULL)
				debugFree(&searchdata[loop][0],"seachGtkDocs searchdata[loop][0]");
			if(searchdata[loop][1]!=NULL)
				debugFree(&searchdata[loop][1],"seachGtkDocs searchdata[loop][1]");
		}
	if((selection!=NULL) && ((gpointer)data==NULL))
		debugFree(&selection,"seachGtkDocs selection");
#endif
}
