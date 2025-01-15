/*
 *
 * ©K. D. Hedger. Mon 13 Jan 16:23:55 GMT 2025 keithdhedger@gmail.com

 * This file (settings.h) is part of LFSTray.

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
 * settings.h
 * Sun,12 Sep 2004 18:06:08 +0700
 * -------------------------------
 * settings parser\container
 * -------------------------------*/

#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <X11/X.h>
#include <X11/Xlib.h>

#include "config.h"
#include "layout.h"

/* Default name of configuration file */
#define LFSTRAY_RC ".config/LFS/lfstrayrc"

struct WindowClass
{
	char *name;
	struct WindowClass *next;
	struct WindowClass *prev;
};

struct Settings
{
	/* Flags */
	int parent_bg; /* Enable pseudo-transparency using parents' background*/
	int transparent; /* Enable root-transparency */
	int skip_taskbar; /* Remove tray from wm`s taskbar */
	int sticky; /* Make tray sticky across desktops/pages */
	int xsync; /* Operate on X server syncronously */
	int pixmap_bg; /* Is pixmap used for background */
	int min_space_policy; /* Use placement that cause minimal grow */
	int full_pmt_search; /* Use non-first-match search algorithm */
	int vertical; /* Use vertical icon layout */
	int shrink_back_mode; /* Keep tray's window size minimal */
	int kludge_flags; /* What kludges to activate */

	int need_help; /* Print usage and exit */

	/* Lists of strings */
	struct WindowClass *ignored_classes; /* List of window classes to ignore */

	/* Strings */
	char *display_str; /* Name of the display */
	char *bg_color_str; /* Background color name */
	char *geometry_str; /* Geometry spec */
	char *max_geometry_str; /* Geometry spec */
	char *config_fname; /* Path to the configuration file */
	char *wnd_layer; /* Window layer */
	char *wnd_name; /* Window name (WM_NAME) */
	char *bg_pmap_path; /* Background pixmap path */
	char *tint_color_str; /* Color used for tinting */

	/* Values */
	int icon_size; /* Icon size */
	struct Point slot_size; /* Grid slot size */
	int grow_gravity; /* Icon gravity (interpretation of icon_gravity_str) */
	int icon_gravity; /* Grow gravity (interpretation of grow_gravity_str) */
	int win_gravity; /* Tray window gravity (computed using grow gravity) */
	int bit_gravity; /* Tray window bit gravity (computed using icon_gravity)*/
	int geom_gravity; /* Tray window gravity when mapping the window (computed using geometry_str) */
	int tint_level; /* Tinting level (0 = disabled) */
	int wm_strut_mode; /* WM strut mode */
	struct Point max_tray_dims; /* Maximal tray dimensions */
	struct Point max_layout_dims; /* Maximal layout dimensions */
	struct Point orig_tray_dims; /* Original tray dimensions */

	XColor tint_color; /* Color used for tinting */

#ifdef DELAY_EMBEDDING_CONFIRMATION
	int confirmation_delay;
#endif

	XColor bg_color; /* Tray background color */
};

extern struct Settings settings;

/* Read settings from cmd line and configuration file */
int read_settings(int argc,char **argv);
/* Interpret all settings that either need an
 * open display or are interpreted from other
 * settings */
void interpret_settings();

#endif
