/*
 *
 * ©K. D. Hedger. Mon 13 Jan 16:29:48 GMT 2025 keithdhedger@gmail.com

 * This file (embed.h) is part of LFSTray.

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
 * embed.h
 * Fri,03 Sep 2004 20:38:55 +0700
 * -------------------------------
 * embedding cycle implementation
 * -------------------------------*/

#ifndef _EMBED_H_

#include "icons.h"

/* Constants for compatibility modes */
/* KDE */
#define CM_KDE 1
/* Generic,freedesktop.org */
#define CM_FDO 2

/* Embed an icon */
int embedder_embed(struct TrayIcon *ti);

/* Unembed an icon */
int embedder_unembed(struct TrayIcon *ti);

/* If (forced)
 * 		recalculate and update positions of all icons;
 * else
 * 		recalculate and update positions of all icons that have requested an
 * update; */
int embedder_update_positions(int force);

/* Show the icon */
int embedder_show(struct TrayIcon *ti);

/* Hide the icon */
int embedder_hide(struct TrayIcon *ti);

/* Refresh icon and its parent */
int embedder_refresh(struct TrayIcon *ti);

/* (Re)set icon size according to the policy */
int embedder_reset_size(struct TrayIcon *ti);

#endif
