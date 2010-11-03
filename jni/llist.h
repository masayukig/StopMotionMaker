/*
  llist.h

  routines for doubly-linked list

  Copyright (C) 2003 Phillip Bruce <dI77IHd@yahoo.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/


#ifndef _LLIST_H_
#define _LLIST_H_


typedef struct _List List;


struct _List
{
  void *data;
  List *next;
  List *prev;
};


/*
  n.b. I've loosely followed the C++/STL naming conventions

  by definition, the back is the List l s.t. l.next == 0
                 the front is the List l s.t. l.prev == 0


  all functions assuming l to be a valid pointer

  in general, there exist "d" version to destroyer functions
  that also free data
*/

List        *list_erase      (List *l);
List        *list_erased     (List *l);
void         list_rerase     (List *l);
void         list_rerased    (List *l);
void         list_insert     (List *l, void *data);
List        *list_push_front (List *l, void *data);
List        *list_push_back  (List *l, void *data);
void         list_pop_front  (List *l);
void         list_pop_back   (List *l);
void         list_popd_front (List *l);
void         list_popd_back  (List *l);
List        *list_front      (List *l);
List        *list_back       (List *l);

void         list_remove     (List *l, void *data);
List        *list_find       (List *l, void *data);
List        *list_nth        (List *l, unsigned int n);
unsigned int list_size       (List *l);



#endif
