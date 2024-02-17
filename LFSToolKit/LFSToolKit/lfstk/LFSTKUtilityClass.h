/*
 *
 * ©K. D. Hedger. Fri 29 Dec 20:15:52 GMT 2023 keithdhedger@gmail.com

 * This file (LFSTKUtilityClass.h) is part of LFSToolKit.

 * LFSToolKit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * LFSToolKit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSToolKit.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _UTILITYCLASS_
#define _UTILITYCLASS_

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <iostream>

class LFSTK_UtilityClass
{
	public:
		LFSTK_UtilityClass();
		~LFSTK_UtilityClass();

		static std::vector<std::string>	LFSTK_strTok(std::string str,std::string delimiter);
		static std::string				LFSTK_strStr(std::string haystack,std::string needle,bool caseinsensitive=false);
		static bool						LFSTK_hasSuffix(std::string haystack,std::string suffix);
		static std::string				LFSTK_strStrip(std::string haystack,std::string whitespace="\t \r\n");
		static std::string				LFSTK_strReplaceAllStr(std::string haystack,std::string needle,std::string newneedle,bool erase=false);
		static std::string				LFSTK_strReplaceAllChar(std::string haystack,std::string needle,std::string newneedle,bool erase=false);
		static unsigned long				LFSTK_hashFromKey(std::string key);
//desktop files
		static std::map<unsigned long,std::vector<std::string>>	LFSTK_readFullDesktopFile(std::string filepath);
		static std::string				LFSTK_getFullEntry(std::string entryname,std::string keyname,std::map<unsigned long,std::vector<std::string>> maplines,bool fallback=false,std::string fallbackgroup="Desktop Entry");
};

#endif
