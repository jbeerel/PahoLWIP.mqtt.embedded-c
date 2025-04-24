#ifndef TCP_TRANSPORT_H
#define TCP_TRANSPORT_H

#include "lwip/ip_addr.h"


#define TCP_TRANSPORT_WAIT (10000)

void Init_TCP_Transport(void);
uint32_t tcp_get_curr_time(void);
int32_t tcp_send(int socket, const void *pBuffer,
                 size_t bytesToSend, TickType_t *xTicksToWait);
int32_t tcp_read(int socket, void *pBuffer,
                 size_t bytesToRecv, TickType_t *xTicksToWait);
int tcp_status(int socket);
bool tcp_close_socket(int socket);
bool tcp_connect_socket(int socket, const char *host, uint16_t port);

#endif  // TCP_TRASNPORT_H