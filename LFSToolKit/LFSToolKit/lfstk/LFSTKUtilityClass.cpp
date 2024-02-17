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
 
#include "LFSTKUtilityClass.h"

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

	for(unsigned i=0;i<key.length();i++)
		hash=31*hash+key.at(i);

	return(hash);
}

/**
* Get if string has suffix.
* \param std::string str haystack.
* \param std::string str suffix.
* \return bool has suffix.
*/
bool LFSTK_UtilityClass::LFSTK_hasSuffix(std::string haystack,std::string suffix)
{
	std::string upperhaystack=haystack;
	std::string upperneedle=suffix;
	std::transform(upperhaystack.begin(),upperhaystack.end(),upperhaystack.begin(),::toupper);
	std::transform(upperneedle.begin(),upperneedle.end(),upperneedle.begin(),::toupper);

	if(upperneedle.length()>upperhaystack.length())
		return(false);
	if(upperhaystack.substr(upperhaystack.length()-upperneedle.length(),upperneedle.length()).compare(upperneedle)==0)
			return(true);
	return(false);
}

/**
* Read desktop file into std::map<unsigned long,std::vector<std::string>>
* \param std::string filepath.
* \returnstd::map<unsigned long,std::vector<std::string>>
* \note can actually be used to read ANY file into a vector.
*/
std::map<unsigned long,std::vector<std::string>> LFSTK_UtilityClass::LFSTK_readFullDesktopFile(std::string filepath)
{
	std::ifstream									myfile ;
	std::map<unsigned long,std::vector<std::string>>	maplines;
	std::string										mapentry;
	unsigned long									currentkey=0;

	maplines.clear();
	myfile.open(filepath,std::fstream::in);
	if(myfile.is_open())
        {
			std::string data;
			while(std::getline(myfile,data))
				{
					if(data.empty()==false)
						{
							data=LFSTK_UtilityClass::LFSTK_strStrip(data);
							if(data.at(0)!='#')
								{
									if(data.at(0)=='[')
										{
											mapentry=LFSTK_UtilityClass::LFSTK_strReplaceAllChar(data,"[]","",true);
											currentkey=LFSTK_UtilityClass::LFSTK_hashFromKey(mapentry);
										}
									else
										maplines[currentkey].push_back(data);
								}
						}
				}
			myfile.close();
		}
	return(maplines);
}

/**
* Simple parser of desktop file previously read into vector.
* \param std::string entryname Entry name '[.*].
* \param std::string keyname Key to look for ( case sensitive ).
* \param std::map<unsigned long,std::vector<std::string>> maplines Desktop file array.
* \return std::string Found arg or "".
*/
std::string LFSTK_UtilityClass::LFSTK_getFullEntry(std::string entryname,std::string keyname,std::map<unsigned long,std::vector<std::string>> maplines,bool fallback,std::string fallbackgroup)
{
	std::vector<std::string>	linestok;
	std::string				t;
	unsigned long			currentkey;

	currentkey=LFSTK_UtilityClass::LFSTK_hashFromKey(entryname);
	for(unsigned j=0;j<maplines[currentkey].size();j++)
		{
			if(LFSTK_UtilityClass::LFSTK_strStr(maplines[currentkey].at(j),keyname).empty()==false)
				{
					t=LFSTK_UtilityClass::LFSTK_strStrip(maplines[currentkey].at(j));
					linestok=LFSTK_UtilityClass::LFSTK_strTok(t,"=");
					return(LFSTK_UtilityClass::LFSTK_strStrip(linestok.at(1)));
				}
		}
	if(fallback==true)
		{
			currentkey=LFSTK_UtilityClass::LFSTK_hashFromKey(fallbackgroup);
			for(unsigned j=0;j<maplines[currentkey].size();j++)
				{
					if(LFSTK_UtilityClass::LFSTK_strStr(maplines[currentkey].at(j),keyname).empty()==false)
						{
							t=LFSTK_UtilityClass::LFSTK_strStrip(maplines[currentkey].at(j));
							linestok=LFSTK_UtilityClass::LFSTK_strTok(t,"=");
							return(LFSTK_UtilityClass::LFSTK_strStrip(linestok.at(1)));
						}
				}	
		}	
	return("");
}