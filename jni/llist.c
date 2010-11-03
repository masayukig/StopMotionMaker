/*
  llist.c

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


#include "llist.h"
#include <stdlib.h>


/*
  spc: pulling l from list and deleting it from heap
  pre: l != 0
  pos: data elements still allocated; returning next
*/

List *list_erase(List *l)
{
  List *next = l->next;

  if (l->prev)
    l->prev->next = next;
  if (next)
    l->next->prev = l->prev;

  free(l);

  return next;
}


/*
  spc: pulling l from list, deleting it and its data
  pre: l != 0
  pos: returning l->next
*/

List *list_erased(List *l)
{
  if (l->data) {
    free(l->data);
    l->data = 0;
  }
  return list_erase(l);
}


/*
  spc: freeing all list structures
  pre: l != 0
  pos: data elements still allocated
*/

void list_rerase(List *l)
{
  if (l->next)
    list_rerase(l->next);
  l->next = 0;

  free(l);
}


/*
  spc: freeing all list structures and their data
  pre: l != 0
*/

void list_rerased(List *l)
{
  if (l->next)
    list_rerased(l->next);
  l->next = 0;

  if (l->data) {
    free(l->data);
    l->data = 0;
  }
  free(l);
}


/*
  spc: putting data in list at point l
*/

void list_insert(List *l, void *data)
{
  List *next = (List *) malloc(sizeof(List));
  next->data = l->data;
  next->next = l->next;
  next->prev = l;
  l->data = data;
  l->next = next;
}


/*
  spc: putting data on front of list
  pos: returning new front
*/

List *list_push_front(List *l, void *data)
{
  l = list_front(l);
  l->prev = (List *) malloc(sizeof(List));
  l->prev->data = data;
  l->prev->next = l;
  l->prev->prev = 0;

  return l->prev;
}


/*
  spc: putting data at end of list
  pos: returning new back
*/

List *list_push_back(List *l, void *data)
{
  l = list_back(l);
  l->next = (List *) malloc(sizeof(List));
  l->next->data = data;
  l->next->next = 0;
  l->next->prev = l;

  return l->next;
}


void list_pop_front(List *l)
{
  list_erase(list_front(l));
}


void list_pop_back(List *l)
{
  list_erase(list_back(l));
}


void list_popd_front(List *l)
{
  list_erased(list_front(l));
}


void list_popd_back(List *l)
{
  list_erased(list_back(l));
}


List *list_front(List *l)
{
  while (l->prev)
    l = l->prev;
  return l;
}


List *list_back(List *l)
{
  while (l->next)
    l = l->next;
  return l;
}


/*
  spc: removing elements l having l.data == data
  pos: data is not deleted
*/

void list_remove(List *l, void *data)
{
  List *l2;
  while ((l2 = list_find(l, data)))
    l = list_erase(l2); 
}


/*
  spc: finding next element l having l.data == data
*/

List *list_find(List *l, void *data)
{
  while (l && (l->data != data))
    l = l->next;
  return l;
}


/*
  spc: indexing l as an array
  pos: returning nth element, or 0 if out of range
*/

List *list_nth(List *l, unsigned int n) 
{
  int i;
  for (i = 0; (i < n) && (l); ++i, l = l->next); 
  return l;
}


unsigned int list_size(List *l)
{
  int i;
  for (i = 0; (l); ++i, l = l->next); 
  return i;
}

