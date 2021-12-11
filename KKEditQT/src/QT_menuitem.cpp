/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "QT_menuitem.h"

MenuItemClass::~MenuItemClass()
{
	if(this->menuString!=NULL)
		delete this->menuString;
}

MenuItemClass::MenuItemClass(const QString &text, QWidget *parent): QAction(parent)
{
	this->callbackVoid=NULL;
	this->callbackBool=NULL;
	this->menuID=0;
	this->menuString=NULL;
	this->setText(text);
}

void MenuItemClass::setMenuID(unsigned int id)
{
	this->menuID=id;
}

unsigned int MenuItemClass::getMenuID(void)
{
	return(this->menuID);
}

void MenuItemClass::setMenuString(QString str)
{
	this->menuString=new QString(str);
}

const QString* MenuItemClass::getMenuString(void)
{
	return(this->menuString);
}

void MenuItemClass::setCallBackVoid(menuCallbackVoid func)
{
	this->callbackVoid=func;
	connect(this,SIGNAL(triggered()),this,SLOT(menuClickedVoid()));
}

void MenuItemClass::setCallBackBool(menuCallbackBool func)
{
	this->callbackBool=func;
	connect(this,SIGNAL(triggered()),this,SLOT(menuClickedBool()));
}

void MenuItemClass::menuClickedVoid()
{
	if(this->callbackVoid!=NULL)
		{
			this->callbackVoid((Widget*)this,this->getMenuID());
		}
}

bool MenuItemClass::menuClickedBool()
{
	if(this->callbackBool!=NULL)
		{
			return(this->callbackBool((Widget*)this,this->getMenuID()));
		}
	return(false);
}

void MenuItemClass::menuClickedGotoLine()
{
	this->mainKKEditClass->gotoLine(this->getMenuID());
}
