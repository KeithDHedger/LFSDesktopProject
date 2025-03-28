/*
 *
 * ©K. D. Hedger. Wed 20 Dec 11:29:50 GMT 2023 keithdhedger@gmail.com

 * This file (calendar.cpp) is part of LFSDock.

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

#include "calendar.h"

LFSTK_windowClass		*calWindow=NULL;
bool						calWindowVisible=false;
LFSTK_toggleButtonClass	*calendarButton;
bool						calendarIsUp=false;
LFSTK_buttonClass		*prev;
LFSTK_buttonClass		*next;
LFSTK_buttonClass		*openDates;
LFSTK_multiLineEditClass	*editbox=NULL;
std::string				thisMonth;
int						currentMonth;
int						holdThisMonth=0;
std::vector<std::string>	calData;
int						dx=0;
int						dy=0;
int						dl=0;
const char				*monthNames[]={"January","February","March","April","May","June","July","August","September","October","November","December"};
std::vector<datesStruct>	datesData;
cairoColor				datesHilite[]={{0,0,0,0.05},{0,0,1,0.05},{0,1,0,0.05},{0,1,1,0.05},{1,0,0,0.05},{1,0,1,0.05},{1,1,0,0.05},{1,1,1,0.05}};

void setEditText(void);

void setImportantDates(void)
{
	std::string	thisday;

	calData=apc->globalLib->LFSTK_runAndGet("%s %S","cal",thisMonth);
//do important dates
	for(int j=0;j<datesData.size();j++)
		{
			if((currentMonth==datesData.at(j).month) )//&& (std::stoi(thisday)==datesData.at(j).date)
				{
						thisday=std::to_string(datesData.at(j).date);
						dl=thisday.length();
						thisday=" "+thisday+" ";
						std::string::size_type found;
						std::string adjstr;
						for(int i=1;i<calData.size();i++)
							{
								adjstr=" "+calData.at(i);
								adjstr.insert(adjstr.end()-1,' ');
								found=adjstr.find(thisday);
								if(found!=std::string::npos)
									{
										dy=i-1;
										dx=found;
										if(editbox!=NULL)
											editbox->LFSTK_addHighLights(dx,dy,dl,datesData.at(j).col);
									}
							}
				}
		}
}

void updateDates(void)
{
	std::ifstream	datesstream;

	datesData.clear();
	editbox->highLights.clear();
	datesstream.open(configDir + std::string("calendardates"),std::fstream::in);
	if(datesstream.is_open())
        {
			int month;
			int date;
			int colour;
			while(datesstream>>month)
				{
					datesstream>>date;
					datesstream>>colour;
					datesData.push_back({month,date,datesHilite[colour]});
				}
			datesstream.close();
		}

	setImportantDates();
}

bool calCB(void *p,void* ud)
{
	LFSTK_toggleButtonClass	*bc=static_cast<LFSTK_toggleButtonClass*>(p);
	geometryStruct			geom;
	const geometryStruct		*wingeom;

	updateDates();

	if(p!=NULL)
		{	
			if(bc->LFSTK_getValue()==1)
				{
					bc->LFSTK_getGeomWindowRelative(&geom,apc->rootWindow);
					wingeom=calWindow->LFSTK_getWindowGeom();
					if(dockGravity==PANELSOUTH)
						calWindow->LFSTK_moveWindow(geom.x+(geom.w/2)-(wingeom->w/2),geom.y-wingeom->h+extraSpace,true);
					else
						calWindow->LFSTK_moveWindow(geom.x+(geom.w/2)-(wingeom->w/2),geom.y+geom.h-extraSpace,true);

					editbox->highLights.clear();
					setEditText();
					if(holdThisMonth==currentMonth)
						editbox->LFSTK_addHighLights(dx,dy,dl,datesHilite[0]);
					setImportantDates();
					editbox->LFSTK_setBuffer(calData.at(0).c_str());
					calWindow->LFSTK_showWindow(true);
					XRaiseWindow(apc->display,calWindow->window);
					calWindow->LFSTK_redrawAllGadgets();
					apc->windows->at(apc->LFSTK_findWindow(calWindow)).showing=true;
				}
			else
				{
					calWindow->LFSTK_hideWindow();
					apc->windows->at(apc->LFSTK_findWindow(calWindow)).showing=false;
				}
		}
	return(true);
}

bool calExitCB(LFSTK_gadgetClass*p,void* ud)
{

	if(calendarButton->LFSTK_getValue()==1)
		return(true);

	if(moveGadget==true)
		setGadgetPosition(calendarButton,false);

	calendarIsUp=false;
	return(true);
}

bool calEnterCB(LFSTK_gadgetClass*p,void* ud)
{
	if(calendarIsUp==true)
		return(true);

	if(moveGadget==true)
		setGadgetPosition(calendarButton,true);

	calendarIsUp=true;
	inSomeWindow=true;
	return(true);
}

void setEditText(void)
{
	std::string	thisday;

	calData=apc->globalLib->LFSTK_runAndGet("%s %S","cal",thisMonth);
	if(holdThisMonth==currentMonth)
		{
			thisday=apc->globalLib->LFSTK_oneLiner("%s","date +%d|sed 's/^0//'");
			dl=thisday.length();
			thisday=" "+thisday+" ";
			std::string::size_type found;
			std::string adjstr;
			for(int j=1;j<calData.size();j++)
				{
					adjstr=" "+calData.at(j);
					adjstr.insert(adjstr.end()-1,' ');
					found=adjstr.find(thisday);
					if(found!=std::string::npos)
						{
							dy=j-1;
							dx=found;
							return;
						}
				}
		}
}



bool doPrev(void *p,void* ud)
{
	updateDates();

	if(currentMonth==0)
		currentMonth=12;
	currentMonth--;	
	thisMonth=monthNames[currentMonth];
	editbox->highLights.clear();
	setEditText();
	if(holdThisMonth==currentMonth)
		editbox->LFSTK_addHighLights(dx,dy,dl,datesHilite[0]);
	setImportantDates();
	editbox->LFSTK_setBuffer(calData.at(0).c_str());//TODO//
	return(true);
}

bool doOpenDates(void *p,void* ud)
{
	std::string opend;

	opend=std::string("xdg-open ")+configDir + std::string("calendardates &");
	system(opend.c_str());
	return(true);
}

bool doNext(void *p,void* ud)
{
	updateDates();

	if(currentMonth==11)
		currentMonth=-1;
	currentMonth++;	
	thisMonth=monthNames[currentMonth];
	editbox->highLights.clear();
	setEditText();
	if(holdThisMonth==currentMonth)
		editbox->LFSTK_addHighLights(dx,dy,dl,datesHilite[0]);
	setImportantDates();
	editbox->LFSTK_setBuffer(calData.at(0).c_str());
	return(true);
}

int addCalendar(int x,int y,int grav)
{
	windowInitStruct	*win;
	std::string		icon;
	std::string		thisday;
	int				txtwid;
	int				txthite;
	int				sy=0;

	calendarButton=new LFSTK_toggleButtonClass(dockWindow,"",x,normalY,iconWidth,iconHeight);
	calendarButton->LFSTK_setToggleStyle(TOGGLENORMAL);
	calendarButton->LFSTK_setMouseCallBack(NULL,calCB,NULL);
	calendarButton->LFSTK_setMouseMoveCallBack(calEnterCB,calExitCB,USERDATA(0));

	setGadgetDetails(calendarButton);

	icon=dockWindow->globalLib->LFSTK_findThemedIcon(desktopTheme,"stock_calendar","");
	if(icon.length()>0)
		calendarButton->LFSTK_setImageFromPath(icon,CENTRE,true);
	else
		calendarButton->LFSTK_setImageFromPath(DATADIR "/pixmaps/windows.png",CENTRE,true);

	win=apc->LFSTK_getDefaultWInit();
	win->level=ABOVEALL;
	win->windowType=apc->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_DIALOG"));
	win->decorated=false;
	apc->LFSTK_addToolWindow(win,"Calendar");
	calWindow=apc->windows->back().window;

	thisday=apc->globalLib->LFSTK_oneLiner("%s","date +%m");
	currentMonth=std::stoi(thisday)-1;
	thisMonth=monthNames[currentMonth];

	holdThisMonth=currentMonth;
	setEditText();
	editbox=new LFSTK_multiLineEditClass(calWindow,calData.at(0).c_str(),0,sy,10000,10000,BUTTONGRAV);

	txtwid=editbox->LFSTK_getTextWidth(calData.at(1).c_str())+1;
	txthite=editbox->totalTextHeight;
	editbox->LFSTK_moveGadget(0,0);
	editbox->LFSTK_resizeWindow(txtwid,std::floor(txthite));

	editbox->LFSTK_setStyle(BEVELNONE);
	editbox->LFSTK_setEditable(false);
	editbox->LFSTK_setIgnores(false,false);
	editbox->LFSTK_addHighLights(dx,dy,dl,datesHilite[0]);
//TODO//
	editbox->LFSTK_setGadgetColourPair(NORMALCOLOUR,dockBGColour,dockTextColour);

	updateDates();

	sy=txthite-4;

	prev=new LFSTK_buttonClass(calWindow,"<<<",0,sy,(txtwid-4)/3,12);
	prev->LFSTK_setMouseCallBack(NULL,doPrev,NULL);

	openDates=new LFSTK_buttonClass(calWindow,"Set Dates",(txtwid-4)/3,sy,(txtwid-4)/3,12);
	openDates->LFSTK_setMouseCallBack(NULL,doOpenDates,NULL);

	next=new LFSTK_buttonClass(calWindow,">>>",txtwid-4-((txtwid-4)/3),sy,(txtwid-4)/3,12);
	next->LFSTK_setMouseCallBack(NULL,doNext,NULL);
	sy+=12;
	calWindow->LFSTK_resizeWindow(txtwid-4,sy,true);
	return(iconWidth);
}

