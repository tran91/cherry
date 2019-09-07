#include "socket.h"
#include "types/bind.h"
#include "types/buffer.h"
#include "types/thread.h"

#if OS == LINUX
    #ifndef __USE_GNU
    #define __USE_GNU 1
    #endif

    #ifndef __USE_XOPEN2K
    #define __USE_XOPEN2K 1
    #endif
#endif

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/conf.h>

static volatile unsigned init_ssl = 0;
static volatile unsigned ssl_barrier = 0;

static void __clear_ssl()
{
    if (init_ssl == 0) return;
    init_ssl = 0;

    FIPS_mode_set(0);
    EVP_cleanup();
    SSL_COMP_free_compression_methods();
    COMP_zlib_cleanup();
    ERR_remove_state(0);
    ERR_remove_thread_state(NULL);
    CONF_modules_unload(1);
    ERR_free_strings();
    CRYPTO_cleanup_all_ex_data();
}

static void __init_ssl()
{
    lock(&ssl_barrier);
    if(!init_ssl) {
        init_ssl = 1;
        OpenSSL_add_all_algorithms();
        SSL_load_error_strings();
        SSL_library_init();
        atexit(__clear_ssl);
    }
    unlock(&ssl_barrier);
}

static void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/* ssl_context */
struct ssl_context
{
    SSL_CTX *ctx;
};
make_type_detail(ssl_context);

static void ssl_context_init(struct ssl_context *p, key k)
{
    __init_ssl();
    p->ctx = NULL;
}

static void ssl_context_clear(struct ssl_context *p)
{
    if (p->ctx) {
        SSL_CTX_free(p->ctx);      
        p->ctx = NULL;
    }
}

static void ssl_context_new_client(id *pid)
{
    struct ssl_context *raw;

    ssl_context_new(pid);
    fetch(*pid, &raw);
    lock(&ssl_barrier);
    raw->ctx = SSL_CTX_new(SSLv23_client_method());
    unlock(&ssl_barrier);
    if (raw->ctx == NULL) {
        release(*pid);
        invalidate(pid);
    }
}

