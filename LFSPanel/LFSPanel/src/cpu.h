/*
 *
 * ©K. D. Hedger. Sun 11 Oct 14:54:02 BST 2015 keithdhedger@gmail.com

 * This file (cpu.h) is part of LFSPanel.

 * LFSPanel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSPanel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSPanel.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef _CPU_
#define _CPU_

extern LFSTK_labelClass	*cpuButton;

int addCpuData(int x,int y,int grav);
void updateCpuStats(void);

#endif
