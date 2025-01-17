/*
 *
 * ©K. D. Hedger. Mon 13 Jan 16:22:43 GMT 2025 keithdhedger@gmail.com

 * This file (layout.c) is part of LFSTray.

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
 * layout.c
 * Tue,24 Aug 2004 12:19:48 +0700
 * -------------------------------
 * Icon layout implementation.
 * (Used to be) the dirtiest place around.
 * -------------------------------*/

#include <limits.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "config.h"
#include "globals.h"
#include "icons.h"
#include "layout.h"
#include "list.h"
#include "tray.h"

/* not very nice (and calculates its arguments twice!),but allows doing things
 * like swap(*a,*b) */
#define swap(a,b) \
    do { \
        int t; \
        t=(a); \
        (a)=(b); \
        (b)=t; \
    } while(0)

/*****************
 * Grid interface *
 *****************/

struct Point grid_sz= {0,0};

int grid_add(struct TrayIcon *ti);
int grid_add_wrapper(struct TrayIcon *ti);
int grid_remove(struct TrayIcon *ti);
int grid_update(struct TrayIcon *ti,int sort);
int grid_translate_from_window(struct TrayIcon *ti);

int layout_translate_to_window(struct TrayIcon *ti);
int layout_unset_flag(struct TrayIcon *ti);

/************************
 * Layout implementation *
 ************************/

int layout_add(struct TrayIcon *ti)
{
	if(grid_add(ti))
		{
			grid_update(ti,True);
			return SUCCESS;
		}
	else
		return FAILURE;
}

int layout_remove(struct TrayIcon *ti)
{
	return ti->is_layed_out ? grid_remove(ti) : SUCCESS;
}

int layout_handle_icon_resize(struct TrayIcon *ti)
{
	struct Rect old_grd_rect;
	struct Point old_grid_sz;
	int rc;

	if(!icon_list_backup()) return FAILURE;
	old_grid_sz=grid_sz;
	if(ti->is_layed_out)
		{
			/* if the icon is already layed up and
			 * its grid rect did not change we do nothing,
			 * since its position inside grid cell will be
			 * updated by embedder_update_positions */
			old_grd_rect=ti->l.grd_rect;
			grid_translate_from_window(ti);
			if(ti->l.grd_rect.w==old_grd_rect.w && ti->l.grd_rect.h==old_grd_rect.h)
				{
					icon_list_backup_purge();
					return SUCCESS;
				}
		}
	/* Here's the place where icon sorting start playing its role.
	 * It is easy to see that resizing ti affects only those icons
	 * that are after ti in the icon list. */
	/* 1. Unset layout flags of all icons after ti */
	icon_list_forall_from(ti,&layout_unset_flag);
	/* 2. If shrink mode is on,recalculate the size of the grid
	 *    for the remaining icons. This ensures that final grid
	 *    will be "minimal" (I did not prove that :) */
	if(settings.shrink_back_mode) grid_update(ti,False);
	/* 3. Start adding icons after ti back to the grid one
	 *    by one. If this fails for some icon,forall_icons_from()
	 *    will return non-NULL value and this will be considered
	 *    as a error condition */
	if(icon_list_forall_from(ti,&grid_add_wrapper)==NULL)
		{
			/* Everything is OK */
			icon_list_backup_purge();
			grid_update(ti,True);
			rc=SUCCESS;
		}
	else
		{
			/* Error has occured */
			icon_list_restore();
			grid_sz=old_grid_sz;
			rc=FAILURE;
		}
	return rc;
}

void layout_get_size(int *width,int *height)
{
	*width=grid_sz.x*settings.slot_size.x;
	*height=grid_sz.y*settings.slot_size.y;
	if(settings.vertical) swap((*width),(*height));
}

struct TrayIcon *layout_next(struct TrayIcon *current)
{
	if((settings.icon_gravity & GRAV_H)==GRAV_W)
		return icon_list_next(current);
	else
		return icon_list_prev(current);
}

