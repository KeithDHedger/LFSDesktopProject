/*
 *
 * ©K. D. Hedger. Thu 30 Dec 16:36:29 GMT 2021 keithdhedger@gmail.com

 * This file (KKEditClassTools.cpp) is part of KKEditQT.

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

void KKEditClass::rebuildToolsMenu(void)
{
	QFile			file;
	QStringList		sl;
	QString			toolsdir=this->toolsFolder;
	QDir			dir(toolsdir);
	QStringList		flist=dir.entryList(QDir::Files);
	MenuItemClass	*menuItemSink;
	int				dropnum=0;

	this->toolSelect->clear();
	this->toolSelect->addItem("New Tool");
	this->toolsMenu->clear();

	menuItemSink=this->makeMenuItemClass(TOOLSMENU,"Manage External Tools",0,"accessories-text-editor","NOTNEEDED",MANAGETOOLSMENUITEM);
	this->toolsMenu->addSeparator();

	//QTextStream(stderr) << ">>>>>>>>>>>>>>>>>>>>>>>>>>" << Qt::endl;
	for(int k=0; k<flist.count(); k++)
		{
			sl=this->verifyTool(QString("%1/%2").arg(toolsdir).arg(flist.at(k)));
			if(sl.isEmpty()==false)
				{
					if((sl.count()>=TOOL_END) && (sl.at(TOOL_NAME).startsWith("name")) && (sl.at(TOOL_COMMAND).startsWith("command")) && (sl.at(TOOL_COMMENT).startsWith("comment")))
						{
							menuItemSink=this->makeMenuItemClass(TOOLSMENU,flist.at(k),0,NULL,"NOTNEEDED",TOOLNUMBER+dropnum++);
							menuItemSink->setMenuString(toolsdir + "/" + flist.at(k));
							menuItemSink->inPopup=(bool)sl.at(TOOL_INPOPUP).section(TOOLSHOWINPOPUP,1,1).toInt();
							menuItemSink->alwaysInPopup=(bool)sl.at(TOOL_ALWAYS_IN_POPUP).section(TOOLALWAYSINPOPUP,1,1).toInt();
							this->toolSelect->addItem(flist.at(k),toolsdir + "/" + flist.at(k));
						}
				}
		}
//	QTextStream(stderr) << "<<<<<<<<<<<<<<<<<<<<<<<<<" << Qt::endl;
}

QStringList KKEditClass::verifyTool(QString filepath)
{
	QFile		file;
	QStringList	sl;

	file.setFileName(filepath);
	if(file.open(QIODevice::Text | QIODevice::ReadOnly))
		{
			QString line;
			QTextStream	in(&file);
			sl=QTextStream(&file).readAll().split("\n",Qt::SkipEmptyParts);
			sl.sort(Qt::CaseInsensitive);
			file.close();
		}

	while(sl.count()<TOOL_END)
		sl<<"XXX";

	if((sl.indexOf(QRegularExpression("^name.*$"))==-1) && (sl.indexOf(QRegularExpression("^command.*$"))==-1) && (sl.indexOf(QRegularExpression("^flags.*$"))==-1))
		{
			//QTextStream(stderr) << "This doesn't look like a tools file not fixing ... " << filepath << Qt::endl;
			sl.clear();
			return(sl);
		}

	if(sl.count()<TOOL_END)
		{
			QTextStream(stderr) << "Possible error with " << filepath << Qt::endl;
			int		cnt=0;
			while(cnt<TOOL_END)
				{
					switch(cnt)
						{
							case TOOL_ALWAYS_IN_POPUP:
								if(sl.at(cnt).section(TOOLALWAYSINPOPUP,0,0,QString::SectionIncludeTrailingSep|QString::SectionCaseInsensitiveSeps).trimmed().compare(TOOLALWAYSINPOPUP)!=0)
									{
										sl<< QString("%1	0").arg(TOOLALWAYSINPOPUP);
										sl.sort(Qt::CaseInsensitive);
										cnt=0;
										continue;
									}
								break;
							case TOOL_CLEAR_VIEW:
								if(sl.at(cnt).section(TOOLCLEAROP,0,0,QString::SectionIncludeTrailingSep|QString::SectionCaseInsensitiveSeps).trimmed().compare(TOOLCLEAROP)!=0)
									{
										sl<< QString("%1	0").arg(TOOLCLEAROP);
										sl.sort(Qt::CaseInsensitive);
										cnt=0;
										continue;
									}
								break;
							case TOOL_COMMAND:
								if(sl.at(cnt).section(TOOLCOMMAND,0,0,QString::SectionIncludeTrailingSep|QString::SectionCaseInsensitiveSeps).trimmed().compare(TOOLCOMMAND)!=0)
									{
										sl<< QString("%1	exit 1").arg(TOOLCOMMAND);
										sl.sort(Qt::CaseInsensitive);
										cnt=0;
										continue;
									}
								break;
							case TOOL_COMMENT:
								if(sl.at(cnt).section(TOOLCOMMENT,0,0,QString::SectionIncludeTrailingSep|QString::SectionCaseInsensitiveSeps).trimmed().compare(TOOLCOMMENT)!=0)
									{
										sl<< QString("%1	").arg(TOOLCOMMENT);
										sl.sort(Qt::CaseInsensitive);
										cnt=0;
										continue;
									}
								break;
							case TOOL_FLAGS:
								if(sl.at(cnt).section(TOOLFLAGS,0,0,QString::SectionIncludeTrailingSep|QString::SectionCaseInsensitiveSeps).trimmed().compare(TOOLFLAGS)!=0)
									{
										sl<< QString("%1	0").arg(TOOLFLAGS);
										sl.sort(Qt::CaseInsensitive);
										cnt=0;
										continue;
									}
								break;
							case TOOL_INPOPUP:
								if(sl.at(cnt).section(TOOLSHOWINPOPUP,0,0,QString::SectionIncludeTrailingSep|QString::SectionCaseInsensitiveSeps).trimmed().compare(TOOLSHOWINPOPUP)!=0)
									{
										sl<< QString("%1	0").arg(TOOLSHOWINPOPUP);
										sl.sort(Qt::CaseInsensitive);
										cnt=0;
										continue;
									}
								break;
							case TOOL_IN_TERM:
								if(sl.at(cnt).section(TOOLRUNINTERM,0,0,QString::SectionIncludeTrailingSep|QString::SectionCaseInsensitiveSeps).trimmed().compare(TOOLRUNINTERM)!=0)
									{
										sl<< QString("%1	0").arg(TOOLRUNINTERM);
										sl.sort(Qt::CaseInsensitive);
										cnt=0;
										continue;
									}
								break;
							case TOOL_NAME:
								if(sl.at(cnt).section(TOOLNAME,0,0,QString::SectionIncludeTrailingSep|QString::SectionCaseInsensitiveSeps).trimmed().compare(TOOLNAME)!=0)
									{
										sl<< QString("%1	Bad Tool").arg(TOOLNAME);
										sl.sort(Qt::CaseInsensitive);
										cnt=0;
										continue;
									}
								break;
							case TOOL_RUN_AS_ROOT:
								if(sl.at(cnt).section(TOOLRUNASROOT,0,0,QString::SectionIncludeTrailingSep|QString::SectionCaseInsensitiveSeps).trimmed().compare(TOOLRUNASROOT)!=0)
									{
										sl<< QString("%1	0").arg(TOOLRUNASROOT);
										sl.sort(Qt::CaseInsensitive);
										cnt=0;
										continue;
									}
								break;
							case TOOL_SHORTCUT_KEY:
								if(sl.at(cnt).section(TOOLKEY,0,0,QString::SectionIncludeTrailingSep|QString::SectionCaseInsensitiveSeps).trimmed().compare(TOOLKEY)!=0)
									{
										sl<< QString("%1	").arg(TOOLKEY);
										sl.sort(Qt::CaseInsensitive);
										cnt=0;
										continue;
									}
								break;
							case TOOL_USE_BAR:
								if(sl.at(cnt).section(TOOLUSEPOLE,0,0,QString::SectionIncludeTrailingSep|QString::SectionCaseInsensitiveSeps).trimmed().compare(TOOLUSEPOLE)!=0)
									{
										sl<< QString("%1	0").arg(TOOLUSEPOLE);
										sl.sort(Qt::CaseInsensitive);
										cnt=0;
										continue;
									}
								break;
						}
					cnt++;
				}
		}
	return(sl);
}

void KKEditClass::setToolsData(int what)
{
	QFile					file;
	QStringList				sl;
	QLineEdit				*edit;
	QCheckBox				*check;//TOGO//
	QRadioButton			*radio;
	const QSignalBlocker	blocker(sender());
	int						flags=0;
	bool					setradioenable=true;
	QCheckBox				*runintermcheck;
	QCheckBox				*inpopupcheck;
	QCheckBox				*alwaysincheck;
	QCheckBox				*synccheck;
	QCheckBox				*doccheck;
	QCheckBox				*clearcheck;
	QCheckBox				*rootcheck;
	QCheckBox				*barcheck;

	sl=this->verifyTool(this->toolSelect->currentData().toString());
	if(sl.isEmpty()==true)
		return;//TODO//make default tool

for(int j=0;j<TOOL_END;j++)
	QTextStream(stderr) << sl.at(j) << Qt::endl;
QTextStream(stderr) << what << Qt::endl;

	sl=this->verifyTool(this->toolSelect->currentData().toString());
	if(sl.isEmpty()==true)
		return;//TODO//make default tool

	runintermcheck=this->toolsWindow->findChild<QCheckBox*>(TOOLRUNINTERM);
	inpopupcheck=this->toolsWindow->findChild<QCheckBox*>(TOOLSHOWINPOPUP);
	alwaysincheck=this->toolsWindow->findChild<QCheckBox*>(TOOLALWAYSINPOPUP);
	synccheck=this->toolsWindow->findChild<QCheckBox*>(TOOLRUNSYNC);
	doccheck=this->toolsWindow->findChild<QCheckBox*>(TOOLSHOWDOC);
	clearcheck=this->toolsWindow->findChild<QCheckBox*>(TOOLCLEAROP);
	rootcheck=this->toolsWindow->findChild<QCheckBox*>(TOOLRUNASROOT);
	barcheck=this->toolsWindow->findChild<QCheckBox*>(TOOLUSEPOLE);

	if(sender()->objectName().compare(TOOLCOMBOBOX)==0)
		{
			QTextStream(stderr) << "from box " << sender()->objectName() << Qt::endl;
			flags=sl.at(TOOL_FLAGS).section(TOOLFLAGS,1,1).toInt();
			runintermcheck->setEnabled(true);
			inpopupcheck->setEnabled(true);
			alwaysincheck->setEnabled(true);
			synccheck->setEnabled(true);
			doccheck->setEnabled(true);
			clearcheck->setEnabled(true);
			rootcheck->setEnabled(true);
			barcheck->setEnabled(true);
//set boxes
			edit=this->toolsWindow->findChild<QLineEdit*>(TOOLNAME);
			edit->setText(sl.at(TOOL_NAME).section(TOOLNAME,1,1).trimmed());

			edit=this->toolsWindow->findChild<QLineEdit*>(TOOLCOMMAND);
			edit->setText(sl.at(TOOL_COMMAND).section(TOOLCOMMAND,1,1).trimmed());

			edit=this->toolsWindow->findChild<QLineEdit*>(TOOLCOMMENT);
			edit->setText(sl.at(TOOL_COMMENT).section(TOOLCOMMENT,1,1).trimmed());

			edit=this->toolsWindow->findChild<QLineEdit*>(TOOLKEY);
			edit->setText(sl.at(TOOL_SHORTCUT_KEY).section(TOOLKEY,1,1).trimmed());


//set sync
			setradioenable=true;
			if((flags & TOOL_ASYNC)==TOOL_ASYNC)
				setradioenable=false;

			synccheck->setCheckState((Qt::CheckState)(2*((flags & TOOL_ASYNC)!=TOOL_ASYNC)));

//set run in term
			runintermcheck->setCheckState((Qt::CheckState)(2*sl.at(TOOL_IN_TERM).section(TOOLRUNINTERM,1,1).toInt()));
			if(runintermcheck->checkState()==Qt::Checked)
				{
					setradioenable=false;
					doccheck->setEnabled(false);
					clearcheck->setEnabled(false);
				}
//set show doc
			doccheck->setCheckState((Qt::CheckState)(2*((flags & TOOL_SHOW_DOC)==TOOL_SHOW_DOC)));
			if(doccheck->checkState()==Qt::Checked)
				{
					setradioenable=false;
					clearcheck->setEnabled(false);
				}
//run as root
			rootcheck->setCheckState((Qt::CheckState)(2*sl.at(TOOL_RUN_AS_ROOT).section(TOOLRUNASROOT,1,1).toInt()));
//use bar
			barcheck->setCheckState((Qt::CheckState)(2*sl.at(TOOL_USE_BAR).section(TOOLUSEPOLE,1,1).toInt()));
//show in popup
			inpopupcheck->setCheckState((Qt::CheckState)(2*sl.at(TOOL_INPOPUP).section(TOOLSHOWINPOPUP,1,1).toInt()));
//always in popup
			alwaysincheck->setCheckState((Qt::CheckState)(2*sl.at(TOOL_ALWAYS_IN_POPUP).section(TOOLALWAYSINPOPUP,1,1).toInt()));
//clear op
			clearcheck->setCheckState((Qt::CheckState)(2*sl.at(TOOL_CLEAR_VIEW).section(TOOLCLEAROP,1,1).toInt()));

//enable radios
			radio=this->toolsWindow->findChild<QRadioButton*>(TOOLIGNOREOUT);
			radio->setChecked(true);
			radio->setEnabled(setradioenable);
			radio=this->toolsWindow->findChild<QRadioButton*>(TOOLPASTEOUT);
			radio->setEnabled(setradioenable);
			radio=this->toolsWindow->findChild<QRadioButton*>(TOOLREPLACEALL);
			radio->setEnabled(setradioenable);
			radio=this->toolsWindow->findChild<QRadioButton*>(TOOLVIEWOUT);
			radio->setEnabled(setradioenable);

			switch((flags & TOOL_INSERT_MASK))
				{
					case TOOL_IGNORE_OP:
						radio=this->toolsWindow->findChild<QRadioButton*>(TOOLIGNOREOUT);
						radio->setChecked(true);
						break;
					case TOOL_PASTE_OP:
						radio=this->toolsWindow->findChild<QRadioButton*>(TOOLPASTEOUT);
						radio->setChecked(true);
						break;
					case TOOL_REPLACE_OP:
						radio=this->toolsWindow->findChild<QRadioButton*>(TOOLREPLACEALL);
						radio->setChecked(true);
						break;
					case TOOL_VIEW_OP:
						radio=this->toolsWindow->findChild<QRadioButton*>(TOOLVIEWOUT);
						radio->setChecked(true);
						break;
				}

			
		}
	else
		{
			bool	resetradios=false;
			bool	syncflag=false;
			bool	termflag=false;
			bool	docflag=false;

			QTextStream(stderr) << "other " << sender()->objectName() << Qt::endl;
//run in term
			if(sender()->objectName().compare(TOOLRUNINTERM)==0)
				resetradios=true;
//set sync
			if(sender()->objectName().compare(TOOLRUNSYNC)==0)
				resetradios=true;
//set show doc
			if(sender()->objectName().compare(TOOLSHOWDOC)==0)
				resetradios=true;

//somthing changed
			if(resetradios==true)
				{
					termflag=(bool)runintermcheck->checkState();
					syncflag=(bool)synccheck->checkState();
					docflag=(bool)doccheck->checkState();
					synccheck->setEnabled(!termflag);
					setradioenable=true;
					if((syncflag==false) || (termflag==true) || (docflag==true))
						setradioenable=false;
					else
						setradioenable=true;

					doccheck->setEnabled(true);
					clearcheck->setEnabled(true);
				
					if(termflag==true)
						{
							doccheck->setEnabled(!termflag);
							clearcheck->setEnabled(!termflag);
							synccheck->setCheckState(Qt::Unchecked);
						}
					
					if(docflag==true)
						clearcheck->setEnabled(!docflag);
				
					radio=this->toolsWindow->findChild<QRadioButton*>(TOOLIGNOREOUT);
					radio->setEnabled(setradioenable);
					radio=this->toolsWindow->findChild<QRadioButton*>(TOOLPASTEOUT);
					radio->setEnabled(setradioenable);
					radio=this->toolsWindow->findChild<QRadioButton*>(TOOLREPLACEALL);
					radio->setEnabled(setradioenable);
					radio=this->toolsWindow->findChild<QRadioButton*>(TOOLVIEWOUT);
					radio->setEnabled(setradioenable);
				}
		}
}


