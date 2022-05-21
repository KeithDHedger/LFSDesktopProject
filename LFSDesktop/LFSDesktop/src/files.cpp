/*
 *
 * ©K. D. Hedger. Thu 13 Aug 16:53:55 BST 2015 keithdhedger@gmail.com

 * This file (files.cpp) is part of LFSDesktop.

 * LFSDesktop is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSDesktop is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSDesktop.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "files.h"

void setLabelFromDesktopFile(desktopItemStruct *data)
{
	const char	*suffix=NULL;

	data->type=ISGENERIC;
	suffix=getSuffix(data->itemPath);
	if(suffix!=NULL)
		{
			if(strcasecmp(suffix,".desktop")==0)
				{
					GKeyFile	*gkf=g_key_file_new();
					if(g_key_file_load_from_file(gkf,data->itemPath,G_KEY_FILE_NONE,NULL)==true)
						{
							freeAndNull(&data->label);
							data->label=g_key_file_get_string(gkf,"Desktop Entry",G_KEY_FILE_DESKTOP_KEY_NAME,NULL);
						}
					g_key_file_free(gkf);

					data->type=ISDESKTOPFILE;
					data->isSymLink=g_file_test(data->itemPath,G_FILE_TEST_IS_SYMLINK);
					return;
				}

			if((strcasecmp(suffix,".jpg")==0) || (strcasecmp(suffix,".png")==0))
				{
					data->type=ISIMAGEFILE;
					data->isSymLink=g_file_test(data->itemPath,G_FILE_TEST_IS_SYMLINK);
				}
		}
}

void makeFileButtons(bool skipvirtuals)
{
	char		*command;
	FILE		*fd=NULL;
	char		buffer[2048];
	char		*cachefilepath=NULL;
	const char	*constants[]={"Home","Computer","Documents",NULL};
	int			constanttypes[]={ISHOMEFOLDER,ISCOMPUTER,ISDOCUMENTSFOLDER};
	int			constcnt=0;
	const char	*base=NULL;
	struct stat	statbuf;

//make standard buttons
	if(skipvirtuals==false)
		{
			while(constants[constcnt]!=NULL)
				{
					asprintf(&cachefilepath,"%s/%s",cachePath,constants[constcnt]);
					if(access(cachefilepath,F_OK)==0)
						loadCacheFile(cachefilepath,&cacheFileData);
					else
						{
							cacheFileData.uuid=strdup(constants[constcnt]);
							cacheFileData.posx=100;
							cacheFileData.posy=100;
							cacheFileData.hasCustomIcon=false;
							cacheFileData.pathToCustomIcon=NULL;
							saveCacheFile(cachefilepath,&cacheFileData);
						}
					switch(constcnt)
						{
							case 0:
								asprintf(&cacheFileData.itemPath,"%s",getenv("HOME"));
								break;
							case 1:
								asprintf(&cacheFileData.itemPath,"/");
								break;
							case 2:
								cacheFileData.itemPath=strdup(documentsPath);
								break;
						}

					cacheFileData.label=strdup(constants[constcnt]);
					cacheFileData.type=constanttypes[constcnt];;
					cacheFileData.dirty=true;
					cacheFileData.mounted=true;
					cacheFileData.isADisk=false;
					createDesktopGadget(fileWindow);
					constcnt++;
					freeAndNull(&cacheFileData.uuid);
					freeAndNull(&cacheFileData.itemPath);
					freeAndNull(&cacheFileData.label);
					free(cachefilepath);
				}
		}

//make user desktop items
	asprintf(&command,"find %s -maxdepth 1 -mindepth 1 |sort",desktopPath);
	fd=popen(command,"r");
	free(command);
	if(fd!=NULL)
		{
			while(feof(fd)==0)
				{
continueWithLoop:
					buffer[0]=0;
					fgets(buffer,2048,fd);
					if(strlen(buffer)>0)
						{
							buffer[strlen(buffer)-1]=0;
							base=basename(buffer);
							for(unsigned j=0;j<desktopItems.size();j++)//TODO//
								{
									if((base!=NULL) && (desktopItems.at(j).item!=NULL))
										{
											if(strcmp(base,desktopItems.at(j).uuid)==0)
												{
													desktopItems.at(j).item->LFSTK_moveGadget(desktopItems.at(j).posx,desktopItems.at(j).posy);
													goto continueWithLoop;
												}
										}
								}

							asprintf(&cachefilepath,"%s/%s",cachePath,base);
							if(access(cachefilepath,F_OK)==0)
								loadCacheFile(cachefilepath,&cacheFileData);
							else
								{
									cacheFileData.uuid=strdup(base);
									cacheFileData.posx=100;
									cacheFileData.posy=100;
									cacheFileData.hasCustomIcon=false;
									cacheFileData.pathToCustomIcon=NULL;
									saveCacheFile(cachefilepath,&cacheFileData);
								}

	
							stat(buffer,&statbuf);
							if(S_ISDIR(statbuf.st_mode)!=0)
								cacheFileData.isAFolder=true;
							else
								cacheFileData.isAFolder=false;

							if(S_ISLNK(statbuf.st_mode)!=0)
								cacheFileData.isSymLink=true;
							else
								cacheFileData.isSymLink=false;
								
							cacheFileData.itemPath=strdup(buffer);
							cacheFileData.label=strdup(base);
							setLabelFromDesktopFile(&cacheFileData);
							cacheFileData.isADisk=false;
							cacheFileData.dirty=true;
							cacheFileData.mounted=true;
							createDesktopGadget(fileWindow);
							freeAndNull(&cacheFileData.uuid);
							freeAndNull(&cacheFileData.itemPath);
							freeAndNull(&cacheFileData.label);
							free(cachefilepath);
						}
				}
			pclose(fd);
		}
}

const char* getSuffix(const char *path)
{
	return(strrchr(path,'.'));
}