struct TrayIcon *layout_prev(struct TrayIcon *current)
{
	if((settings.icon_gravity & GRAV_H)==GRAV_W)
		return icon_list_prev(current);
	else
		return icon_list_next(current);
}

/**********************
 * Grid implementation *
 **********************/

/* Structure to hold possible placement for an icon */
struct IconPlacement
{
	struct Point pos; /* Position */
	struct Point sz_delta; /* Layout size delta */
	int valid; /* Is the placement valid */
};

/* Find best placement for an icon */
struct IconPlacement *grid_find_placement(struct TrayIcon *ti);
/* Place tray icon as specified by ip */
int grid_place_icon(struct TrayIcon *ti,struct IconPlacement *ip);
/* Comparison function for icon_list_sort */
int trayicon_cmp_func(struct TrayIcon *ti1,struct TrayIcon *ti2);
/* Update grid dimentions */
int grid_recalc_size(struct TrayIcon *ti);

/* A. Coords translations */
int grid_translate_from_window(struct TrayIcon *ti)
{
	ti->l.grd_rect.w=ti->l.wnd_sz.x/settings.slot_size.x+(ti->l.wnd_sz.x % settings.slot_size.x!=0);
	ti->l.grd_rect.h=ti->l.wnd_sz.y/settings.slot_size.y+(ti->l.wnd_sz.y % settings.slot_size.y!=0);
	if(settings.vertical) swap(ti->l.grd_rect.w,ti->l.grd_rect.h);
	return NO_MATCH;
}

int layout_translate_to_window(struct TrayIcon *ti)
{
	struct Rect old_icn_rect=ti->l.icn_rect;
	if(!ti->is_layed_out) return NO_MATCH;
	/* Swap vert & horz dimentions temporarily */
	if(settings.vertical)
		{
			swap(ti->l.grd_rect.w,ti->l.grd_rect.h);
			swap(ti->l.grd_rect.x,ti->l.grd_rect.y);
		}
	/* Compute icon position and dimensions */
	if(settings.icon_gravity & GRAV_W)
		ti->l.icn_rect.x=ti->l.grd_rect.x*settings.slot_size.x;
	else
		ti->l.icn_rect.x=tray_data.xsh.width-(ti->l.grd_rect.x+ti->l.grd_rect.w)*settings.slot_size.x;
	if(settings.icon_gravity & GRAV_N)
		ti->l.icn_rect.y=ti->l.grd_rect.y*settings.slot_size.y;
	else
		ti->l.icn_rect.y=tray_data.xsh.height-(ti->l.grd_rect.y+ti->l.grd_rect.h)*settings.slot_size.y;
	ti->l.icn_rect.w=ti->l.grd_rect.w*settings.slot_size.y;
	ti->l.icn_rect.h=ti->l.grd_rect.h*settings.slot_size.y;
	/* Swap vert & horz dimentions back */
	if(settings.vertical)
		{
			swap(ti->l.grd_rect.w,ti->l.grd_rect.h);
			swap(ti->l.grd_rect.x,ti->l.grd_rect.y);
		}
	/* Set flag indicating if new position is different from old *///TODO//
	ti->is_updated=ti->is_updated || !(ti->l.icn_rect.x==old_icn_rect.x && ti->l.icn_rect.y==old_icn_rect.y && ti->l.icn_rect.w==old_icn_rect.w && ti->l.icn_rect.h==old_icn_rect.h);
	return NO_MATCH;
}

/* B. Adding/removing icon from the grid */
int grid_add(struct TrayIcon *ti)
{
	struct IconPlacement *pmt;
	/* Invalidate grid position data */
	ti->l.grd_rect.x=-1;
	ti->l.grd_rect.y=-1;
	ti->l.grd_rect.w=-1;
	ti->l.grd_rect.h=-1;
	/* Invalidate window position data */
	ti->l.icn_rect.x=-1;
	ti->l.icn_rect.y=-1;
	ti->l.icn_rect.w=-1;
	ti->l.icn_rect.h=-1;
	/* Get window dimensions */
	grid_translate_from_window(ti);
	pmt=grid_find_placement(ti);
	if(pmt==NULL)
		{
			ti->is_layed_out=False;
			return FAILURE;
		}
	ti->is_layed_out=True;
	grid_place_icon(ti,pmt);
	return SUCCESS;
}

