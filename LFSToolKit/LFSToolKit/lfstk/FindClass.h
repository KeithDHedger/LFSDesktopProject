/*
 *
 * ©K. D. Hedger. Wed 17 Oct 11:25:59 BST 2018 keithdhedger@gmail.com

 * This file (findClass.h) is part of CScripts.

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

#include <stdio.h>
#include <iostream>     // std::cout
#include <algorithm>    // std::sort
#include <vector>       // std::vector
#include <stdlib.h>
#include <ftw.h>
#include <functional>   // std::function, std::negate

#ifndef _FINDCLASS_
#define _FINDCLASS_

/**
 *
 * \brief Find files etc.
 */
class FindClass
{
	public:
		~FindClass();
		FindClass();

		void	setDepth(int min,int max);
		void	setFindType(int type);
		int		getFindType(void);
		void	setFollowLinks(bool follow);
		bool	getFollowlinks(void);
		void	setIncludeHidden(bool hidden);
		bool	getIncludeHidden(void);
		void	setFullPath(bool usefull);
		bool	getFullPath(void);

		void	findFiles(const char *dir);
		void	sortData(void);
		int		getDataCount(void);

		int		getMinDepth(void);
		int		getMaxDepth(void);

		std::vector<std::string> data;
	private:
		int		dataCnt=0;
		int		minDepth=-1;
		int		maxDepth=10000;
		int		findType=FTW_F;
		bool	followLinks=false;
		bool	includeHidden=false;
		bool	fullPath=false;
		
	//int getFiles(const char *fpath, const struct stat *sb,int tflag, struct FTW *ftwbuf);

};

#endif
