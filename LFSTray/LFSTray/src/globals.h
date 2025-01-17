/*
 *
 * ©K. D. Hedger. Wed 15 Jan 20:14:30 GMT 2025 keithdhedger@gmail.com

 * This file (globals.h) is part of LFSTray.

 * LFSTray is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * LFSTray is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSTray.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 Forked from stalonetray ( big thanks )
 Original Code here:
 https://github.com/kolbusa/stalonetray
 * common.h
 * Mon,01 May 2006 01:45:08 +0700
 * -------------------------------
 * Common declarations
 * -------------------------------*/

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <ft2build.h>

#include "config.h"
#include "tray.h"
#include "settings.h"

#define PROGNAME PACKAGE

/* Default icon size */
#define FALLBACK_ICON_SIZE 24
/* Minimal icon size */
#define MIN_ICON_SIZE 16
/* Default KDE icon size */
#define KDE_ICON_SIZE 22
/* Number of time icon is allowed to change its size after which
 * lfstray gives up */
#define ICON_SIZE_RESETS_THRESHOLD 2

/* Meaningful names for return values */
#define SUCCESS 1
#define FAILURE 0

/* Meaningful names for return values of icon mass-operations */
#define MATCH 1
#define NO_MATCH 0

/* Meaningful names for return values of icon mass-operations */
#define MATCH 1
/* Portable macro for function name */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define __FUNC__ __func__
#elif defined(__GNUC__) && __GNUC__ >= 3
#define __FUNC__ __FUNCTION__
#else
#define __FUNC__ "unknown"
#endif

/* DIE */
/* Print a message and... DIE */
#define DIEDIE() exit(-1)
/* DIE on OOM error */
#define DIE_OOM(message) { fprintf(stderr,"%s",message); exit(-1); }

/*** WARNING: feed following macros only with side-effects-free expressions
 * ***/
/* Get a value within target interval */
#define cutoff(tgt,min,max) (tgt)<(min) ? (min) : ((tgt) > (max) ? max : tgt)
/* Update value to fit into target interval */
#define val_range(tgt,min,max) (tgt)=cutoff(tgt,min,max)

struct TrayData
{
	/* General */
	Window tray; /* ID of tray window */
	Window hint_win; /* ID of icon window */
	Display *dpy; /* Display pointer */
	XSizeHints xsh; /* Size & position of the tray window */
	XSizeHints root_wnd; /* Size & position :) of the root window */
	Window old_selection_owner; /* Old owner of tray selection */
	int terminated; /* Exit flag */
	int is_active; /* Is the tray active? */
	int is_reparented; /* Was the tray reparented in smth like FvwmButtons ? */
	int kde_tray_old_mode; /* Use legacy scheme to handle KDE icons via MapNotify */

	/* Atoms */
	Atom xa_tray_selection; /* Atom: _NET_SYSTEM_TRAY_SELECTION_S<creen number>
                             */
	Atom xa_tray_opcode; /* Atom: _NET_SYSTEM_TRAY_MESSAGE_OPCODE */
	Atom xa_tray_data; /* Atom: _NET_SYSTEM_TRAY_MESSAGE_DATA */
	Atom xa_wm_protocols; /* Atom: WM_PROTOCOLS */
	Atom xa_wm_delete_window; /* Atom: WM_DELETE_WINDOW */
	Atom xa_net_wm_ping; /* Atom: WM_PING */
	Atom xa_wm_take_focus; /* Atom: WM_TAKE_FOCUS */
	Atom xa_kde_net_system_tray_windows; /* Atom: _KDE_NET_SYSTEM_TRAY_WINDOWS
                                          */
	Atom xa_net_client_list; /* Atom: _NET_CLIENT_LIST */

	/* Background pixmap */
	Atom xa_xrootpmap_id; /* Atom: _XROOTPMAP_ID */
	Atom xa_xsetroot_id; /* Atom: _XSETROOT_ID */
	Pixmap bg_pmap; /* Pixmap for tray background */
	struct Point bg_pmap_dims; /* Background pixmap dimensions */

	/* XEMBED data */
	struct XEMBEDData xembed_data; /* XEMBED data */
};

extern struct TrayData	tray_data;

#endif
