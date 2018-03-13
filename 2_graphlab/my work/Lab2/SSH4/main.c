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
	vertex_t *v1 = NULL, *v2 = NULL;
    
	int i = 1;
	int distance = 0;
	char *v1_name = NULL, *v2_name = NULL;
    
    
    	/* print out our adjacency list */
	// for(i=1;i<argc;i+=3 ){
	while (argv[i] != NULL && argv[i+1] != NULL && argv[i+2] != NULL)	// less memory leaks
	{
        	v1_name =  argv[i];
        	v2_name = argv[i+1];
        	distance = atoi(argv[i+2]);     // atoi() convert a string to an integer
        
        	if (vert_exist(vlist_head, v1_name) == 0)
		{
            		v1 = add_vert(&vlist_head, v1_name);
            		vp++;
        	}
        	else
		{
      			v1 = find_vert(vlist_head, v1_name);
		}
        
        	if (vert_exist(vlist_head, v2_name) == 0)
		{
        		v2 = add_vert(&vlist_head, v2_name);
            		vp++;
        	}
        	else
            		v2 = find_vert(vlist_head, v2_name);
        
        	add_edge(v1, v2, distance);
        	add_edge(v2, v1, distance);
        	i+=3;
  	}
	
	print_adj(vlist_head);
	tour(vlist_head, vp);

	/* free memory. every time malloc is called, free is used in dynamic memory allocation.
 	 * free deallocates the memory so that it can be used for other purposes 
 	 */		

 	free_verts(vlist_head);

	return 0;
}
