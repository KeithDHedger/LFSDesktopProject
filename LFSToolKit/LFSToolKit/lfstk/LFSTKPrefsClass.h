/*
 *
 * ©K. D. Hedger. Mon 15 Feb 13:47:46 GMT 2021 keithdhedger@gmail.com

 * This file (LFSTKPrefsClass.h) is part of LFSToolKit.

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

#ifndef _PREFSCLASS_
#define _PREFSCLASS_

#include <map>
#include <string>
#include <string.h>

enum dataType {TYPEINVALID,TYPESTRING,TYPEBOOL,TYPEINT};

struct prefsData
{
	dataType	type;
	std::string	keyName;
	std::string	strData;
	bool		boolData;
	int			intData;
};

/**
 * \brief Line edit class for LFSToolKit.
*/
class LFSTK_prefsClass
{
	public:
		LFSTK_prefsClass();
		~LFSTK_prefsClass();
		std::map<unsigned long,struct prefsData> prefsMap;

		void			LFSTK_addToPrefs(const char *key,prefsData data);
		void			LFSTK_deleteFromPrefs(const char *key);
		const char		*LFSTK_boolToString(bool val);
		bool			LFSTK_stringToBool(const char	*val);
		void			LFSTK_saveVarsToFile(const char *filepath);
		void			LFSTK_loadVarsFromFile(const char *filepath);

//get
//strings
		const char		*LFSTK_getCString(const char *key);
		const char		*LFSTK_getCString(unsigned long key);
		std::string		LFSTK_getString(const char *key);
		const std::string*	LFSTK_getStringObject(const char *key);
		std::string		LFSTK_getString(unsigned long key);
//set
//string
		void			LFSTK_setString(unsigned long key,const char *newstr);
		void			LFSTK_setString(const char *key,const char *newstr);

//get
//boolean
		bool			LFSTK_getBool(const char *key);
		bool			LFSTK_getBool(unsigned long key);
//set
//boolean
		void			LFSTK_setBool(const char *key,bool val);
		void			LFSTK_setBool(unsigned long key,bool val);

//get
//integer
		int				LFSTK_getInt(const char *key);
		int				LFSTK_getInt(unsigned long key);

		unsigned long	LFSTK_hashFromKey(const char *key);

	private:
		const char		*bools[2]={"false","true"};
};

#endif
