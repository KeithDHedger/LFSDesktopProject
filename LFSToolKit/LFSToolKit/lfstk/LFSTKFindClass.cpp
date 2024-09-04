/*
 *
 * ©K. D. Hedger. Sun 21 Oct 13:08:54 BST 2018 keithdhedger@gmail.com

 * This file (LFSTK_findClass.cpp) is part of LFSToolKit.

 * LFSToolKit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSToolKit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSToolKit.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <dirent.h>
#include <libgen.h>

#include "LFSTKFindClass.h"

LFSTK_findClass *fc;

void LFSTK_findClass::deleteData(void)
{
	if(!this->data.empty())
		{
			for(unsigned j=0;j<this->data.size();j++)
				{
					this->data.at(j).path.clear();
					this->data.at(j).name.clear();
				}
			this->data.clear();
		}
}

/**
* Find class destructor.
*/
LFSTK_findClass::~LFSTK_findClass()
{
	this->deleteData();
	this->fileTypes.clear();
}

/**
* Find class constructor.
* \note NOT thread safe.
*/
LFSTK_findClass::LFSTK_findClass()
{
	this->data.clear();
}

/**
* Set search depth
* \param int,int min,max
*/
void LFSTK_findClass::LFSTK_setDepth(int min,int max)
{
	this->minDepth=min;
	this->maxDepth=max;
}

/**
* Get minimum search depth
* \return int.
*/
int LFSTK_findClass::LFSTK_getMinDepth(void)
{
	return(this->minDepth);
}

/**
* Get maximum search depth
* \return int.
*/
int LFSTK_findClass::LFSTK_getMaxDepth(void)
{
	return(this->maxDepth);
}

/**
* Set find type
* \param int type.
* \note type is same as ntfw flags.
*/
void LFSTK_findClass::LFSTK_setFindType(int type)
{
	this->findType=type;
}

/**
* Get find type
* \return int type.
* \note type is same as ntfw flags.
*/
int LFSTK_findClass::LFSTK_getFindType(void)
{
	return(this->findType);
}

/**
* Set follow links
* \param bool follow.
*/
void LFSTK_findClass::LFSTK_setFollowLinks(bool follow)
{
	this->followLinks=follow;
}

/**
* Get follow links
* \return bool.
*/
bool LFSTK_findClass::LFSTK_getFollowlinks(void)
{
	return(this->followLinks);
}

/**
* Set include hiiden files
* \param bool hidden.
*/
void LFSTK_findClass::LFSTK_setIncludeHidden(bool hidden)
{
	this->includeHidden=hidden;
}

/**
* Get include hiiden files
* \return bool.
*/
bool LFSTK_findClass::LFSTK_getIncludeHidden(void)
{
	return(this->includeHidden);
}

/**
* Set sort direction
* \param bool down.
*/
void LFSTK_findClass::LFSTK_setSortDecending(bool down)
{
	this->sortDescending=down;
}

/**
* Get sort direction
* \return bool.
*/
bool LFSTK_findClass::LFSTK_getSort(void)
{
	return(this->sortDescending);
}

/**
* Get ignore boroken links
* \return bool.
*/
bool LFSTK_findClass::LFSTK_getIgnoreBroken(void)
{
	return(this->ignoreBroken);
}

/**
* Set ignore boroken links
* \param bool ignore.
*/
void LFSTK_findClass::LFSTK_setIgnoreBroken(bool ignore)
{
	this->ignoreBroken=ignore;
}

/**
* Set file types filter
* \param const char *types ( eg "png;jpg" );
*/
void LFSTK_findClass::LFSTK_setFileTypes(std::string suffix)
{
	this->fileTypes=suffix;
}

/**
* Set file name filter
* \param const char *types.
*/
void LFSTK_findClass::LFSTK_setNameFilter(std::string name)
{
	this->nameFilter=name;
}

/**
* Get file types filter
* \return std::string.
*/
std::string LFSTK_findClass::LFSTK_getFileTypes(void)
{
	return(this->fileTypes);
}

static bool sortDataN(dataStruct i,dataStruct j)
{
	std::string ifixed=i.name;
	std::string jfixed=j.name;

	if(fc->caseSensitive==false)
		{
			std::transform(ifixed.begin(),ifixed.end(),ifixed.begin(),::toupper);
			std::transform(jfixed.begin(),jfixed.end(),jfixed.begin(),::toupper);
		}

	if(fc->LFSTK_getSort()==true)
		{
			if(ifixed.compare(jfixed)>0)
				return(true);
		}
	else
		{
			if(ifixed.compare(jfixed)<0)
				return(true);
		}
	return(false);
}

