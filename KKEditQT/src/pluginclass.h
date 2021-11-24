/******************************************************
*
*     ©keithhedger Tue  8 Jul 19:39:52 BST 2014
*     kdhedger68713@gmail.com
*
*     pluginclass.h
* 
******************************************************/

#include <gmodule.h>

#include "kkedit-includes.h"

#ifndef _PLUGINCLASS_
#define _PLUGINCLASS_

#ifndef _PLUGINDATA_
#define _PLUGINDATA_

struct moduleData
{
	char*		name;
	bool		enabled;
	GModule*	module;
	bool		loaded;
	char*		path;
	bool		unload;
};
#endif

class PluginClass
{
	public:
		GList*			plugins;
		char*			plugFolderPaths[2];
		plugData*		globalPlugData;
		int				plugCount;
		bool			doLoadPlugs;

		PluginClass(bool loadPlugs);
		~PluginClass();
		void			loadPlugins(void);
		void			setPlugFolder(void);
		void			appendToBlackList(char* name);
		void			deleteBlackList();
		moduleData*		getPluginByName(char* name);
		char*			getPluginPathByName(char* name);
		int				runPlugFunction(moduleData* pdata,const char* func);
		bool			enablePlugin(char* name,bool wanttounload);
		bool			checkForFunction(char* name,const char* func);

	private:
		GList*			plugEnabledList;

		bool			checkForEnabled(char* plugname);
		void			getEnabledList(void);
		char*			getNameFromModule(GModule* module);
		char*			getNameFromPath(char* path);
};

#endif
