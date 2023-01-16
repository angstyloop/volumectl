/* COMPILE
 *
gcc -Wall -g -o client client.c
 *
 * This is a client UDP datagram socket for sending events to the
 * volumectl server.
 *
 * USAGE
 *
 * Mute master volume.
 *
./client 0
 *
 * Set master volume to 50% of max safe threshold.
 *
./client 50
 *
 * Set master volume to max safe threshold.
 *
./client 100
 */

#include "stdio_h.h"
#include "stdlib_h.h"
#include "errno_h.h"
#include "string_h.h"
#include "sys_types_h.h"
#include "sys_socket_h.h"
#include "unistd_h.h"
#include "arpa_inet_h.h"
#include "netinet_in_h.h"

#define SERVER_PORT 6666 
#define CLIENT_BUF_SIZE 256

int main(int argc, char* argv[argc])
{
    /** Check args.
     */
    if (argc != 2) {
        printf("USAGE: %s <MESSAGE>", argv[0]);
        exit(EXIT_FAILURE);
    }
  
    /** Create a UDP datagram socket
     */
    int client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket == -1) {
        perror("socket");
        exit(1);
    }

    /** Create a sockaddr pointing to our server path
     */
    struct sockaddr_in remote = {0};
    remote.sin_family = AF_INET;        
    remote.sin_port = htons(SERVER_PORT);
    remote.sin_addr.s_addr = INADDR_ANY;

    /** Chop the input at 3 characters, by making the 4th a null byte,
     * unless a null third or second byte exists. There is always at
     * least one byte in an argv string - the terminating null byte.
     */
    char* p = argv[1];
    while (*p && p - argv[1] < 4) { ++p; }
    *p = 0;
    p = argv[1];

    /** Parse the 0-3 digit number (a size 1-4 string, including the null
     * terminator) out of the string @p points to
     */
    int z = atoi(p);
    z = z < 0 ? 0 : z > 100 ? 100 : z;

    /** Make a pointer to a single byte - the int @z cast to the more narrow
     * char data type. 
     */
    char byte[1] = {0};
    byte[0] = (char) z;
    
    /** Copy the single byte representing our number between 0 and 255
     * (really, between 0 and 100) the buffer. 
     */
    char buf[CLIENT_BUF_SIZE] = {0};
    strncpy(buf, byte, CLIENT_BUF_SIZE);

    /** Send the byte - the volume event - to the server.
     */
    if ((sendto(client_socket, buf, strlen(buf), 0,
                    (struct sockaddr *) &remote, sizeof(remote))) == -1)
    {
        perror("sendto");
        close(client_socket);
        exit(1);
    }   
    
    /** Close the socket and exit
     */
    if(close(client_socket) == -1) {
        perror("close");
    }

    return EXIT_SUCCESS;
}
