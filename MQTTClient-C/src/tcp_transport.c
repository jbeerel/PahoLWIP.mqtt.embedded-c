/**
 * @file tcp_transport.c
 * @author jbeerel
 * @brief Custom TCP transport layer c file
 * @version 0.1
 * @date 2025-04-19
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "semphr.h"

#include "lwip/dns.h"
#include "lwip/ip4_addr.h"
#include "lwip/sockets.h"

#include "tcp_transport.h"

static StaticSemaphore_t sem_buffer;
static SemaphoreHandle_t xHostDNSFound;

static bool static_connect(int sock, ip_addr_t *host, const int port);
static void dns_cb(const char *name, const ip_addr_t *ipaddr, void *callback_arg);
static void dns_found(const char *name, const ip_addr_t *ipaddr, void *callback_arg);

/**
 * @brief Initialize TCP Network layer
 *
 */
void Init_TCP_Transport(void) {
  xHostDNSFound = xSemaphoreCreateBinaryStatic(&sem_buffer);
  configASSERT(xHostDNSFound);
}

/**
 * @brief Get current time in ms for MQTT
 *
 * @return uint32_t
 */
uint32_t tcp_get_curr_time(void) {
  return to_ms_since_boot(get_absolute_time());
}

/**
 * Send bytes through socket
 * @param pNetworkContext - Network context object from MQTT
 * @param pBuffer - Buffer to send from
 * @param bytesToSend - number of bytes to send
 * @return number of bytes sent
 */
int32_t tcp_send(int socket, const void *pBuffer,
                 size_t bytesToSend, TickType_t *ticksToWait) {

  uint32_t data_out = 0;

  setsockopt(socket, 0, SO_SNDTIMEO, ticksToWait, sizeof(ticksToWait));
  data_out = lwip_write(socket, (uint8_t *)pBuffer, bytesToSend);
  if (data_out != bytesToSend) {
    LogError(("Send failed %d\n", data_out));
  }
  return data_out;
}

/**
 * Read bytes through socket
 * @param pNetworkContext
 * @param pBuffer
 * @param bytesToRecv
 * @return
 */
int32_t tcp_read(int socket, void *pBuffer, size_t bytesToRecv, TickType_t *ticksToWait) {
    
    int32_t data_in = 0;

    setsockopt(socket, 0, SO_RCVTIMEO, ticksToWait, sizeof(ticksToWait));
    data_in = read(socket, (uint8_t *)pBuffer, bytesToRecv);

    if (data_in < 0) {
        if (errno == 0) {
            data_in = 0;
        }
    }

    return data_in;
}

/**
 * Connect to remote TCP Socket
 * @param host - Host address
 * @param port - Port number
 * @return true on success
 */
bool tcp_connect_socket(int socket, const char *host, uint16_t port) {
    ip_addr_t xHost;
    err_t res = dns_gethostbyname(host, &xHost, dns_cb, (void *)0);

    if (xSemaphoreTake(xHostDNSFound, TCP_TRANSPORT_WAIT) != pdTRUE) {
        LogError(("DNS Timeout on Connect: %s, %d", host, res));
        // return false;
    }

    return static_connect(socket, &xHost, port);
}

/**
 * @brief Connect to socket previously stored ip address and port number
 * 
 * @param h tcp_handle_t provided by application
 * @return true if socket openned
 * @return false 
 */
static bool static_connect(int sock, ip_addr_t *host, const int port)
{
    struct sockaddr_in serv_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        LogError(("ERROR opening socket\n"));
        return false;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    memcpy(&serv_addr.sin_addr.s_addr, host, sizeof(*host));

    int res = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (res < 0) {
        char *s = ipaddr_ntoa(host);
        LogError(("ERROR connecting %d to %s port %d\n", res, s, port));
        return false;
    }

    int nonblock = 1;
    ioctlsocket(sock, FIONBIO, &nonblock);

    LogInfo(("Connect success\n"));
    return true;
}

/**
 * Get status of the socket
 * @return int <0 is error
 */
int tcp_status(int socket) {
    int error = 0;
    socklen_t len = sizeof(error);
    int retval = getsockopt(socket, SOL_SOCKET, SO_ERROR, &error, &len);
    return error;
}

/**
 * Close the socket
 * @return true on success
 */
bool tcp_close_socket(int socket) {
    closesocket(socket);
    return true;
}

/**
 * Call back function for the DNS lookup
 * @param name - server name
 * @param ipaddr - resulting IP address
 * @param callback_arg - poiter to TCPTransport object
 */
static void dns_cb(const char *name, const ip_addr_t *ipaddr, void *callback_arg) {
    dns_found(name, ipaddr, callback_arg);
}

/**
 * Called when DNS is returned
 * @param name - server name
 * @param ipaddr - ip address of server
 * @param callback_arg - this TCPtransport object
 */
static void dns_found(const char *name, const ip_addr_t *ipaddr, void *callback_arg) {
    ip_addr_t host;
    memcpy(&host, ipaddr, sizeof(host));

    LogInfo(("DNS Found %s copied to xHost %s\n", ipaddr_ntoa(ipaddr), ipaddr_ntoa(&host)));
    xSemaphoreGiveFromISR(xHostDNSFound, NULL);
}