int grid_remove(struct TrayIcon *ti)
{
	/* implementation is similar to that of layout_handle_icon_resize(),
	 * see detailed description there */
	icon_list_forall_from(ti,&layout_unset_flag);
	if(settings.shrink_back_mode)
		grid_update(ti,False);
	/* Since NULL is a special case for icon_list_froall_from,
	 * avoid calling it for the last icon */
	if(ti->next!=NULL)
		return (icon_list_forall_from(ti->next,&grid_add_wrapper)==NULL);
	else
		return SUCCESS;
}

/* C. Grid manipulations */
int grid_update(struct TrayIcon *ti,int sort)
{
	icon_list_forall_from(ti,&layout_translate_to_window);
	if(sort) icon_list_sort(&trayicon_cmp_func);
	/* recalculate minimal size */
	grid_sz.x=0;
	grid_sz.y=0;
	icon_list_forall(&grid_recalc_size);
	return SUCCESS;
}

/* D. Placement functions */
int grid_place_icon(struct TrayIcon *ti,struct IconPlacement *ip)
{
	struct Layout *l=&ti->l;
	struct Point *p=&ip->pos;
	/* Set the flag if icon position was really updated */
	ti->is_updated=ti->is_updated || (p->x!=l->grd_rect.x || p->y!=l->grd_rect.y);
	if(ti->is_updated || !ti->is_layed_out)
		{
			l->grd_rect.x=p->x;
			l->grd_rect.y=p->y;
		}
	/* Update grid dimensions */
	grid_sz.x += ip->sz_delta.x;
	grid_sz.y += ip->sz_delta.y;
	/* Update icon position */
	layout_translate_to_window(ti);
	return ti->is_updated;
}

/* Oh,the wonders of languages without closures... */
static struct Rect chk_rect;

/* Check if grid rect of ti intersects with chk_rect */
int find_obstacle(struct TrayIcon *ti)
{
	return ti->is_layed_out && (RECTS_ISECT(chk_rect,ti->l.grd_rect) || RECTS_ISECT(ti->l.grd_rect,chk_rect));
}

/* Check if grid rect of ti intersects with any other tray icon
 * grid rect and return its width or height,depending on tray orientation */
int grid_check_rect_free(int x,int y,int w,int h)
{
	struct TrayIcon *obst;
	chk_rect.x=x;
	chk_rect.y=y;
	chk_rect.w=w;
	chk_rect.h=h;
	obst=icon_list_advanced_find(&find_obstacle);
	if(obst==NULL)
		{
			return 0;
		}
	else
		{
			if(settings.vertical)
				return obst->l.grd_rect.h;
			else
				return obst->l.grd_rect.w;
		}
}

/* Simple function to fill in fields of IconPlacement struct
 * 	x,y: position of the icon
 * 	rect: provides the size of the icon
 * 	return value: returns true if the placement is valid; returns false
 * otherwise.
 */
int icon_placement_create(struct IconPlacement *ip,int x,int y,struct Rect *rect)
{
#define max_layout_width ((settings.vertical ? settings.max_tray_dims.y : settings.max_tray_dims.x) / settings.slot_size.x)
#define max_layout_height ((settings.vertical ? settings.max_tray_dims.x : settings.max_tray_dims.y) / settings.slot_size.y)
	ip->valid=0;
	ip->pos.x=x;
	ip->pos.y=y;
	ip->sz_delta.x=x+rect->w-grid_sz.x;
	ip->sz_delta.y=y+rect->h-grid_sz.y;
	ip->sz_delta.x=ip->sz_delta.x > 0 ? ip->sz_delta.x : 0;
	ip->sz_delta.y=ip->sz_delta.y > 0 ? ip->sz_delta.y : 0;
	ip->valid=(ip->pos.x+rect->w <= max_layout_width) && (ip->pos.y+rect->h <= max_layout_height);

	return ip->valid;
}