static bool sortDataP(dataStruct i,dataStruct j)
{
	if(fc->LFSTK_getSort()==true)
		return (i.path<j.path);
	else
		return (i.path>j.path);
}

static bool sortDataT(dataStruct i,dataStruct j)
{
	if(fc->LFSTK_getSort()==true)
		return (i.fileType<j.fileType);
	else
		return (i.fileType>j.fileType);
}

static bool sortDataTN(dataStruct i,dataStruct j)
{
	if(fc->LFSTK_getSort()==true)
		{
			if(i.fileType<j.fileType)
				return(true);

			if((i.fileType==j.fileType) && (i.name<j.name))
				return(true);
			return(false);
		}
	else
		{
			if(i.fileType>j.fileType)
				return(true);

			if((i.fileType==j.fileType) && (i.name<j.name))
				return(true);
			return(false);
		}

	return(false);
}

/**
* Sort data by name
*/
void LFSTK_findClass::LFSTK_sortCaseSensitive(bool casesensitive)
{
	this->caseSensitive=casesensitive;
}

/**
* Sort data by name
*/
void LFSTK_findClass::LFSTK_sortByName(void)
{
	std::sort(this->data.begin(),this->data.end(),sortDataN);
	this->moveNavItem();
}

/**
* Sort data by path
*/
void LFSTK_findClass::LFSTK_sortByPath(void)
{
	std::sort(this->data.begin(),this->data.end(),sortDataP);
	this->moveNavItem();
}

/**
* Sort data by type
*/
void LFSTK_findClass::LFSTK_sortByType(void)
{
	std::sort(this->data.begin(),this->data.end(),sortDataT);
	this->moveNavItem();
}

/**
* Sort data LFSTK_sortByTypeAndName
*/
void LFSTK_findClass::LFSTK_sortByTypeAndName(void)
{
	std::sort(this->data.begin(),this->data.end(),sortDataTN);
	this->moveNavItem();
}

/**
* Move nav item to top
*/
void LFSTK_findClass::moveNavItem(void)
{
	dataStruct data;

	if(this->ignoreNavLinks==true)
		return;

	for(unsigned j=0;j<this->data.size();j++)
		{
			if(this->data.at(j).name.compare("..")==0)
				{
					data=this->data.at(j);
					this->data.erase(this->data.begin()+j);
					this->data.emplace(this->data.begin(),data);
					return;
				}
		}
}

/**
* Get number of items.
* \return int.
*/
int LFSTK_findClass::LFSTK_getDataCount(void)
{
	return(this->data.size());
}

/**
* Use full path
* \param bool.
*/
void LFSTK_findClass::LFSTK_setFullPath(bool usefull)
{
	this->fullPath=usefull;
}

/**
* Get use full path
* \return bool.
*/
bool LFSTK_findClass::LFSTK_getFullPath(void)
{
	return(this->fullPath);
}

bool LFSTK_findClass::fileTypeTest(int filetype)
{
	if(this->LFSTK_getFindType()==ANYTYPE)
		return(true);

	switch(this->LFSTK_getFindType())
		{
			case FOLDERTYPE:
				if((filetype==FOLDERTYPE) || (filetype==FOLDERLINKTYPE))
					return(true);
				break;

			case FILETYPE:
				if((filetype==FILETYPE) || (filetype==FILELINKTYPE))
					return(true);
				break;
		}
	return(false);
}

/**
* Set ignore hard folder links (".", "..").
* \param bool true=ignore.
*/
void LFSTK_findClass::LFSTK_setIgnoreNavLinks(bool ignore)
{
	this->ignoreNavLinks=ignore;
}

/**
* Get ignore hard folder links (".", "..").
* \return bool.
*/
bool LFSTK_findClass::LFSTK_getIgnoreNavLinks(void)
{
	return(this->ignoreNavLinks);
}

/**
* Set ignore folders.
* \param bool true=ignore.
* \note If set to true nav links will also be ignored.
*/
void LFSTK_findClass::LFSTK_setIgnoreFolders(bool ignore)
{
	this->ignoreFolders=ignore;
}

/**
* Get ignore folders.
* \return bool.
*/
bool LFSTK_findClass::LFSTK_getIgnoreFolderss(void)
{
	return(this->ignoreFolders);
}

