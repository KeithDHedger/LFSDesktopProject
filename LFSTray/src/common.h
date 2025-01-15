/*
 *
 * ©K. D. Hedger. Mon 13 Jan 16:18:50 GMT 2025 keithdhedger@gmail.com

 * This file (common.h) is part of LFSTray.

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
 * common.h
 * Mon,01 May 2006 01:45:08 +0700
 * -------------------------------
 * Common declarations
 * -------------------------------*/

#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

#endif
