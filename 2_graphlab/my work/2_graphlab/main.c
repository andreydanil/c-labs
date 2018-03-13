/*
 * main.c
 * CS351_2_graphlab
 *
 * Created by Andrey Danilkovich on 9/17/2013
 * Copyright (c) 2013 Andrey Danilkovich. All rights reserved.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include "graph.h"	// graph header file

int main (int argc, char *argv[]) {

	vertex_t *v1 = NULL, *v2 = NULL, *v3 = NULL, *vlist_head = NULL;
	*vlist_head = NULL;

	int i = 1;
	int vp = 0;	// visited points
	int distance = 0;

	char *v1_name = NULL, *v2_name = NULL;


	/* free memory. every time malloc is called, free is used in dynamic memory allocation.
 	 * free deallocates the memory so that it can be used for other purposes 
 	 */

	/* print out our adjacency list */
	printf("Adjacency list:\n");
	for (vp = vlist_head; vp != NULL; vp = vp->next) {
		printf("  %s: ", vp->name);
		for (adj_v = vp->adj_list; adj_v != NULL; adj_v = adj_v->next) {
			printf("%s(%d) ", adj_v->vertex->name, adj_v->edge_weight);
		}
		printf("\n");
	}

	return 0;
}
