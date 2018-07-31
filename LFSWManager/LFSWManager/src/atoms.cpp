/*
 *
 * ©K. D. Hedger. Tue 31 Jul 13:30:48 BST 2018 keithdhedger@gmail.com

 * This file (atoms.cpp) is part of LFSWManager.

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

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include "atoms.h"

/*
 * The list of supported properties. Note that we need to
 * include some properties that we actually never use in
 * the _NET_SUPPORTED list in order to show applications
 * that we do indeed know about them. For example,pagers
 * act strangely if we don't mention _NET_WM_STATE_HIDDEN.
 *
 * NB: Keep this list sorted.
 */
Atom NET_ACTIVE_WINDOW;
Atom NET_CLIENT_LIST;
Atom NET_CLIENT_LIST_STACKING;
Atom NET_CLOSE_WINDOW;
Atom NET_CURRENT_DESKTOP;
Atom NET_DESKTOP_GEOMETRY;
Atom NET_DESKTOP_VIEWPORT;
Atom NET_FRAME_EXTENTS;
Atom NET_NUMBER_OF_DESKTOPS;
Atom NET_REQUEST_FRAME_EXTENTS;
Atom NET_SUPPORTED;
Atom NET_SUPPORTING_WM_CHECK;
Atom NET_WM_ACTION_CHANGE_DESKTOP;
Atom NET_WM_ACTION_CLOSE;
Atom NET_WM_ACTION_FULLSCREEN;
Atom NET_WM_ACTION_MINIMIZE;
Atom NET_WM_ALLOWED_ACTIONS;
Atom NET_WM_DESKTOP;
Atom NET_WM_ICON_NAME;
Atom NET_WM_NAME;
Atom NET_WM_STATE;
Atom NET_WM_STATE_ABOVE;
Atom NET_WM_STATE_BELOW;
Atom NET_WM_STATE_FULLSCREEN;
Atom NET_WM_STATE_HIDDEN;
Atom NET_WM_STATE_SKIP_TASKBAR;
Atom NET_WM_VISIBLE_ICON_NAME;
Atom NET_WM_VISIBLE_NAME;
Atom NET_WM_WINDOW_TYPE;
Atom NET_WM_WINDOW_TYPE_DOCK;
Atom NET_WORKAREA;
Atom NET_WM_WINDOW_TYPE_DESKTOP;
Atom NET_WM_STATE_STICKY;
Atom NET_WM_ACTION_MAXIMIZE_HORZ;
Atom NET_WM_ACTION_MAXIMIZE_VERT;
Atom NET_WM_STATE_MAXIMIZED_VERT;
Atom NET_WM_STATE_MAXIMIZED_HORZ;

Atom UTF8_STRING;
Atom MOTIF_WM_HINTS;

Atom WM_CHANGE_STATE;
Atom WM_DELETE_WINDOW;
Atom WM_PROTOCOLS;
Atom WM_STATE;
