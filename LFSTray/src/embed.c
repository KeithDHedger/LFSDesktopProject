/*
 *
 * ©K. D. Hedger. Mon 13 Jan 16:22:59 GMT 2025 keithdhedger@gmail.com

 * This file (embed.c) is part of LFSTray.

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
 * embed.c
 * Fri,03 Sep 2004 20:38:55 +0700
 * -------------------------------
 * embedding cycle implementation
 * -------------------------------*/

#include "config.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <assert.h>
#include <unistd.h>

#include "embed.h"
#include "common.h"
#include "icons.h"
#include "settings.h"
#include "tray.h"
#include "xembed.h"
#include "xutils.h"

#define CALC_INNER_POS(x_,y_,ti_) \
    do { \
        x_=(ti_->l.icn_rect.w-ti_->l.wnd_sz.x) / 2; \
        y_=(ti_->l.icn_rect.h-ti_->l.wnd_sz.y) / 2; \
    } while(0);

int embedder_embed(struct TrayIcon *ti)
{
	int x,y,rc;
	XSetWindowAttributes xswa;
	/* If the icon is being embedded as hidden,
	 * we just start listening for property changes
	 * to track _XEMBED mapped state */
	if(!ti->is_visible)
		{
			XSelectInput(tray_data.dpy,ti->wid,PropertyChangeMask);
			return x11_ok();
		}
	/* 0. Start listening for events on icon window */
	XSelectInput(tray_data.dpy,ti->wid,StructureNotifyMask | PropertyChangeMask);
	if(!x11_ok()) return FAILURE;
	/* 1. Calculate position of mid-parent window */
	CALC_INNER_POS(x,y,ti);
	/* 2. Create mid-parent window */
	ti->mid_parent=XCreateSimpleWindow(tray_data.dpy,tray_data.tray,ti->l.icn_rect.x+x,ti->l.icn_rect.y+y,ti->l.wnd_sz.x,ti->l.wnd_sz.y,0,0,0);
	/* 2.5. Setup mid-parent window properties */
	xswa.win_gravity=settings.bit_gravity;
	XChangeWindowAttributes(tray_data.dpy,ti->mid_parent,CWWinGravity,&xswa);
	XSetWindowBackgroundPixmap(tray_data.dpy,ti->mid_parent,ParentRelative);
	if(!x11_ok() || ti->mid_parent==None) return FAILURE;
	/* 3. Embed window into mid-parent */
	switch (ti->cmode)
		{
		case CM_KDE:
		case CM_FDO:
			XReparentWindow(tray_data.dpy,ti->wid,ti->mid_parent,0,0);
			XMapRaised(tray_data.dpy,ti->wid);
			break;
		default:
			break;
		}
	/* 4. Show mid-parent */
	XMapWindow(tray_data.dpy,ti->mid_parent);
	/* mid-parent must be lowered so that it does not osbcure
	 * scollbar windows */
	XLowerWindow(tray_data.dpy,ti->mid_parent);
	if(!x11_ok()) return FAILURE;
	/* 5. Send message confirming embedding */
	rc=x11_send_client_msg32(tray_data.dpy,tray_data.tray,tray_data.tray,tray_data.xa_tray_opcode,0,STALONE_TRAY_DOCK_CONFIRMED,ti->wid,0,0);
	return rc!=0;
}

int embedder_unembed(struct TrayIcon *ti)
{
	if(!ti->is_embedded) return SUCCESS;
	switch (ti->cmode)
		{
		case CM_KDE:
		case CM_FDO:
			/* Unembed icon as described in system tray protocol */
			if(ti->is_embedded)
				{
					XSelectInput(tray_data.dpy,ti->wid,NoEventMask);
					XUnmapWindow(tray_data.dpy,ti->wid);
					XReparentWindow(tray_data.dpy,ti->wid,DefaultRootWindow(tray_data.dpy),ti->l.icn_rect.x,ti->l.icn_rect.y);
					XMapRaised(tray_data.dpy,ti->wid);
				}
			/* Destroy mid-parent */
			if(ti->mid_parent!=None)
				{
					XDestroyWindow(tray_data.dpy,ti->mid_parent);
				}
			break;
		default:
			return FAILURE;
		}
	return (x11_ok()==0); /* This resets error status for the generations to come (XXX) */
}

int embedder_hide(struct TrayIcon *ti)
{
	XUnmapWindow(tray_data.dpy,ti->mid_parent);
	/* We do not wany any StructureNotify events for icon window anymore */
	XSelectInput(tray_data.dpy,ti->wid,PropertyChangeMask);
	if(!x11_ok())
		{
			ti->is_invalid=True;
			return FAILURE;
		}
	else
		{
			ti->is_size_set=False;
			ti->num_size_resets=0;
			ti->is_visible=False;
			return SUCCESS;
		}
}

