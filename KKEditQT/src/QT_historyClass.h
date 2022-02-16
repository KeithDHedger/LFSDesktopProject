/*
 *
 * ©K. D. Hedger. Tue 15 Feb 16:17:10 GMT 2022 keithdhedger@gmail.com

 * This file (QT_historylass.h) is part of KKEditQT.

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

#ifndef _QT_HISTORYCLASS_
#define _QT_HISTORYCLASS_

#include "kkedit-includes.h"

struct historyData
{
	int		line;
	QString	documentPath;
};
 
class HistoryClass : public QObject
{
	Q_OBJECT

	public:
		HistoryClass(KKEditClass *kk);
		~HistoryClass();

	QVector<historyData>	oneList;

	void					pushToBackList(int line,QString documentpath);
	void					goBack(void);
	void					goForward(void);

	private:
		KKEditClass			*mainKKEditClass;
		int					currentIndex=0;
//		void				debuglist(void);
};
#endif
