#include <stdlib.h>
#include <stdio.h>
#include "graph.h"

int main (int argc, char *argv[]) {
	/* 
	 * Programatically constructing the following simple graph:
	 *
	 *     10         5
	 * A <-----> B <-----> C
	 *
	 * Delete this code and add your own!
	 */
	vertex_t *v1, *v2, *v3, *vlist_head, *vp;
	adj_vertex_t *adj_v;

	vlist_head = v1 = malloc(sizeof(vertex_t));
	v1->name = "A";
	v2 = malloc(sizeof(vertex_t));
	v2->name = "B";
	v3 = malloc(sizeof(vertex_t));
	v3->name = "C";

	v1->next = v2;
	v2->next = v3;
	v3->next = NULL;

	adj_v = v1->adj_list = malloc(sizeof(adj_vertex_t));
	adj_v->vertex = v2;
	adj_v->edge_weight = 10;
	adj_v->next = NULL;

	adj_v = v2->adj_list = malloc(sizeof(adj_vertex_t));
	adj_v->vertex = v1;
	adj_v->edge_weight = 10;
	adj_v = adj_v->next = malloc(sizeof(adj_vertex_t));
	adj_v->vertex = v3;
	adj_v->edge_weight = 5;
	adj_v->next = NULL;

	adj_v = v3->adj_list = malloc(sizeof(adj_vertex_t));
	adj_v->vertex = v2;
	adj_v->edge_weight = 5;
	adj_v->next = NULL;

	/* print out our adjacency list */
	printf("Adjacency list:\n");
	for (vp = vlist_head; vp != NULL; vp = vp->next) {
		printf("  %s: ", vp->name);
		for (adj_v = vp->adj_list; adj_v != NULL; adj_v = adj_v->next) {
			printf("%s(%d) ", adj_v->vertex->name, adj_v->edge_weight);
		}
		printf("\n");
	}

	/* note, I'm not free-ing here, but you should! */

	return 0;
}