static void ssl_context_new_server(id *pid, const char *cert, const char *key)
{
    struct ssl_context *raw;

    ssl_context_new(pid);
    fetch(*pid, &raw);
    lock(&ssl_barrier);
    raw->ctx = SSL_CTX_new(SSLv23_method());
    unlock(&ssl_barrier);
    SSL_CTX_set_ecdh_auto(raw->ctx, 1);
    if (SSL_CTX_use_certificate_file(raw->ctx, cert, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        debug("ssl_context: failed to use certificate\n");
        release(*pid);
        invalidate(pid);
        return;
    }
    if (SSL_CTX_use_PrivateKey_file(raw->ctx, key, SSL_FILETYPE_PEM) <= 0 ) {
        ERR_print_errors_fp(stderr);
        debug("ssl_context: failed to use private key\n");
        release(*pid);
        invalidate(pid);
        return;
    }
}

/* socket */
struct socket
{
    char *address;
    unsigned port;

    int sd;
    SSL *ssl;
    char non_blocking;

    id ctx;
};
make_type(socket);

static void init(struct socket *p, key k)
{
    p->sd = -1;
    p->ssl = NULL;
    p->address = NULL;
    p->port = 0;
    p->ctx = id_null;
    p->non_blocking = 0;
}

static void clear(struct socket *p)
{
    if (p->ssl) {
        SSL_free(p->ssl);
        p->ssl = NULL;
    }
    if (p->sd >= 0) {
        close(p->sd);
        p->sd = -1;
    }
    if (p->address) {
        free(p->address);
        p->address = NULL;
    }
    p->port = 0;    
    release(p->ctx);
    p->ctx = id_null;
    p->non_blocking = 0;
}

static void socket_set_non_blocking(id pid, int *flags)
{
    struct socket *raw;

    fetch(pid, &raw);
    assert(raw != NULL);

	*flags = fcntl(raw->sd, F_GETFL, 0);
	if (*flags == -1) {
		return;
	}

	*flags |= O_NONBLOCK;
	if (fcntl(raw->sd, F_SETFL, *flags) == -1) {
		return;
	}

    raw->non_blocking = 1;
}

void socket_connect(id pid, const char *address, const unsigned port)
{
    struct addrinfo hints, *servinfo = NULL, *p;
    int rv;
    char s[INET6_ADDRSTRLEN], sport[10];
    struct socket *raw;

    fetch(pid, &raw);
    assert(raw != NULL);
    clear(raw);

    raw->address = realloc(raw->address, strlen(address) + 1);
    strcpy(raw->address, address);
    raw->port = port;
    sprintf(sport, "%d", port);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(address, sport, &hints, &servinfo)) != 0) {
        debug("socket-connect: failed to resolve %s\n", address);
        goto finish;
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((raw->sd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            debug("socket-connect: failed to create socket\n");
            continue;
        }
        if (connect(raw->sd, p->ai_addr, p->ai_addrlen) == -1) {
            close(raw->sd);
            raw->sd = -1;            
            continue;
        }
        break;
    }

    if (p == NULL) {
        debug("socket-connect: failed to connect %s:%d\n", address, port);
        goto finish;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    debug("socket-connect: connected to %s:%d ~ [%s]:%d\n", address, port, s, raw->port);

    socket_set_non_blocking(pid, &rv);

finish:
    if (servinfo) {
        freeaddrinfo(servinfo);
    }
}

void socket_connect_ssl(id pid, const char *address, const unsigned port)
{
    struct socket *raw;
    struct ssl_context *sc;
    int rv;

    fetch(pid, &raw);
    assert(raw != NULL);
    socket_connect(pid, address, port);
    if (raw->sd == -1) {
        return;
    }
    ssl_context_new_client(&raw->ctx);
    if (id_validate(raw->ctx)) {
        fetch(raw->ctx, &sc);
        raw->ssl = SSL_new(sc->ctx);
        SSL_set_fd(raw->ssl, raw->sd);

    try_connect: 
        lock(&ssl_barrier);
        rv = SSL_connect(raw->ssl);
        unlock(&ssl_barrier);
        if (rv < 0) {
            rv = SSL_get_error(raw->ssl, rv);
            switch (rv)
            {
            case SSL_ERROR_WANT_READ:
            case SSL_ERROR_WANT_WRITE:
                goto try_connect;
            }
            debug("socket-connect-ssl: failed to connect ssl\n");
            SSL_free(raw->ssl);
            raw->ssl = NULL;
            close(raw->sd);
            raw->sd = -1;
        } else {
            debug("socket-connect-ssl: successfully\n");
        }
    } else {
        close(raw->sd);
        raw->sd = -1;
        debug("socket-connect-ssl: failed to create ssl context\n");
    }
}

void socket_bind(id pid, const unsigned port)
{
    struct socket *raw;
    struct addrinfo hints, *servinfo = NULL, *p;
    int rv, yes = 1;
    char sport[10];

    fetch(pid, &raw);
    assert(raw != NULL);
    clear(raw);

    raw->port = port;
    sprintf(sport, "%d", raw->port);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, sport, &hints, &servinfo)) != 0) {
        debug("socket-bind: failed to resolve\n");
        goto finish;
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((raw->sd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            debug("socket-bind: failed to create socket\n");
            continue;
        }

        if (setsockopt(raw->sd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            raw->sd = -1;
            debug("socket-bind: error on setsockopt\n");
            goto finish;
        }

        if (bind(raw->sd, p->ai_addr, p->ai_addrlen) == -1) {
            close(raw->sd);
            raw->sd = -1;
            continue;
        }

        break;
    }

    if (p == NULL)  {
        debug("socket-bind: failed to bind\n");
        goto finish;
    }

    socket_set_non_blocking(pid, &rv);
    if (rv == -1) {
        debug("socket-bind: failed to set non-blocking\n");
        close(raw->sd);
        raw->sd = -1;
        goto finish;
    }

    if (listen(raw->sd, 10) == -1) {
        debug("socket-bind: failed to listen\n");
        close(raw->sd);
        raw->sd = -1;
        goto finish;
    }

    debug("socket-bind: connected to %d\n", raw->port);

finish:
    if (servinfo) {
        freeaddrinfo(servinfo);
    }
}

void socket_bind_ssl(id pid, const unsigned port, const char *cert, const char *key)
{
    struct socket *raw;

    fetch(pid, &raw);
    assert(raw != NULL);
    socket_bind(pid, port);
    if (raw->sd == -1) {
        return;
    }
    ssl_context_new_server(&raw->ctx, cert, key);
    if (!id_validate(raw->ctx)) {
        close(raw->sd);
        raw->sd = -1;
        debug("socket-bind-ssl: failed to create ssl context\n");
    } else {
        debug("socket-bind-ssl: successfully\n");
    }
}

void socket_send(id pid, const char *buf, signed len, signed *ret)
{
    struct socket *raw;
    unsigned total = 0;
    int bytesleft = len;
    int n;

    fetch(pid, &raw);
    assert(raw != NULL);

    if (raw->sd == -1) {
        *ret = -1;
        return;
    }

    while(total < len) {    
        if (raw->ssl) {
            n = SSL_write(raw->ssl, buf + total, bytesleft);
            if (n <= 0) {
                n = SSL_get_error(raw->ssl, n);
                switch (n) {
                    case SSL_ERROR_WANT_READ:
                    case SSL_ERROR_WANT_WRITE:
                        goto next;
                    default:
                        n = -1;
                        break;
                }
            }
        } else {
            n = send(raw->sd, buf + total, bytesleft, 0);  
            if (n == -1) {
                switch (errno) {
                case EAGAIN:
                    goto next;  
                }
            }               
        }                       
        if (n == -1) {
            break; 
        } 
        total += n;
        bytesleft -= n;
    next:
        n = 0;
    }

    *ret = n;
}

void socket_read(id pid, id buf)
{
    struct socket *raw;
    int n;
    char tmp[4097];

    fetch(pid, &raw);
    assert(raw != NULL);
    buffer_clear(buf);

    if (raw->sd < 0) {
        return;
    }

    if (raw->ssl) {
    ssl_read_again:
        n = SSL_read(raw->ssl, tmp, 4096);
        if (n <= 0) {
            n = SSL_get_error(raw->ssl, n);
            switch (n) {
                case SSL_ERROR_WANT_READ:
                case SSL_ERROR_WANT_WRITE:
                    goto ssl_read_again;
            }         
        } else {
            buffer_append(buf, tmp, n);
        }
    } else {
    read_again:
        n = read(raw->sd, tmp, 4096);
        if (n > 0) {
            buffer_append(buf, tmp, n);
        } else if (n == -1) {
            switch (errno) {
                case EAGAIN:
                    goto read_again;    
            }
        }
    }
}

/* run server */
void socket_run(id pid, id user, socket_callback cb)
{
    struct socket *raw;
    struct socket *client;
    struct ssl_context *ctx;
    struct epoll_event event, *events = NULL;
    int epoll_fd;
    int i, n, rv;
    id *cl;
    struct sockaddr in_addr;
	socklen_t in_len = sizeof(in_addr);
	int infd;
    id buf = id_null;

    fetch(pid, &raw);
    assert(raw != NULL);

    epoll_fd = epoll_create1(0);
	if (epoll_fd == -1) {
        debug("socket-run: error epoll_create1\n");
		goto finish;
	}

    event.data.fd = raw->sd;
	event.events = EPOLLIN | EPOLLET;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, raw->sd, &event) == -1) {
		debug("socket-run: error epoll_ctl\n");
		goto finish;
	}

