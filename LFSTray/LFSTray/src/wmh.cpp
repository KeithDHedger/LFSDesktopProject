/*
 *
 * ©K. D. Hedger. Mon 13 Jan 16:31:11 GMT 2025 keithdhedger@gmail.com

 * This file (wmh.c) is part of LFSTray.

 * LFSTray is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation,either version 3 of the License,or
 * (at your option) any later version.

 * LFSTray is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSTray.  If not,see <http://www.gnu.org/licenses/>.
*/

/*
 Forked from stalonetray ( big thanks )
 Original Code here:
 https://github.com/kolbusa/stalonetray
 * ewmh.c
 * Thu,30 Mar 2006 23:15:37 +0700
 * -------------------------------
 * EWMH/MWM support
 * ------------------------------- */
#include <X11/Xatom.h>
#include <X11/Xmd.h>

#include "globals.h"
#include "wmh.h"
#include "xutils.h"

/* Structure for Motif WM hints */
typedef struct
{
	unsigned long flags;
	unsigned long functions;
	unsigned long decorations;
	long inputMode;
	unsigned long status;
} PropMotifWmHints;
/* Bit flags for fields of MWM hints data structure */
#define MWM_HINTS_FUNCTIONS (1L << 0)
#define MWM_HINTS_DECORATIONS (1L << 1)
#define MWM_HINTS_INPUT_MODE (1L << 2)
#define MWM_HINTS_STATUS (1L << 3)
/* Number of CARD32 entries in MWM hints data structure */
#define PROP_MOTIF_WM_HINTS_ELEMENTS 5

/* Check if WM that supports EWMH hints is present on given display */
int ewmh_wm_present(Display *dpy)
{
	Window *check_win,*check_win_self_ref;
	unsigned long cw_len=0,cwsr_len=0;
	int rc=False;
	/* see _NET_SUPPORTING_WM_CHECK in the WM spec. */
	rc=x11_get_root_winlist_prop(dpy,XInternAtom(dpy,_NET_SUPPORTING_WM_CHECK,False),(unsigned char **)&check_win,&cw_len);
	if(x11_ok() && rc && cw_len==1)
		{
			x11_get_window_prop32(dpy,check_win[0],XInternAtom(dpy,_NET_SUPPORTING_WM_CHECK,False),XA_WINDOW,(unsigned char **)&check_win_self_ref,&cwsr_len);
			rc=(x11_ok() && rc && cwsr_len==1 && check_win[0]==check_win_self_ref[0]);
		}
	if(cw_len!=0) XFree(check_win);
	if(cwsr_len!=0) XFree(check_win_self_ref);
	return rc;
}

/* Add EWMH window state for the given window */
int ewmh_add_window_state(Display *dpy,Window wnd,char *state)
{
	Atom prop;
	Atom atom;
	XWindowAttributes xwa;
	int rc;
	prop=XInternAtom(dpy,"_NET_WM_STATE",False);
	atom=XInternAtom(dpy,state,False);
	/* Ping the window and get its state */
	rc=XGetWindowAttributes(dpy,wnd,&xwa);
	if(!x11_ok() || !rc) return FAILURE;

	if(xwa.map_state!=IsUnmapped && ewmh_wm_present(dpy))
		{
			/* For mapped windows,ask WM (if it is here) to add the window state
			 */
			rc=x11_send_client_msg32(dpy,xwa.root,wnd,prop,_NET_WM_STATE_ADD,atom,0,0,0);
		}
	else
		{
			/* Else,alter the window state atom value ourselves */
			rc=XChangeProperty(dpy,wnd,prop,XA_ATOM,32,PropModeAppend,(unsigned char *)&atom,1);
			rc=x11_ok() && rc;
		}
	return rc;
}

