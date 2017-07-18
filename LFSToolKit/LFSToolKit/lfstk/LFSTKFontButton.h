/*
 *
 * ©K. D. Hedger. Tue  6 Sep 20:05:02 BST 2016 kdhedger68713@gmail.com

 * This file (LFSTKFontButton.h) is part of LFSToolKit.

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

#ifndef _LFSTKFONTBUTTON_
#define _LFSTKFONTBUTTON_

#define DWIDTH 600
#define DHITE 400
#define DBORDER 8
#define DGAP 4
#define MAXPREVIEW 6
#define DBUTTONHITE 24
#define DBUTTONWIDTH 72

#define DIALOGDATA(x) (void*)(&this->x)

#define LFSTKGADGET(x) static_cast<LFSTK_gadgetClass*>(x)

#define DPREVIEWTEXT "ABCDEFGHIJK abcdefghijk 0123456789"

enum {DAPPLY=0,DCANCEL,DBOLD,DITALIC,DUP,DDOWN,DHOME,DEND,DMAXBUTTONS};
enum {DSIZE=1,DPREVIEW};

class LFSTK_fontButtonClass  : public  LFSTK_buttonClass
{
	public:
		~LFSTK_fontButtonClass();
		LFSTK_fontButtonClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,int w,int h,int gravity);

		void					LFSTK_showDialog(const char *fs);	
		const char				*LFSTK_getCurrentFontName(void);
		bool					LFSTK_getBold(void);
		bool					LFSTK_getItalic(void);
		const char				*LFSTK_getFontSize(void);
		const char				*LFSTK_getFontString(void);
		void					LFSTK_setLabelIsFont(bool val);

	private:
		struct dialogData
			{
				LFSTK_fontButtonClass	*mainObject;
				unsigned				userData;
			};


		char					*fontDesc;
		char					*fontSize;
		char					*fontName;
		bool					bold;
		bool					italic;
		unsigned				fontNumber;
		int						fontOffset;

		LFSTK_lineEditClass		*fontSizeEdit;
		LFSTK_buttonClass		*previews[MAXPREVIEW];

		unsigned				maxFonts;
		char					**fontsAZ;
		bool					labelIsFont;

		bool					mainLoop;

		LFSTK_windowClass		*dialog;
		LFSTK_toggleButtonClass	*toggleBold;
		LFSTK_toggleButtonClass	*toggleItalic;
		LFSTK_buttonClass		*buttonHome;
		LFSTK_buttonClass		*buttonEnd;
		LFSTK_buttonClass		*buttonUp;
		LFSTK_buttonClass		*buttonDown;
		LFSTK_lineEditClass		*previewEdit;

		dialogData				*buttonData;
		dialogData				*previewData;

		static bool				dialogCB(void *object,void* userdata);
		static bool				styleCB(void *object,void* userdata);
		static bool				scrollCB(void *object,void* userdata);
		static bool				selectFontCB(void *object,void* userdata);

		void					setNavSensitive(void);
		void					loadFontStrings(void);
		void					buildFontString(bool usedata);
		void					updateDialog(bool fonts);
		void					parseFontString(const char *fontstr);
};

#endif
