#ifndef __LDIGRAPH_HELPERS_H__
#define __LDIGRAPH_HELPERS_H__

struct ldigraph
{
  size_t n;          // the number of vertices
  size_t *list_size; // the size of each adjacency list
  size_t *list_cap;  // the capacity of each adjacency list
  int **adj;      // the adjacency lists
};

typedef struct
{
  const ldigraph *g; // the graph that was searched
  int from; // the starting vertex of the search (if applicable)
  int *color; // current status of each vertex (using enum below)
  int *dist; // number of edges on the path that was found to each vertex
  int *pred; // predecessor along the path that was found (won't be needed)
  int *finish; // vertices that are marked DONE in reverse order if finish
  int count; // the number of vertices that have been found so far
  bool found_cycle;  // a flag indicating whether we have seen a cycle
  // YOU CAN ADD MORE THINGS HERE!
} ldigraph_search;

enum {LDIGRAPH_UNSEEN, LDIGRAPH_PROCESSING, LDIGRAPH_DONE};

#define LDIGRAPH_ADJ_LIST_INITIAL_CAPACITY 4
#define NO_PATH -1

/**
 * Returns the result of running breadth-first search on the given
 * graph starting with the given vertex.  When the search arrives
 * at a vertex, its neighbors are considered in the order the
 * corresponding edges were added to the graph.
 * It is the caller's responsibility to destroy to result.
 *
 * @param g a pointer to a directed graph, non-NULL
 * @param from the index of a vertex in the given graph
 * @return the result of the search
 */
ldigraph_search *ldigraph_bfs(const ldigraph *g, int from);

/*
 * Personally made a recursive function for ldigraph_longest() in cyclic graphs
 */
int brute_force(const ldigraph* g, int v, bool* visited, int path_length, int to);

/**
 * Returns the result of running depth-first search on the given
 * graph starting with the given vertex.  When the search arrives
 * at a vertex, its neighbors are considered in the order the
 * corresponding edges were added to the graph.
 * It is the caller's responsibility to destroy to result.
 *
 * @param g a pointer to a directed graph, non-NULL
 * @param from the index of a vertex in the given graph
 * @return the result of the search
 */
ldigraph_search *ldigraph_dfs(const ldigraph *g, int from);


/**
 * Returns the result of running depth-first search on the given
 * graph.  Each connected component is searched from an arbitrarily
 * selected starting point.  When the search arrives
 * at a vertex, its neighbors are considered in the order the
 * corresponding edges were added to the graph.
 * It is the caller's responsibility to destroy to result.
 *
 * @param g a pointer to a directed graph, non-NULL
 * @return the result of the search
 */
ldigraph_search *ldigraph_dfs_with_restart(const ldigraph *g);

/**
 * Visits the given vertex in the given search of the given graph.
 *
 * @param g a pointer to a directed graph
 * @param s a search in that graph
 * @param from a vertex in that graph
 */
void ldigraph_dfs_visit(const ldigraph* g, ldigraph_search *s, int from);


/**
 * Resizes the adjacency list for the given vertex in the given graph.
 * 
 * @param g a pointer to a directed graph
 * @param from the index of a vertex in that graph
 */
void ldigraph_list_embiggen(ldigraph *g, int from);


/**
 * Prepares a search result for the given graph starting from the given
 * vertex.  It is the responsibility of the caller to destroy the result.
 *
 * @param g a pointer to a directed graph
 * @param from the index of a vertex in that graph
 * @return a pointer to a search result
 */
ldigraph_search *ldigraph_search_create(const ldigraph *g, int from);


/**
 * Initializes a search result that has its associated graph set and
 * has space allocated for its arrays.
 *
 * @param s a pointer to a search result, non-NULL
 * @param from the index of a vertex in the graph s holds a search result from
 */
void ldigraph_search_init(ldigraph_search *s, int from);


/**
 * Destroys the given search result.
 *
 * @param s a pointer to a search result, non-NULL
 */
void ldigraph_search_destroy(ldigraph_search *s);

#endif