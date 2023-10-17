/*
 * This file contains helper functions for gmap.c
 */

/**
 * Returns the node in the given chained hash table containing the given
 * key, or NULL if there is no such node.
 *
 * @param table a pointer to an array of num_chains heads of chains, non-NULL
 * @param key a pointer to a string, non-NULL
 * @param hash a pointer to a hash function for strings, non-NULL
 * @param num_chains the number of chains in the hash table
 * @return a pointer to a node containing key, or NULL
 */
node* simap_table_find_key(node **table, const char *key, size_t (*hash)(const char *), size_t num_chains);