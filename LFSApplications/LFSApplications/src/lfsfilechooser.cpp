/*
 *
 * ©K. D. Hedger. Wed 17 Jan 15:59:32 GMT 2024 keithdhedger@gmail.com

 * This file (lfsfilechooser.cpp) is part of LFSApplications.

 * LFSApplications is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * LFSApplications is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSApplications.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include <getopt.h>

#include "config.h"
#include <lfstk/LFSTKGlobals.h>

LFSTK_applicationClass	*apc=NULL;
LFSTK_windowClass		*wc=NULL;
LFSTK_fileDialogClass	*filedialogfile;
LFSTK_fileDialogClass	*filedialogdir;

bool						showFolder=false;
Window					parentWindow=-1;
char						*wd;

int main(int argc, char **argv)
{
	int			c=0;
	int			option_index=0;
	const char	*shortOpts="fh?w:";
	int			retval=1;

	option 		longOptions[]=
		{
				{"window",1,0,'w'},
				{"folder",0,0,'f'},
				{"help",0,0,'h'},
				{0, 0, 0, 0}
		};

	while(1)
		{
			option_index=0;
			c=getopt_long_only(argc,argv,shortOpts,longOptions,&option_index);
			if (c==-1)
				break;
			switch (c)
				{
					case 'f':
						showFolder=true;
						break;
					case 'h':
					case '?':
						printf("-?,-h,--help\t\tPrint this help\n");
						printf("-f,--folder\t\tSelect folder instead of file\n");
						printf("-w,--window\t\tSet transient for window\n");
						exit(0);
					case 'w':
						parentWindow=atoi(optarg);
						break;
				}
		}

	apc=new LFSTK_applicationClass();
	apc->LFSTK_addWindow(NULL,"","");
	wc=apc->mainWindow;

	asprintf(&wd,"%s",apc->userHome.c_str());
	if(parentWindow!=-1)
		wc->LFSTK_setTransientFor(parentWindow);

	if(showFolder==false)
		{
			std::string	mimetype;

			filedialogfile=new LFSTK_fileDialogClass(wc,"Select File",wd,FILEDIALOG);
			filedialogfile->LFSTK_showFileDialog(NULL,"Select A File");

			if(filedialogfile->LFSTK_isValid()==true)
				{
					printf("Selected File Path=%s\n",filedialogfile->LFSTK_getCurrentPath().c_str());
					printf("Selected Dir Path=%s\n",filedialogfile->LFSTK_getCurrentDir().c_str());
					printf("Selected File Name=%s\n",filedialogfile->LFSTK_getCurrentFile().c_str());
					mimetype=wc->globalLib->LFSTK_getMimeType(filedialogfile->LFSTK_getCurrentPath());
					printf("File Mime-Type=%s\n",mimetype.c_str());
					retval=0;
				}
		}
	else
		{
			filedialogdir=new LFSTK_fileDialogClass(wc,"Select Folder",NULL,FOLDERDIALOG);
			filedialogdir->LFSTK_showFileDialog(NULL,"Select A Folder");
			if(filedialogdir->LFSTK_isValid()==true)
				{
					printf("Selected Folder=%s\n",filedialogdir->LFSTK_getCurrentDir().c_str());
				}
		}

	delete filedialogfile;
	delete filedialogdir;
	delete apc;
	free(wd);
	cairo_debug_reset_static_data();
	return(retval);
}