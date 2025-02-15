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
}

LFSWM2_messageClass::~LFSWM2_messageClass(void)
{
}

LFSWM2_messageClass::LFSWM2_messageClass(LFSWM2_Class *mainclass,int newkey)
{
	this->mainClass=mainclass;
	theMainClass=mainclass;

	this->key=newkey;
	if((this->queueID=msgget(this->key,IPC_CREAT|0660))==-1)
		fprintf(stderr,"Can't create message queue\n");

	signal(SIGUSR1,alarmCallBack);
}

bool LFSWM2_messageClass::LFSWM2_readMsg(void)
{
	int			retcode;
	msgBuffer	buffer;
	bool			retval=false;
	bool			allDone=false;

	this->whatMsg=NOMSG;
	buffer.mText[0]=0;
	retcode=msgrcv(this->queueID,&buffer,MAX_MSG_SIZE,LFSWM2_MSG,IPC_NOWAIT);

	if(retcode>0)
		{
			if(strcmp(buffer.mText,"debugmsg")==0)
				{
					this->whatMsg=DEBUGMSG;
					retval=true;
				}
			if(strcmp(buffer.mText,"reloadtheme")==0)
				{
					this->whatMsg=REFRESHTHEME;
					retval=true;
				}
			if(strcmp(buffer.mText,"restartwm")==0)
				{
					this->whatMsg=RESTARTLFSWM;
					retval=true;
				}
			if(strcmp(buffer.mText,"quit")==0)
				{
					this->whatMsg=QUITLFSWM;
					retval=true;
				}
		}

//	while(allDone==false)
//		{
//			retcode=msgrcv(this->queueID,&buffer,MAX_MSG_SIZE,LFSWM2_MSG,IPC_NOWAIT);
//			if(retcode<=1)
//				allDone=true;
//		}

#ifdef __DEBUG__
	fprintf(stderr,"what=%i whattxt=%s\n",this->mainClass->messages->whatMsg,buffer.mText);
#endif
	return(retval);
}