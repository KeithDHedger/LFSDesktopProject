/*
 *
 * ©K. D. Hedger. Mon 13 Jan 16:22:11 GMT 2025 keithdhedger@gmail.com

 * This file (icons.c) is part of LFSTray.

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
 * icons.c
 * Tue,24 Aug 2004 12:05:38 +0700
 * -------------------------------
 * Manipulations with reparented
 * windows --- tray icons
 * -------------------------------*/

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <assert.h>

#include "icons.h"
#include "globals.h"
#include "layout.h"
#include "list.h"
#include "tray.h"


struct TrayIcon *icons_head=NULL;

struct TrayIcon *icon_list_new(Window wid,int cmode)
{
	struct TrayIcon *new_icon;
	/* Do not allocate second structure for the same window */
	if(icon_list_find(wid)!=NULL) return NULL;
	if((new_icon=(TrayIcon*)malloc(sizeof(struct TrayIcon)))==NULL)
		{
			return NULL;
		}
	new_icon->wid=wid;
	new_icon->l.wnd_sz.x=0;
	new_icon->l.wnd_sz.y=0;
	new_icon->mid_parent=None;
	new_icon->cmode=cmode;
	new_icon->is_embedded=False;
	new_icon->is_layed_out=False;
	new_icon->is_updated=False;
	new_icon->is_resized=True;
	new_icon->is_visible=False;
	new_icon->is_invalid=False;
	new_icon->is_xembed_supported=False;
	new_icon->is_size_set=False;
	new_icon->num_size_resets=0;
	LIST_ADD_ITEM(icons_head,new_icon);
	return new_icon;
}

int icon_list_free(struct TrayIcon *ti)
{
	if(ti!=NULL)
		{
			LIST_DEL_ITEM(icons_head,ti);
			free(ti);
		}
	return SUCCESS;
}

struct TrayIcon *icon_list_next(struct TrayIcon *ti)
{
	return (ti!=NULL && ti->next!=NULL) ? ti->next : icons_head;
}

struct TrayIcon *icon_list_prev(struct TrayIcon *ti)
{
	struct TrayIcon *tmp;
	if(ti!=NULL && ti->prev!=NULL)
		return ti->prev;
	else
		{
			tmp=icons_head;
			for(; tmp->next!=NULL; tmp=tmp->next);
			return tmp;
		}
}

static struct TrayIcon *backup_head=NULL;

int icon_list_backup()
{
	struct TrayIcon *tmp,*cur,*cur2;
	/* Refuse to perform second backup in a row */
	if(backup_head!=NULL)
		{
			fprintf(stderr,"Only one backup of icon list at a time is supported\n");
			exit(-1);
		}
	/* For each icon in the list we allocate new temporary structure and add it
	 * to the end of temporary list backup_head */
	for(cur=icons_head,cur2=NULL; cur!=NULL;
	        cur=cur->next,cur2=tmp)
		{
			tmp=(struct TrayIcon *)malloc(sizeof(struct TrayIcon));
			if(tmp==NULL)
				{
					icon_list_backup_purge();
					return FAILURE;
				}
			memcpy(tmp,cur,sizeof(struct TrayIcon));
			LIST_INSERT_AFTER(backup_head,cur2,tmp);
			cur2=tmp;
		}
	return SUCCESS;
}

int icon_list_restore()
{
	struct TrayIcon *cur_b,*cur_i,*prev_sv,*next_sv;
	// LOG_TRACE(("restoring the icon list from the backup\n"));
	/* Restore the list by copying raw data from
	 * backup list. This assumes that sequences have the
	 * same length. */
	for(cur_i=icons_head,cur_b=backup_head;
	        cur_i!=NULL && cur_b!=NULL;
	        cur_i=cur_i->next,cur_b=cur_b->next)
		{
			prev_sv=cur_i->prev;
			next_sv=cur_i->next;
			memcpy(cur_i,cur_b,sizeof(struct TrayIcon));
			cur_i->prev=prev_sv;
			cur_i->next=next_sv;
		}
	/* Some consistency checking: ensures that
	 * both lists had the same length */
	assert(cur_i==NULL && cur_b==NULL);
	/* Clean backup list */
	LIST_CLEAN(backup_head,cur_b);
	backup_head=NULL;
	return SUCCESS;
}

int icon_list_backup_purge()
{
	struct TrayIcon *tmp;
	/* Clean backup list */
	LIST_CLEAN(backup_head,tmp);
	backup_head=NULL;
	return SUCCESS;
}

struct TrayIcon *icon_list_find(Window wid)
{
	/* Traverse the whole list */
	struct TrayIcon *tmp;
	for(tmp=icons_head; tmp!=NULL; tmp=tmp->next)
		if(tmp->wid==wid) return tmp;
	return NULL;
}

struct TrayIcon *icon_list_find_ex(Window wid)
{
	/* Traverse the whole list */
	struct TrayIcon *tmp;
	for(tmp=icons_head; tmp!=NULL; tmp=tmp->next)
		if(tmp->wid==wid || tmp->mid_parent==wid) return tmp;
	return NULL;
}

int icon_list_clean()
{
	struct TrayIcon *tmp;
	LIST_CLEAN(icons_head,tmp);
	return SUCCESS;
}

int icon_list_clean_callback(IconCallbackFunc cbk)
{
	struct TrayIcon *tmp;
	LIST_CLEAN_CBK(icons_head,tmp,cbk);
	return SUCCESS;
}

/* TODO: is it necessary always to sort the full list? */
void icon_list_sort(IconCmpFunc cmp)
{
	struct TrayIcon *new_head=NULL,*cur,*tmp;
	while(icons_head!=NULL)
		{
			/* Find the least element and move it to temporary list */
			cur=icons_head;
			for(tmp=icons_head; tmp!=NULL; tmp=tmp->next)
				if(cmp(tmp,cur) > 0) cur=tmp;
			LIST_DEL_ITEM(icons_head,cur);
			LIST_ADD_ITEM(new_head,cur);
		}
	icons_head=new_head;
}

struct TrayIcon *icon_list_forall(IconCallbackFunc cbk)
{
	return icon_list_forall_from(icons_head,cbk);
}

struct TrayIcon *icon_list_forall_from(
    struct TrayIcon *tgt,IconCallbackFunc cbk)
{
	/* Traverse the list starting from tgt*/
	struct TrayIcon *tmp;
	for(tmp=(tgt!=NULL ? tgt : icons_head); tmp!=NULL; tmp=tmp->next)
		if(cbk(tmp)==MATCH)
			{
				return tmp;
			}
	return NULL;
}
