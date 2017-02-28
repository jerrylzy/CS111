//
//  SortedList.c
//  Lab2B
//
//  Created by Jerry Liu on 2/12/17.
//  Copyright © 2017 Jerry Liu. All rights reserved.
//

#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "SortedList.h"

void SortedList_insert(SortedList_t *list, SortedListElement_t *element) {
    if (list == NULL || element == NULL)
        return;
    SortedList_t *prev = list;
    SortedList_t *cur = prev->next;
    if (prev->key != NULL) //  *list is not the head
        return;
    while (cur != list) {
        if (strcmp(cur->key, element->key) < 0)
            break;
        prev = cur;
        cur = cur->next;
    }
    //  may reach the end of the sorted list, largest, insert at the end
    if (opt_yield & INSERT_YIELD)
        pthread_yield();

    //   prev       element      next
    // |      | <- | prev | <- | prev |
    // | next | -> | next | -> |      |
    element->next = prev->next;
    element->prev = prev;
    prev->next = element;
    element->next->prev = element;
}

int SortedList_delete(SortedListElement_t *element) {
    if (element == NULL || element->key == NULL)
        return 1;  //  Cannot delete nothing or the head
    if (element->prev->next != element || element->next->prev != element)
        return 1;  //  List is corrupted

    if (opt_yield & DELETE_YIELD)
        pthread_yield();

    //  delete node
    element->prev->next = element->next;
    element->next->prev = element->prev;
    return 0;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key) {
    if (list == NULL || list->key != NULL)
        return NULL;  //  No head or list doesn't have a head
    SortedListElement_t *cur = list->next;
    
    while (cur != list) {
        if ( ! strcmp(key, cur->key))
            return cur;  //  Find the element
        if (opt_yield & LOOKUP_YIELD)
            pthread_yield();
        cur = cur->next;
    }
    return NULL;  //  Find nothing
}

int SortedList_length(SortedList_t *list) {
    if (list == NULL || list->key != NULL)
        return -1;  //  No list or list doesn't have a head

    int len = 0;
    SortedList_t *cur = list->next;
    while (cur != list) {
        if (cur->prev->next != cur || cur->next->prev != cur)
            return -1;   //  List is corrupted
        len++;
        if (opt_yield & LOOKUP_YIELD)
            pthread_yield();
        cur = cur->next;
    }
    return len;
}
