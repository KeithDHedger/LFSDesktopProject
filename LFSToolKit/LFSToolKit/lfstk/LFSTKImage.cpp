/*
 *
 * ©K. D. Hedger. Thu  5 Nov 16:40:48 GMT 2015 kdhedger68713@gmail.com

 * This file (LFSTKImage.cpp) is part of LFSToolKit.

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
 
#include "lfstk/LFSTKGlobals.h"

LFSTK_imageClass::~LFSTK_imageClass()
{
}

LFSTK_imageClass::LFSTK_imageClass()
{
}


void LFSTK_imageClass::LFSTK_clearParentWindowRect(void)
{
int alpha=0xff;

const geometryStruct *wingeom=this->wc->LFSTK_getWindowGeom();

XRectangle rect={wingeom->x+this->gadgetGeom.x,wingeom->y+this->gadgetGeom.y,500,500};
XSetClipRectangles(this->display,this->wc->gc,rect.x,rect.y,&rect,1,Unsorted);




//	if(this->wc->useTile==true)
		{
		printf("clear tile\n");
			XSetClipMask(this->display,this->wc->gc,None);
			XSetTSOrigin(this->display,this->wc->gc,0,0);
			XSetFillStyle(this->display,this->wc->gc,FillTiled);
			XSetTile(this->display,this->wc->gc,this->wc->tile[0]);
			//	XSetFillStyle(this->display,this->gc,FillSolid);
			//	XSetForeground(this->display,this->gc,0x80808080);
			//	XSetBackground(this->display,this->gc,0x80808080);
			XFillRectangle(this->display,this->parent,this->wc->gc,0,0,rect.width,rect.height);
			//XMapRaised(this->display, this->window);
//printf("clear window\n");
		}
//	else
//		{
//			XSetFillStyle(this->display,this->gc,FillSolid);
//			XSetClipMask(this->display,this->gc,None);
//			if(this->isActive==true)
//			{
//			//printf("for wc=%x\n",this->windowColourNames[NORMALCOLOUR].pixel);
//				XSetForeground(this->display,this->gc,this->windowColourNames[NORMALCOLOUR].pixel);
//				//XSetForeground(this->display,this->gc,0xff808000);
//		//	printf("pixel=%x\n",this->windowColourNames[NORMALCOLOUR].pixel);
//			}
//			else
//				XSetForeground(this->display,this->gc,this->windowColourNames[INACTIVECOLOUR].pixel);
//			XFillRectangle(this->display,this->window,this->gc,0,0,this->windowGeom.w,this->windowGeom.h);
//		}
}



void LFSTK_imageClass::LFSTK_clearWindow(void)
{


//	cairo_save(this->cr);
//		cairo_reset_clip (this->cr);
//		cairo_translate(this->cr,0,0);
//		cairo_set_source_rgba (this->cr, 1, 1, 0, 0.4);
//		cairo_paint(this->cr);
//	cairo_restore(this->cr);
//return;
#if 1
	if(this->wc->useTile==true) 
		{
		printf("tile\n");
			XSetTSOrigin(this->display,this->gc,0-this->gadgetGeom.x,0-this->gadgetGeom.y);
			XSetFillStyle(this->display,this->gc,FillTiled);
			XSetTile(this->display,this->gc,this->wc->tile[0]);
			XFillRectangle(this->display,this->window,this->gc,0,0,this->gadgetGeom.w,this->gadgetGeom.h);
			XSetFillStyle(this->display,this->gc,FillSolid);
		}
	else
		{
		printf("fill\n");
			XSetFillStyle(this->display,this->gc,FillSolid);
		//	XSetForeground(this->display,this->gc,0x0);
			XSetForeground(this->display,this->gc,this->wc->windowColourNames[NORMALCOLOUR].pixel);
			//XSetForeground(this->display,this->gc,0x80808080);
			//XSetForeground(this->display,this->gc,0x0);
			XFillRectangle(this->display,this->window,this->gc,0,0,this->gadgetGeom.w,this->gadgetGeom.h);
		}
//	XClearArea(this->display,this->window,this->gadgetGeom.x,this->gadgetGeom.y,this->gadgetGeom.w, this->gadgetGeom.h, false);
#else
geometryStruct geom;
	XUnmapWindow(this->display,this->window);
//	XFlush(this->display);
	XSync(this->display,true);

//this->wc->LFSTK_clearWindow();
this->LFSTK_clearParentWindowRect();
const geometryStruct *wingeom=this->wc->LFSTK_getWindowGeom();
this->LFSTK_getGlobalGeom(&geom);
//printf("global x=%i local x=%i winx=%i\n",geom.x,this->gadgetGeom.x,wingeom->x);
XCopyArea(this->display,this->parent,this->window,this->gc,wingeom->x+this->gadgetGeom.x,wingeom->y+this->gadgetGeom.y,500,500,0,0);
XMapRaised(this->display,this->window);
	XSync(this->display,true);
#endif
	if(this->useImage==true)
		this->drawImage();
}


/**
* Mouse down callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_imageClass::mouseDown(XButtonEvent *e)
{
	this->mouseDownX=e->x;
	this->mouseDownY=e->y;
	return(true);
}

/**
* Mouse up callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_imageClass::mouseUp(XButtonEvent *e)
{
	pointStruct 	pt;

	pt.x=this->gadgetGeom.x+e->x;
	pt.y=this->gadgetGeom.y+e->y;

	if(this->wc->globalLib->LFSTK_pointInRect(&pt,&(this->gadgetGeom))==true)
		{
			if(this->callback.releaseCallback!=NULL)
				return(this->callback.releaseCallback(this,this->callback.userData));
		}
	return(true);
}


/**
* Mouse drag callback.
* \param e XMotionEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_imageClass::mouseDrag(XMotionEvent *e)
{
	if(this->canDrag==true)
		{
			//this->wc->LFSTK_clearWindow();
			this->gadgetGeom.x+=e->x-this->mouseDownX;
			this->gadgetGeom.y+=e->y-this->mouseDownY;
			
			this->gadgetGeom.x=(this->gadgetGeom.x/this->snap)*this->snap;
			this->gadgetGeom.y=(this->gadgetGeom.y/this->snap)*this->snap;
			XMoveWindow(this->display,this->window,this->gadgetGeom.x,this->gadgetGeom.y);

//wc->LFSTK_resizeWindow(400,800,true);
			this->LFSTK_clearWindow();


//			XSetClipMask(this->display,this->wc->gc,None);
//			XSetTSOrigin(this->display,this->wc->gc,0,0);
//			XSetFillStyle(this->display,this->wc->gc,FillTiled);
//			XSetTile(this->display,this->wc->gc,this->wc->tile[0]);
//			XFillRectangle(this->display,this->wc->window,this->wc->gc,0,0,400,800);

//			XFlush(this->display);
//			XFlushGC(this->display,this->wc->gc);
//			XSync(this->display,false);


		}
	return(true);
}

/**
* Set if image can be dragged
*
* \param candrag draggable.
*/
void LFSTK_imageClass::LFSTK_setCanDrag(bool candrag)
{
	this->canDrag=candrag;
}

