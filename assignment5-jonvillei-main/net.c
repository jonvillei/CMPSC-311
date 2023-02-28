#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <err.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "net.h"
#include "jbod.h"

/* the client socket descriptor for the connection to the server */
int cli_sd = -1;

/* attempts to read n bytes from fd; returns true on success and false on
 * failure */
static bool nread(int fd, int len, uint8_t *buf) {
     int n = 0;
     while (n < len)
     {
	  int r = read(fd, &buf[n], len - n);
	  if (r <= 0)
	  {
	       return false;
	  }
	  n += r;
     }
     return true;
}

/* attempts to write n bytes to fd; returns true on success and false on
 * failure */
static bool nwrite(int fd, int len, uint8_t *buf) {
     int n = 0;
     while (n < len)
     {
	  int r = write(fd, &buf[n], len - n);
	  if (r < 0)
	  {
	       return false;
	  }
	  n += r;
     }
     return true;
}

/* attempts to receive a packet from fd; returns true on success and false on
 * failure */
static bool recv_packet(int fd, uint32_t *op, uint16_t *ret, uint8_t *block) {
     uint16_t len;
     uint8_t header[HEADER_LEN];

     if (!nread(fd, HEADER_LEN, header))
     {
	  return false;
     }

     int offset = 0;
     memcpy(&len, header + offset, sizeof(len));
     offset += sizeof(len);
     memcpy(op, header + offset, sizeof(*op));
     offset += sizeof(*op);
     memcpy(ret, header + offset, sizeof(*ret));
     offset += sizeof(*ret);

     len = ntohs(len);
     *op = ntohl(*op);
     *ret = ntohs(*ret);

     if (len == HEADER_LEN)
     {
          return true;
     }

     uint8_t tmp[256];
     if(!nread(fd, 256, tmp))
     {
	  return false;
     }

     tmp = ntohs(tmp);
     return true;

	  

     

     
}

/* attempts to send a packet to sd; returns true on success and false on
 * failure */
static bool send_packet(int sd, uint32_t op, uint8_t *block) {
     uint16_t len;
     uint8_t header[HEADER_LEN];


     int offset = 0;
     memcpy(&len, header + offset, sizeof(len));
     offset += sizeof(len);
     memcpy(op, header + offset, sizeof(*op));
     offset += sizeof(*op);

     if (!nwrite(sd, HEADER_LEN, header))
	  {
	       return false;
	  }

     len = htons(len);
     *op = htonl(*op);

     if (len == HEADER_LEN)
     {
          return true;
     }

     uint8_t tmp[256];
     if(!nwrite(sd, 256, tmp))
     {
	  return false;
     }

     tmp = ntohs(tmp);
     return true;

     
}

/* attempts to connect to server and set the global cli_sd variable to the
 * socket; returns true if successful and false if not. */
bool jbod_connect(const char *ip, uint16_t port) {
     //create socket
     int sockfd;
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd == -1)
     {
	  printf("Error on socket creation [%s]\n", strerror(errno));
	  return false;
     }
     
     //convert ip to binary form
     struct sockaddr_in caddr;

     caddr.sin_family = AF_INET;
     caddr.sin_port = htons(port);
     if (inet_aton(ip, &caddr.sin_addr) == 0)
     {
	  return false;
     }
     //call connect function to connect
     if (connect(sockfd, (const struct sockaddr *)&caddr, sizeof(caddr)) == -1)
     {
	  return false;
     }
     cli_sd = sockfd;
     return true;
}

/* disconnects from the server and resets cli_sd */
void jbod_disconnect(void) {
     //close connection
     if (cli_sd != -1)
     {
	  close(cli_sd);
	  cli_sd = -1;
     }
}

/* sends the JBOD operation to the server and receives and processes the
 * response. */
int jbod_client_operation(uint32_t op, uint8_t *block) {
     //write packet
     send_packet(sd, op, block);
     //read response
     recv_packet(fd, op, ret, block);
}