/* Compare points in lexographical order */
#define compare_points(a,b) (((a).y<(b).y) || ((a).y==(b).y && (a).x<(b).x))

/* Choose best placement --- placement policy is defined here
 * Placement A is considered to be better than placement B
 * if either
 * 	- min_space_policy is on and (window) size delta for A is
 * 	  strictly less then size delta for B;
 * 	- position for A is less (in lexographical order)
 * 	  than position for B.
 * 	  */
void icon_placement_choose_best(struct IconPlacement *old,struct IconPlacement *newst)
{
	int old_lout_delta_norm,new_lout_delta_norm;
	int lout_norm_cmp;
	struct Point new_wnd_sz_delta,old_wnd_sz_delta;
	int old_wnd_delta_norm=0,new_wnd_delta_norm=0;
	int wnd_norm_cmp=0;
	/* I whish there were nested functions in C standard */
#define calc_wnd_sz_delta(delta,pmt) \
    do { \
        delta.x=cutoff((grid_sz.x+pmt->sz_delta.x) * settings.slot_size.x -tray_data.xsh.width,0,pmt->sz_delta.x*settings.slot_size.x); \
		delta.y=cutoff((grid_sz.y+pmt->sz_delta.y) * settings.slot_size.y-tray_data.xsh.height,0,pmt->sz_delta.y*settings.slot_size.y); \
    } while(0)
	/* If tray is vertically oriented,swap width <-> height
	 * (just for readability sake) */
	if(settings.vertical)
		{
			swap(tray_data.xsh.width,tray_data.xsh.height);
			swap(settings.orig_tray_dims.x,settings.orig_tray_dims.y);
		}
	calc_wnd_sz_delta(old_wnd_sz_delta,old);
	calc_wnd_sz_delta(new_wnd_sz_delta,newst);
	/* Some black magic. This is probably buggy and you are not supposed to
	 * understand this (I definetly don't). The basic idea is that sometimes
	 * layout resize means window resize. Sometimes it does not. */
	if(settings.shrink_back_mode)
		{
			if(grid_sz.x >= settings.orig_tray_dims.x/settings.slot_size.x)
				{
					old_wnd_sz_delta.x=old->sz_delta.x*settings.slot_size.x;
					new_wnd_sz_delta.x=newst->sz_delta.x *settings.slot_size.x;
				}
			if(grid_sz.y >= settings.orig_tray_dims.y/settings.slot_size.y)
				{
					old_wnd_sz_delta.y=old->sz_delta.y*settings.slot_size.y;
					new_wnd_sz_delta.y=newst->sz_delta.y *settings.slot_size.y;
				}
		}
	/* Restore values */
	if(settings.vertical)
		{
			swap(tray_data.xsh.width,tray_data.xsh.height);
			swap(settings.orig_tray_dims.x,settings.orig_tray_dims.y);
		}
	/* Compute norms for window deltas */
	old_wnd_delta_norm=old_wnd_sz_delta.x+old_wnd_sz_delta.y;
	new_wnd_delta_norm=new_wnd_sz_delta.x+new_wnd_sz_delta.y;
	wnd_norm_cmp=new_wnd_delta_norm<old_wnd_delta_norm;
	/* Compute norms for layout deltas */
	old_lout_delta_norm=old->sz_delta.x+old->sz_delta.y;
	new_lout_delta_norm=newst->sz_delta.x+newst->sz_delta.y;
	lout_norm_cmp=new_lout_delta_norm<old_lout_delta_norm;

	/* CORE of placement logic. In short,
	 *-valid placement is always better than invalid
	 *-with minimal space policy on (off by default),placement with smaller
	 * deltas is better
	 *-placement which does not cause tray window to grow is always better
	 * than one that does
	 *-otherwise,placement that is "closer" to initial point (that depends
	 * on gravity) is better */
	if(!old->valid && newst->valid) goto replace;
	if(settings.min_space_policy && (lout_norm_cmp || wnd_norm_cmp))
		goto replace;
	if(old_lout_delta_norm && !new_lout_delta_norm) goto replace;
	if(old_wnd_delta_norm && !new_wnd_delta_norm) goto replace;
	if((!old_lout_delta_norm==!new_lout_delta_norm) && compare_points(newst->pos,old->pos))
		goto replace;
	return;
replace:
	*old=*newst;
}

