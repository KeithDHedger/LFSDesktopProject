/*
 *
 * ©K. D. Hedger. Mon 13 Jan 16:29:14 GMT 2025 keithdhedger@gmail.com

 * This file (list.h) is part of LFSTray.

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
 * list.h
 * Fri,10 Sep 2004 23:34:48 +0700
 * -------------------------------
 * Simple double linked list
 * -------------------------------*/

#ifndef _LIST_H_
#define _LIST_H_

/* Implement basic functions for doubly-linked list.
 * Each structure to hold an element of the list must have two
 * fields pointer fields: prev and next. */

/* Add item i_ to the head of the list h_ (pointer to the head) */
#define LIST_ADD_ITEM(h_,i_) \
    do { \
        (i_)->prev = NULL; \
        if ((h_) != NULL) { \
            (i_)->next = (h_); \
            (h_)->prev = (i_); \
        } else { \
            (i_)->next = NULL; \
        } \
        (h_) = (i_); \
    }while (0)

/* Insert item i_ after item t_ to the list h_ (pointer to the head) */
#define LIST_INSERT_AFTER(h_,t_,i_) \
    do { \
        (i_)->prev = (t_); \
        if ((t_) != NULL) { \
            (i_)->next = (t_)->next; \
            (t_)->next = (i_); \
        } else { \
            if ((h_) != NULL) { \
                (i_)->next = (h_); \
                (h_)->prev = (i_); \
            } else { \
                (i_)->next = NULL; \
            } \
            (h_) = (i_); \
        } \
    }while (0)

/* Delete item i_ from the list h_ (pointer to the head) */
#define LIST_DEL_ITEM(h_,i_) \
    do { \
        if (i_->prev != NULL) i_->prev->next = i_->next; \
        if (i_->next != NULL) i_->next->prev = i_->prev; \
        if (i_ == h_) h_ = i_->next; \
    }while (0)

/* Clean the list h_ (pointer to the head);
 * tmp_ is a temporary variable */
#define LIST_CLEAN(h_,tmp_) \
    do { \
        for (tmp_ = h_,h_ = (h_ != NULL) ? h_->next : NULL; tmp_ != NULL; \
             tmp_ = h_,h_ = h_ != NULL ? h_->next : NULL) { \
            free(tmp_); \
        } \
        h_ = NULL; \
    }while (0)

/* Clean the list h_ (pointer to the head) calling cbk_ for every
 * element of the list; tmp_ is a temporary variable */
#define LIST_CLEAN_CBK(h_,tmp_,cbk_) \
    do { \
        for (tmp_ = h_,h_ = (h_ != NULL) ? h_->next : NULL; tmp_ != NULL; \
             tmp_ = h_,h_ = h_ != NULL ? h_->next : NULL) { \
            cbk_(tmp_); \
            free(tmp_); \
        } \
        h_ = NULL; \
    }while (0)

#endif
