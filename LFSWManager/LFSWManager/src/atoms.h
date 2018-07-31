/*
 *
 * ©K. D. Hedger. Tue 31 Jul 13:30:58 BST 2018 keithdhedger@gmail.com

 * This file (atoms.h) is part of LFSWManager.

 * LFSWManager is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSWManager is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSWManager.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _ATOMS_
#define _ATOMS_

#include <X11/Xlib.h>
#include <X11/Xatom.h>

//atoms
#define xatom(name) XInternAtom(dpy,(name),False)

extern Atom NET_ACTIVE_WINDOW;
extern Atom NET_CLIENT_LIST;
extern Atom NET_CLIENT_LIST_STACKING;
extern Atom NET_CLOSE_WINDOW;
extern Atom NET_CURRENT_DESKTOP;
extern Atom NET_DESKTOP_GEOMETRY;
extern Atom NET_DESKTOP_VIEWPORT;
extern Atom NET_FRAME_EXTENTS;
extern Atom NET_NUMBER_OF_DESKTOPS;
extern Atom NET_REQUEST_FRAME_EXTENTS;
extern Atom NET_SUPPORTED;
extern Atom NET_SUPPORTING_WM_CHECK;
extern Atom NET_WM_ACTION_CHANGE_DESKTOP;
extern Atom NET_WM_ACTION_CLOSE;
extern Atom NET_WM_ACTION_FULLSCREEN;
extern Atom NET_WM_ACTION_MINIMIZE;
extern Atom NET_WM_ALLOWED_ACTIONS;
extern Atom NET_WM_DESKTOP;
extern Atom NET_WM_ICON_NAME;
extern Atom NET_WM_NAME;
extern Atom NET_WM_STATE;
extern Atom NET_WM_STATE_ABOVE;
extern Atom NET_WM_STATE_BELOW;
extern Atom NET_WM_STATE_FULLSCREEN;
extern Atom NET_WM_STATE_HIDDEN;
extern Atom NET_WM_STATE_SKIP_TASKBAR;
extern Atom NET_WM_VISIBLE_ICON_NAME;
extern Atom NET_WM_VISIBLE_NAME;
extern Atom NET_WM_WINDOW_TYPE;
extern Atom NET_WM_WINDOW_TYPE_DOCK;
extern Atom NET_WORKAREA;
extern Atom NET_WM_WINDOW_TYPE_DESKTOP;
extern Atom NET_WM_STATE_STICKY;
extern Atom NET_WM_ACTION_MAXIMIZE_HORZ;
extern Atom NET_WM_ACTION_MAXIMIZE_VERT;
extern Atom NET_WM_STATE_MAXIMIZED_VERT;
extern Atom NET_WM_STATE_MAXIMIZED_HORZ;

extern Atom UTF8_STRING;
extern Atom MOTIF_WM_HINTS;

extern Atom WM_CHANGE_STATE;
extern Atom WM_DELETE_WINDOW;
extern Atom WM_PROTOCOLS;
extern Atom WM_STATE;

#endif