/* WARNING: returns ptr to STATIC buffer */
struct IconPlacement *grid_find_placement(struct TrayIcon *ti)
{
	static struct IconPlacement cur_pmt;
	struct IconPlacement tmp_pmt;
	int x=0,y=0,skip,orig_layed_out;
	orig_layed_out=ti->is_layed_out;
	ti->is_layed_out=0; /* to avoid self-intersections */
	/* General idea is to look through all possible placements
	 * while keeping one which is considered to be the "best" and
	 * comparing current placement with "best" one and updating
	 * it accordingly */
	if(grid_sz.x==0 || grid_sz.y==0)   /* Grid is empty */
		{
			/* This is the only possible placement */
			icon_placement_create(&cur_pmt,0,0,&ti->l.grd_rect);
		}
	else     /* Seek and place */
		{
			/* Create two obvious placements */
			/* Rightmost-top position */
			icon_placement_create(&cur_pmt,grid_sz.x,0,&ti->l.grd_rect);
			/* Bottommost-left position */
			icon_placement_create(&tmp_pmt,0,grid_sz.y,&ti->l.grd_rect);
			/* Choose the best one */
			icon_placement_choose_best(&cur_pmt,&tmp_pmt);
			/* Start looking through possible placements.
			 * Current placement is determined by x and y variables. */
			do
				{
					/* Check if the current rect is free */
					skip=grid_check_rect_free(x,y,ti->l.grd_rect.w,ti->l.grd_rect.h);
					/* If so,create new placement */
					if(skip==0)
						{
							if(icon_placement_create(&tmp_pmt,x,y,&ti->l.grd_rect))
								{
									icon_placement_choose_best(&cur_pmt,&tmp_pmt);
									/* If not settings.full_pmt_search,take the first valid
									 * non-resizing placement and run */
									if(!settings.full_pmt_search && !cur_pmt.sz_delta.x && !cur_pmt.sz_delta.y)
										break;
								}
							skip=ti->l.grd_rect.w;
						}
					/* Advance to the next possible placement */
					x+=skip;
					if(x>=grid_sz.x)
						{
							x=0;
							y++;
						}
				}
			while(x<grid_sz.x && y<grid_sz.y);
		}
	/* Restore layed out state */
	ti->is_layed_out=orig_layed_out;
	if(cur_pmt.valid)
		{
			return &cur_pmt;
		}
	else
		{
			return NULL;
		}
}

/* E. Mass-ops helpers */

int trayicon_cmp_func(struct TrayIcon *ti1,struct TrayIcon *ti2)
{
	return (!ti2->is_layed_out && ti1->is_layed_out) || compare_points(ti2->l.grd_rect,ti1->l.grd_rect);
}

int grid_add_wrapper(struct TrayIcon *ti)
{
	/* Ignore hidden icons */
	return !ti->is_visible || grid_add(ti) ? NO_MATCH : MATCH;
}

int grid_recalc_size(struct TrayIcon *ti)
{
	int x,y;
	/* Ignore icons that are not layed out */
	if(!ti->is_layed_out) return NO_MATCH;
	/* Calculate coordinates for bottom-right corner of ti */
	x=ti->l.grd_rect.x+ti->l.grd_rect.w;
	y=ti->l.grd_rect.y+ti->l.grd_rect.h;
	/* Update grid dimensions */
	grid_sz.x=x > grid_sz.x ? x : grid_sz.x;
	grid_sz.y=y > grid_sz.y ? y : grid_sz.y;
	return NO_MATCH;
}

int layout_unset_flag(struct TrayIcon *ti)
{
	ti->is_layed_out=0;
	return NO_MATCH;
}
