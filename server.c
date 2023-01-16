/*
COMPILE
gcc -Wall -g -o server server.c -lasound
*/

/* This is a server unix datagram socket that listens for
 * events containing new values of the volume. Each event
 * is a single byte, representing a number between 0 and
 * 255. The maximum value the volume will be set to is
 * 100, for now, which corresponds to the maximum safe
 * volume. The behavior can modified later, but for now
 * the volume value is clamped to the interval [0, 100].
 */

/* Use guarded header files.
 */
#include "stdio_h.h"
#include "stdlib_h.h"
#include "errno_h.h"
#include "string_h.h"
#include "sys_types_h.h"
#include "sys_socket_h.h"
#include "arpa_inet_h.h"
#include "netinet_in_h.h"
#include "unistd_h.h"
#include "time_h.h"
#include "msleep.h"
#include "set_volume.h"

/* Path to socket. You pick the name.
 * The OS creates the file for you.
 */
#define SERVER_PORT 6666

/* Throttle sleep time, during which datagrams are cached up to the Unix
 * defined limit of 256*256. (milliseconds) Works together with
 * SERVER_BUF_SIZE.
 */
#define SLEEP_TIME_MS 100 

/* The size in bytes of the buffer used to read from the local domain socket.
 * The goal is to wipe the cache every time, to avoid delays after floods.
 * Works together with SLEEP_TIME_MS.
 */
#define SERVER_BUF_SIZE 256 * 256

/* The device / sound card are we targeting.
 */
#define SERVER_DEVICE "default"

/* The control we are targeting.
 */
#define SERVER_CONTROL "Master"

int main(void){
    
    /* Create a new sockaddr_in object
     */
    struct sockaddr_in server_sockaddr = {0};
    
    /* Create a UNIX domain datagram socket
     */
    int server_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_sock == -1){
        printf("SOCKET ERROR");
        exit(1);
    }
    
    /* Set up the sockaddr structure
     */
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_addr.s_addr = INADDR_ANY;
    server_sockaddr.sin_port = htons(SERVER_PORT);

    /* bind the socket to the network. After bind, the client can send
     * messages.
     */
    if(bind(server_sock,
            (struct sockaddr *) &server_sockaddr,
            sizeof(server_sockaddr)) < 0)
    {
        printf("BIND ERROR");
        close(server_sock);
        exit(1);
    }

    /* A variable for the previous volume, initialized to 0,
     * and set the default main volume to match.
     */
    unsigned prev_vol = 0;
    set_volume(SERVER_DEVICE, SERVER_CONTROL, prev_vol);

    
    /* Loop forever or until an error occurs
     */
    for (;;) {
        /* The buffer for the data, initialized to zero.
         */
        char buf[SERVER_BUF_SIZE] = {0};

        /* The peer socket, initialized to zero. recvfrom expects this
         * argument, but we don't actually use it. recvfrom fills it
         * out for us.
         */
        struct sockaddr_in peer_sock = {0};

        /* The length of the sockaddr_un - we don't actually use this.
         * recvfrom does to match the length of peer_sock after it
         * sets it.
         */
        unsigned sz = 0;

        /* Receive at most SERVER_BUF_SIZE bytes from the client. Note that if
         * there is more than one byte, we throw the rest away anyway, since
         * we clamp the volume to the interval [0, 100] (< 256). However,
         * SERVER_BUF_SIZE should be set larger than 1 (e.g. 128), to avoid
         * piling up of events during a flood of events, which is likely for,
         * e.g., a volume slider. In the case of a buffer of size 128, the
         * remaining 127 bytes - events - will be ignored.
         */
        int bytes_rec = recvfrom(server_sock, buf, SERVER_BUF_SIZE, 0,
                                 (struct sockaddr *) &peer_sock, &sz);

        /* Negative size is how <socket.h> communicates errors.
         */
        if (bytes_rec == -1){
            printf("recvfrom error");
            close(server_sock);
            exit(1);
        } else {
            /* We need a pointer into the buffer in case we need to "remove"
             * some characters.
             */

            /* The new volume - the event - is just the next byte.
             */
            long vol = *buf;

            /* Clamp volume.
             * (0-100)
             */
            vol = (vol < 0) ? 0 : (vol > 100) ? 100 : vol;

            /* Set the volume if it's different than the previous volume.
             */
            if (vol != prev_vol) {
                prev_vol = vol;
                set_volume("default", "Master", vol);
            }            

            /* Throttle events.
             */
            msleep(SLEEP_TIME_MS);
        }
    }
}
