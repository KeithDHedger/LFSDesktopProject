/*
 *
 * ©K. D. Hedger. Fri 24 Jul 10:54:59 BST 2015 keithdhedger@gmail.com

 * This file (LFSTKButton.h) is part of LFSToolKit.

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


#ifndef _LFSTKFONTDIALOG_
#define _LFSTKFONTDIALOG_

/**
* Font data
*/
struct fontDataStruct
{
	std::string	fontString;
	std::string	fontName;
	bool			bold;
	bool			italic;
	int			fontSize;
	bool			isValid;
};

/**
 *
 * \brief Font dialog class for LFSToolKit.
*/
class LFSTK_fontDialogClass : public LFSTK_gadgetClass
{
	public:
		LFSTK_fontDialogClass();
		~LFSTK_fontDialogClass();
		LFSTK_fontDialogClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,unsigned w,unsigned h,int gravity);

		bool						LFSTK_showDialog(const char* fontstring);
		const fontDataStruct		*LFSTK_getFontData(bool rebuild);

		LFSTK_multiLineEditClass	*preview=NULL;
		LFSTK_lineEditClass		*fontsize=NULL;
		LFSTK_windowClass		*dialog=NULL;
	private:
		void						buildFontString(void);
		void						loadFontStrings(void);
		void						buildDialog(void);
		void						parseFontString(std::string fontstr);

		LFSTK_listGadgetClass	*fontlist=NULL;
		LFSTK_toggleButtonClass	*boldcheck=NULL;
		LFSTK_toggleButtonClass	*italiccheck=NULL;
		LFSTK_buttonClass		*seperator=NULL;
		LFSTK_buttonClass		*apply=NULL;
		LFSTK_buttonClass		*cancel=NULL;

		unsigned					selectedFontNumber=0;
		bool						mainLoop;
		unsigned					maxFonts=0;
		std::vector<std::string>	fontsAZV;
		unsigned					size=10;
		fontDataStruct			fontData;
};

#endif
