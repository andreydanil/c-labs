#include <stdio.h>
#include <stdlib.h>


typedef struct {
  char valid;
  unsigned int tag;
  unsigned int timestamp;
} cache_line_t;

typedef struct {
  cache_line_t *lines;
} cache_set_t;

typedef struct {
  int num_sets;
  int associativity;
  int block_size;
  cache_set_t *sets;
} cache_t;


cache_t *make_cache(int num_lines, int associativity, int bytes_per_block);
int cache_lookup(long addr_req, cache_t *cache);

int main (int argc, char *argv[]) {
    int num_lines = atoi(argv[1]),
        lines_per_set = atoi(argv[2]),
        bytes_per_block = atoi(argv[3]);
    int num_reqs = 0, num_hits = 0;

    char line[80];

    long addr_req;

    //printf("Simulating cache with:\n");
    //printf(" - Total lines   = %d\n", num_lines);
    //printf(" - Lines per set = %d\n", lines_per_set);
    //printf(" - Block size    = %d bytes\n", bytes_per_block);
 
    cache_t *cache = make_cache(num_lines, lines_per_set, bytes_per_block);

    while (fgets(line, 80, stdin)) {
        addr_req = strtol(line, NULL, 0);

        /* simulate cache fetch with address `addr_req` */
        //printf("Processing request: 0x%lX\n", addr_req);
	
	int hit = cache_lookup(addr_req, cache);
	if(hit)
	  num_hits++;
	num_reqs++;
    }
    double hit_rate = (num_hits / (double)num_reqs) * 100.0;
    printf("Hit rate: %.2f%% | Miss Rate: %.2f%%\n", hit_rate, 100 - hit_rate);

    return 0;
}

cache_t *make_cache(int num_lines, int associativity, int bytes_per_block){
  cache_t *cache = calloc(1, sizeof(cache_t));
  cache->num_sets = num_lines/associativity;
  cache->associativity = associativity;
  cache->block_size = bytes_per_block;
  cache->sets = calloc(cache->num_sets, sizeof(cache_set_t));
		      
  int i;
  for(i = 0; i < cache->num_sets; i++)
    cache->sets[i].lines = calloc(associativity, sizeof(cache_line_t));

  return cache;
}


int cache_lookup(long addr_req, cache_t *cache){
  unsigned int set_idx = (addr_req / cache->block_size) % cache->num_sets;
  unsigned int tag = addr_req / (cache->block_size * cache->num_sets);
  cache_set_t *set = cache->sets + set_idx;

  int i, hit = 0, miss_flag = 0, index_low_st;
  for(i = 0; i < cache->associativity; i++){
    if(set->lines[i].valid && set->lines[i].tag == tag){
      hit = 1;
      break;
    }
  }

  if(hit){
    set->lines[i].timestamp++; 
    return 1;
  }
  else {
    index_low_st = 0;
    for(i = 0; i < cache->associativity; i++){
      if(set->lines[i].timestamp < set->lines[index_low_st].timestamp)
	index_low_st = i;
  
      if(!(set->lines[i].valid)){
	set->lines[i].valid = 1;
	set->lines[i].tag = tag;
	set->lines[i].timestamp++;
	miss_flag = 1;
	//break;
      }
    }
    if(!miss_flag){
      set->lines[index_low_st].valid = 1;
      set->lines[index_low_st].tag = tag;
      set->lines[index_low_st].timestamp++;
    }
  }

  return 0;
}