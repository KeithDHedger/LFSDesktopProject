/*
 *
 * ©K. D. Hedger. Thu 26 Nov 14:19:15 GMT 2015 keithdhedger@gmail.com

 * This file (QT_AboutBox.h) is part of AspellGUI.

 * AspellGUI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * AspellGUI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with AspellGUI.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _QT_ABOUTBOX_
#define _QT_ABOUTBOX_

#include <QtWidgets>

#include "internet.h"

static const char *defaultauthors="K.D.Hedger ©2013-2022<br><a href=\"mailto:" MYEMAIL "\">Email Me</a><br>" \
				"<a href=\"" GLOBALWEBSITE "\">Homepage</a>" \
				"<br><br>More by the same author<br>" \
				
				"<a href=\"https://keithdhedger.github.io/KKEdit/\">KKEdit<br>" \
				"<a href=\"https://keithdhedger.github.io/pages/xdecorations/xdecorations.html\">XDecorations<br>" \
				"<a href=\"https://keithdhedger.github.io/pages/apps.html#themeed\">Xfce-Theme-Manager<br>" \
				"<a href=\"https://keithdhedger.github.io/pages/apps.html#xfcecomp\">Xfce4-Composite-Editor<br>" \
				"<a href=\"https://keithdhedger.github.io/pages/manpageeditor/manpageeditor.html\">Manpage Editor<br>" \
				"<a href=\"https://keithdhedger.github.io/pages/gtksu/gtksu.html\">GtkSu<br>" \
				"<a href=\"https://keithdhedger.github.io/pages/aspellgui/aspelgui.html\">ASpell<br>" \
				"<a href=\"https://keithdhedger.github.io/pages/clipviewer/clip.html\">Clipboard Viewer<br>";

class AboutBoxClass : public QObject
{
    Q_OBJECT

	public:
		AboutBoxClass(QWidget* window,QString pixpath);
		~AboutBoxClass();

		void runAbout(void);
		void setLicence(QString);
		void setAuthors(QString authors);
		void showLicence(void);
		void showCredits(void);
		void killAboutBox(void);

	private:
		QDialog*	aboutdialog;
		QDialog*	licencedialog;
		QDialog*	creditsdialog;
		QString		licence;
		QString		authors;
};

#endif
