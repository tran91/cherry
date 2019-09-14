#include "types/map.h"
#include "types/buffer.h"
#include "types/cmath.h"
#include "types/thread.h"
#include "net/http_client.h"
#include "net/socket.h"

#if OS != WEB

/*
 * server side
 */
static void add_connection(id server, id sock)
{

}

static void read_data(id server, id sock, id buf)
{
    char msg[] = "Comming soon.";
    int rv;
    socket_send(sock, msg, strlen(msg), &rv);
}

static void remove_connection(id server, id sock)
{
    
}

/*
 * client side
 */
static void cresponse(id user, id response)
{
    const char *str;

    buffer_get_ptr(response, &str);
    debug("received: %s\n", str);
}

static void cerror(id user)
{
    debug("error\n");
}

int main(int argc, char **argv)
{
    id client, server;

    if (argc == 1  || strcmp(argv[1], "client") == 0) {
        for (int i = 0; i < 4; ++i) {
            http_client_new(&client);
            http_client_get(client, "https://127.0.0.1:4433", id_null, (http_client_callback){
                .response = cresponse,
                .error = cerror
            });
            release(client);
            usleep(60 * 1000);
        }
    } else {
        socket_new(&server);
        socket_bind_ssl(server, 4433, "res/cert.pem", "res/private.pem");
        socket_run(server, server, (socket_callback){
            .add_connection = add_connection,
            .read_data = read_data,
            .remove_connection = remove_connection
        });
        release(server);
    }    
    return 0;
}

#endif