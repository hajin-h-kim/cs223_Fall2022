#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "ldigraph.h" // for typedef'd ldigraph

#include "ldigraph_helpers.h"

// ==============================================================
// Queue helper functions for ldigraph_shortest()
// ==============================================================
typedef struct{
  int* que;
  int head;
  int tail;
} queue;

static queue* new_queue(int n)
{
  queue* q = malloc(sizeof(queue));
  q->que = malloc(sizeof(int) * n);
  q->head = -1;
  q->tail = -1;
  return q;
}

static void enqueue(queue* q, int value)
{
  if (q->que == NULL)
    return;

  q->que[++q->tail] = value;
  if(q->head == -1) q->head++;
}

static int dequeue(queue* q)
{
  if (q->que == NULL)
    return -1;
  
  int value = q->que[q->head++];
  // maybe unecessary
  // q->que[tail] = -1;
  return value;
}

static bool queue_isempty(queue* q)
{
  if(q->head > q->tail)
    return true;
  else
    return false;
}

static void destroy_queue(queue* q)
{
  free(q->que);
  free(q);
}

// ==============================================================
// Helper functions for ldigraph_longest(), cyclic
// ==============================================================
static int max_of_neighbors(int* arr, int m)
{
  int max = NO_PATH;
  for(int i = 0; i < m; ++i)
    if(max < arr[i]) max = arr[i];
  free(arr);
  return max;
}


int brute_force(const ldigraph* g, int v, bool* visited, int path_length, int to)
{
  // number of neighbors of vertex v
  int m = g->list_size[v];
  
  if(v == to) // we found "to"
  {
    return path_length;
  }
  else if(visited[v] == true) // a cycle
  {
    return NO_PATH;
  }
  else if(m == 0) // this path did not leat to "to"
  {
    return NO_PATH;
  }
  
  int* arr = malloc(sizeof(int) * m);
  visited[v] = true;
  for(int i = 0; i < m; ++i)
  {
    // v's neighbor u
    int u = g->adj[v][i];
    arr[i] = brute_force(g, u, visited, path_length+1, to);
  }
  visited[v] = false;
  return max_of_neighbors(arr, m);
}

// ==============================================================
// Prof. Glenn's helper functions
// ==============================================================
ldigraph_search *ldigraph_bfs(const ldigraph *g, int from)
{
  ldigraph_search* s = ldigraph_search_create(g, from);

  queue* q = new_queue(g->n);
  enqueue(q, from);
  s->dist[from] = 0;
  
  while(!queue_isempty(q))
  {
    int v = dequeue(q);
    // for all edges of vertex v
    int u;
    for(int i = 0; i < g->list_size[v]; ++i)
    {
      u = g->adj[v][i];
      if(s->dist[u] == -1)
      {
        s->dist[u] = s->dist[v] + 1;
        enqueue(q, u);
      }
    }
  }
  destroy_queue(q);
  return s;
}

// Did not use, instead made brute_force()
ldigraph_search *ldigraph_dfs(const ldigraph *g, int from)
{
  if (g == NULL || from < 0 || from >= g->n)
    {
      return NULL;
    }

  ldigraph_search *s = ldigraph_search_create(g, from);
  if (s != NULL)
    {
      // start at from
      s->dist[from] = 0;
      ldigraph_dfs_visit(g, s, from);
    }
  return s;
}


ldigraph_search *ldigraph_dfs_with_restart(const ldigraph *g)
{
  if (g == NULL)
    {
      return NULL;
    }

  ldigraph_search *s = ldigraph_search_create(g, 0);
  if (s != NULL)
    {
      // try all starting points for DFS
      for (int from = 0; from < g->n; from++)
	{
	  // use from as a starting point if no previous search found it
	  if (s->color[from] == LDIGRAPH_UNSEEN)
	    {
	      s->dist[from] = 0;
	      ldigraph_dfs_visit(g, s, from);
	    }
	}
    }
  return s;
}


void ldigraph_dfs_visit(const ldigraph* g, ldigraph_search *s, int from)
{
  s->color[from] = LDIGRAPH_PROCESSING;

  // make alias for adjacency list for from vertex
  const int *neighbors = g->adj[from];
  
  // iterate over outgoing edges
  for (int i = 0; i < g->list_size[from]; i++)
  {
      int to = neighbors[i];
      if (s->color[to] == LDIGRAPH_UNSEEN)
      {
        // found an edge to a new vertex -- explore it
        s->dist[to] = s->dist[from] + 1;
        s->pred[to] = from;
        
        ldigraph_dfs_visit(g, s, to);
      }
      else if (s->color[to] == LDIGRAPH_PROCESSING)
      {
        // edge from current vertex to a still active vertex forms a cycle
        s->found_cycle = true;
      }
  }
  
  // mark and record current vertex finished
  s->color[from] = LDIGRAPH_DONE;
  s->finish[g->n - s->count - 1] = from;
  s->count++;
}


void ldigraph_list_embiggen(ldigraph *g, int from)
{
  if (g->list_cap[from] != 0)
    {
      g->adj[from] = realloc(g->adj[from], sizeof(int*) * g->list_cap[from] * 2);
      g->list_cap[from] = g->adj[from] != NULL ? g->list_cap[from] * 2 : 0;
    }
}


ldigraph_search *ldigraph_search_create(const ldigraph *g, int from)
{
  if (g != NULL && from >= 0 && from < g->n)
    {
      ldigraph_search *s = malloc(sizeof(ldigraph_search));
      
      if (s != NULL)
	{
	  s->g = g;
	  s->color = malloc(sizeof(int) * g->n);
	  s->dist = malloc(sizeof(int) * g->n);
	  s->pred = malloc(sizeof(int) * g->n);
	  s->finish = malloc(sizeof(int) * g->n);

	  if (s->color != NULL && s->dist != NULL && s->pred != NULL)
	    {
	      ldigraph_search_init(s, from);
	    }
	  else
	    {
	      free(s->pred);
	      free(s->dist);
	      free(s->color);
	      free(s);
	      return NULL;
	    }
	}

      return s;
    }
  else
    {
      return NULL;
    }
}


void ldigraph_search_init(ldigraph_search *s, int from)
{
  // set from vertex
  s->from = from;
  
  // initialize all vertices to unseen
  for (int i = 0; i < s->g->n; i++)
    {
      s->color[i] = LDIGRAPH_UNSEEN;
      s->dist[i] = -1; // -1 for no path yet
      s->pred[i] = -1; // no predecessor yet
    }

  // other bookkeeping
  s->found_cycle = false;
  s->count = 0;

}


void ldigraph_search_destroy(ldigraph_search *s)
{
  if (s != NULL)
    {
      free(s->color);
      free(s->dist);
      free(s->pred);
      free(s->finish);
      free(s);
    }
}
