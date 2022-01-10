/*
 *
 * ©K. D. Hedger. Thu 23 Dec 20:38:16 GMT 2021 keithdhedger@gmail.com

 * This file (main.cpp) is part of KKEditQT.

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

#include "kkedit-includes.h"
#include "SingleInstanceClass.h"

bool	singleOverRide=false;
bool	loadPluginsFlag=true;

int main (int argc, char **argv)
{
	int					status;
	bool				safeflag=false;
	bool				retval=false;
	QDir				commsDir;
	QApplication		app(argc,argv);
	KKEditClass  		*kkedit;

	app.setStyleSheet("QMenu { menu-scrollable: true ;}");
	app.setOrganizationName("KDHedger");
	app.setApplicationName("KKEditQT");

	singleOverRide=false;
	loadPluginsFlag=true;

	if(safeflag==true)
		{
			singleOverRide=true;
			loadPluginsFlag=false;
		}

	kkedit=new KKEditClass(&app);
	kkedit->parser.addHelpOption();
	QCommandLineOption forcemultiinst(QStringList() << "m" << "multi","Force multiple instance.");
	kkedit->parser.addOption(forcemultiinst);
	QCommandLineOption opensession(QStringList() << "s" << "restore-session","SessionName","Open session by name.");
	kkedit->parser.addOption(opensession);
	QCommandLineOption quitapp(QStringList() << "q" << "quit","Quit app.");
	kkedit->parser.addOption(quitapp);

	kkedit->parser.process(app);

	SingleInstanceClass siapp(&app,&kkedit->parser);

	if(siapp.getRunning()==true)
		{
			kkedit->runCLICommands(siapp.queueID);
			return(0);
		}

	kkedit->forcedMultInst=kkedit->parser.isSet("multi");
	kkedit->currentWorkSpace=siapp.workspace;;
	kkedit->sessionID=kkedit->currentWorkSpace+MSGKEY;
	kkedit->forceDefaultGeom=!siapp.isOnX11;

	kkedit->initApp(argc,argv);

	kkedit->runCLICommands(kkedit->queueID);

	if(getuid()!=0)
		app.setWindowIcon(QIcon::fromTheme(PACKAGE,QIcon(DATADIR"/pixmaps/KKEdit.png")));
	else
		app.setWindowIcon(QIcon::fromTheme(PACKAGE "Root",QIcon(DATADIR"/pixmaps/KKEditRoot.png")));

	status=app.exec();

	delete kkedit;
	return status;
}


