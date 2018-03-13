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

	vertex_t *vlist_head = NULL;    // set head to null
	int vp = 0;	// set initial visited points
	
	get_edges(argc, argv, vlist_head, vp);	
	print_adj(vlist_head);
	tour(vlist_head, vp);

	/* free memory. every time malloc is called, free is used in dynamic memory allocation.
 	 * free deallocates the memory so that it can be used for other purposes 
 	 */		

 	free_verts(vlist_head);

	return 0;
}
