#ifndef __socket_h
#define __socket_h

#include "types/id.h"

typedef struct {
    /*
     * @sock: socket
     */
    void(*add_connection)(id user, id sock);
    /*
     * @buf: buffer
     */
    void(*read_data)(id user, id sock, id buf);
    /*
     * @sock: socket
     */
    void(*remove_connection)(id user, id sock);
} socket_callback;

type(socket);
/*
 * @pid: socket
 * 
 * example:
 *      id sock;
 *      socket_new(&sock);
 *      socket_connect(sock, "192.168.1.96", 5000);
 * 
 * example:
 *      id sock;
 *      socket_new(&sock);
 *      socket_connect(sock, "www.cphilosophy.com", 5000);
 * 
 */
void socket_connect(id pid, const char *address, const unsigned int port);

/*
 * @pid: socket
 *
 * example:
 *      id sock;
 *      socket_new(&sock);
 *      socket_bind(sock, 5000);
 */
void socket_bind(id pid, const unsigned int port);

/*
 * @pid: socket
 * 
 * example:
 *      id sock;
 *      socket_new(&sock);
 *      socket_connect_ssl(sock, "192.168.1.96", 5000);
 * 
 * example:
 *      id sock;
 *      socket_new(&sock);
 *      socket_connect_ssl(sock, "www.cphilosophy.com", 5000);
 * 
 */
void socket_connect_ssl(id pid, const char *address, const unsigned int port);

/*
 * @pid: socket
 *
 * example:
 *      id sock;
 *      socket_new(&sock);
 *      socket_bind_ssl(sock, 5000, "res/cert.pem", "res/private.pem");
 */
void socket_bind_ssl(id pid, const unsigned int port, const char *cert, const char *key);

/*
 * @pid: socket
 *
 * example:
 *      id sock;
 *      socket_new(&sock);
 * 
 *      ...
 * 
 *      char *msg = "Hello World";
 *      int r;
 *      socket_send(sock, msg, strlen(msg), &r);
 *      if (r == 0) {
 *          success code...
 *      } else {
 *          error code...
 *      }
 */
void socket_send(id pid, const char *buf, signed len, signed *ret);

/*
 * @pid: socket
 * @buf: buffer
 * 
 * example:
 *      id sock;
 *      sock_new(&sock);
 * 
 *      ...
 * 
 *      id buf;
 *      buffer_new(&buf);
 * 
 *      ...
 * 
 *      socket_read(sock, buf);
 */
void socket_read(id pid, id buf);

/*
 * @pid: socket
 * 
 * example:
 *      static void your_add_connection(id user, id sock)
 *      {
 *      }
 * 
 *      static void your_read_data(id user, id sock, id buf)
 *      {
 * 
 *      }
 * 
 *      static void your_remove_connection(id user, id sock)
 *      {
 * 
 *      }
 * 
 *      ...
 * 
 *      id sock;
 *      sock_new(&sock);
 *      sock_bind(sock, 5000);
 * 
 *      ...
 * 
 *      id user;
 * 
 *      ...
 * 
 *      socket_run(sock, user, (socket_callback){
 *          .add_connection = your_add_connection,
 *          .read_data = your_read_data,
 *          .remove_connection = your_remove_connection
 *      });
 */
void socket_run(id pid, id user, socket_callback cb);

#endif