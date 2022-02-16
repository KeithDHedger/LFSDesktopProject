/*
 *
 * ©K. D. Hedger. Tue 15 Feb 16:17:02 GMT 2022 keithdhedger@gmail.com

 * This file (QT_historylass.cpp) is part of KKEditQT.

 * KKEditQT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * KKEditQT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with KKEditQT.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include "QT_historyClass.h"

HistoryClass::~HistoryClass()
{
}

HistoryClass::HistoryClass(KKEditClass *kk)
{
	this->mainKKEditClass=kk;
}

void HistoryClass::pushToBackList(int line,QString documentpath)
{
	historyData fromhere;

	fromhere.line=line;
	fromhere.documentPath=documentpath;

	if(this->oneList.isEmpty()==false)
		{
			for(int j=this->currentIndex;j<this->oneList.count();j++)
				this->oneList.pop_back();
		}

	if(this->currentIndex==0)
		this->oneList.clear();
	this->oneList.push_back(fromhere);
	this->currentIndex++;

	//this->debuglist();
}

void HistoryClass::goBack(void)
{
	DocumentClass	*doc;
	historyData		fromhere;
	historyData		fromherethisline;

	if((this->oneList.isEmpty()==true) || (this->currentIndex==0))
		return;

	doc=this->mainKKEditClass->getDocumentForTab(-1);
	fromherethisline.line=doc->getCurrentLineNumber();
	fromherethisline.documentPath=doc->getFilePath();

	if(this->currentIndex<this->oneList.count())
		this->oneList.replace(this->currentIndex,fromherethisline);
	else
		this->oneList.push_back(fromherethisline);
	
	fromhere=this->oneList.at(--this->currentIndex);
	if(this->mainKKEditClass->checkForOpenFile(fromhere.documentPath)==true)
		{
			this->mainKKEditClass->gotoLine(fromhere.line);
		}
	else
		{
			this->mainKKEditClass->openFile(fromhere.documentPath,fromhere.line);
		}
}

void HistoryClass::goForward(void)
{
	historyData		fromhere;
	DocumentClass	*doc;
	historyData		fromherethisline;

	doc=this->mainKKEditClass->getDocumentForTab(-1);
	fromherethisline.line=doc->getCurrentLineNumber();
	fromherethisline.documentPath=doc->getFilePath();

	if(this->currentIndex<this->oneList.count())
		this->oneList.replace(this->currentIndex,fromherethisline);
	else
		this->oneList.push_back(fromherethisline);

	this->currentIndex++;
	if(this->currentIndex>this->oneList.count()-1)
		{
			this->currentIndex=this->oneList.count()-1;
			return;
		}

	fromhere=this->oneList.at(this->currentIndex);
	if(this->mainKKEditClass->checkForOpenFile(fromhere.documentPath)==true)
		{
			this->mainKKEditClass->gotoLine(fromhere.line);
		}
	else
		{
			this->mainKKEditClass->openFile(fromhere.documentPath,fromhere.line);
		}
}
//
//void HistoryClass::debuglist(void)
//{
//	DEBUGSTR("onelist:")
//	for(int j=0;j<this->oneList.count();j++)
//		{
//			DEBUGSTR(j << " current=" << this->currentIndex << " doc  " << this->oneList.at(j).documentPath << " line " << this->oneList.at(j).line);
//		}
//}
//



