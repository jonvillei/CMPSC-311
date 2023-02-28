#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cache.h"

static cache_entry_t *cache = NULL;
static int cache_size = 0;
static int clock = 0;
static int num_queries = 0;
static int num_hits = 0;

int cache_create(int num_entries) {
     bool x = cache_enabled();
     if (x == true)
     {
	  return -1;
     }
     if (num_entries < 2 || num_entries > 4096)
     {
	  return -1;
     }
     cache = calloc(num_entries, sizeof(cache_entry_t));
     cache_size = num_entries;
  return 1;
}

int cache_destroy(void) {
     if (cache != NULL)
     {
	  free(cache);
	  cache = NULL;
	  return 1;
     }
     return -1;
}

int cache_lookup(int disk_num, int block_num, uint8_t *buf) {
     bool x = cache_enabled();
     if (x == false)
     {
	  return -1;
     }
     else if (buf == NULL)
     {
	  return -1;
     }
     
     for (int i = 0; i < cache_size; ++i)
     {
	  if (cache[i].valid == true)
	  {
	       if (cache[i].disk_num == disk_num && cache[i].block_num == block_num)
	       {
		    memcpy(buf, cache[i].block, JBOD_BLOCK_SIZE);
		    cache[i].access_time = clock;
		    clock += 1;
		    num_hits += 1;
		    num_queries += 1;
		    return 1;
	       }
	  }
     }
     clock += 1;
     num_queries += 1;
     return -1;
}

void cache_update(int disk_num, int block_num, const uint8_t *buf) {
     for (int i = 0; i < cache_size; ++i)
     {
	  if (cache[i].valid == true)
	  {
	       if (cache[i].disk_num == disk_num && cache[i].block_num == block_num)
	       {
		    memcpy(cache[i].block, buf, JBOD_BLOCK_SIZE);
		    cache[i].access_time = clock;
		    clock += 1;
		    break;

	       }
	  }
     }
     
     
}

int cache_insert(int disk_num, int block_num, const uint8_t *buf) {
     bool x = cache_enabled();
     int lru = -1;
     if (x == false)
     {
	  return -1;
     }
     else if (buf == NULL)
     {
	  return -1;
     }
     else if (disk_num < 0 || disk_num > JBOD_NUM_DISKS)
     {
	  return -1;
     }
     else if (block_num < 0 || block_num > JBOD_NUM_BLOCKS_PER_DISK)
     {
	  return -1;
     }

     for (int i = 0; i < cache_size; ++i)
     {
	  if (cache[i].valid == true)
	  {
	       if (cache[i].disk_num == disk_num && cache[i].block_num == block_num)
	       {
		    return -1;

	       }
	  }
     }

     for (int i = 0; i < cache_size; ++i)
     {
	  if (cache[i].valid == false)
	  {
	       cache[i].valid = true;
	       cache[i].disk_num = disk_num;
	       cache[i].block_num = block_num;
	       memcpy(cache[i].block, buf, JBOD_BLOCK_SIZE);
	       cache[i].access_time = clock;
	       clock += 1;
	       return 1;
	  }
     }

     for (int i = 0; i < cache_size; ++i)
     {
	  if (i == 0)
	  {
	       lru = cache[i].access_time;
	  }
	  else
	  {
	       if (cache[i].access_time < lru)
	       {
		    lru = cache[i].access_time;
	       }
	  }
     }

     for (int i = 0; i < cache_size; ++i)
     {
	  if (cache[i].access_time == lru)
	  {
	       cache[i].disk_num = disk_num;
	       cache[i].block_num = block_num;
	       memcpy(cache[i].block, buf, JBOD_BLOCK_SIZE);
	       cache[i].access_time = clock;
	       clock += 1;
	       return 1;
	  }
     }
     return -1;
}

bool cache_enabled(void) {
     if (cache != NULL)
     {
	  return true;
     }
     return false;
}

void cache_print_hit_rate(void) {
  fprintf(stderr, "Hit rate: %5.1f%%\n", 100 * (float) num_hits / num_queries);
}
