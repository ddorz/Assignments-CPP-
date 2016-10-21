/* ======================================================================================================================= *
 * Dave Dorzback                                                                                                           *
 * dlUtils.h                                                                                                               *
 * ======================================================================================================================= */

#ifndef __DLNODE__
#define __DLNODE__

#include <iostream>
#include <cassert>

/*
 * dlNode - Template type for doubly linked list node
 */
template <class T>
struct dlNode {
    T info;
    dlNode<T> *prev;
    dlNode<T> *next;
    dlNode<T>(T val, dlNode<T> *p, dlNode<T> *n): info(val), prev(p), next(n) {};
};

/*
 * printDlList - Prints the info member for each node in doubly linked list along with a string parameter
 */
template <class T>
void printDlList(dlNode<T>* first, const char *sep) {
    
    for (dlNode<T> *cursor = first; cursor != NULL; cursor = cursor->next) {
        std::cout << cursor->info << (cursor->next ? sep : "\n");
    }
}

/*
 * insertAsFirst - Inserts a new node as the first node
 */
template <class T>
void insertAsFirst(dlNode<T>* &first, T newval) {
    
    first = new dlNode<T>(newval, NULL, first);
    first->next->prev = first;
}

/*
 * insertAfter - Inserts a new node after the current node
 */
template <class T>
void insertAfter(dlNode<T> *first, dlNode<T> *current, T newval) {
    
    assert(current != NULL);
    
    current->next = new dlNode<T>(newval, current, current->next);
    
    if ((current= current->next)->next != NULL) {
        current->next->prev = current;
    }
}

/*
 * insertBefore - Inserts a new node before the current node
 */
template <class T>
void insertBefore(dlNode<T>* &first, dlNode<T> *current, T newval) {
    
    assert(current != NULL);
    
    if (current == first) {
        insertAsFirst(first, newval);
    } else insertAfter(first, current->prev, newval);
}

/*
 * deleteNext - Deletes the next node from the list
 */
template <class T>
void deleteNext(dlNode<T> *current) {
    
    assert(current != NULL && current->next != NULL);
    
    dlNode<T> *hold = current->next;
    
    if ((current->next= hold->next) != NULL) {
        current->next->prev = current;
    }
    
    delete hold;
}

/*
 * deletePrevious - Deletes the previous node from the list
 */
template <class T>
void deletePrevious(dlNode<T> * &first,dlNode<T> *current) {
    
    assert(first != NULL && current != NULL && current->prev != NULL);
    
    dlNode<T> *hold = current->prev;
    
    if ((current->prev= hold->prev) != NULL) {
        current->prev->next = current;
    } else first = current;
    
    delete hold;
}

/*
 * deleteNode - Deletes the current node from the list
 */
template <class T>
void deleteNode(dlNode<T>* &first, dlNode<T>* current) {
    
    assert(first != NULL && current != NULL);
    
    dlNode<T> *hold = current;
    
    if (current == first) {
        first = first->next;
        first->prev = NULL;
        current = first;
    } else {
        current->prev->next = current->next;
        current->next->prev = current->prev;
        current = current->prev;
    }
    
    delete hold;
}

/*
 * reverse - Reverses the list
 */
template <class T>
void reverse(dlNode<T>* &first) {
    
    assert(first != NULL);
    
    dlNode<T> *tmp = NULL;
    for (dlNode<T> *current = first; current != NULL; current = current->prev) {
        tmp = current->prev;
        current->prev = current->next;
        current->next = tmp;
    }
    if (tmp != NULL) {
        first = tmp->prev;
    }
}

#endif
