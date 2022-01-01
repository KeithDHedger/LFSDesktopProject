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

	for(int k=0; k<flist.count(); k++)
		{
			file.setFileName(toolsdir + "/" + flist.at(k));
			if(file.open(QIODevice::Text | QIODevice::ReadOnly))
				{
					QString line;
					QTextStream	in(&file);
					sl=QTextStream(&file).readAll().split("\n",Qt::SkipEmptyParts);

					if((sl.count()>2) && (sl.at(0).startsWith("name")) && (sl.at(1).startsWith("command")) && (sl.at(2).startsWith("comment")))
						{
							menuItemSink=this->makeMenuItemClass(TOOLSMENU,flist.at(k),0,NULL,"NOTNEEDED",TOOLNUMBER+dropnum++);
							menuItemSink->setMenuString(toolsdir + "/" + flist.at(k));
							this->toolSelect->addItem(flist.at(k),toolsdir + "/" + flist.at(k));
						}
					file.close();
				}
		}
}

void KKEditClass::setToolsData(int what)
{
	QFile					file;
	QStringList				sl;
	QLineEdit				*edit;
	QCheckBox				*check;
	QRadioButton			*radio;
	const QSignalBlocker	blocker(sender());
	int						flags=0;

	if(sender()->objectName().compare(TOOLRUNSYNC)!=0)
		{
			file.setFileName(this->toolSelect->currentData().toString());
			if(file.open(QIODevice::Text | QIODevice::ReadOnly))
				{
					QString line;
					QTextStream	in(&file);
					sl=QTextStream(&file).readAll().split("\n",Qt::SkipEmptyParts);
					file.close();
				}

			edit=this->toolsWindow->findChild<QLineEdit*>(TOOLNAME);
			edit->setText(sl.at(TNAME).section(TOOLNAME,1,1).trimmed());

			edit=this->toolsWindow->findChild<QLineEdit*>(TOOLCOMMAND);
			edit->setText(sl.at(TCOMMAND).section(TOOLCOMMAND,1,1).trimmed());

			edit=this->toolsWindow->findChild<QLineEdit*>(TOOLCOMMENT);
			edit->setText(sl.at(TCOMMENT).section(TOOLCOMMENT,1,1).trimmed());

//run in term
			check=this->toolsWindow->findChild<QCheckBox*>(TOOLRUNINTERM);
			check->setCheckState((Qt::CheckState)(2*sl.at(TINTERM).section(TOOLRUNINTERM,1,1).toInt()));
//show in popup
			check=this->toolsWindow->findChild<QCheckBox*>(TOOLSHOWINPOPUP);
			check->setCheckState((Qt::CheckState)(2*sl.at(TINPOPUP).section(TOOLSHOWINPOPUP,1,1).toInt()));
//always show in popup
			check=this->toolsWindow->findChild<QCheckBox*>(TOOLALWAYSINPOPUP);
			check->setCheckState((Qt::CheckState)(2*sl.at(TALWAYSPOPUP).section(TOOLALWAYSINPOPUP,1,1).toInt()));
//clear tool out
			check=this->toolsWindow->findChild<QCheckBox*>(TOOLCLEAROP);
			check->setCheckState((Qt::CheckState)(2*sl.at(TCLEARVIEW).section(TOOLCLEAROP,1,1).toInt()));
//run as root
			check=this->toolsWindow->findChild<QCheckBox*>(TOOLRUNASROOT);
			check->setCheckState((Qt::CheckState)(2*sl.at(TRUNASROOT).section(TOOLRUNASROOT,1,1).toInt()));
//use pole
			check=this->toolsWindow->findChild<QCheckBox*>(TOOLUSEPOLE);
			check->setCheckState((Qt::CheckState)(2*sl.at(TUSEBAR).section(TOOLUSEPOLE,1,1).toInt()));
//flags
			flags=sl.at(TFLAGS).section(TOOLFLAGS,1,1).toInt();
//show html doc
			check=this->toolsWindow->findChild<QCheckBox*>(TOOLSHOWDOC);
			check->setCheckState((Qt::CheckState)(2*((flags & TOOL_SHOW_DOC)==TOOL_SHOW_DOC)));
		}

//set default
	radio=this->toolsWindow->findChild<QRadioButton*>(TOOLIGNOREOUT);
	radio->setChecked(true);

//run sync
	check=this->toolsWindow->findChild<QCheckBox*>(TOOLRUNSYNC);

	if(((flags & TOOL_ASYNC)==TOOL_ASYNC) || (check->checkState()==Qt::Unchecked))
		{
			radio=this->toolsWindow->findChild<QRadioButton*>(TOOLIGNOREOUT);
			radio->setEnabled(false);
			radio=this->toolsWindow->findChild<QRadioButton*>(TOOLPASTEOUT);
			radio->setEnabled(false);
			radio=this->toolsWindow->findChild<QRadioButton*>(TOOLREPLACEALL);
			radio->setEnabled(false);
			radio=this->toolsWindow->findChild<QRadioButton*>(TOOLVIEWOUT);
			radio->setEnabled(false);
		}
	else
		{
			radio=this->toolsWindow->findChild<QRadioButton*>(TOOLIGNOREOUT);
			radio->setEnabled(true);
			radio=this->toolsWindow->findChild<QRadioButton*>(TOOLPASTEOUT);
			radio->setEnabled(true);
			radio=this->toolsWindow->findChild<QRadioButton*>(TOOLREPLACEALL);
			radio->setEnabled(true);
			radio=this->toolsWindow->findChild<QRadioButton*>(TOOLVIEWOUT);
			radio->setEnabled(true);
		}

	if(sender()->objectName().compare(TOOLRUNSYNC)==0)
		return;

	check->setCheckState((Qt::CheckState)(2*((flags & TOOL_ASYNC)!=TOOL_ASYNC)));

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


