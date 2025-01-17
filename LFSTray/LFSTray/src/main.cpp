/*
 *
 * ©K. D. Hedger. Mon 13 Jan 16:21:40 GMT 2025 keithdhedger@gmail.com

 * This file (main.c) is part of LFSTray.

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
 * main.c
 * Tue,24 Aug 2004 12:19:48 +0700
 * -------------------------------
 * main is main
 * -------------------------------*/

#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xmd.h>
#include <X11/Xutil.h>

#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "config.h"

#include "globals.h"
#include "embed.h"
#include "icons.h"
#include "layout.h"
#include "wmh.h"
#include "xembed.h"
#include "xutils.h"
#include "settings.h"
#include "tray.h"

struct TrayData tray_data;

void my_usleep(useconds_t usec)
{
	struct timeval timeout;
	fd_set rfds;
	FD_ZERO(&rfds);
	timeout.tv_sec=0;
	timeout.tv_usec=usec;
	select(1,&rfds,NULL,NULL,&timeout);
}

/****************************
 * Signal handlers,cleanup
 ***************************/

void cleanup()
{
	static int clean=0;
	static int cleanup_in_progress=0;
	if(!clean && cleanup_in_progress)
		{
			abort();
		}
	if(clean) return;
	cleanup_in_progress=1;
	if(tray_data.dpy!=NULL && x11_connection_status())
		{
			/* Clean the list unembedding icons one by one */
			icon_list_clean_callback(&embedder_unembed);
			/* Give away the selection */
			if(tray_data.is_active)
				XSetSelectionOwner(tray_data.dpy,tray_data.xa_tray_selection,None,CurrentTime);
			/* Sync in order to wait until all icons finish their reparenting
			 * process */
			XSync(tray_data.dpy,False);
			XCloseDisplay(tray_data.dpy);
			tray_data.dpy=NULL;
		}
	cleanup_in_progress=0;
	clean=1;
}

/**************************************
 * Helper functions
 **************************************/
/* Checks whether a given window class should be ignored */
int is_ignored_class(const char *name)
{
	struct WindowClass *haystack=NULL;

	for(haystack=settings.ignored_classes; haystack; haystack=haystack->next)
		{
			if(!strcmp(name,haystack->name))
				return 1;
		}

	return 0;
}

/**************************************
 * (Un)embedding cycle implementation
 **************************************/
/* Add icon to the tray */
void add_icon(Window w,int cmode)
{
	struct TrayIcon *ti;
	const char *classname=NULL;
	/* Aviod adding duplicates */
	if((ti=icon_list_find(w))!=NULL)
		{
			return;
		}
	/* Dear Edsger W. Dijkstra,I see you behind my back =( */
	if((ti=icon_list_new(w,cmode))==NULL) goto icon_allocation_failed;

	classname=x11_get_window_class(tray_data.dpy,w);
	if(classname==NULL)
		{
			goto embedding_failed;
		}

	if(is_ignored_class(classname))
		{
			goto done;
		}

	/* Start embedding cycle */
	if(!xembed_check_support(ti)) goto embedding_failed;
	if(ti->is_xembed_supported)
		ti->is_visible=xembed_get_mapped_state(ti);
	else
		ti->is_visible=True;
	if(ti->is_visible)
		{
			if(!embedder_reset_size(ti)) goto embedding_failed;
			if(!layout_add(ti)) goto embedding_failed;
		}
	if(!xembed_embed(ti)) goto embedding_failed_after_layout;
	if(!embedder_embed(ti)) goto embedding_failed_after_layout;
	embedder_update_positions(False);
	tray_update_window_props();
	/* Report success */
	goto done;

embedding_failed_after_layout:
	layout_remove(ti);

embedding_failed:
	icon_list_free(ti);

icon_allocation_failed:

done:
	if(classname!=NULL)
		free((void *) classname);

	return;
}

/* Remove icon from the tray */
void remove_icon(Window w)
{
	struct TrayIcon *ti;
	char *name;
	/* Ignore false alarms */
	if((ti=icon_list_find(w))==NULL) return;
	embedder_unembed(ti);
	xembed_unembed(ti);
	layout_remove(ti);
	icon_list_free(ti);
	tray_update_window_props();
}

/* Track icon visibility state changes */
void icon_track_visibility_changes(Window w)
{
	struct TrayIcon *ti;
	int mapped;
	/* Ignore false alarms */
	if((ti=icon_list_find(w))==NULL || !ti->is_xembed_supported) return;
	mapped=xembed_get_mapped_state(ti);
	/* Nothing has changed */
	if(mapped==ti->is_visible) return;
	ti->is_visible=mapped;
	if(mapped)
		{
			/* Icon has become mapped and is listed as hidden. Show this icon. */
			embedder_reset_size(ti);
			if(!layout_add(ti))
				{
					xembed_set_mapped_state(ti,False);
					return;
				}
			embedder_show(ti);
		}
	else
		{
			/* Icon has become unmapped and is listed as visible. Hide this icon. */
			layout_remove(ti);
			embedder_hide(ti);
		}
	embedder_update_positions(False);
	tray_update_window_props();
}

