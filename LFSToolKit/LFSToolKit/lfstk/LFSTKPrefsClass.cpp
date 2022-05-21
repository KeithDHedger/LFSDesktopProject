/*
 *
 * ©K. D. Hedger. Mon 15 Feb 13:47:35 GMT 2021 keithdhedger@gmail.com

 * This file (LFSTKPrefsClass.cpp) is part of LFSToolKit.

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

#include "lfstk/LFSTKGlobals.h"

LFSTK_prefsClass::~LFSTK_prefsClass()
{
}

LFSTK_prefsClass::LFSTK_prefsClass()
{
}

/**
* Add data with key
*
* \param const char *key
* \param prefsData data
*
* \note data struct is:
* \note dataType	type.
* \note std::string	keyName;
* \note std::string	strData.
* \note bool		boolData.
* \note int			intData.
*
* \note type is		TYPEINVALID,TYPESTRING,TYPEBOOL or TYPEINT.
*/
void LFSTK_prefsClass::LFSTK_addToPrefs(const char *key,prefsData data)
{
	this->prefsMap[this->LFSTK_hashFromKey(key)]=data;
}

/**
* Delete data by key
*
* \param const char *key
*/
void LFSTK_prefsClass::LFSTK_deleteFromPrefs(const char *key)
{
	this->prefsMap.erase(this->LFSTK_hashFromKey(key));
}

/**
* Convert bool to string.
*
* \param val =true or false
* \return const char* "true" or "false".
*/
const char* LFSTK_prefsClass::LFSTK_boolToString(bool val)
{
	return(this->bools[val]);
}

/**
* Convert string to bool.
*
* \param val ="true" or "false"
* \return boolean
* \note case insensitive.
*/
bool LFSTK_prefsClass::LFSTK_stringToBool(const char *val)
{
	if((strcasecmp(val,"true")==0) || (strcasecmp(val,"1")==0))
		return(true);
	return(false);
}

/**
* Save variables to prefs file.
*
* \param filepath Path to prefs file.
* \note if filepath = '-' write to stdout.
*/
void LFSTK_prefsClass::LFSTK_saveVarsToFile(const char *filepath)
{
//write
	FILE*	fd=NULL;

	if(filepath[0]=='-')
		fd=stdout;
	else
		fd=fopen(filepath,"w");

	if(fd!=NULL)
		{
			for(auto& x:this->prefsMap)
				{
		  	 		 switch(x.second.type)
						{
							case TYPESTRING:
								if(x.second.strData.length()>0)
									fprintf(fd,"%s %s\n",x.second.keyName.c_str(),x.second.strData.c_str());
								else
									fprintf(fd,"%s \"\"\n",x.second.keyName.c_str());
								break;
							case TYPEBOOL:
								fprintf(fd,"%s %s\n",x.second.keyName.c_str(),this->LFSTK_boolToString(x.second.boolData));
								break;
							case TYPEINT:
								fprintf(fd,"%s %i\n",x.second.keyName.c_str(),x.second.intData);
								break;
							default:
								break;
						}
				}
			if(fd!=stdout)
				fclose(fd);
		}
}

/**
* Load variables from prefs file.
*
* \param filepath Path to prefs file.
*/
void LFSTK_prefsClass::LFSTK_loadVarsFromFile(const char *filepath)
{
//read
	FILE*	fd=NULL;
	char	argname[256];
	char	strarg[8192];
	char	buffer[8448];

	fd=fopen(filepath,"r");
	if(fd!=NULL)
		{
			while(feof(fd)==0)
				{
					buffer[0]=0;
					argname[0]=0;
					strarg[0]=0;
					
					fgets(buffer,8447,fd);
					sscanf(buffer,"%s %[^\n]s",argname,strarg);
					if((strlen(argname)==0) || (strlen(buffer)==0) || (strlen(strarg)==0))
						continue;
					for(auto& x:this->prefsMap)
						{
							if(this->LFSTK_hashFromKey(argname)==x.first)
								{
									switch(x.second.type)
										{
											case TYPESTRING:
												x.second.strData=strarg;
												break;
											case TYPEBOOL:
												x.second.boolData=this->LFSTK_stringToBool(strarg);
												break;
											case TYPEINT:
												x.second.intData=atoi(strarg);
												break;
											default:
												break;
										}
								}
						}
				}
			fclose(fd);
		}
}

unsigned long LFSTK_prefsClass::LFSTK_hashFromKey(const char *key)
{
	unsigned long hash=0;

	for(unsigned i=0;key[i]!=0;i++)
		hash=31*hash+key[i];

	return(hash);
}

/**
* Get c type string.
* \param key.
* \return const char*
*/
const char* LFSTK_prefsClass::LFSTK_getCString(const char *key)
{
	return(this->prefsMap.at(this->LFSTK_hashFromKey(key)).strData.c_str());
}

/**
* Get c type string.
* \param key.
* \return const char*
*/
const char* LFSTK_prefsClass::LFSTK_getCString(unsigned long key)
{
	return(this->prefsMap.at(key).strData.c_str());
}

/**
* Get c++ type std::string.
* \param key.
* \return std::string.
*/
std::string LFSTK_prefsClass::LFSTK_getString(const char *key)
{
	return(this->prefsMap.at(this->LFSTK_hashFromKey(key)).strData);
}

/**
* Get pointer to c++ type const std::string.
* \param key.
* \return const std::string*.
*/
const std::string* LFSTK_prefsClass::LFSTK_getStringObject(const char *key)
{
	return(&(this->prefsMap.at(this->LFSTK_hashFromKey(key)).strData));
}

/**
* Get c++ type std::string.
* \param key.
* \return std::string.
*/
std::string LFSTK_prefsClass::LFSTK_getString(unsigned long key)
{
	return(this->prefsMap.at(key).strData);
}

/**
* Set string.
* \param unsigned long key.
* \param const char *newstr.
*/
void LFSTK_prefsClass::LFSTK_setString(unsigned long key,const char *newstr)
{
	this->prefsMap.at(key).strData=newstr;
}

/**
* Set string.
* \param const char *key.
* \param const char *newstr.
*/
void LFSTK_prefsClass::LFSTK_setString(const char *key,const char *newstr)
{
	this->prefsMap.at(this->LFSTK_hashFromKey(key)).strData=newstr;
}

/**
* Get boolean.
* \param key.
* \return bool.
*/
bool LFSTK_prefsClass::LFSTK_getBool(const char *key)
{
	return(this->prefsMap.at(this->LFSTK_hashFromKey(key)).boolData);
}

/**
* Get boolean.
* \param key.
* \return bool.
*/
bool LFSTK_prefsClass::LFSTK_getBool(unsigned long key)
{
	return(this->prefsMap.at(key).boolData);
}

/**
* Set boolean.
* \param const char *key.
* \param bool val.
*/
void LFSTK_prefsClass::LFSTK_setBool(const char *key,bool val)
{
	this->prefsMap.at(this->LFSTK_hashFromKey(key)).boolData=val;
}

/**
* Set boolean.
* \paramunsigned long key.
* \param bool val.
*/
void LFSTK_prefsClass::LFSTK_setBool(unsigned long key,bool val)
{
	this->prefsMap.at(key).boolData=val;
}

/**
* Get int.
* \param key.
* \return int.
*/
int LFSTK_prefsClass::LFSTK_getInt(const char *key)
{
	return(this->prefsMap.at(this->LFSTK_hashFromKey(key)).intData);
}

/**
* Get int.
* \param key.
* \return int.
*/
int LFSTK_prefsClass::LFSTK_getInt(unsigned long key)
{
	return(this->prefsMap.at(key).intData);
}

