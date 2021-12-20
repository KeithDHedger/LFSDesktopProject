/*
 *
 * ©K. D. Hedger. Sat 30 Oct 13:36:19 BST 2021 keithdhedger@gmail.com

 * This file (main.cpp) is part of KKEdit.

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
	QCommandLineParser	parser;
	bool				forcemulti;

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

	parser.addHelpOption();
	QCommandLineOption forcemultiinst(QStringList() << "m" << "multi","Force multiple instance.");
	parser.addOption(forcemultiinst);

	kkedit=new KKEditClass(&app);

	parser.process(app);
	forcemulti=parser.isSet(forcemultiinst);

	SingleInstanceClass siapp(&app,forcemulti);

	kkedit->forcedMultInst=forcemulti;
	if(siapp.getRunning()==true)
		return(0);

	kkedit->currentWorkSpace=siapp.workspace;;
	kkedit->sessionID=kkedit->currentWorkSpace+MSGKEY;
	kkedit->forceDefaultGeom=!siapp.isOnX11;

	history=new HistoryClass;

	kkedit->initApp(argc,argv);

	for(int j=1;j<argc;j++)
		kkedit->openFile(argv[j],0,true);

	if(getuid()!=0)
		app.setWindowIcon(QIcon::fromTheme(PACKAGE,QIcon(DATADIR"/pixmaps/KKEdit.png")));
	else
		app.setWindowIcon(QIcon::fromTheme(PACKAGE "Root",QIcon(DATADIR"/pixmaps/KKEditRoot.png")));
	status=app.exec();

	delete history;
	delete kkedit;
	return status;
}


