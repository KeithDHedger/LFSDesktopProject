/*
 *
 * ©K. D. Hedger. Sun 19 Nov 19:10:07 GMT 2023 keithdhedger@gmail.com

 * This file (slider.h) is part of LFSDock.

 * LFSDock is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * LFSDock is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSDock.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SLIDER_
#define _SLIDER_

#define SLIDERWIDTH 100
#define SLIDERLABEL "Vol"

extern LFSTK_windowClass			*scwindow;
extern bool						windowVisible;
extern LFSTK_toggleButtonClass	*volumeButton;
extern LFSTK_scrollBarClass		*vsb;
extern std::string				iconH;
extern std::string				iconM;
extern std::string				iconL;
extern std::string				iconZ;

extern int						oldVolVal;

void setIcon(void);
int addSlider(int x,int y,int grav);
void updateSlider(void);
int getAlsaVolume(bool setvol,int volume);
bool volExitCB(LFSTK_gadgetClass*p,void* ud);

#endif
