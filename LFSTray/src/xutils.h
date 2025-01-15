/*
 *
 * ©K. D. Hedger. Mon 13 Jan 16:24:45 GMT 2025 keithdhedger@gmail.com

 * This file (xutils.h) is part of LFSTray.

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
 * xutils.h
 * Sun,05 Mar 2006 17:16:44 +0600
 * ************************************
 * misc X11 utilities
 * ************************************/

#ifndef _XUTILS_H_
#define _XUTILS_H_

#include <X11/X.h>
#include <X11/Xatom.h>

#include "common.h"
#include "icons.h"

/* Returns 1 if connection is active,0 otherwise */
int x11_connection_status();

/* Return current server timestamp */
Time x11_get_server_timestamp(Display *dpy,Window wnd);

/* Convinient way to send a client message event */
int x11_send_client_msg32(Display *dpy,Window dst,Window wnd,Atom type,long data0,long data1,long data2,long data3,long data4);

/* Same for visibility event */
int x11_send_visibility(Display *dpy,Window dst,long state);

/* Same for expose event */
int x11_send_expose(Display *dpy,Window dst,int x,int y,int width,int height);

/* Refresh window */
int x11_refresh_window(Display *dpy,Window dst,int width,int height,int exposures);

/* Set window size updating its size hints */
int x11_set_window_size(Display *dpy,Window w,int x,int y);

/* Get window size (uses XGetWindowAttributes) */
int x11_get_window_size(Display *dpy,Window w,int *x,int *y);

/* Get window minimal size hints if they are available */
int x11_get_window_min_size(Display *dpy,Window w,int *x,int *y);

/* Retrive 32-bit property from the target window */
int x11_get_window_prop32(Display *dpy,Window dst,Atom atom,Atom type,unsigned char **data,unsigned long *len);

/* Retrive window-list property from the specified window */
#define x11_get_winlist_prop(dpy,dst,atom,data,len) \
    x11_get_window_prop32(dpy,dst,atom,XA_WINDOW,data,len)

/* Shortcut for the root window case */
#define x11_get_root_winlist_prop(dpy,atom,data,len) x11_get_winlist_prop(dpy,DefaultRootWindow(dpy),atom,data,len)

/* Returns window absolute position (relative to the root window) */
int x11_get_window_abs_coords(Display *dpy,Window dst,int *x,int *y);

/* Get window name. NOT THREAD SAFE. Returns pointer to static buffer */
char *x11_get_window_name(Display *dpy,Window dst,char *def);

/* Get window class name. Returns pointer to string created with strdup() */
char *x11_get_window_class(Display *dpy,Window w);

/* Find subwindow by name */
Window x11_find_subwindow_by_name(Display *dpy,Window tgt,char *name);

/* Find subwindow by at coords */
Window x11_find_subwindow_at(Display *dpy,Window top,int *x,int *y,int depth);

/* Extends event mask of the root window */
void x11_extend_root_event_mask(Display *dpy,long mask);

/* Parse text representation of a color */
int x11_parse_color(Display *dpy,char *str,XColor *color);

/* Checks if any X11 errors have occured so far. */
/* ACHTUNG!!! after any sequence X operations any of which
 * that might have failed,you _must_ call x11_ok(),since
 * it resets the internal error flag. If you dont,it will show
 * up as a error elsewhere. JFYI,always check for x11_ok() first,
 * since
 * 		if (!rc || x11_ok()) { fail; }
 * is likely to leave error condition on: x11_ok() wont be called
 * if rc != 0. */
#define x11_ok() x11_ok_helper(__FILE__,__LINE__,__FUNC__)
int x11_ok_helper(const char *file,const int line,const char *func);

/* WARNING: following functions do not support nested calls */

/* Installs custom X11 error handler */
void x11_trap_errors();

/* Removes custom X11 error handler */
int x11_untrap_errors();

#endif
