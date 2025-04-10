/*
 *
 * ©K. D. Hedger. Sun  4 Sep 14:01:13 BST 2016 keithdhedger@gmail.com

 * This file (lfsfontselect.cpp) is part of LFSApplications.

 * LFSApplications is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSApplications is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSApplications.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include <lfstk/LFSTKGlobals.h>

#define BOXLABEL			"Font Dialog"
#define MAXMAINMENUS		25

LFSTK_applicationClass	*apc=NULL;
LFSTK_windowClass		*wc=NULL;
LFSTK_labelClass			*label=NULL;
LFSTK_labelClass			*personal=NULL;
LFSTK_labelClass			*copyrite=NULL;
LFSTK_buttonClass		*seperator=NULL;
LFSTK_buttonClass		*quit=NULL;
LFSTK_listGadgetClass	*fontlist=NULL;
LFSTK_toggleButtonClass	*boldcheck=NULL;
LFSTK_toggleButtonClass	*italiccheck=NULL;
LFSTK_lineEditClass		*fontsize=NULL;
LFSTK_lineEditClass		*preview=NULL;
LFSTK_fontDialogClass	*fontdialog=NULL;
LFSTK_prefsClass			prefs("lfsfontselect",VERSION);

unsigned					maxFonts=0;
char						**fontsAZ=NULL;
unsigned					size=10;
const char				*fontname=NULL;
unsigned					selectnumber=0;

bool						useDetail=false;
int						parentWindow=-1;

void printXtraHelp(void)
{
	printf("lfsfontselect [FONTSTRING]\n");
	printf("FONTSTRING = [FONTNAME][SIZE][BOLD][ITALIC]\n");
	printf("eg:Helvetica:size=18:bold:italic\n");
	printf("If --details is set outputs details on seperate line eg:\n");
	printf("Font String:Arial:size=10:italic\n");
	printf("Font:Arial\n");
	printf("Size:10\n");
	printf("Bold:false\n");
	printf("Italic:true\n");
}

int main(int argc, char **argv)
{
	const fontDataStruct	*fd;
	const char				*bools[]={"false","true"};

	option	longOptions[]=
		{
			{"window",1,0,'w'},
			{"detail",0,0,'d'},
			{0, 0, 0, 0}
		};
	prefs.prefsMap=
		{
			{LFSTK_UtilityClass::LFSTK_hashFromKey("window"),{TYPEINT,"window","Set transient for window ARG","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("detail"),{TYPEBOOL,"detail","Show details","",false,0}},
		};
	if(prefs.LFSTK_argsToPrefs(argc,argv,longOptions,true)==false)
		{
			printXtraHelp();
			return(1);
		}

	useDetail=prefs.LFSTK_getBool("detail");
	parentWindow=prefs.LFSTK_getInt("window");

	apc=new LFSTK_applicationClass();
	apc->LFSTK_addWindow(NULL,"");
	wc=apc->mainWindow;
	wc->LFSTK_showWindow();

	fontdialog=new LFSTK_fontDialogClass(wc,"Select Font",0,0,1,1,BUTTONGRAV);

	if(argv[optind]!=NULL)
		fontdialog->LFSTK_showDialog(argv[optind]);
	else
		fontdialog->LFSTK_showDialog("");

	fd=fontdialog->LFSTK_getFontData(false);
	if(fd->isValid==true)
		if(useDetail==false)
			printf("FONTSTRING='%s'\n",fd->fontString.c_str());
		else
			printf("Font String:%s\nFont:%s\nSize:%i\nBold:%s\nItalic:%s\n",fd->fontString.c_str(),fd->fontName.c_str(),fd->fontSize,bools[fd->bold],bools[fd->italic]);
		
	delete apc;
	cairo_debug_reset_static_data();

	return(0);
}
