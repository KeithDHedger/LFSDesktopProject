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

class LFSTK_UtilityClass
{
	public:
		LFSTK_UtilityClass();
		~LFSTK_UtilityClass();

		static std::vector<std::string>	LFSTK_strTok(std::string str,std::string delimiter);
		static std::string				LFSTK_strStr(std::string haystack,std::string needle,bool caseinsensitive=false);
		static std::string				LFSTK_strStrip(std::string haystack);
};

#endif