#define MAX_EVENTS 64
	events = calloc(MAX_EVENTS, sizeof(event));

    while (1) {
        debug("socket-run: waiting...\n");
		n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        debug("____________%d____________\n", n);
		for (i = 0; i < n; i++) {
			if (events[i].events & EPOLLERR || events[i].events & EPOLLHUP || !(events[i].events & EPOLLIN)) {
				debug("socket-run: client closed\n");
                cl = events[i].data.ptr;
                if (cl) {
                    events[i].data.ptr = NULL;
                    if (cb.remove_connection) {
                        cb.remove_connection(user, *cl);
                    }
                    release(*cl);
                    free(cl);
                }
			} else if (events[i].data.fd == raw->sd) {
            try_accept:
                infd = accept(raw->sd, &in_addr, &in_len);
                if (infd == -1) {
                    /*
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        } else {
                            goto try_accept;
                        }
                    */
                } else {
                    /* create new incomming socket */
                    cl = malloc(sizeof(id));
                    socket_new(cl);
                    fetch(*cl, &client);
                    client->sd = infd;
                    client->ctx = raw->ctx;
                    retain(client->ctx);

                    /* set socket non blocking */
                    socket_set_non_blocking(*cl, &rv);
                    if (rv == -1) {
                        debug("socket-run: error non blocking on new connection\n");
                        release(*cl);
                        free(cl);
                        goto next;
                    } else {
                        /* trying to enable socket ssl state */
                        fetch(client->ctx, &ctx);
                        if (ctx) {                            
                            client->ssl = SSL_new(ctx->ctx);
                            SSL_set_fd(client->ssl, client->sd);

                        try_ssl_accept:
                            rv = SSL_accept(client->ssl);
                            if (rv <= 0) {
                                rv = SSL_get_error(client->ssl, rv);
                                switch (rv) {
                                    case SSL_ERROR_WANT_READ:
                                    case SSL_ERROR_WANT_WRITE:
                                        goto try_ssl_accept;
                                    default:
                                        debug("socket-run: error ssl on new connection\n");
                                        release(*cl);
                                        free(cl);
                                        goto next;
                                }
                            }                            
                        }
                        /* trying to enable socket epoll slot */
                        memset(&event, 0, sizeof(event));
                        event.data.fd = infd;
                        event.data.ptr = cl;
                        event.events = EPOLLIN | EPOLLET;
                        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, infd, &event) == -1) {
                            debug("socket-run: error epoll state on new connection\n");
                            release(*cl);
                            free(cl);
                            goto next;
                        }
                        debug("socket-run: accepted new connection\n");
                        if (cb.add_connection) {
                            cb.add_connection(user, *cl);
                        }
                    }    
                next:
                    in_len = sizeof(in_addr);   
                    goto try_accept;
                }
			} else {                
                debug("socket-run: new incomming data\n");
                cl = events[i].data.ptr;
                if (cl) {
                    debug("socket-run: read incomming data\n");
                    buffer_new(&buf);
                    socket_read(*cl, buf);    
                    if (cb.read_data) {
                        cb.read_data(user, *cl, buf);
                    }
                    release(buf);
                }
			}
		}
	}

finish:
    if (events) {
        free(events);
    }
}