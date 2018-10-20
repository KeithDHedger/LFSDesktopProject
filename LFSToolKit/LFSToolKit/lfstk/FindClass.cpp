/*
 *
 * ©K. D. Hedger. Wed 17 Oct 11:25:50 BST 2018 keithdhedger@gmail.com

 * This file (findClass.cpp) is part of CScripts.

 * CScripts is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * CScripts is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with CScripts.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "FindClass.h"

FindClass *fc;
/**
* Find class destructor.
*/
FindClass::~FindClass()
{
	for(int j=0;j<this->data.size();j++)
		this->data.at(j).clear();
	
	this->data.clear();
}

/**
* Find class constructor.
* \note NOT thread safe.
*/
FindClass::FindClass()
{
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
* Sort data
*/
void FindClass::sortData(void)
{
	std::sort(this->data.begin(),this->data.end());
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

int getFiles(const char *fpath, const struct stat *sb,int tflag, struct FTW *ftwbuf)
{
	if((ftwbuf->level>=fc->getMinDepth()) && (ftwbuf->level<=fc->getMaxDepth()))
		{
			if((fc->getIncludeHidden()==false) && (*(fpath+ftwbuf->base)=='.'))
				return FTW_CONTINUE;

			if((tflag==FTW_DNR) && (fc->getFindType()==FTW_D))
				{
					if(fc->getFullPath()==true)
						fc->data.push_back(fpath);
					else
						fc->data.push_back(fpath+ftwbuf->base);
					return FTW_CONTINUE;
				}

			if((tflag==fc->getFindType()) || (fc->getFindType()==-1))
				{
					if(fc->getFullPath()==true)
						fc->data.push_back(fpath);
					else
						fc->data.push_back(fpath+ftwbuf->base);
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

	for(int j=0;j<this->data.size();j++)
		this->data.at(j).clear();
	
	this->data.clear();

	if(this->followLinks==false)
		flags+=FTW_PHYS;
	fc=this;
	nftw(dir,getFiles,20,flags);
	this->dataCnt=this->data.size();
}

