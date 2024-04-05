/*
 *
 * ©K. D. Hedger. Sun 11 Feb 14:14:45 GMT 2024 keithdhedger@gmail.com

 * This file (ChooserDialog.h) is part of QT5FileDialog.

 * QT5FileDialog is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * QT5FileDialog is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with QT5FileDialog.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _CHOOSERDIALOG_
#define _CHOOSERDIALOG_

#include <QWidget>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QStringList>
#include <QListView>
#include <QStringListModel>
#include <QDebug>
#include <QStorageInfo>
#include <QVariant>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>

#include <QStandardItemModel>
#include <QMimeDatabase>
#include <QInputDialog>
#include <QGuiApplication>
#include <QSettings>
#include <QMap>

#include "LFSTKFindClass.h"
#include "LFSTKUtilityClass.h"

#define MAXIMAGESIZETOTHUMB 2000000

enum class chooserDialogType{saveDialog,loadDialog};

class chooserDialogClass
{
	public:
		chooserDialogClass(chooserDialogType type,QString savename="",QString startfolder="");
		~chooserDialogClass();

		QDialog				dialogWindow;
		QString				localWD;
		QString				selectedFileName;
		QString				selectedFilePath;
		QString				realFolderPath;
		QString				realName;
		QString				realFilePath;
		QVector<QString>		multiFileList;
		LFSTK_findClass		gFind;
		bool					fileExists=false;
		bool					useMulti=false;
		bool					valid=true;

		void					setSideListMode(QListView::ViewMode mode);
		void					setFileListMode(QListView::ViewMode mode);
		void					setShowImagesInList(bool show=false);
		void					setMultipleSelect(bool select);
		void					setOverwriteWarning(bool warn);
		void					addFileTypes(QString types);
		void					setMaxRecents(int maxrecents);

	private:
		QLineEdit			filepathEdit;
		QComboBox			fileTypes;
		QListView			fileList;
		QStandardItemModel	*fileListModel;

		QListView			sideList;
		QStandardItemModel	*sideListModel;

		QLabel				previewIcon;
		QLabel				previewMimeType;
		QLabel				previewSize;
		QLabel				previewMode;
		QLabel				previewFileName;
		QString				saveName;

		bool					showHidden=false;
		bool					showThumbsInList=false;
		bool					saveDialog=false;
		bool					overwriteWarning=true;
		QString				lastSaveFolder;
		QString				lastLoadFolder;
		int					maxRecents=21;
		QString				recentFoldersPath;
		QString				recentFilesPath;

		void					buildMainGui(void);
		void					setSideList(void);
		void					setFileList(void);
		QIcon				getFileIcon(QString path);
		void					selectItem(const QModelIndex &index);
		void					selectSideItem(const QModelIndex &index);
		void					showPreViewData(void);
		void					setFileData(void);
		void					setFavs(void);
};
#endif
