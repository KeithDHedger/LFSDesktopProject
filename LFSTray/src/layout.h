/*
 *
 * ©K. D. Hedger. Mon 13 Jan 16:28:10 GMT 2025 keithdhedger@gmail.com

 * This file (layout.h) is part of LFSTray.

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
 * layout.h
 * Tue,24 Aug 2004 12:19:27 +0700
 * -------------------------------
 * Icon layout implementation
 * -------------------------------*/

#ifndef _LAYOUT_H_
#define _LAYOUT_H_

#include "icons.h"

/* Gravity constants */
/* East gravity */
#define GRAV_E (1L << 0)
/* West gravity */
#define GRAV_W (1L << 1)
/* South gravity */
#define GRAV_S (1L << 2)
/* North gravity */
#define GRAV_N (1L << 3)
/* Shortcut for horisontal gravity */
#define GRAV_H (GRAV_W | GRAV_E)
/* Shortcut for vertical gravity */
#define GRAV_V (GRAV_S | GRAV_N)

/* Macros to test rect intersection */
/* Helpers */
#define RX1(r) (r.x)
#define RY1(r) (r.y)
#define RX2(r) (r.x + r.w - 1)
#define RY2(r) (r.y + r.h - 1)
#define RECTS_ISECT_(r1,r2) \
    (((RX1(r1) <= RX1(r2) && RX1(r2) <= RX2(r1)) \
         || (RX1(r1) <= RX2(r2) && RX2(r2) <= RX2(r1))) \
        && ((RY1(r1) <= RY1(r2) && RY1(r2) <= RY2(r1)) \
            || (RY1(r1) <= RY2(r2) && RY2(r2) <= RY2(r1))))
/* Macro itself */
#define RECTS_ISECT(r1,r2) (RECTS_ISECT_(r1,r2) || RECTS_ISECT_(r2,r1))

/* Add icon to layout */
int layout_add(struct TrayIcon *ti);

/* Remove icon from layout */
int layout_remove(struct TrayIcon *ti);

/* Relayout the icon which has changed its size */
int layout_handle_icon_resize(struct TrayIcon *ti);

/* Get current layout dimensions */
void layout_get_size(int *width,int *height);

/* Translate grid coordinates into window coordinates */
int layout_translate_to_window(struct TrayIcon *ti);

/* Return next icon in tab chain */
struct TrayIcon *layout_next(struct TrayIcon *current);

/* Return previous icon in tab chain */
struct TrayIcon *layout_prev(struct TrayIcon *current);

#endif