/* Add EWMH window type for the given window */
int ewmh_add_window_type(Display *dpy,Window wnd,char *type)
{
	Atom prop;
	Atom atom;
	prop=XInternAtom(dpy,"_NET_WM_WINDOW_TYPE",False);
	atom=XInternAtom(dpy,type,False);
	/* Update property value (append) */
	XChangeProperty(dpy,wnd,prop,XA_ATOM,32,PropModeAppend,(unsigned char *)&atom,1);
	return x11_ok();
}

/* Set data for _NET_WM_STRUT{,_PARTIAL} hints */
int ewmh_set_window_strut(Display *dpy,Window wnd,wm_strut_t wm_strut)
{
	Atom prop_strut;
	Atom prop_strut_partial;
	prop_strut=XInternAtom(dpy,_NET_WM_STRUT,False);
	prop_strut_partial=XInternAtom(dpy,_NET_WM_STRUT_PARTIAL,False);
	XChangeProperty(dpy,wnd,prop_strut,XA_CARDINAL,32,PropModeReplace,(unsigned char *)wm_strut,_NET_WM_STRUT_SZ);
	XChangeProperty(dpy,wnd,prop_strut_partial,XA_CARDINAL,32,PropModeReplace,(unsigned char *)wm_strut,_NET_WM_STRUT_PARTIAL_SZ);
	return x11_ok();
}

/* Set CARD32 value of EWMH atom for a given window */
int ewmh_set_window_atom32(
    Display *dpy,Window wnd,char *prop_name,CARD32 value)
{
	Atom prop;
	XWindowAttributes xwa;
	int rc;
	prop=XInternAtom(dpy,prop_name,False);
	/* Ping the window and get its state */
	rc=XGetWindowAttributes(dpy,wnd,&xwa);
	if(!x11_ok() || !rc) return FAILURE;
	if(xwa.map_state!=IsUnmapped && ewmh_wm_present(dpy))
		{
			/* For mapped windows,ask WM (if it is here) to add the window state
			 */
			return x11_send_client_msg32(dpy,DefaultRootWindow(dpy),wnd,prop,value,2 /* source indication */,0,0,0);
		}
	else
		{
			Atom atom=value;
			/* Else,alter the window state atom value ourselves */
			XChangeProperty(dpy,wnd,prop,XA_ATOM,32,PropModeAppend,(unsigned char *)&atom,1);
			return x11_ok();
		}
}

/* Set MWM hints */
int mwm_set_hints(Display *dpy,Window wnd,unsigned long decorations,unsigned long functions)
{
	PropMotifWmHints *prop=NULL,new_prop;
	int act_fmt,rc;
	unsigned long nitems,bytes_after;
	static Atom atom=None,act_type;
	/* Check if WM supports Motif WM hints */
	if(atom==None) atom=XInternAtom(dpy,"_MOTIF_WM_HINTS",False);
	if(atom==None) return FAILURE;
	/* Get current hints */
	rc=XGetWindowProperty(dpy,wnd,atom,0,5,False,atom,&act_type,&act_fmt,&nitems,&bytes_after,(unsigned char **)&prop);
	if((act_type==None && act_fmt==0 && bytes_after==0) || nitems==0)
		{
			/* Hints are either not set or have some other type.
			 * Reset all values. */
			memset(&new_prop,0,sizeof(PropMotifWmHints));
			if(prop!=NULL) XFree(prop);
			prop=&new_prop;
		}
	else if(prop!=NULL)
		{
			/* Copy value */
			new_prop=*prop;
			XFree(prop);
			prop=&new_prop;
		}
	else
		{
			/* Something is broken */
			x11_ok(); /* Reset x11 error status */
			return FAILURE;
		}
	/* Update value */
	prop->flags |= MWM_HINTS_DECORATIONS | MWM_HINTS_FUNCTIONS;
	prop->decorations=decorations;
	prop->functions=functions;
	XChangeProperty(dpy,wnd,atom,atom,32,PropModeReplace,(unsigned char *)prop,PROP_MOTIF_WM_HINTS_ELEMENTS);
	return x11_ok();
}

