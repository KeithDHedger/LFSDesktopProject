/*
 *
 * ©K. D. Hedger. Mon 13 Jan 16:30:51 GMT 2025 keithdhedger@gmail.com

 * This file (tray.h) is part of LFSTray.

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
 * tray.h
 * Wed,29 Sep 2004 23:10:02 +0700
 * -------------------------------
 * Common tray routines
 * -------------------------------*/

#ifndef _TRAY_H_
#define _TRAY_H_

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <limits.h>

#include "globals.h"
#include "config.h"
#include "icons.h"

#include "xembed.h"

/* Tray opcode messages from System Tray Protocol Specification
 * http:freedesktop.org/Standards/systemtray-spec/systemtray-spec-0.2.html */
#define SYSTEM_TRAY_REQUEST_DOCK 0
/* These two are unused */
#define SYSTEM_TRAY_BEGIN_MESSAGE 1
#define SYSTEM_TRAY_CANCEL_MESSAGE 2
/* Custom message: remote control */
#define STALONE_TRAY_REMOTE_CONTROL 0xFFFD
/* Custom message: request for status */
#define STALONE_TRAY_STATUS_REQUESTED 0xFFFE
/* Custom message: confirmation of embedding */
#define STALONE_TRAY_DOCK_CONFIRMED 0xFFFF
/* Name of tray selection atom */
#define TRAY_SEL_ATOM "_NET_SYSTEM_TRAY_S"
/* Name of tray orientation atom*/
#define TRAY_ORIENTATION_ATOM "_NET_SYSTEM_TRAY_ORIENTATION"
/* Name of tray orientation atom*/
#define STALONETRAY_REMOTE_ATOM "STALONETRAY_REMOTE"
/* Values of tray orientation property */
#define _NET_SYSTEM_TRAY_ORIENTATION_HORZ 0
#define _NET_SYSTEM_TRAY_ORIENTATION_VERT 1

/* Window decoration flags */
#define DECO_BORDER (1 << 0)
#define DECO_TITLE (1 << 1)
#define DECO_NONE 0
#define DECO_ALL (DECO_BORDER | DECO_TITLE)

/* WM struts flags */
#define WM_STRUT_NONE 0
#define WM_STRUT_LFT 1
#define WM_STRUT_RHT 2
#define WM_STRUT_TOP 3
#define WM_STRUT_BOT 4
#define WM_STRUT_AUTO 5

/* Dockapp modes */
#define DOCKAPP_NONE 0
#define DOCKAPP_SIMPLE 1
#define DOCKAPP_WMAKER 2

/* Kludge flags */
#define KLUDGE_FIX_WND_SIZE (1L << 1)
#define KLUDGE_FIX_WND_POS (1L << 2)
#define KLUDGE_USE_ICONS_HINTS (1L << 3)
#define KLUDGE_FORCE_ICONS_SIZE (1L << 4)

/* Initialize all tray data structures */
void tray_init();
/* Create tray window */
void tray_create_window(int argc,char **argv);
/* Create phony tray window so that certain x11_ calls work */
void tray_create_phony_window();
/* Initialize tray selection atoms */
void tray_init_selection_atoms();
/* Acquire tray selection */
void tray_acquire_selection();
/* Show tray window */
void tray_show_window();
/* Refresh tray window */
void tray_refresh_window(int exposures);
/* Update tray background (and pixmap,if update_pixmap is true) */
int tray_update_bg(int update_pixmap);
/* Calculate tray window size given the size of icon area in pixels. */
int tray_calc_window_size(int base_width,int base_height,int *new_width,int *new_height);
/* Calculate size of icon area given the tray window size in pixels. */
int tray_calc_tray_area_size(int wnd_width,int wnd_height,int *base_width,int *base_height);
/* Update window struts (if enabled) */
int tray_update_window_strut();
/* Update tray window size and hints */
int tray_update_window_props();
/* Set tray window WM hints */
int tray_set_wm_hints();

#endif
