#include <stdlib.h>
#include <string.h>
#include "graph.h"

/* implement the functions you declare in graph.h here */

/*
 * `add_edge` adds the specified edge to the graph passed in via the 
 * first argument. If either of the edge's vertices are not already 
 * in the graph, they are added before their adjacency lists are 
 * updated. 
 */

void add_edge (vertex_t **vtxhead, char *v1_name, char *v2_name, int weight) {
	adj_vertex_t *p_adj = NULL;	// sample pointer set to '0\'

  	if (v1_name->adj_list == NULL)
	{
		v1_name->adj_list = malloc(sizeof(adj_vertex_t));	// get size of adj_vertex and add to list
		v1_name->adj_list->vertex = v2_name;			// set to second point (city2)
		v1_name->adj_list->edge_weight = distance;
		v1_name->adj_list->next = NULL;
	}	
 	else{
		p_adj = v1_name->adj_list;

		while (p_adj->next != NULL)
		{
			p_adj = p_adj->next;	// set to existing points
		}
    		
		// repeat the same process of malloc and next
		p_adj->next = malloc(sizeof(adj_vertex_t));
		p_adj = p_adj->next;
		p_adj->vertex = v2_name;
		p_adj->edge_weight = distance;
		p_adj->next = NULL;
  }
}

vertex_t * add_vert (vertex_t **vlist_head, char *city){
	vertex_t *p_temp = *vlist_head;		// set temporary check to head	

	if (*vlist_head == NULL)
	{
		*vlist_head = malloc(sizeof(vertex_t));		// get site of head
		(*vlist_head)->name = city;			// set the head to the first city
		return *vlist_head;				// return the head
	}
	else 
	{
		while (p_temp->next != NULL){
		p_temp = p_temp->next;
	}
		// repeat the process when next is not equal to '0\'
		p_temp->next = malloc(sizeof(vertex_t));
		p_temp = p_temp->next;
		p_temp->name = city;
		return p_temp;
	}
}



