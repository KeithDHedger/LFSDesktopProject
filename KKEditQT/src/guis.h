/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
*/

#ifndef _GUIS_
#define _GUIS_

enum {STOCKMENU=0,IMAGEMENU,PIXMAPMENU,NORMALMENU};


//void buildTools(void);

void buildWordCheck(int documentCheck);
int showFunctionEntry(void);


//
#ifdef _USEQT5_
void cancelPrefs(void);
#endif
//
//typedef void (*menuCallbackVoid)(Widget* widget,long data);
//typedef bool (*menuCallbackBool)(Widget* widget,long data);
//
//

#endif