int embedder_show(struct TrayIcon *ti)
{
	unsigned int x,y;
	/* If the window has never been embedded,
	 * perform real embedding */
	if(ti->mid_parent==None)
		{
			ti->is_visible=True;
			return embedder_embed(ti);
		}
	/* 0. calculate new position for mid-parent */
	CALC_INNER_POS(x,y,ti);
	/* 1. move mid-parent to new location */
	XMoveResizeWindow(tray_data.dpy,ti->mid_parent,ti->l.icn_rect.x+x,ti->l.icn_rect.y+y,ti->l.wnd_sz.x,ti->l.wnd_sz.y);
	/* 2. adjust icon position inside mid-parent */
	XMoveWindow(tray_data.dpy,ti->wid,0,0);
	/* 3. map icon ? */
	XMapRaised(tray_data.dpy,ti->wid);
	/* 4. map mid-parent */
	XMapWindow(tray_data.dpy,ti->mid_parent);
	XSelectInput(tray_data.dpy,ti->wid,StructureNotifyMask | PropertyChangeMask);
	if(!x11_ok())
		{
			ti->is_invalid=True;
			return FAILURE;
		}
	else
		{
			ti->is_visible=True;
			return SUCCESS;
		}
}

static int update_forced=False;

static int embedder_update_window_position(struct TrayIcon *ti)
{
	int x,y;
	/* Ignore hidden icons */
	if(!ti->is_visible) return NO_MATCH;
	/* Update only those icons that do want it (everyone if update was forced)
	 */
	if(!update_forced && !ti->is_updated && !ti->is_resized && ti->is_embedded)
		return NO_MATCH;
	/* Recalculate icon position */
	CALC_INNER_POS(x,y,ti);
	/* Reset the flags */
	ti->is_resized=False;
	ti->is_updated=False;
	/* Move mid-parent window */
	XMoveResizeWindow(tray_data.dpy,ti->mid_parent,ti->l.icn_rect.x+x,ti->l.icn_rect.y+y,ti->l.wnd_sz.x,ti->l.wnd_sz.y);
	/* Sanitize icon position inside mid-parent */
	XMoveWindow(tray_data.dpy,ti->wid,0,0);
	/* Refresh the icon */
	embedder_refresh(ti);
	if(!x11_ok())
		ti->is_invalid=True;
	return NO_MATCH;
}

int embedder_update_positions(int forced)
{
	/* I wish C had closures =( */
	update_forced=forced;
	icon_list_forall(&embedder_update_window_position);
	return SUCCESS;
}

int embedder_refresh(struct TrayIcon *ti)
{
	if(!ti->is_visible) return NO_MATCH;
	XClearWindow(tray_data.dpy,ti->mid_parent);
	x11_refresh_window(
	    tray_data.dpy,ti->wid,ti->l.wnd_sz.x,ti->l.wnd_sz.y,True);
	/* Check if the icon has survived all these manipulations */
	if(!x11_ok())
		{
			ti->is_invalid=True;
		}
	return NO_MATCH;
}

/* This function defines initial icon size or
 * is used to reset size of the icon window */
int embedder_reset_size(struct TrayIcon *ti)
{
	struct Point icon_sz= {0,0};
	int rc=FAILURE;
	/* Do not reset size for non-KDE icons with size set if icon_resizes
	 * are handled */
	if(ti->is_size_set && ti->cmode!=CM_KDE && !(settings.kludge_flags & KLUDGE_FORCE_ICONS_SIZE))
		return SUCCESS;
	/* Increase counter of size resets for given icon. If this number
	 * exeeds the threshold,do nothing. This should work around the icons
	 * that react badly to size changes */
	if(ti->is_size_set) ti->num_size_resets++;
	if(ti->num_size_resets > ICON_SIZE_RESETS_THRESHOLD) return SUCCESS;
	if(ti->cmode==CM_KDE)
		{
			icon_sz.x=settings.icon_size<KDE_ICON_SIZE ? settings.icon_size : KDE_ICON_SIZE;
			icon_sz.y=icon_sz.x;
		}
	else
		{
			/* If icon hints are to be respected,retrive the data */
			if(settings.kludge_flags & KLUDGE_USE_ICONS_HINTS)
				rc=x11_get_window_min_size(tray_data.dpy,ti->wid,&icon_sz.x,&icon_sz.y);
			/* If this has failed,or icon hinst are not respected,or minimal size
			 * hints are too small,fall back to default values */
			if(!rc || !(settings.kludge_flags & KLUDGE_USE_ICONS_HINTS) || (settings.kludge_flags & KLUDGE_FORCE_ICONS_SIZE) || (icon_sz.x<settings.icon_size && icon_sz.y<settings.icon_size))
				{
					icon_sz.x=settings.icon_size;
					icon_sz.y=settings.icon_size;
				}
		}
	if(x11_set_window_size(tray_data.dpy,ti->wid,icon_sz.x,icon_sz.y))
		{
			ti->l.wnd_sz=icon_sz;
			ti->is_size_set=True;
			return SUCCESS;
		}
	else
		{
			ti->is_invalid=True;
			return FAILURE;
		}
}