/* helper to identify invalid icons */
int find_invalid_icons(struct TrayIcon *ti)
{
	return ti->is_invalid;
}

void find_unmanaged_chromium_icons()
{
	unsigned int n;
	Window *topwins,dummy;
	XQueryTree(tray_data.dpy,DefaultRootWindow(tray_data.dpy),&dummy,&dummy,
	           &topwins,&n);
	if(topwins==NULL) return;

	// Find toplevel windows (unmanaged) that have:
	//_NET_WM_WINDOW_TYPE(ATOM)==_NET_WM_WINDOW_TYPE_NOTIFICATION
	// CHROMIUM_COMPOSITE_WINDOW(CARDINAL)==1
	Atom win_type=XInternAtom(tray_data.dpy,"_NET_WM_WINDOW_TYPE",False);
	Atom win_type_notif =
	    XInternAtom(tray_data.dpy,"_NET_WM_WINDOW_TYPE_NOTIFICATION",False);
	Atom chrom_composite =
	    XInternAtom(tray_data.dpy,"CHROMIUM_COMPOSITE_WINDOW",False);
	for(unsigned int i=0; i<n; i++)
		{
			Atom *aitem;
			unsigned int *citem;
			unsigned long nitems;
			int rc=False;
			Bool ok=True;

			rc=x11_get_window_prop32(tray_data.dpy,topwins[i],win_type,XA_ATOM,(unsigned char **)&aitem,&nitems);
			if(!(x11_ok() && rc==SUCCESS && nitems==1)) continue;
			ok=aitem[0]==win_type_notif;
			XFree(aitem);
			if(!ok) continue;

			rc=x11_get_window_prop32(tray_data.dpy,topwins[i],chrom_composite,XA_CARDINAL,(unsigned char **)&citem,&nitems);
			if(!(x11_ok() && rc==SUCCESS && nitems==1)) continue;
			ok=citem[0]==1;
			XFree(citem);
			if(!ok) continue;

			add_icon(topwins[i],CM_FDO);
		}

	XFree(topwins);
}

/* Perform several periodic tasks */
void perform_periodic_tasks(void)
{
	struct TrayIcon *ti;
	/* 1. Remove all invalid icons */
	while((ti=icon_list_forall(&find_invalid_icons))!=NULL)
		{
			remove_icon(ti->wid);
		}
	/* 3. KLUDGE to fix window size on (buggy?) WMs */
	if(settings.kludge_flags & KLUDGE_FIX_WND_SIZE)
		{
			/* KLUDGE TODO: resolve */
			XWindowAttributes xwa;
			XGetWindowAttributes(tray_data.dpy,tray_data.tray,&xwa);
			if(!tray_data.is_reparented && (xwa.width!=tray_data.xsh.width || xwa.height!=tray_data.xsh.height))
				{
					tray_update_window_props();
				}
		}
}

/**********************
 * Event handlers
 **********************/

void expose(XExposeEvent ev)
{
	if(ev.window==tray_data.tray && settings.parent_bg && ev.count==0)
		tray_refresh_window(False);
}

void property_notify(XPropertyEvent ev)
{
	/* React on wallpaper change */
	if(ev.atom==tray_data.xa_xrootpmap_id || ev.atom==tray_data.xa_xsetroot_id)
		{
			if(settings.transparent) tray_update_bg(True);
			if(settings.parent_bg || settings.transparent)
				tray_refresh_window(True);
		}

	/* React on WM (re)starts */
	if(ev.atom==XInternAtom(tray_data.dpy,_NET_SUPPORTING_WM_CHECK,False))
		tray_set_wm_hints();

	/* React on _XEMBED_INFO changes of embedded icons
	 * (currently used to track icon visibility status) */
	if(ev.atom==tray_data.xembed_data.xa_xembed_info)
		icon_track_visibility_changes(ev.window);

	if(ev.atom==tray_data.xa_net_client_list)
		{
			Window *windows;
			unsigned long nwindows,rc,i;
			rc=x11_get_root_winlist_prop(tray_data.dpy,tray_data.xa_net_client_list,(unsigned char **)&windows,&nwindows);
			if(x11_ok() && rc)
				{
					tray_data.is_reparented=True;
					for(i=0; i<nwindows; i++)
						if(windows[i]==tray_data.tray)
							{
								tray_data.is_reparented=False;
								break;
							}
				}
			if(nwindows) XFree(windows);
		}
}