/**
* Get the real file type
* \return int.
*/
int LFSTK_findClass::getRealType(std::string path)
{
	struct stat	filestat;
	bool			islink=false;
	int			type=0;
	int			ret=0;

	ret=lstat(path.c_str(),&filestat);
	if((filestat.st_mode & S_IFMT) ==S_IFLNK)
		islink=true;
	ret=stat(path.c_str(),&filestat);

	if((ret!=0) && (islink==true))
		return(BROKENLINKTYPE);

	switch(filestat.st_mode & S_IFMT)
		{
			case S_IFDIR:
				type=FOLDERTYPE;
				break;
			case S_IFREG:
			case S_IFBLK:
			case S_IFCHR:
			case S_IFIFO:
			case S_IFSOCK:
				type=FILETYPE;
				break;
			default:
				type=ANYTYPE;
				break;
		}

	if((islink==true) && (type>0))
		type--;
	return(type);
}

/**
* Main search function.
* \param const char *dir Path to search.
* \param bool Tru=Add this search to last, False=New search.
* \note If LFSTK_getIgnoreBroken()==true broken links not reported.
*/
void LFSTK_findClass::LFSTK_findFiles(const char *dir,bool multi)//TODO//
{
	DIR			*dirhandle;
	dirent		*entry;
	dataStruct	datas;
	std::string	filepath;
	bool			skip;

	fc=this;
	if(multi==false)
		this->deleteData();

	dirhandle=opendir(dir);
	if(dirhandle!=NULL)
		{
			while((entry=readdir(dirhandle)) != NULL)
				{
					skip=false;
					filepath=dir+std::string("/")+entry->d_name;
					if(strcmp(entry->d_name,".")==0)
						continue;
					if((this->LFSTK_getIncludeHidden()==false) && ((strlen(entry->d_name)>2) && ((entry->d_name[0]=='.') && (entry->d_name[1]!='.') )))
						continue;

					datas.fileType=this->getRealType(filepath);

					if((this->ignoreFolders==true) && ((datas.fileType==FOLDERTYPE) || (datas.fileType==FOLDERLINKTYPE)))
						continue;

					if((datas.fileType==BROKENLINKTYPE) && (this->ignoreBroken==true))
						continue;

					if((this->findType==FILETYPE) || (this->findType==FILELINKTYPE))
						{
							if( ((datas.fileType==FILETYPE) || (datas.fileType==FILELINKTYPE)) || ((this->ignoreBroken==false) && (datas.fileType==BROKENLINKTYPE)))
								{
									if((this->nameFilter.empty()==false) && (LFSTK_UtilityClass::LFSTK_strStr(std::string(entry->d_name),this->nameFilter).empty()==true))
										continue;

									if(this->LFSTK_getFileTypes().empty()==false)
										{
											skip=true;
											std::vector<std::string> tokenstrings=LFSTK_UtilityClass::LFSTK_strTok(this->LFSTK_getFileTypes(),";");
											for(unsigned j=0;j<tokenstrings.size();j++)
												{
													if(LFSTK_UtilityClass::LFSTK_hasSuffix(entry->d_name,tokenstrings.at(j))==true)
														{
															skip=false;
															j=tokenstrings.size()+1;
														}
												}
											if(skip==true)
												continue;
										}
								}
						}
					else
						{
							if( ((datas.fileType==FOLDERTYPE) || (datas.fileType==FOLDERLINKTYPE)) || ((this->ignoreBroken==false) && (datas.fileType==BROKENLINKTYPE)))
								{
									if(this->LFSTK_getFileTypes().empty()==false)
										{
											skip=true;
											std::vector<std::string> tokenstrings=LFSTK_UtilityClass::LFSTK_strTok(this->LFSTK_getFileTypes(),";");
											for(unsigned j=0;j<tokenstrings.size();j++)
												{
													if(LFSTK_UtilityClass::LFSTK_hasSuffix(entry->d_name,tokenstrings.at(j))==true)
														{
															skip=false;
															j=tokenstrings.size()+1;
														}
												}
										}
									if(skip==true)
										continue;
								}
							else
								continue;
						}

					if((this->ignoreNavLinks==true) && ((strcmp(entry->d_name,".")==0) || (strcmp(entry->d_name,"..")==0)))
						continue;

					datas.name=entry->d_name;
					datas.path=filepath;
					this->data.push_back(datas);
					datas.name.clear();
					datas.path.clear();
				}
			closedir(dirhandle);
		}
}

/**
* Find in data list .
* \param std::string name filename to search for.
* \param bool includesuffix include suffix in search.
*/
dataStruct *LFSTK_findClass::LFSTK_findNamed(std::string name,std::string suffix)
{
	std::string compto=name+suffix;

	for(unsigned d=0;d<this->data.size();d++)
		{
			if(this->data.at(d).name.compare(compto)==0)
				return(&this->data.at(d));
		}
	return(NULL);
}

