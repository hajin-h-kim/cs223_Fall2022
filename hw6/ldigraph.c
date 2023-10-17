#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "ldigraph.h"
#include "ldigraph_helpers.h"

ldigraph *ldigraph_create(size_t n)
{
  if (n < 1)
    {
      return NULL;
    }
  
  ldigraph *g = malloc(sizeof(ldigraph));
  if (g != NULL)
    {
      g->n = n;
      g->list_size = malloc(sizeof(size_t) * n);
      g->list_cap = malloc(sizeof(size_t) * n);
      g->adj = malloc(sizeof(int *) * n);
      
      if (g->list_size == NULL || g->list_cap == NULL || g->adj == NULL)
	{
	  free(g->list_size);
	  free(g->list_cap);
	  free(g->adj);
	  free(g);

	  return NULL;
	}

      for (int i = 0; i < n; i++)
	{
	  g->list_size[i] = 0;
	  g->adj[i] = malloc(sizeof(int) * LDIGRAPH_ADJ_LIST_INITIAL_CAPACITY);
	  g->list_cap[i] = g->adj[i] != NULL ? LDIGRAPH_ADJ_LIST_INITIAL_CAPACITY : 0;
	}
    }

  return g;
}


size_t ldigraph_size(const ldigraph *g)
{
  if (g != NULL)
    {
      return g->n;
    }
  else
    {
      return 0;
    }
}


void ldigraph_add_edge(ldigraph *g, int from, int to)
{
  if (g != NULL && from >= 0 && to >= 0 && from < g->n && to < g->n && from != to)
    {
      // make room if necessary
      if (g->list_size[from] == g->list_cap[from])
	{
	  ldigraph_list_embiggen(g, from);
	}

      // add to end of array if there is room
      if (g->list_size[from] < g->list_cap[from])
	{
	  g->adj[from][g->list_size[from]++] = to;
	}
    }
}


bool ldigraph_has_edge(const ldigraph *g, int from, int to)
{
  if (g != NULL && from >= 0 && to >= 0 && from < g->n && to < g->n && from != to)
    {
      // sequential search of from's adjacency list
      int i = 0;
      while (i < g->list_size[from] && g->adj[from][i] != to)
	{
	  i++;
	}
      return i < g->list_size[from];
    }
  else
    {
      return false;
    }
}


int ldigraph_shortest_path(const ldigraph *g, int from, int to)
{
  if (g == NULL || from < 0 || from >= g->n || to < 0 || to >= g->n)
    {
      return -1;
    }

  // do BFS starting from the from vertex
  ldigraph_search *s = ldigraph_bfs(g, from);

  if (s != NULL)
    {
      // look up the distance to the to vertex in the result and return it
      int shortest = s->dist[to];
      ldigraph_search_destroy(s);
      return shortest;
    }
  else
    {
      return -1;
    }
}


int ldigraph_longest_path(const ldigraph *g, int from, int to)
{
  if (g == NULL || from < 0 || from >= g->n || to < 0 || to >= g->n)
    {
      return -1;
    }
    
  // do full DFS, recording vertices in reverse order of finish time
  ldigraph_search *s = ldigraph_dfs_with_restart(g);
  int result = -1;
  
  // Acyclic Graph
  if (!s->found_cycle)
    {
      // dynamic programming in order of topological sort
      // walk through the finish array
      for(int i = g->n-1; i >= 0; --i){
        int v = s->finish[i];
        // mark the to vertex as having longest path of length 0 to itself
        if(v == to)
          s->dist[v] = 0;
        // number of neighbors that v has
        else {
          int max = NO_PATH;
          int m = g->list_size[v];
          for(int j = 0; j < m; ++j)
          {
            int w = g->adj[v][j];
            if(max < s->dist[w])
              max = s->dist[w];
          }
          // non-NO_PATH max means there was a path from w to v
          // must increase dist[v] by 1 from dist[w]
          if(max != NO_PATH) max = max+1;
          s->dist[v] = max;
        }
      }

      result = s->dist[from]; // this should be -1 if no path
      ldigraph_search_destroy(s);
      return result;
    }
  // Cyclic Graph
  else
  {
    // brute force search on cyclic graph starting from from vertex,
    // going down all simple paths
    bool* visited = calloc(g->n, sizeof(bool));
    result = brute_force(g, from, visited, 0, to);
    free(visited);
    ldigraph_search_destroy(s);
    return result;
  }
}


void ldigraph_destroy(ldigraph *g)
{
  if (g != NULL)
    {
      for (int i = 0; i < g->n; i++)
	{
	  free(g->adj[i]);
	}
      free(g->adj);
      free(g->list_cap);
      free(g->list_size);
      free(g);
    }
}