void reparent_notify(XReparentEvent ev)
{
	struct TrayIcon *ti;
	ti=icon_list_find(ev.window);
	if(ti==NULL) return;
	/* Reparenting out of the tray is one of non-destructive
	 * ways to end XEMBED protocol (see spec) */
	if(ti->is_embedded && ti->mid_parent!=ev.parent)
		remove_icon(ev.window);
}

void client_message(XClientMessageEvent ev)
{
	int cmode=CM_FDO;
	struct TrayIcon *ti;

	/* Graceful exit */
	if(ev.message_type==tray_data.xa_wm_protocols
	        && ev.data.l[0]==tray_data.xa_wm_delete_window
	        && ev.window==tray_data.tray)
		{
			exit(0);// atexit will call cleanup()
		}
	/* Handle _NET_WM_PING */
	if(ev.message_type==tray_data.xa_wm_protocols
	        && ev.data.l[0]==tray_data.xa_net_wm_ping
	        && ev.window==tray_data.tray)
		{
			XEvent reply;
			reply.xclient=ev;
			reply.xclient.window=DefaultRootWindow(tray_data.dpy);
			XSendEvent(tray_data.dpy,DefaultRootWindow(tray_data.dpy),False,
			           (SubstructureNotifyMask | SubstructureRedirectMask),&reply);
		}
	/* Handle _NET_SYSTEM_TRAY_* messages */
	if(ev.message_type==tray_data.xa_tray_opcode && tray_data.is_active)
		{
			switch (ev.data.l[1])
				{
				/* This is the starting point of NET SYSTEM TRAY protocol */
				case SYSTEM_TRAY_REQUEST_DOCK:
					add_icon(ev.data.l[2],cmode);
					break;
				/* We ignore these messages,since we do not show
				 * any baloons anyways */
				case SYSTEM_TRAY_BEGIN_MESSAGE:
				case SYSTEM_TRAY_CANCEL_MESSAGE:
					break;
				/* Below are special cases added by this implementation */
				/* STALONETRAY_TRAY_DOCK_CONFIRMED is sent by lfstray
				 * to itself. (see embed.c) */
				case STALONE_TRAY_DOCK_CONFIRMED:
					ti=icon_list_find(ev.data.l[2]);
					if(ti!=NULL && !ti->is_embedded)
						{
							ti->is_embedded=True;
						}
					tray_update_window_props();
					break;
				/* Dump tray status on request */
				case STALONE_TRAY_STATUS_REQUESTED:
					break;
				/* Find icon and scroll to it if necessary */
				case STALONE_TRAY_REMOTE_CONTROL:
					ti=icon_list_find(ev.window);
					break;
				default:
					break;
				}
		}
}

void destroy_notify(XDestroyWindowEvent ev)
{
	if(!tray_data.is_active && ev.window==tray_data.old_selection_owner)
		{
			/* Old tray selection owner was destroyed. Take over selection
			 * ownership. */
			tray_acquire_selection();
		}
	else if(ev.window!=tray_data.tray)
		{
			/* Try to remove icon from the tray */
			remove_icon(ev.window);
		}
}

void configure_notify(XConfigureEvent ev)
{
	struct TrayIcon *ti;
	struct Point sz;
	XWindowAttributes xwa;
	if(ev.window==tray_data.tray)
		{
			/* Tray window was resized */
			/* TODO: distinguish between synthetic and real configure notifies */
			/* TODO: catch rejected configure requests */
			/* XXX: Geometry stuff is a mess. Geometry
			 * is specified in slots,but almost always is
			 * stored in pixels... */
			/* Sometimes,configure notifies come too late,so we fetch real
			 * geometry ourselves */
			XGetWindowAttributes(tray_data.dpy,tray_data.tray,&xwa);
			x11_get_window_abs_coords(
			    tray_data.dpy,tray_data.tray,&tray_data.xsh.x,&tray_data.xsh.y);
			tray_data.xsh.width=xwa.width;
			tray_data.xsh.height=xwa.height;
			/* Update icons positions */
			/* XXX: internal API is bad. example below */
			icon_list_forall(&layout_translate_to_window);
			embedder_update_positions(True);
			/* Adjust window background if necessary */
			tray_update_bg(False);
			tray_refresh_window(True);
			tray_update_window_strut();
		}
	else if((ti=icon_list_find(ev.window)) !=NULL)   /* Some icon has resized its window */
		{
			/* KDE icons are not allowed to change their size. Reset icon size. */
			if(ti->cmode==CM_KDE || settings.kludge_flags & KLUDGE_FORCE_ICONS_SIZE)
				{
					embedder_reset_size(ti);
					return;
				}
			if(settings.kludge_flags & KLUDGE_FORCE_ICONS_SIZE) return;
			/* Get new window size */
			if(!x11_get_window_size(tray_data.dpy,ti->wid,&sz.x,&sz.y))
				{
					embedder_unembed(ti);
					return;
				}
			/* Check if the size has really changed */
			if(sz.x==ti->l.wnd_sz.x && sz.y==ti->l.wnd_sz.y) return;
			ti->l.wnd_sz=sz;
			ti->is_resized=True;
			/* Do the job */
			layout_handle_icon_resize(ti);
			embedder_refresh(ti);
			embedder_update_positions(False);
			tray_update_window_props();
		}
}

