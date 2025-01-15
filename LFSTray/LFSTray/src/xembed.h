/*
 *
 * ©K. D. Hedger. Mon 13 Jan 16:32:27 GMT 2025 keithdhedger@gmail.com

 * This file (xembed.h) is part of LFSTray.

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
 * icons.h
 * Tue,24 Aug 2004 12:05:38 +0700
 * -------------------------------
 * XEMBED protocol implementation
 * -------------------------------*/

#ifndef _XEMBED_H_
#define _XEMBED_H_

#include <X11/X.h>

#include "icons.h"

/* Data structure for all XEMBED-related data for the tray */
struct XEMBEDData
{
	struct TrayIcon *current; /* Pointer to the currently focused icon */
	struct XEMBEDAccel *accels; /* List of registered XEMBED accelerators */
	int window_has_focus; /* Flag: does tray's window have focus */
	int focus_requested; /* Flag: if there is not completed focus request */
	Window focus_proxy; /* Window ID of XEMBED focus proxy */
	long timestamp; /* Timestamp of current processed message */
	Atom xa_xembed_info; /* Atom: XEMBED_INFO */
	Atom xa_xembed; /* Atom: XEMBED */
};

/* Initialize XEMBED data structures */
void xembed_init();

/* Event handling routine for XEMBED support */
void xembed_handle_event(XEvent ev);

/* Check if icon ti supports XEMBED */
int xembed_check_support(struct TrayIcon *ti);

/* Send XEMBED embedding acknowledgement to icon ti */
int xembed_embed(struct TrayIcon *ti);

/* Same as above for unembedding */
int xembed_unembed(struct TrayIcon *ti);

/* Get XEMBED mapped state from XEMBED info */
int xembed_get_mapped_state(struct TrayIcon *ti);

/* Set XEMBED mapped state in XEMBED info */
int xembed_set_mapped_state(struct TrayIcon *ti,int state);

#endif
