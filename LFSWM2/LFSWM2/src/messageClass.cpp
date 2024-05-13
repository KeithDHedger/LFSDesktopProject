/*
 *
 * ©K. D. Hedger. Tue 31 Jan 12:14:00 GMT 2023 keithdhedger@gmail.com

 * This file (messageClass.cpp) is part of LFSWM2.

 * LFSWM2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * LFSWM2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSWM2.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "messageClass.h"

static void alarmCallBack(int sig)
{
	XExposeEvent		event;
	bool				needsRefresh=false;

	needsRefresh=theMainClass->messages->LFSWM2_readMsg();
	if(needsRefresh==true)
		{
			event.type=Expose;
			event.window=theMainClass->rootWindow;
			XSendEvent(theMainClass->display,theMainClass->rootWindow,true,ExposureMask,(XEvent*)&event);
			XFlush(theMainClass->display);
		}
	alarm(theMainClass->messages->delay);
}

LFSWM2_messageClass::~LFSWM2_messageClass(void)
{
	alarm(0);
}

LFSWM2_messageClass::LFSWM2_messageClass(LFSWM2_Class *mainclass,int newkey)
{
	this->mainClass=mainclass;
	theMainClass=mainclass;

	this->key=newkey;
	if((this->queueID=msgget(this->key,IPC_CREAT|0660))==-1)
		fprintf(stderr,"Can't create message queue\n");

	signal(SIGALRM,alarmCallBack);
	alarm(this->delay);
}

bool LFSWM2_messageClass::LFSWM2_readMsg(void)
{
	int		retcode;
	msgBuffer	buffer;
	
	buffer.mText[0]=0;
	retcode=msgrcv(this->queueID,&buffer,MAX_MSG_SIZE,LFSWM2_MSG,IPC_NOWAIT);

	if(retcode>0)
		{
			//fprintf(stderr,"message=%s\n",buffer.mText);
			if(strcmp(buffer.mText,"reloadtheme")==0)
				{
					whatMsg=REFRESHTHEME;
					return(true);
				}
			if(strcmp(buffer.mText,"restartwm")==0)
				{
					this->whatMsg=RESTARTLFSWM;
					return(true);
				}
			if(strcmp(buffer.mText,"quit")==0)
				{
					this->whatMsg=QUITLFSWM;
					return(true);
				}
		}
	return(false);
}