void selection_clear(XSelectionClearEvent ev)
{
	/* Is it _NET_SYSTEM_TRAY selection? */
	if(ev.selection==tray_data.xa_tray_selection)
		{
			/* Is it us who has lost the selection */
			if(ev.window==tray_data.tray)
				{
					tray_data.is_active=False;
					tray_data.old_selection_owner=XGetSelectionOwner(tray_data.dpy,tray_data.xa_tray_selection);
					if(!x11_ok())
						tray_acquire_selection();
					XSelectInput(tray_data.dpy,tray_data.old_selection_owner,StructureNotifyMask);
					return;
				}
			else if(!tray_data.is_active)
				{
					/* Someone else has lost selection and tray is not active --- take
					 * over the selection */
					tray_acquire_selection();
				}
			else
				{
					/* Just in case */
				}
		}
}

void unmap_notify(XUnmapEvent ev)
{
	struct TrayIcon *ti;
	ti=icon_list_find(ev.window);
	if(ti!=NULL && !ti->is_invalid)
		{
			/* KLUDGE! sometimes icons occasionally
			 * unmap their windows,but do _not_ destroy
			 * them. We map those windows back */
			/* XXX: not root caused */
			XMapRaised(tray_data.dpy,ti->wid);
			if(!x11_ok()) ti->is_invalid=True;
		}
}

/*********************************************************/
/* main() for usual operation */
int tray_main(int argc,char **argv)
{
	XEvent ev;
	/* Interpret those settings that need an open display */
	interpret_settings();
	/* Create and show tray window */
	tray_create_window(argc,argv);
	tray_acquire_selection();
	tray_show_window();
	xembed_init();
	find_unmanaged_chromium_icons();
	/* Main event loop */
	while("my guitar gently wheeps")
		{
			/* This is ugly and extra dependency. But who cares?
			 * Rationale: we want to block unless absolutely needed.
			 * This way we ensure that lfstray does not show up
			 * in powertop (i.e. does not eat unnecessary power and
			 * CPU cycles)
			 * Drawback: handling of signals is very limited. XNextEvent()
			 * does not if signal occurs. This means that graceful
			 * exit on e.g. Ctrl-C cannot be implemented without hacks. */
			//while(XPending(tray_data.dpy) || -1==-1 )
			while(True)
				{
					XNextEvent(tray_data.dpy,&ev);
					xembed_handle_event(ev);
					switch (ev.type)
						{
						case VisibilityNotify:
							break;
						case Expose:
							expose(ev.xexpose);
							break;
						case PropertyNotify:
							property_notify(ev.xproperty);
							break;
						case DestroyNotify:
							destroy_notify(ev.xdestroywindow);
							break;
						case ClientMessage:
							client_message(ev.xclient);
							break;
						case ConfigureNotify:
							configure_notify(ev.xconfigure);
							break;
						case MapNotify:
							break;
						case ReparentNotify:
							reparent_notify(ev.xreparent);
							break;
						case SelectionClear:
							selection_clear(ev.xselectionclear);
							break;
						case SelectionNotify:
							break;
						case SelectionRequest:
							break;
						case UnmapNotify:
							unmap_notify(ev.xunmap);
							break;
						default:

							break;
						}
					if(tray_data.terminated) goto bailout;
				}
			perform_periodic_tasks();
			my_usleep(500000L);
		}
bailout:
	return 0;
}


/* main() */
int main(int argc,char **argv)
{
	/* Read settings */
	tray_init();
	read_settings(argc,argv);
	/* Register cleanup and signal handlers */
	atexit(cleanup);

	/* Open display */
	if((tray_data.dpy=XOpenDisplay(settings.display_str))==NULL)
		{
			fprintf(stderr,"could not open display\n");
			exit(-1);
		}

	if(settings.xsync) XSynchronize(tray_data.dpy,True);
	x11_trap_errors();
	/* Execute proper main() function */
	return tray_main(argc,argv);
}
