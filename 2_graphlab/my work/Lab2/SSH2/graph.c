#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "graph.h"

/* implement the functions you declare in graph.h here */

/*
 * `add_edge` adds the specified edge to the graph passed in via the 
 * first argument. If either of the edge's vertices are not already 
 * in the graph, they are added before their adjacency lists are 
 * updated. 
 */

void add_edge (vertex_t *v1_name, vertex_t *v2_name, int distance) {
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

void get_edges(int argc, char *argv[], vertex_t *vlist_head, int vp){
    
    vertex_t *v1 = NULL, *v2 = NULL;
    
    int i = 1;
	
	int distance = 0;
    
	char *v1_name = NULL, *v2_name = NULL;

	/* read arguments  */
	/*
 	 * for (adj_v = vp->adj_list; adj_v != NULL; adj_v = adj_v->next) {
 	 * printf("%s(%d) ", adj_v->vertex->name, adj_v->edge_weight);
 	 */
	while (argv[i] != NULL && argv[i+1] != NULL && argv[i+2] != NULL)
	{
        	v1_name =  argv[i];		// city 1
        	v2_name = argv[i+1];		// city 2
        	distance = atoi(argv[i+2]);     // atoi() convert a string to an integer
        	
		// case 1
        	if (vert_exist(vlist_head, v1_name) == 0){
        		v1 = add_vert(&vlist_head, v1_name);
            		vp++;
        	}
        	else{
      			v1 = find_vert(vlist_head, v1_name);
		}
        	
		// case 2
        	if (vert_exist(vlist_head, v2_name) == 0){
            		v2 = add_vert(&vlist_head, v2_name);
            		vp++;
        	}
        	else{
            	v2 = find_vert(vlist_head, v2_name);
        	}

      	  	add_edge(v1, v2, distance);	// call add edges
        	add_edge(v2, v1, distance);	// call add edges reversed
        	i+=3;
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

vertex_t *find_vert (vertex_t *vlist_head, char *city){
	vertex_t *v = vlist_head;	// set vertex to head

	if (vlist_head == NULL)		// case 1: equals nothing
	{
 		return NULL;
	}
 	else if (strncmp(vlist_head->name, city, strlen(city)) == 0)	// case 2: if city length is 0
	{
		return vlist_head;	// list the head
	}
  	else
	{
    		while (v->next != NULL)	// case 3: next vertice is not null
		{
			v = v->next;	// set next vertice
			if (strncmp(v->name, city, strlen(city)) == 0)
			return v;
    		}
  	}
  	return NULL;
}

void print_adj (vertex_t *vlist_head){
	vertex_t *point_to = vlist_head;	// point to head
	adj_vertex_t *p_adj = NULL;	// set adj to null
    
 	// case 1, there were no vertices after the file name
	if (vlist_head == NULL){
        	printf("No vertices were defined after the compiled file.\nPlease specify vertices and their weight.\n");
    	}
	// case 2, show the adjacency list, reading the name and weight. output the string and the weight as an unsiged int
    	else{
        	printf("Adjacency List:\n");
        	while (point_to != NULL){	// visit all points
            		p_adj = point_to->adj_list;
            		printf("%s:", point_to->name);
            		while (p_adj != NULL){	// print all points
                		printf(" %s(%u)", p_adj->vertex->name, p_adj->edge_weight);
                		p_adj = p_adj->next;
            		}		
            		printf("\n");
            		point_to = point_to->next;
        	}	
        printf("\n");
    }
}

int vert_exist (vertex_t *vlist_head, char *city){
	vertex_t *point_to = vlist_head;
    
	// compare the string and return the next
    	while (point_to != NULL){
        	if (strcmp(point_to->name, city) == 0)
            	return 1;
        else
            	point_to = point_to->next;
    	}	
    	return 0;
}

void free_adj (adj_vertex_t *adj){
	adj_vertex *p_adj = adj, *next = NULL;
	// free the allocated memory when adjacency matrix is called
	while (p_adj != NULL){
		next = p_adj->next;
		free(p_adj);
		p_adj = next;
	}
}


void free_verts (vertex_t *vlist_head){
    	vertex_t *p_temp = vlist_head, *next = NULL;
    
    	if (vlist_head == NULL);
	// free the memory for the vlist_head that are not null. use a temp pointer as a ref.
    	else{
        	while (p_temp != NULL){
            	free_adj(p_temp->adj_list);
            	next = p_temp->next;
            	free(p_temp);
            	p_temp = next;
        	}
    	}
}

/* tour search function
 * 1. an ordered list of vertices that constitute a "tour" of the graph — i.e., a path on which each vertex is visited only once; if no tour exists, your program should print a message to that effect
 * 2. shows lengths message
 */
int print_tour (vertex_t *vlist_head, int rem_cities, int dist){
 	vertex_t *p_temp = vlist_head;
    	adj_vertex_t *p_adj = NULL;
    	int distance = 0;		// initialize distance to o
    
    	if (p_temp->visited != 0)	// if vertices not equl to NULL
        	return 0;
    	else if (rem_cities == 0){
        	printf("%s ", p_temp->name);	// print remaining vertices
        return dist;				// return distance
    	}
	// show all cities by visiting each vertice
    	else{
        	p_temp->visited = 1;		
        	p_adj = p_temp->adj_list;
        	while (p_adj != NULL){		// visit all cities
           		distance = print_tour(p_adj->vertex, rem_cities-1, p_adj->edge_weight);
            		if (distance == 0)
                		p_adj = p_adj->next;
            		else{
                		printf("%s ", p_temp->name);
                		return (distance+dist);
            		}
        	}
        	p_temp->visited = 0;
    	}
    	return 0;
}

// printing the tour path - final function
void tour (vertex_t *vlist_head, int vp){
	vertex_t *p_temp = vlist_head;	// similar implementation as print_tour
	int tour_dist = 0, counter = 50;	// check cities
    
	printf("Tour Path:\n");
    
	while (p_temp != NULL && counter != 0){
        	tour_dist = print_tour(p_temp, vp-1, 0);	// call print tour to get the distance
        
        	if (tour_dist == 0)
            		p_temp = p_temp->next;
        	else
            		break;
        counter--;	// decrement loop from 50
    	}
    	if (tour_dist == 0)
        	printf("There is no tour!\n");
    	else
        	printf("\n---\nTour length: %u\n", tour_dist);
}
