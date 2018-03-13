/* forward declarations so self-referential structures are simpler */
typedef struct vertex vertex_t;
typedef struct adj_vertex adj_vertex_t;
typedef struct edge edge_t;

struct vertex {
	char *name;
	adj_vertex_t *adj_list;
	vertex_t *next;
	int visited;
};

struct adj_vertex {
	vertex_t *vertex;
	int edge_weight;
	adj_vertex_t *next;	
};

struct edge{
	vertex_t *v1;
	vertex_t *v2;
	int weight;
	edge_t *next;
};

/* This is the one function you really should implement as part of your 
 * graph data structure's public API. 
 *
 * `add_edge` adds the specified edge to the graph passed in via the first
 * argument. If either of the edge's vertices are not already in the graph,
 * they are added before their adjacency lists are updated. If the graph
 * is currently empty (i.e., *vtxhead == NULL), a new graph is created,
 * and the caller's vtxhead pointer is modified. 
 *
 * `vtxhead`: the pointer to the graph (more specifically, the head of the
 *            list of vertex_t structures)
 * `v1_name`: the name of the first vertex of the edge to add
 * `v2_name`: the name of the second vertex of the edge to add
 * `weight` : the weight of the edge to add
 *
 * 'add_vert' adds a vertex in graph.c.
 * 'find_vert' finds a vertex in graph.c.
 * 'vert_exist' checks if vertex exists in graph.c.
 */
void add_edge(vertex_t *v1_name, vertex_t *v2_name, int distance);// original add_edge - creates graph
void get_edges(int argc, char *argv[], vertex_t *vlist_head, int vp);	//get edges by reading the arguments (cities)
vertex_t *add_vert(vertex_t **vlist_head, char *city);		// added - add vertex
vertex_t *find_vert(vertex_t *vlist_head, char *city);		// added - find vertex
int vert_exist(vertex_t *vlist_head, char *city); 		// added - check if vertice exists

/*
 * functions used for printing in main.c
 */

void print_adj (vertex_t *vlist_head);                          // added - print adjacences - used in main.c
void tour (vertex_t *vlist_head, int vp);			// added - print tour
void free_verts (vertex_t *vlist_head);                         // added - free vertices

