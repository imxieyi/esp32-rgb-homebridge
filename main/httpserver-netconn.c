/*
 * httpserver-netconn.c
 *
 *  Created on: 2016Äê12ÔÂ24ÈÕ
 *      Author: xieyi
 */

#include "include/httpserver-netconn.h"

#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/ip.h"
#include "lwip/ip_addr.h"
#include "lwip/ip4_addr.h"
#include "lwip/api.h"

#include "string.h"

#include "esp_log.h"
#include "esp_system.h"
#include "sdkconfig.h"


#ifndef HTTPD_DEBUG
#define HTTPD_DEBUG LWIP_DBG_ON
#endif

const static char http_cmd_hdr[] =
		"HTTP/1.1 200 OK\r\nContent-type: text/plain\r\n\r\n";
//const static char http_index_html[] =
//		"<html><head><title>Congrats!</title></head><body><h1>Welcome to our lwIP HTTP server!</h1><p>This is a small test page, served by httpserver-netconn.</body></html>";

char ipstr[16];

cmd_callback_t cmd_callback;

static void ip2str(uint32_t ip) {
	memset(ipstr,0,sizeof(ipstr));
	uint8_t bytes[4];
	bytes[0] = ip & 0xFF;
	bytes[1] = (ip >> 8) & 0xFF;
	bytes[2] = (ip >> 16) & 0xFF;
	bytes[3] = (ip >> 24) & 0xFF;
	sprintf(ipstr, "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
}

static char* getcmd(char* raw){
	char* st=strstr(raw,"/cmd/");
	if(st==NULL){
		return NULL;
	}
	st+=5;
	char* en=strstr(st,"/");
	char* temp=(char*)malloc(en-st+1);
	memset(temp,0,en-st+1);
	strncpy(temp,st,en-st);
	return temp;
}

/** Serve one HTTP connection accepted in the http thread */
static void http_server_netconn_serve(struct netconn *conn) {
	struct netbuf *inbuf;
	char *buf;
	uint16_t buflen;
	err_t err;

	/* Read the data from the port, blocking if nothing yet there.
	 We assume the request (the part we care about) is in one netbuf */
	err = netconn_recv(conn, &inbuf);

	if (err == ERR_OK) {
		netbuf_data(inbuf, (void**) &buf, &buflen);

		/* Is this an HTTP GET command? (only check the first 5 chars, since
		 there are other formats for GET, and we're keeping it very simple )*/
		if (buflen >= 5 && buf[0] == 'G' && buf[1] == 'E' && buf[2] == 'T'
				&& buf[3] == ' ' && buf[4] == '/') {

			if(getcmd(buf)==NULL){
				ESP_LOGI("httpserver","empty cmd got");
				netconn_write(conn, http_cmd_hdr, sizeof(http_cmd_hdr) - 1,
						NETCONN_NOCOPY);
				netconn_write(conn,"command error",13,NETCONN_NOCOPY);
				goto finish;
			}

			char* ret=cmd_callback(getcmd(buf));
			ESP_LOGI("httpserver","got ret:%s",ret);
			ESP_LOGI("httpserver","ret length:%d",strlen(ret));
			free(ret);

			/* Send the HTML header
			 * subtract 1 from the size, since we dont send the \0 in the string
			 * NETCONN_NOCOPY: our data is const static, so no need to copy it
			 */
			netconn_write(conn, http_cmd_hdr, sizeof(http_cmd_hdr) - 1,
					NETCONN_NOCOPY);

			netconn_write(conn,ret,strlen(ret),NETCONN_NOCOPY);
			/* Send our HTML page */
//			netconn_write(conn, http_index_html, sizeof(http_index_html) - 1,
//					NETCONN_NOCOPY);
		}
	}
	finish:
	/* Close the connection (server closes in HTTP) */
	netconn_close(conn);

	/* Delete the buffer (netconn_recv gives us ownership,
	 so we have to make sure to deallocate the buffer) */
	netbuf_delete(inbuf);
}

/** The main function, never returns! */
static void http_server_netconn_thread(void *pvParameters) {
	ESP_LOGI("httpserver", "server thread started");
	struct netconn *conn, *newconn;
	err_t err;
	struct ip4_addr remoteip;
	LWIP_UNUSED_ARG(pvParameters);

	/* Create a new TCP connection handle */
	conn = netconn_new(NETCONN_TCP);
	if (conn == NULL) {
		ESP_LOGE("httpserver", "invalid conn");
		return;
	}
//  LWIP_ERROR("http_server: invalid conn", (conn != NULL), return;);
	ESP_LOGI("httpserver", "tcp connection inited");

	/* Bind to port 80 (HTTP) with default IP address */
	err=netconn_bind(conn, NULL, 80);
	if(err!=ERR_OK){
		ESP_LOGI("httpserver", "bind to port failed");
		goto fail;
	}

	/* Put the connection into LISTEN state */
	err=netconn_listen(conn);
	if(err!=ERR_OK){
		ESP_LOGI("httpserver", "start listening failed");
		goto fail;
	}

	ESP_LOGI("httpserver", "start listening on port 80");

	do {
		err = netconn_accept(conn, &newconn);
		if (err == ERR_OK) {
			memcpy(&remoteip,&newconn->pcb.ip->remote_ip,sizeof(uint32_t));
			//remoteip=newconn->pcb.ip->remote_ip;
			ip2str(remoteip.addr);
			ESP_LOGI("httpserver","incoming http connection from %s",ipstr);
			http_server_netconn_serve(newconn);
			netconn_delete(newconn);
		}
	} while (err == ERR_OK);
	ESP_LOGI("httpserver", "netconn_accept received error %d, shutting down",
			err);
//  LWIP_DEBUGF(HTTPD_DEBUG,
//    ("http_server_netconn_thread: netconn_accept received error %d, shutting down",
//    err));
	fail:
	netconn_close(conn);
	netconn_delete(conn);
}

/** Initialize the HTTP server (start its thread) */
void http_server_netconn_init(cmd_callback_t callback) {
	cmd_callback=callback;
	xTaskCreatePinnedToCore(&http_server_netconn_thread,"http_server_thread",2048,NULL,5,NULL,1);
//	sys_thread_new("http_server_netconn", http_server_netconn_thread, NULL,
//			DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}
