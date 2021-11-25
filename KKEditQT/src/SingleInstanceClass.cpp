/*
 *
 * ©K. D. Hedger. Mon 15 Nov 16:33:43 GMT 2021 keithdhedger@gmail.com

 * This file (SingleInstanceClass.cpp) is part of KKEdit.

 * KKEdit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * KKEdit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with KKEdit.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "SingleInstanceClass.h"

enum msgActions {OPENFILE=100,SAVEFILE,QUITAPP,ACTIVATEAPP};

struct msgStruct
{
	long mType;
	char mText[MAXMSGSIZE];
};

SingleInstanceClass::SingleInstanceClass(QApplication *app,bool forcem)
{
	QSettings	prefs;
	bool		single=prefs.value("app/usesingle",QVariant(bool(true))).value<bool>();

	if(QX11Info::isPlatformX11()==false)
		this->isOnX11=false;

	if((QX11Info::isPlatformX11()==false) || (single==false) || (forcem==true))
		{
			this->workspace=(int)random();
			this->usingMulti=true;
		}
	else
		this->workspace=this->getSIWorkSpace();
}

SingleInstanceClass::~SingleInstanceClass()
{
	if(this->deleteComfiles==true)
		{
			fileMsg.remove();
			filePID.remove();
		}
}

bool SingleInstanceClass::getRunning(void)
{
	QString	commsFolderName;
	QString	commsDeskfile;
	QString	commsDeskfilePID;
	bool	retval=false;
	QDir	commsDir;
	bool	isrunning=false;

	if(this->usingMulti==true)
		return(false);

	commsFolderName=commsDir.tempPath() + "/KKEditQTComms";
	commsDir.mkpath(commsFolderName);

	commsDeskfile=QString("%1/desktop%2").arg(commsFolderName).arg(this->workspace);
	commsDeskfilePID=QString("%1/pid%2").arg(commsFolderName).arg(this->workspace);

	this->fileMsg.setFileName(commsDeskfile);
	this->filePID.setFileName(commsDeskfilePID);

	QFileInfo	fileinfo(this->fileMsg);

	retval=this->filePID.open(QIODevice::Text | QIODevice::ReadOnly);
	if(retval==true)
		{
			QString		pidcontent=this->filePID.readAll();
			if(kill(pidcontent.toInt(nullptr,10),0)!=0)
				{
					this->fileMsg.remove();
					this->filePID.remove();
				}
			this->filePID.close();
		}

	if(fileinfo.exists()==true)
		{
			retval=this->fileMsg.open(QIODevice::Text | QIODevice::ReadOnly);
			if(retval==true)
				{
					int			queueID=-1;
					msgStruct	message;
					int			msglen;
					QString		content=this->fileMsg.readAll();
					this->fileMsg.close();
					if(this->app->arguments().size()<2)
						{
							if((queueID=msgget(content.toInt(nullptr,10),IPC_CREAT|0660))==-1)
								fprintf(stderr,"Can't create message queue, scripting wont work :( ...\n");
							else
								{
									msglen=snprintf(message.mText,MAXMSGSIZE-1,"%s","ACTIVATE");
									message.mType=ACTIVATEAPP;
									msgsnd(queueID,&message,msglen,0);
									isrunning=true;
								}
						}
					else
						{
							if((queueID=msgget(content.toInt(nullptr,10),IPC_CREAT|0660))==-1)
								fprintf(stderr,"Can't create message queue, scripting wont work :( ...\n");
							else
								{
									for(int j=1;j<this->app->arguments().size();j++)
										{
											msglen=snprintf(message.mText,MAXMSGSIZE-1,"%s",this->app->arguments().at(j).toStdString().c_str());
											message.mType=OPENFILE;
											msgsnd(queueID,&message,msglen,0);
										}
									msglen=snprintf(message.mText,MAXMSGSIZE-1,"%s","ACTIVATE");
									message.mType=ACTIVATEAPP;
									msgsnd(queueID,&message,msglen,0);
									isrunning=true;
								}
						}
				}
		}
	else
		{
			retval=this->fileMsg.open(QIODevice::Text | QIODevice::WriteOnly);
			if(retval==true)
				{
					QTextStream	out(&this->fileMsg);
					out << MSGKEY+this->workspace << "\n";
					this->fileMsg.close();
				}
			retval=this->filePID.open(QIODevice::Text | QIODevice::WriteOnly);
			if(retval==true)
				{
					QTextStream	out(&this->filePID);
					out << getpid() << "\n";
					this->filePID.close();
				}
			this->deleteComfiles=true;
		}

	return(isrunning);
}

void* SingleInstanceClass::getX11Prop(Window w,Atom prop,Atom type,int fmt,unsigned long *rcountp)
{
	void			*ptr=NULL;
	unsigned long	count=32;
	Atom			rtype;
	int				rfmt;
	unsigned long	rafter;

	for (;;)
		{
			if (XGetWindowProperty(QX11Info::display(),w,prop,0L,count,False,type,&rtype,&rfmt,rcountp,&rafter,(unsigned char **)&ptr) != Success)
				return NULL;
			else if (rtype != type || rfmt != fmt)
				return NULL;
			else if (rafter>0)
				{
					XFree(ptr);
					ptr=NULL;
					count *= 2;
				}
			else
				return ptr;
		}
}

long SingleInstanceClass::getSIWorkSpace(void)
{
	Display			*disp=QX11Info::display();
	unsigned long	rootwin=QX11Info::appRootWindow(-1);
	unsigned long	n=0;
	Atom			NET_WM_DESKTOP;
	long			*deskp;

fprintf(stderr,"disp=%pdisp rootwin=%p\n",disp,rootwin);
	NET_WM_DESKTOP=XInternAtom(disp, "_NET_CURRENT_DESKTOP",False);
	deskp=(long*)getX11Prop(rootwin,NET_WM_DESKTOP,XA_CARDINAL,32,&n);
	if (n !=0)
		return(*deskp);

	return(-1);
}

