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
LFSTK_prefsClass::LFSTK_prefsClass(std::string name,std::string version)
{
	this->appName=name;
	this->appVersion=version;
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
	this->prefsMap[LFSTK_UtilityClass::LFSTK_hashFromKey(key)]=data;
}

/**
* Delete data by key
*
* \param const char *key
*/
void LFSTK_prefsClass::LFSTK_deleteFromPrefs(const char *key)
{
	this->prefsMap.erase(LFSTK_UtilityClass::LFSTK_hashFromKey(key));
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
bool LFSTK_prefsClass::LFSTK_stringToBool(std::string val)
{
	if((val.compare("true")==0) || (val.compare("1")==0))
		return(true);
	return(false);
}

/**
* Save variables to prefs file.
*
* \param filepath Path to prefs file.
* \note if filepath = '-' write to stdout.
*/
void LFSTK_prefsClass::LFSTK_saveVarsToFile(std::string filepath)
{
	std::ofstream	myfile ;
	std::string		keyname;
	std::string		data;
	std::streambuf	*buf;
	bool				dook=false;

	if(filepath.compare("-")!=0)
		{
			myfile.open(filepath,std::ofstream::out);
			buf=myfile.rdbuf();
			if(myfile.is_open())
				dook=true;
		}
	else
		{
			buf=std::cout.rdbuf();
			dook=true;
		}

	if(dook==true)
        {
			std::ostream out(buf);
			for(auto& x:this->prefsMap)
				{
		  	 		 switch(x.second.type)
						{
							case TYPESTRING:
								if(x.second.strData.empty()==false)
									out<<x.second.keyName<<" "<<x.second.strData<<std::endl;
								else
									out<<x.second.keyName<<" \"\""<<std::endl;
								break;
							case TYPEBOOL:
								out<<x.second.keyName<<" "<<this->LFSTK_boolToString(x.second.boolData)<<std::endl;
								break;
							case TYPEINT:
								out<<x.second.keyName<<" "<<x.second.intData<<std::endl;
								break;
							default:
								break;
						}
				}
			if(myfile.is_open())
				myfile.close();
		}
}

/**
* Load variables from prefs file.
*
* \param filepath Path to prefs file.
*/
void LFSTK_prefsClass::LFSTK_loadVarsFromFile(std::string filepath)
{
	std::ifstream	myfile ;
	std::string		keyname;
	std::string		data;
	std::string		line;
	
	myfile.open(filepath,std::fstream::in);
	if(myfile.is_open())
        {
			while(myfile>>keyname)
				{
					std::getline(myfile,line);
					data=LFSTK_UtilityClass::LFSTK_strStrip(line);
					for(auto& x:this->prefsMap)
						{
							if(LFSTK_UtilityClass::LFSTK_hashFromKey(keyname)==x.first)
								{
									switch(x.second.type)
										{
											case TYPESTRING:
												x.second.strData=data;
												break;
											case TYPEBOOL:
												x.second.boolData=this->LFSTK_stringToBool(data);
												break;
											case TYPEINT:												
												try
													{
														x.second.intData=std::stoll(data,nullptr,0);
													}
												catch(std::exception &err) {}
												break;
											default:
												break;
										}
								}
						}
				}
			myfile.close();
		}
}

/**
* Get c type string.
* \param key.
* \return const char*
*/
const char* LFSTK_prefsClass::LFSTK_getCString(const char *key)
{
	return(this->prefsMap.at(LFSTK_UtilityClass::LFSTK_hashFromKey(key)).strData.c_str());
}

/**
* Get c++ type std::string.
* \param key.
* \return std::string.
*/
std::string LFSTK_prefsClass::LFSTK_getString(const char *key)
{
	return(this->prefsMap.at(LFSTK_UtilityClass::LFSTK_hashFromKey(key)).strData);
}

/**
* Get pointer to c++ type const std::string.
* \param key.
* \return const std::string*.
*/
const std::string* LFSTK_prefsClass::LFSTK_getStringObject(const char *key)
{
	return(&(this->prefsMap.at(LFSTK_UtilityClass::LFSTK_hashFromKey(key)).strData));
}

/**
* Set string.
* \param const char *key.
* \param const char *newstr.
*/
void LFSTK_prefsClass::LFSTK_setString(const char *key,const char *newstr)
{
	this->prefsMap.at(LFSTK_UtilityClass::LFSTK_hashFromKey(key)).strData=newstr;
}

/**
* Get boolean.
* \param key.
* \return bool.
*/
bool LFSTK_prefsClass::LFSTK_getBool(const char *key)
{
	return(this->prefsMap.at(LFSTK_UtilityClass::LFSTK_hashFromKey(key)).boolData);
}

/**
* Set boolean.
* \param const char *key.
* \param bool val.
*/
void LFSTK_prefsClass::LFSTK_setBool(const char *key,bool val)
{
	this->prefsMap.at(LFSTK_UtilityClass::LFSTK_hashFromKey(key)).boolData=val;
}

/**
* Get int.
* \param key.
* \return int.
*/
int LFSTK_prefsClass::LFSTK_getInt(const char *key)
{
	return(this->prefsMap.at(LFSTK_UtilityClass::LFSTK_hashFromKey(key)).intData);
}

/**
* Set int.
* \param const char *key.
* \param bool val.
*/
void LFSTK_prefsClass::LFSTK_setInt(const char *key,int val)
{
	this->prefsMap.at(LFSTK_UtilityClass::LFSTK_hashFromKey(key)).intData=val;
}

void LFSTK_prefsClass::printHelp(option longoptions[])
{
	int	cnt=0;
	const char		*ob;
	const char		*cb;
	int				typeit;
	std::string		desc;

	fprintf(stderr,"Usage:\n");
	fprintf(stderr,"%s [ARG 0] ... [ARG N]\n",this->appName.c_str());
	while(longoptions[cnt].name!=0)
		{
			fprintf(stderr,"-%c, --%s",longoptions[cnt].val,longoptions[cnt].name);
			typeit=this->prefsMap.find(LFSTK_UtilityClass::LFSTK_hashFromKey(longoptions[cnt].name))->second.type;
			if(this->prefsMap.find(LFSTK_UtilityClass::LFSTK_hashFromKey(longoptions[cnt].name))->second.description.length()>0)
				desc="\n\t"+this->prefsMap.find(LFSTK_UtilityClass::LFSTK_hashFromKey(longoptions[cnt].name))->second.description+"\n";
			else
				desc="\n";
			switch(longoptions[cnt].has_arg)
				{
					case optional_argument:
						ob="[";
						cb="]";
						break;
					case required_argument:
						ob="";
						cb="";
						break;
					case no_argument:
						typeit=-1;
						break;
				}

			switch(typeit)
				{
					case -1:
						fprintf(stderr," \t");
						break;
					case TYPESTRING:
						fprintf(stderr," %sSTRING ARG%s\t",ob,cb);
						break;
					case TYPEINT:
						fprintf(stderr," %sINTEGER ARG%s\t",ob,cb);
						break;
					case TYPEBOOL:
						fprintf(stderr," %sBOOLEAN ARG%s\t",ob,cb);
						break;
				}
			fprintf(stderr," %s",desc.c_str());
			cnt++;
		}
}

/**
* Set prefs from command line.
* \param int argc, char **argv as passed to application.
* \param longoptions[] normal option long_options[] from getopt_long.
* \param bool addhelp Auto add '-?' and '-h' options ( default true ).
* \return boolean false=No prefs set or -? or -h on command line, true=OK.
*/
bool LFSTK_prefsClass::LFSTK_argsToPrefs(int argc, char **argv,option longoptions[],bool addhelp)
{
	int			ocnt=0;
	int			c;
	std::string	optstr="";
	int			option_index;

	if(this->prefsMap.size()==0)
		{
			fprintf(stderr,"No prefs set ...\n");
			return(false);
		}

	while(longoptions[ocnt].name!=0)
		{
			optstr+=longoptions[ocnt].val;
			if(longoptions[ocnt].has_arg==required_argument)
				optstr+=":";
			if(longoptions[ocnt].has_arg==optional_argument)
				optstr+="::";
			ocnt++;
		}
	if(addhelp==true)
		optstr+="?hv";

	while (1)
		{
			option_index=0;
			c=getopt_long(argc,argv,optstr.c_str(),longoptions,&option_index);
			if(c==-1)
				break;
			if(addhelp==true && (c=='?' || c=='h'))
				{
					this->printHelp(longoptions);
					return(false);
				}
			if(addhelp==true && c=='v' )
				{
					fprintf(stderr,"%s %s\n",this->appName.c_str(),this->appVersion.c_str());
					return(false);
				}

			ocnt=0;
			while(longoptions[ocnt].name!=0)
				{
					if(longoptions[ocnt].val==c)
						{
							dataType typeit=this->prefsMap.at(LFSTK_UtilityClass::LFSTK_hashFromKey(longoptions[ocnt].name)).type;
							switch(typeit)
								{
									case TYPESTRING:
										if(optarg==NULL)
											{
												fprintf(stderr,"Option --%s requires an argument ...\n",longoptions[ocnt].name);
												return(false);
											}
										else
											this->LFSTK_setString(longoptions[ocnt].name,optarg);
										break;
									case TYPEINT:
										if(optarg==NULL)
											{
												fprintf(stderr,"Option --%s requires an argument ...\n",longoptions[ocnt].name);
												return(false);
											}
										else
											this->LFSTK_setInt(longoptions[ocnt].name,strtol(optarg,NULL,0));
										break;
									case TYPEBOOL:
										if(optarg==NULL)
											this->LFSTK_setBool(longoptions[ocnt].name,true);
										else
											this->LFSTK_setBool(longoptions[ocnt].name,this->LFSTK_stringToBool(optarg));
										break;
								}
						}
					ocnt++;
				}
		}
	return(true);
}