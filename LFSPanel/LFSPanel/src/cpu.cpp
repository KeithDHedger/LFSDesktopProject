/*
 *
 * ©K. D. Hedger. Sun 11 Oct 14:53:50 BST 2015 keithdhedger@gmail.com

 * This file (cpu.cpp) is part of LFSPanel.

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

enum CPUDATA {CPUUSER=0,CPUNICE,CPUSYSTEM,CPUIDLE,CPUIOWAIT,CPUIRQ,CPUSOFTIRQ,CPUSTEAL,CPUDATACNT};

LFSTK_labelClass	*cpuButton=NULL;
char				cpuStatBuffer[256];
unsigned int		newData[CPUDATACNT]={0,};
unsigned int		prevtotal;
unsigned int		previdle;

void setNewData(void)
{
	FILE		*fp=NULL;
	const char	*command="cat /proc/stat|head -n1|awk '{print $2 \"\\n\" $3 \"\\n\" $4 \"\\n\" $5 \"\\n\" $6 \"\\n\" $7 \"\\n\" $8 \"\\n\" $9 \"\\n\" }'";

	fp=popen(command,"r");
	if(fp!=NULL)
		{
			for(int j=CPUUSER;j<CPUDATACNT;j++)
				{
					fgets(cpuStatBuffer,255,fp);
					newData[j]=atol(cpuStatBuffer);
				}
			pclose(fp);
		}
}

void updateCpuStats(void)
{
	unsigned int	idle;
	unsigned int	nonidle;
	unsigned int	total;
	unsigned int	totald;
	unsigned int	idled;

	setNewData();

	idle=newData[CPUIDLE]+newData[CPUIOWAIT];
	nonidle=newData[CPUUSER]+newData[CPUNICE]+newData[CPUSYSTEM]+newData[CPUIRQ]+newData[CPUSOFTIRQ]+newData[CPUSTEAL];

	total=idle+nonidle;
	totald=total-prevtotal;
	idled=idle-previdle;

	prevtotal=total;
	previdle=idle;

	sprintf(cpuStatBuffer,"CPU=%0.1f",((float)totald - (float)idled) / (float)totald *100.0);
	cpuButton->LFSTK_setLabel(cpuStatBuffer);
}

int addCpuData(int x,int y,int grav)
{
	int	xpos=0;
	int width=BWIDTH;
	int	retval=width;

	if((panelGravity==PANELEAST) || (panelGravity==PANELWEST))
		{
			printError("CPU stats not allowed with this panel's orientation.");
			return(0);
		}

	if(cpuButton!=NULL)
		{
			printError("Duplicate cpu stats");
			return(0);
		}

	if(grav==NorthWestGravity)
		xpos=x;
	else
		xpos=x-width;

	setNewData();
	cpuButton=new LFSTK_labelClass(mainwind,"CPU=0.0%",xpos,0,width,panelHeight,CENTRE,grav);
	cpuButton->LFSTK_setLabelGravity(LEFT);
	setGadgetDetails(cpuButton);

	return(retval);
}

