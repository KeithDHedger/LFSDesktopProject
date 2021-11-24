/******************************************************
*
*     ©keithhedger Sat 11 Oct 11:51:45 BST 2014
*     kdhedger68713@gmail.com
*
*     completion.h
* 
******************************************************/

#include "config.h"

#ifndef _USEQT5_
#include <gtksourceview/completion-providers/words/gtksourcecompletionwords.h>
#endif

#ifndef _COMPLETION_
#define _COMPLETION_

#include "kkedit-includes.h"

#ifndef _USEQT5_
typedef struct _FunctionProvider FunctionProvider;
typedef struct _FunctionProviderClass FunctionProviderClass;
#endif

struct _FunctionProvider
{
#ifndef _USEQT5_
	GObject						parent;
	GList*						proposals;
	gint						priority;
	const char*					name;
	GtkSourceCompletionContext*	context;
	GdkPixbuf*					icon;
#else
//TODO//
#endif
};

#ifndef _USEQT5_
void function_provider_iface_init(GtkSourceCompletionProviderIface* iface);
#else
//TODO//
void function_provider_iface_init(void);
#endif

#ifndef _USEQT5_
GType function_provider_get_type(void);
#else
//TODO//
void function_provider_get_type(void);
#endif

extern bool							forcePopup;

#ifndef _USEQT5_
extern FunctionProvider*			funcProv;
extern FunctionProvider*			varsProv;
extern GtkSourceCompletionWords*	docWordsProv;
extern FunctionProvider*			customProv;
#else
//TODO//
#endif


void doCompletionPopUp(pageStruct* page);
void createCompletion(pageStruct* page);
void addProp(pageStruct* page);
void removeProps(void);

#endif
