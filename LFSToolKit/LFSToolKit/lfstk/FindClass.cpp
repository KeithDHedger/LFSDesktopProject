/*
 *
 * ©K. D. Hedger. Sun 21 Oct 13:08:54 BST 2018 keithdhedger@gmail.com

 * This file (FindClass.cpp) is part of LFSToolKit.

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

#include "FindClass.h"

FindClass *fc;

void FindClass::deleteData(void)
{
	if(!this->data.empty())
		{
			for(int j=0;j<this->data.size();j++)
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
FindClass::~FindClass()
{
	this->deleteData();
}

/**
* Find class constructor.
* \note NOT thread safe.
*/
FindClass::FindClass()
{
	this->data.clear();
}

/**
* Set search depth
* \param int,int min,max
*/
void FindClass::setDepth(int min,int max)
{
	this->minDepth=min;
	this->maxDepth=max;
}

/**
* Get minimum search depth
* \return int.
*/
int FindClass::getMinDepth(void)
{
	return(this->minDepth);
}

/**
* Get maximum search depth
* \return int.
*/
int FindClass::getMaxDepth(void)
{
	return(this->maxDepth);
}

/**
* Set find type
* \param int type.
* \note type is same as ntfw flags.
*/
void FindClass::setFindType(int type)
{
	this->findType=type;
}

/**
* Get find type
* \return int type.
* \note type is same as ntfw flags.
*/
int FindClass::getFindType(void)
{
	return(this->findType);
}

/**
* Set follow links
* \param bool follow.
*/
void FindClass::setFollowLinks(bool follow)
{
	this->followLinks=follow;
}

/**
* Get follow links
* \return bool.
*/
bool FindClass::getFollowlinks(void)
{
	return(this->followLinks);
}

/**
* Set include hiiden files
* \param bool hidden.
*/
void FindClass::setIncludeHidden(bool hidden)
{
	this->includeHidden=hidden;
}

/**
* Get include hiiden files
* \return bool.
*/
bool FindClass::getIncludeHidden(void)
{
	return(this->includeHidden);
}

/**
* Set sort direction
* \param bool down.
*/
void FindClass::setSort(bool down)
{
	this->sortDescending=down;
}

/**
* Get ignore boroken links
* \return bool.
*/
bool FindClass::getIgnoreBroken(void)
{
	return(this->ignoreBroken);
}

/**
* Set ignore boroken links
* \param bool ignore.
*/
void FindClass::setIgnoreBroken(bool ignore)
{
	this->ignoreBroken=ignore;
}

/**
* Get sort direction
* \return bool.
*/
bool FindClass::getSort(void)
{
	return(this->sortDescending);
}

static bool sortDataN(dataStruct i,dataStruct j)
{
	if(fc->getSort()==true)
		return (i.name<j.name);
	else
		return (i.name>j.name);
}

static bool sortDataP(dataStruct i,dataStruct j)
{
	if(fc->getSort()==true)
		return (i.path<j.path);
	else
		return (i.path>j.path);
}

static bool sortDataT(dataStruct i,dataStruct j)
{
	if(fc->getSort()==true)
		return (i.fileType<j.fileType);
	else
		return (i.fileType>j.fileType);
}

static bool sortDataTN(dataStruct i,dataStruct j)
{
	if(fc->getSort()==true)
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
void FindClass::sortByName(void)
{
	std::sort(this->data.begin(),this->data.end(),sortDataN);
}

/**
* Sort data by path
*/
void FindClass::sortByPath(void)
{
	std::sort(this->data.begin(),this->data.end(),sortDataP);
}

/**
* Sort data by type
*/
void FindClass::sortByType(void)
{
	std::sort(this->data.begin(),this->data.end(),sortDataT);
}

/**
* Sort data sortByTypeAndName
*/
void FindClass::sortByTypeAndName(void)
{
	std::sort(this->data.begin(),this->data.end(),sortDataTN);
}

/**
* Get number of items.
* \return int.
*/
int FindClass::getDataCount(void)
{
	return((int)this->dataCnt);
}

/**
* Use full path
* \param bool.
*/
void FindClass::setFullPath(bool usefull)
{
	this->fullPath=usefull;
}

/**
* Get use full path
* \return bool.
*/
bool FindClass::getFullPath(void)
{
	return(this->fullPath);
}

static int getFiles(const char *fpath, const struct stat *sb,int tflag, struct FTW *ftwbuf)
{
	dataStruct	d;

	if((ftwbuf->level>=fc->getMinDepth()) && (ftwbuf->level<=fc->getMaxDepth()))
		{
			if((fc->getIncludeHidden()==false) && (*(fpath+ftwbuf->base)=='.'))
					return FTW_CONTINUE;

			if((fc->getIgnoreBroken()==true) && (tflag==FTW_SLN))
					return FTW_CONTINUE;

			if(((tflag==FTW_DNR) || (tflag==FTW_SLN)) && (fc->getIgnoreBroken()==true))
			//if((tflag==FTW_DNR) && (fc->getIgnoreBroken()==true))
			//if(fc->getIgnoreBroken()==true)
				return FTW_CONTINUE;

			if((tflag==fc->getFindType()) || (fc->getFindType()==-1))
				{
					d.name=fpath+ftwbuf->base;
					if(fc->getFullPath()==true)
						{
							char	*pth=NULL;
							pth=realpath(fpath,NULL);
							d.path=pth;
							free(pth);
						}
					else
						d.path=fpath;
					d.fileType=tflag;
					fc->data.push_back(d);
					d.name.clear();
					d.path.clear();
					return FTW_CONTINUE;
				}
		}

	if(ftwbuf->level>fc->getMaxDepth())
		return FTW_SKIP_SIBLINGS;

	return FTW_CONTINUE;
}

/**
* Main search function.
*/
void FindClass::findFiles(const char *dir)
{
	int	flags=FTW_ACTIONRETVAL;

	this->deleteData();

	if(this->followLinks==false)
		flags+=FTW_PHYS;
	fc=this;
	nftw(dir,getFiles,20,flags);
	this->dataCnt=this->data.size();
}

