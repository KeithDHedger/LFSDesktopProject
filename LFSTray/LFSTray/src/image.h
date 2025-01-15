/*
 *
 * ©K. D. Hedger. Mon 13 Jan 16:33:05 GMT 2025 keithdhedger@gmail.com

 * This file (image.h) is part of LFSTray.

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
 * image.h
 * Fri,22 Jun 2007 23:32:27 +0700
 * -------------------------------
 * Simple XImage manipulation
 * interface
 * -------------------------------*/

#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <X11/Xlib.h>
#include <X11/Xmd.h>

/* outstanding TODO (for 0.8): use Xrender when available */

/* WARNING: works with ZPixmaps only */

/* Creates alpha channel mask with specified fade-out order */
CARD8 *image_create_alpha_mask(int ord,int w,int h);

/* Alpha-tint image using color. */
int image_tint(XImage *image,XColor *color,CARD8 alpha);

/* Compose image stored in tgt with image stored in bg.
 * Alpha of each pixel is defined by mask,which should */
int image_compose(XImage *tgt,XImage *bg,CARD8 *mask);

#endif
