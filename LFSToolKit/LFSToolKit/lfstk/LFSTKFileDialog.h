/*
 *
 * ©K. D. Hedger. Tue 13 Sep 20:00:53 BST 2016 keithdhedger@gmail.com

 * This file (LFSTKFileDialog.h) is part of LFSToolKit.

 * LFSToolKit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSToolKit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSToolKit.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef _LFSTKFILEDIALOG_
#define _LFSTKFILEDIALOG_

#define FGAP 4
#define FNAVBUTTONWID 24
#define FNAVGADGETHITE 24
#define FICONSIZE 22
#define FFILEHITE 20
#define FDIRHITE 10
#define IMAGESIZE 64

#define FILEDIALOG false
#define FOLDERDIALOG true

//#include "config.h"

#define PREVIEWWIDTH 192
#define DOCSPNG LFSTKPIXMAPSDIR "/documents.png"
#define FOLDERPNG LFSTKPIXMAPSDIR "/folder.png"
#define	IMAGEPNG LFSTKPIXMAPSDIR "/image.png"
#define DOCLINKPNG LFSTKPIXMAPSDIR "/document.link.png"
#define FOLDERLINKPNG LFSTKPIXMAPSDIR "/folder.link.png"
#define IMAGELINKPNG LFSTKPIXMAPSDIR "/image.link.png"
#define BOKENLINKPNG LFSTKPIXMAPSDIR "/document.link.broken.png"

/**
 * \brief File dialog class for LFSToolKit.
*/
class LFSTK_fileDialogClass
{
	public:
		~LFSTK_fileDialogClass();
		LFSTK_fileDialogClass(LFSTK_windowClass* parentwc,const char *label,const char *startdir,bool type,const char *recentname=NULL);

		void						LFSTK_showFileDialog(void);
		void						LFSTK_showFileDialog(const char *dir,const char *title);
		bool						LFSTK_isValid(void);
		void						LFSTK_getLastFolder(void);
		std::string				LFSTK_getCurrentDir(void);
		std::string				LFSTK_getCurrentFile(void);
		std::string				LFSTK_getCurrentPath(void);
		std::string				LFSTK_getCurrentFileSelection(void);
		void						LFSTK_setWorkingDir(std::string dir);
		void						LFSTK_setRequestType(bool type);
		bool						LFSTK_getRequestType(void);
//TODO
		void						LFSTK_setNameFilter(const char *filt);

	private:
		void						setPreviewData(bool fromlist);
//TODO
		void						resizeWindow(int w,int h);

		bool						useThumbs=false;
		std::string				recentsName="universal";
		unsigned					fileListCnt=0;
		std::string				currentDir;
		std::string				currentFile;
		std::string				currentPath;

//TODO
		std::string				filter="";

		bool						mainLoop;
		bool						apply;
		bool						dialogType;
		cairo_surface_t			*fileImage;
		cairo_surface_t			*folderImage;
		cairo_surface_t			*imageImage;
		cairo_surface_t			*fileImageLink;
		cairo_surface_t			*folderImageLink;
		cairo_surface_t			*imageImageLink;
		cairo_surface_t			*brokenLink;

		bool						showHidden=false;
		LFSTK_imageClass			*tux=NULL;
		LFSTK_labelClass			*previewMimeType;
		LFSTK_labelClass			*previewSize;
		LFSTK_labelClass			*previewFileName;
		LFSTK_labelClass			*previewMode;
		LFSTK_labelClass			*previewIsLink;

		LFSTK_windowClass		*wc;
		LFSTK_windowClass		*dialog;
		LFSTK_listGadgetClass	*fileListGadget;
		LFSTK_buttonClass		*buttonApply;
		LFSTK_buttonClass		*buttonCancel;
		LFSTK_buttonClass		*buttonHidden;
		LFSTK_lineEditClass		*dirEdit;

//files and folders
		LFSTK_findClass			*fc=NULL;
		void						getFileList(void);
		void						cleanDirPath(void);
		void						openDir(void);

		void						setFileData(void);
		bool						isADir(const char *path);
//callbacks
		static bool				select(void *object,void* userdata);
		geometryStruct			dialogGeom;

};

#endif
