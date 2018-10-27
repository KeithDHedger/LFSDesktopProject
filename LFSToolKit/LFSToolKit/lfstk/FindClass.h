/*
 *
 * ©K. D. Hedger. Sun 21 Oct 13:09:07 BST 2018 keithdhedger@gmail.com

 * This file (FindClass.h) is part of LFSToolKit.

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

#include <stdio.h>
#include <iostream>     // std::cout
#include <algorithm>    // std::sort
#include <vector>       // std::vector
#include <stdlib.h>
#include <ftw.h>
#include <functional>   // std::function, std::negate
#include <string.h>

#ifndef _FINDCLASS_
#define _FINDCLASS_

enum {BROKENLINKTYPE=0,FILELINKTYPE,FILETYPE,FOLDERLINKTYPE,FOLDERTYPE,ANYTYPE};

struct	dataStruct
{
	std::string name;
	std::string path;
	int			fileType;
};

/**
 *
 * \brief Find files etc.
 */
class FindClass
{
	public:
		~FindClass();
		FindClass();

		void		setDepth(int min,int max);
		void		setFindType(int type);
		int			getFindType(void);
		void		setFollowLinks(bool follow);
		bool		getFollowlinks(void);
		void		setIncludeHidden(bool hidden);
		bool		getIncludeHidden(void);
		void		setFullPath(bool usefull);
		bool		getFullPath(void);
		void		setSort(bool down);
		bool		getSort(void);
		void		setIgnoreBroken(bool ignore);
		bool		getIgnoreBroken(void);
		void		setFileTypes(const char *suffix);
		const char*	getFileTypes(void);


		void		findFiles(const char *dir);
		void		sortByName(void);
		void		sortByPath(void);
		void		sortByType(void);
		void		sortByTypeAndName(void);
		int			getDataCount(void);

		int			getMinDepth(void);
		int			getMaxDepth(void);

		std::vector<dataStruct> data;
	private:
		void		deleteData(void);
		bool		fileTypeTest(int filetype);

		int			dataCnt=0;
		int			minDepth=-1;
		int			maxDepth=10000;
		int			findType=ANYTYPE;
		bool		followLinks=true;
		bool		includeHidden=false;
		bool		fullPath=false;
		bool		sortDescending=true;
		bool		ignoreBroken=false;
		char		*fileTypes=NULL;
};

#endif
