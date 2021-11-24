/******************************************************
*
*     ©keithhedger Tue 16 Jul 20:46:19 BST 2013
*     kdhedger68713@gmail.com
*
*     spellcheck.h
* 
******************************************************/

#ifndef _SPELLCHECK_
#define _SPELLCHECK_
#ifdef _ASPELL_

void checkWord(Widget* widget,uPtr data);
void doChangeWord(Widget* widget,uPtr data);
void doAddIgnoreWord(Widget* widget,uPtr data);
void doSpellCheckDoc(Widget* widget,uPtr data);
void doCancelCheck(Widget* widget,uPtr data);

#endif
#endif
