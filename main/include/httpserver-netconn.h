/*
 * httpserver-netconn.h
 *
 *  Created on: 2016Äê12ÔÂ24ÈÕ
 *      Author: xieyi
 */

#ifndef MAIN_INCLUDE_HTTPSERVER_NETCONN_H_
#define MAIN_INCLUDE_HTTPSERVER_NETCONN_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef char* (* cmd_callback_t)(char *);

void http_server_netconn_init(cmd_callback_t callback);

#ifdef __cplusplus
}
#endif

#endif /* MAIN_INCLUDE_HTTPSERVER_NETCONN_H_ */
