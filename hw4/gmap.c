/* CPSC223 Fall 2022 hw4 
 * This file implements the specific functions required for the gmap ADT
 */

#include "gmap.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

char *gmap_error = "error";

// Nodes to implement chains as singly-linked lists. //
typedef struct _node
{
  void* key;
  void* value;
  struct _node* next;
} node;

// meta struct of the map
struct _gmap{
    // array of key-value nodes
    node** table;  
    // capacity of the table (max number of chains)
    size_t cap;
    // number of key-value pairs in the table
    size_t nkey;
    // function pointers :
    void* (*copier)(const void *);
    int (*comparer)(const void *, const void *);
    size_t (*hasher)(const void *);
    void (*freer)(void *);
};

#define SIMAP_INITIAL_CAPACITY 101

// helper function declarations
node* gmap_find_key(const gmap* m, const char* key);
void gmap_embiggen(gmap* m);
/**
 * A location in an array where a key can be stored.  The location is
 * represented by a (array, index) pair.
 */
typedef struct _gmap_store_location
{
  const void **arr;
  size_t index;
} gmap_store_location;

void gmap_store_key_in_array(const void *key, void *value, void *arg)
{
  gmap_store_location *where = arg;
  where->arr[where->index] = key;
  where->index++;
}

// =================================================================================================================
// Required Function Implementations
// =================================================================================================================
/**
 * Creates an empty map that uses the given hash function.
 *
 * @param cp a function that take a pointer to a key and returns a pointer to a deep copy of that key
 * @param comp a pointer to a function that takes two keys and returns the result of comparing them,
 * with return value as for strcmp
 * @param h a pointer to a function that takes a pointer to a key and returns its hash code
 * @param f a pointer to a function that takes a pointer to a copy of a key make by cp and frees it
 * @return a pointer to the new map or NULL if it could not be created;
 * it is the caller's responsibility to destroy the map
 */
gmap *gmap_create(void *(*cp)(const void *), int (*comp)(const void *, const void *), size_t (*h)(const void *s), void (*f)(void *))
{
    if (h == NULL || cp == NULL || comp == NULL || f == NULL)
    {
      // one of the required functions was missing
      return NULL;
    }

    // pointer to a new map
    struct _gmap* newmap = malloc(sizeof(*newmap));
    if (newmap != NULL) // fill in newmap and return a pointer to it
    {
        newmap->table = calloc(SIMAP_INITIAL_CAPACITY, sizeof(node*));
        newmap->cap = SIMAP_INITIAL_CAPACITY;
        newmap->nkey = 0;
        newmap->copier = cp;
        newmap->comparer = comp;
        newmap->hasher = h;
        newmap->freer = f;
    }

    // malloc will return NULL if map creation fails
    return newmap;
}


/**
 * Returns the number of (key, value) pairs in the given map.
 *
 * @param m a pointer to a map, non-NULL
 * @return the size of the map pointed to by m
 */
size_t gmap_size(const gmap *m)
{
    if (m == NULL)
        return 0;
    return m->nkey;
}


/**
 * Adds a copy of the given key with value to this map.  If the key is
 * already present then the old value is replaced and returned.  The
 * map copies the key, so the caller retains ownership of the original
 * key and may modify it or destroy it without affecting the map.  The
 * map copies the pointer to the value, but the caller retains
 * ownership of the value.
 *
 * @param m a pointer to a map, non-NULL
 * @param key a pointer to a key, non-NULL
 * @param value a pointer to a value
 * @return a pointer to the old value, or NULL, or a pointer to gmap_error
 */
void *gmap_put(gmap *m, const void *key, void *value)
{
    if (m == NULL || key == NULL)
        return NULL;

    // resize table if load factor == 1
    if (m->nkey == m->cap)
        gmap_embiggen(m);
    
    // add key-value pair
    node* thisnode = gmap_find_key(m, key);
    if(thisnode != NULL)
    {
        // key is already present
        // update value in the key-value pair
        void* oldvalue = thisnode->value;
        thisnode->value = value;
        return oldvalue;
    }
    else
    {
        // key was not present, must add a new node
        // copy the key
        void* keycopy;
        keycopy = m->copier(key);

        if (keycopy != NULL)
        {
            // create a new node
            node* newnode = malloc(sizeof(node));
            newnode->key = keycopy;
            newnode->value = value;
            newnode->next = NULL;
            
            // find place to add
            size_t index = m->hasher(keycopy) % m->cap;

            // add to the chain's head
            newnode->next = m->table[index];
            m->table[index] = newnode;

            // update counter
            m->nkey++;
            return NULL;
        }
        else
        {
            return NULL;
        }
    }
    
}

/**
 * Removes the given key and its associated value from the given map if
 * the key is present.  The return value is NULL and there is no effect
 * on the map if the key is not present.  The copy of the key held by
 * the map is destroyed.  It is the caller's responsibility to free the
 * returned value if necessary.
 *
 * @param m a pointer to a map, non-NULL
 * @param key a key, non-NULL
 * @return the value associated with the removed key, or NULL
 */
void *gmap_remove(gmap *m, const void *key)
{
    if (m == NULL || key == NULL)
        return NULL;
    
    node* prev=NULL;
    node* curr=NULL;
    node* thisnode=NULL;
    void* result=NULL;

    // find the hash index
    size_t index = m->hasher(key) % m->cap;
    
    // traverse through that chain until
    // 1) found the key or 2) end of chain (i.e. curr->next == NULL)
    curr = m->table[index];
    while(curr != NULL)
    {
        // check this node's key
        if(m->comparer(key, curr->key) == 0)
        {
            thisnode = curr;
            // update prev node's pointer
            if(prev != NULL) prev->next = curr->next;
            else m->table[index] = curr->next;

            result = thisnode->value;

            // free memories associated with node
            m->freer(thisnode->key);
            free(thisnode);

            // update counter
            m->nkey--;
            break;
        }
        prev = curr;
        curr = curr->next;
    }
    return result;
}


