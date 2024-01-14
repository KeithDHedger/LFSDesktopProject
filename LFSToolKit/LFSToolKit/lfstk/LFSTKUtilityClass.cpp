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

/**
* Duplicates 'c' strtok.
* \param std::string str String to split.
* \param std::string delimiter Split on these chars.
* \return std::vector<std::string>.
*/
std::vector<std::string>	LFSTK_UtilityClass::LFSTK_strTok(std::string str,std::string delimiter)
{
	std::vector<std::string>	tokenstrings;
	std::string::size_type	foundit=str.find_first_of(delimiter);

	tokenstrings.push_back(str.substr(0,foundit));
	str=str.erase(0,foundit+1);
	while(foundit!=std::string::npos)
		{
			foundit=str.find_first_of(delimiter);
			tokenstrings.push_back(str.substr(0,foundit));
			str=str.erase(0,foundit+1);
  		}
	return(tokenstrings);
}

/**
* Duplicates 'c' strstr/strcasestr.
* \param std::string str Haystack to search.
* \param std::string needle Needle.
* \param bool caseinsensitive Do case insensitive search (strcasestr).
* \return std::string Found str or "".
*/
std::string LFSTK_UtilityClass::LFSTK_strStr(std::string haystack,std::string needle,bool caseinsensitive)
{
	std::string::size_type found=haystack.find(needle);
	if(found!=std::string::npos)
		{
			return(haystack.substr(found,std::string::npos));
		}
	else
		{
			if(caseinsensitive==false)
				return("");
			std::string upperhaystack=haystack;
			std::string upperneedle=needle;
			std::transform(upperhaystack.begin(), upperhaystack.end(), upperhaystack.begin(), ::toupper);
			std::transform(upperneedle.begin(), upperneedle.end(), upperneedle.begin(), ::toupper);
			found=upperhaystack.find(upperneedle);
			if (found!=std::string::npos)
				{
					return(haystack.substr(found,std::string::npos));
				}
		}
	return("");
}

/**
* Trim whitespace from front and back of string.
* \param std::string str hastack.
* \param std::string whitespace Default whitespace="\t \r\n".
* \return std::string.
*/
std::string LFSTK_UtilityClass::LFSTK_strStrip(std::string haystack,std::string whitespace)
{
	std::string::size_type	foundfront;
	std::string::size_type	foundback;

	foundfront=haystack.find_first_not_of(whitespace);
	foundback=haystack.find_last_not_of(whitespace);
	return(haystack.substr(foundfront,foundback-foundfront+1));
}

/**
* Replace/erase all from string.
* \param std::string str hastack.
* \param std::string needle.
* \param std::string replacement  needle.
* \param bool erase.
* \note Default is to replace.
* \note replace/erase whole needle.
* \return std::string.
*/
std::string LFSTK_UtilityClass::LFSTK_strReplaceAllStr(std::string haystack,std::string needle,std::string newneedle,bool erase)
{
	std::string::size_type	found;
	std::string				localhaystack=haystack;
	bool						flag=false;

	do
		{
			flag=false;
			found=localhaystack.find(needle);
			if(found!=std::string::npos)
				{
					if(erase==true)
						localhaystack.erase(found,needle.length());
					else
						localhaystack.replace(found,needle.length(),newneedle);
					flag=true;
				}
		}while(flag==true);
	return(localhaystack);
}

/**
* Replace/erase all from string.
* \param std::string str hastack.
* \param std::string needle.
* \param std::string replacement  needle.
* \param bool erase.
* \note Default is to replace.
* \note replace/erase any char in needle.
* \return std::string.
*/
std::string LFSTK_UtilityClass::LFSTK_strReplaceAllChar(std::string haystack,std::string needle,std::string newneedle,bool erase)
{
	std::string::size_type	found;
	std::string				localhaystack=haystack;
	bool						flag=false;

	do
		{
			flag=false;
			found=localhaystack.find_first_of(needle);
			if(found!=std::string::npos)
				{
					if(erase==true)
						localhaystack.erase(found,1);
					else
						localhaystack.replace(found,1,newneedle);
					flag=true;
				}
		}while(flag==true);
	return(localhaystack);
}

/**
* Get hash from key string.
* \param std::string str key.
* \return unsigned long hashed key string.
*/
unsigned long LFSTK_UtilityClass::LFSTK_hashFromKey(std::string key)
{
	unsigned long hash=0;

	for(int i=0;i<key.length();i++)
		hash=31*hash+key.at(i);

	return(hash);
}

/**
* Read desktop file into std::vector<std::string>
* \param std::string filepath.
* \return std::vector<std::string>
* \note can actually be used to read ANY file into a vector.
*/
std::vector<std::string> LFSTK_UtilityClass::LFSTK_readDesktopFile(std::string filepath)
{
	std::ifstream			myfile ;
	std::vector<std::string>	lines;

	lines.clear();
	myfile.open(filepath,std::fstream::in);
	if(myfile.is_open())
        {
			std::string data;
			while(std::getline(myfile,data))
				{
					if(data.empty()==false)
						lines.push_back(data);
				}
			myfile.close();
		}
	return(lines);
}

/**
* Simple parser of desktop file previously read into vector.
* \param std::string keyname Key to look for ( case sensitive ).
* \param std::vector<std::string> lines Desktop file array.
* \return std::string Found arg or "".
*/
std::string LFSTK_UtilityClass::LFSTK_getEntry(std::string keyname,std::vector<std::string> lines)
{
	std::vector<std::string>	linestok;
	for(unsigned j=0;j<lines.size();j++)
		{
			if(LFSTK_UtilityClass::LFSTK_strStr(lines.at(j),keyname).empty()==false)
				{
					linestok=LFSTK_UtilityClass::LFSTK_strTok(lines.at(j),"=");
					return(LFSTK_UtilityClass::LFSTK_strStrip(linestok.at(1)));
				}
		}
	return("");
}