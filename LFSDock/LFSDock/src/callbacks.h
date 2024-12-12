/*
 *
 * ©K. D. Hedger. Sun 19 Nov 19:09:12 GMT 2023 keithdhedger@gmail.com

 * This file (callbacks.h) is part of LFSDock.

 * LFSDock is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * LFSDock is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSDock.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _CALLBACKS_
#define _CALLBACKS_

enum {BUTTONLAUNCH=0,BUTTONREMOVE,BUTTONPREFS,BUTTONQUIT,BUTTONHIDE,NOMOREBUTONS};

extern LFSTK_windowClass	*taskContextWindow;

void sendClientMessage(Window win,const char *msg,unsigned long data0,unsigned long data1,unsigned long data2,unsigned long data3,unsigned long data4);
bool gadgetDrop(void *lwc,propertyStruct *data,void* ud);
bool timerCB(LFSTK_applicationClass *p,void* ud);
void readMsg(void);
void showhidetActionList(LFSTK_gadgetClass *bc,LFSTK_windowClass *winc,LFSTK_listGadgetClass *list);
bool popActionListEnterCB(LFSTK_gadgetClass*p,void* ud);
bool popActionListExitCB(LFSTK_gadgetClass*p,void* ud);
bool popActionWindowSelect(void *object,void* userdata);
bool checkInBorder(LFSTK_gadgetClass *gadg);
bool exitPopList(LFSTK_gadgetClass*p,void* ud);
bool hideCB(void* p,void* ud);

#endif