/**
 * Determines if the given key is present in this map.
 *
 * @param m a pointer to a map, non-NULL
 * @param key a pointer to a key, non-NULL
 * @return true if a key equal to the one pointed to is present in this map,
 * false otherwise
 */
bool gmap_contains_key(const gmap *m, const void *key)
{
    if(m == NULL || key == NULL)
        return false;
    
    return gmap_find_key(m, key) != NULL;
}


/**
 * Returns the value associated with the given key in this map.
 * If the key is not present in this map then the returned value is
 * NULL.  The pointer returned is the original pointer passed to gmap_put,
 * and it remains the responsibility of whatever called gmap_put to
 * release the value it points to (no ownership transfer results from
 * gmap_get).
 *
 * @param m a pointer to a map, non-NULL
 * @param key a pointer to a key, non-NULL
 * @return a pointer to the assocated value, or NULL if they key is not present
 */
void *gmap_get(gmap *m, const void *key)
{
    if (m == NULL || key == NULL)
      return NULL;

    node* thisnode = gmap_find_key(m, key);
    if(thisnode != NULL)
        return thisnode->value;
    else
        return NULL;
}


/**
 * Calls the given function for each (key, value) pair in this map, passing
 * the extra argument as well.
 *
 * @param m a pointer to a map, non-NULL
 * @param f a pointer to a function that takes a key, a value, and an
 * extra piece of information, and does not add or remove keys from the
 * map, non-NULL
 * @param arg a pointer
 */
void gmap_for_each(gmap *m, void (*f)(const void *, void *, void *), void *arg)
{
    if (m == NULL || f == NULL)
      return;
    
    for(int i = 0; i < m->cap; i++)
    {
        node* curr = m->table[i];
        while(curr != NULL)
        {
            f(curr->key, curr->value, arg);
            curr = curr->next;
        }
    }
    return;
}


/**
 * Returns an array containing pointers to all of the keys in the
 * given map.  The return value is NULL if there was an error
 * allocating the array.  The map retains ownership of the keys, and
 * the pointers to them are only valid as long until they are removed
 * from the map, or until the map is destroyed, whichever comes first.
 * It is the caller's responsibility to destroy the returned array if
 * it is non-NULL.
 *
 * @param m a pointer to a map, non-NULL
 * @return a pointer to an array of pointers to the keys, or NULL
 */
// used the optional code gmap_array.c
const void **gmap_keys(gmap *m)
{
    if (m == NULL) return NULL;

    const void **keys = malloc(sizeof(*keys) * m->nkey);

    if (keys != NULL)
    {
        gmap_store_location loc = {keys, 0};
        gmap_for_each(m, gmap_store_key_in_array, &loc);
    }
    return keys;
}

/**
 * Destroys the given map.  There is no effect if the given pointer is NULL.
 *
 * @param m a pointer to a map, or NULL
 */
void gmap_destroy(gmap *m)
{
    if(m != NULL)
    {
        // free all nodes and their keys
        for(int i = 0; i < m->cap; i++)
        {
            node* curr = m->table[i];
            node* aux;
            while(curr != NULL)
            {
                aux = curr;
                if(curr->key != NULL) m->freer(curr->key); // free key of the node
                // if(curr->value != NULL) m->freer(curr->value); // free value of the node
                curr = curr->next;
                free(aux); // free node
            }
        }
        // free the table
        free(m->table);

        // free the gmap struct
        free(m);
        m = NULL;
    }
    return;
}

// =================================================================================================================
// Helper Function Implementations
// =================================================================================================================

/**
 * Returns the node in the given chained hash table containing the given
 * key, or NULL if there is no such node.
 */
node* gmap_find_key(const gmap* m, const char* key)
{
    // we already account for this case in implementing functions, but just in case:
    if(m == NULL || key == NULL)
        return NULL;
        
    node* result = NULL;
    
    // find the hash index
    size_t index = m->hasher(key) % m->cap;
    
    // traverse through that chain until
    // 1) found the key or 2) end of chain (i.e. curr->next == NULL)
    node* curr = m->table[index];
    
    while(curr != NULL)
    {
        // check this node's key
        if(m->comparer(key, curr->key) == 0)
        {
            result = curr;
            break;
        }
        curr = curr->next;
    }
    return result;
}

/*
 * Resize gmap's table (to ~double the previous size)
 */
void gmap_embiggen(gmap* m)
{
    size_t oldcap = m->cap;
    // increase table size
    m->cap = (m->cap)*2 + 1; // not a prime, but good enough
    node** oldtable = m->table;
    m->table = calloc(m->cap, sizeof(node*));
    if(m->table == NULL) return;
    
    node* save=NULL;
    node* curr=NULL;

    // go through the table
    for(int i = 0; i < oldcap; i++)
    {
        // go through the chain
        curr = oldtable[i];
        while(curr != NULL)
        {
            save = curr->next;
            
            // put node('s pointer)s into new table
            size_t index = m->hasher(curr->key) % m->cap;
            
            // put into head of the chain and adjust next pointers
            curr->next = m->table[index];
            m->table[index] = curr;
            curr = save;
        }
    }

    free(oldtable);
    return;
}