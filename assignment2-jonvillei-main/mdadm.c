#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "mdadm.h"
#include "jbod.h"

int mounted = 0;

uint32_t encode_operation(jbod_cmd_t cmd, int disk_num, int block_num)
{
     uint32_t op = cmd << 26 | disk_num << 22 | block_num;

     return op;
}

int mdadm_mount(void) {
     if (mounted == 0)
     {
	  uint32_t op = encode_operation(JBOD_MOUNT, 0, 0);
	  jbod_operation(op, NULL);
	  mounted = 1;
	  return 1;
     }
     return -1;
}

int mdadm_unmount(void) {
     if (mounted == 1)
     {
	  uint32_t op = encode_operation(JBOD_UNMOUNT, 0, 0);
	  jbod_operation(op, NULL);
	  mounted = 0;
	  return 1;
     }
     return -1;
}

int mdadm_read(uint32_t addr, uint32_t len, uint8_t *buf) {
     if (mounted == 0)
     {
	  return -1;
     }
     else if (len > 1024)
     {
	  return -1;
     }
     else if ((addr + len) > 1048576)
     {
	  return -1;
     }
     else if (len != 0 && buf == NULL)
     {
	  return -1;
     }
     else if (len == 0 && buf == NULL)
     {
	  return 0;
     }

     int currAddr = addr;
     while (currAddr < addr + len)
     {
	  // seek

	  int diskNum = currAddr / JBOD_DISK_SIZE;
	  int blockNum = currAddr % JBOD_DISK_SIZE / JBOD_BLOCK_SIZE;
	  int offset = currAddr % JBOD_DISK_SIZE % JBOD_BLOCK_SIZE;

	  jbod_operation(encode_operation(JBOD_SEEK_TO_DISK, diskNum, 0), NULL);

	  jbod_operation(encode_operation(JBOD_SEEK_TO_BLOCK, 0, blockNum), NULL);
	  
          // read
	  uint8_t tmp[JBOD_BLOCK_SIZE];
	  jbod_operation(encode_operation(JBOD_READ_BLOCK, 0, 0), tmp);
     
	  // process
	  if (offset != 0 /*then you are on first block, or are taking full first blocK*/)
	  {
	       memcpy(buf, tmp + offset, JBOD_BLOCK_SIZE - offset);
	  }
	  else if ((addr + len) - currAddr <= JBOD_BLOCK_SIZE /*you are on last block*/)
	  {
	       memcpy(buf + (currAddr - addr), tmp, (addr + len) - currAddr);
	  }
	  else
	  {
	       memcpy(buf + (currAddr - addr), tmp, JBOD_BLOCK_SIZE);
	  }
	  
	  currAddr = currAddr - offset + JBOD_BLOCK_SIZE;
	  
	  
     }
     
  return len;
}

