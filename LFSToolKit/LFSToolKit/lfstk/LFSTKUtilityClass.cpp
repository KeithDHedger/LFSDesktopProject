/*
 *
 * ©K. D. Hedger. Fri 29 Dec 20:16:05 GMT 2023 keithdhedger@gmail.com

 * This file (LFSTKUtilityClass.cpp) is part of LFSToolKit.

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
 
#include "lfstk/LFSTKGlobals.h"
LFSTK_UtilityClass::~LFSTK_UtilityClass()
{
}

LFSTK_UtilityClass::LFSTK_UtilityClass()
{
}

std::vector<std::string>	LFSTK_UtilityClass::LFSTK_strTok(std::string str,std::string delimiter)
{
	std::vector<std::string>	tokenstrings;
	std::string::size_type	foundit=str.find_first_of(delimiter);

	tokenstrings.push_back(str.substr(0,foundit));
	str=str.erase(0,foundit+1);
	while (foundit!=std::string::npos)
		{
			foundit=str.find_first_of(delimiter);
			tokenstrings.push_back(str.substr(0,foundit));
			str=str.erase(0,foundit+1);
  		}
	return(tokenstrings);
}

