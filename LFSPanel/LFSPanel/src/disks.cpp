/*
 *
 * ©K. D. Hedger. Thu  8 Oct 17:13:40 BST 2015 keithdhedger@gmail.com

 * This file (disks.cpp) is part of LFSPanel.

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

#include "globals.h"

LFSTK_labelClass	*diskButton=NULL;
char				diskStatBuffer[256];
unsigned long		oldRead=0,oldWrite=0,newRead=0,newWrite=0;

int getReadStats(void)
{
	FILE			*fp=NULL;
	unsigned long	cnt=0;

	fp=popen("cat /proc/diskstats|awk '{print $6}'","r");
	if(fp!=NULL)
		{
			while(fgets(diskStatBuffer,255,fp))
				cnt+=atol(diskStatBuffer);
			pclose(fp);
		}
	return(cnt);
}

int getWriteStats(void)
{
	FILE			*fp=NULL;
	unsigned long	cnt=0;

	fp=popen("cat /proc/diskstats|awk '{print $10}'","r");
	if(fp!=NULL)
		{
			while(fgets(diskStatBuffer,255,fp))
				cnt+=atol(diskStatBuffer);
			pclose(fp);
		}
	return(cnt);
}

void updateDiskStats(void)
{
	unsigned long	rdiff=0,wdiff=0;

	newRead=getReadStats();
	rdiff=(newRead-oldRead)*512/1024/1024;
	oldRead=getReadStats();

	newWrite=getWriteStats();
	wdiff=(newWrite-oldWrite)*512/1024/1024;
	oldWrite=getWriteStats();

	sprintf(diskStatBuffer,"R=%iMb/s W=%iMb/s",rdiff,wdiff);
	diskButton->LFSTK_setLabel(diskStatBuffer);
}

int addDiskData(int x,int y,int grav)
{
	int	xpos=0;
	int width=BWIDTH*2;
	int	retval=width;

	if((panelGravity==PANELEAST) || (panelGravity==PANELWEST))
		{
			printError("Disk stats not allowed with this panel's orientation.");
			return(0);
		}

	if(diskButton!=NULL)
		{
			printError("Duplicate disk stats");
			return(0);
		}
	if(grav==NorthWestGravity)
		xpos=x;
	else
		xpos=x-width;

	oldRead=getReadStats();
	oldWrite=getWriteStats();
	diskButton=new LFSTK_labelClass(mainwind,"R=0Mb/s W=0Mb/s",xpos,0,width,panelHeight,CENTRE,grav);
	diskButton->LFSTK_setLabelGravity(LEFT);
	return(retval);
}