/**
* Get if image can be dragged
*
* \return Draggable.
*/

bool LFSTK_imageClass::LFSTK_getCanDrag(void)
{
	return(this->canDrag);
}

/**
* Set snap to grid size
*
* \param int sze.
*/
void LFSTK_imageClass::LFSTK_snapSize(int sze)
{
	this->snap=sze;
}


/**
* Main button constructor.
*
* \param parentwc Main parent window class.
* \param imagepath Path to image.
* \param x X pos.
* \param y Y pos.
* \param w Width.
* \param gravity Button gravity.
*/
LFSTK_imageClass::LFSTK_imageClass(LFSTK_windowClass* parentwc,const char* imagepath,int x,int y,int w,int h,int gravity,bool scale)
{
	XSetWindowAttributes	wa;
	mappedListener			*ml=new mappedListener;
	this->LFSTK_setCommon(parentwc,imagepath,x,y,w,h,gravity);

	wa.win_gravity=gravity;
	#if 0
//XVisualInfo vinfo;
  int depth;
	
  XVisualInfo *visual_list;
  XVisualInfo visual_template;
  int nxvisuals;
  int i;
//  XSetWindowAttributes attrs;
int cls;

printf("0000000000000000000\n");
nxvisuals = 0;
  visual_template.screen = DefaultScreen(this->display);
  visual_list = XGetVisualInfo (this->display, VisualScreenMask, &visual_template, &nxvisuals);
//	for (i = 0; i < nxvisuals; ++i)
//		{
//			if( visual_list[i].depth==32)
//				cls=visual_list[i].class;
//		}
//    {
//      printf("  %3d: visual 0x%lx  (%s) depth %d\n",
//             i,
//             visual_list[i].visualid,
//             visual_list[i].depth);
//    }
   
if (!XMatchVisualInfo(this->display, XDefaultScreen(this->display), 32, TrueColor, &this->vinfo))
    {
      fprintf(stderr, "no such visual\n");
      this->window=XCreateWindow(this->display,this->parent,x,y,w,this->gadgetGeom.h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
    }
else
{
//printf("Matched visual 0x%lx  depth %d\n",
//         vinfo.visualid,
//         vinfo.depth);
//printf("99999999999999999\n");
//XSync(this->display, True);


printf("creating RGBA child\n");

  this->visual = this->vinfo.visual;
  depth = this->vinfo.depth;

this->cm=XCreateColormap(this->display, this->parent, this->visual, AllocNone);
  wa.colormap = this->cm;//XCreateColormap(this->display, this->rootWindow, this->visual, AllocNone);
  wa.background_pixel = 0;
  wa.border_pixel = 0;

  this->window=XCreateWindow(this->display, this->parent, x,y,w,this->gadgetGeom.h, 0, 32, InputOutput,this->visual, CWBackPixel | CWColormap | CWBorderPixel, &wa);

//	XSetBackground(this->display, this->gc,0xffffff80);
XSync(this->display, True);

}	
	
	
	
// this->window=XCreateWindow(this->display,this->parent,x,y,w,this->gadgetGeom.h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
#else	
	this->window=XCreateWindow(this->display,this->parent,x,y,w,this->gadgetGeom.h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
#endif
	this->gc=XCreateGC(this->display,this->window,0,NULL);
	XSelectInput(this->display,this->window,ButtonPressMask|ButtonReleaseMask|ExposureMask|ButtonMotionMask);

	ml->function=&LFSTK_lib::LFSTK_gadgetEvent;
	ml->gadget=this;
	ml->type=IMAGEGADGET;
	this->wc->LFSTK_addMappedListener(this->window,ml);

	this->LFSTK_setImageFromPath(imagepath,gravity,scale);
}
