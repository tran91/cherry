#include "http_client.h"
#include "socket.h"
#include "types/thread.h"
#include "types/bind.h"
#include "types/buffer.h"

#if OS != WEB

struct http_client {
    id tid;
};
make_type(http_client);

static void http_client_init(struct http_client *p, key k)
{
    thread_new(&p->tid);
}

static void http_client_clear(struct http_client *p)
{
    release(p->tid);
}

static void read_url(id link, char address[257], unsigned short *port, char path[8193], unsigned char *ssl)
{
    const char *str, *t, *bl;
    unsigned len;
    char sport[10];

    *port = 80;

    buffer_get_ptr(link, &str);
    if (strncmp(str, "http://", sizeof("http://") - 1) == 0) {
        str += sizeof("http://") - 1;
    } else if (strncmp(str, "https://", sizeof("https://") - 1) == 0) {
        str += sizeof("https://") - 1;
        *port = 443;
        *ssl = 1;
    }

    t = strstr(str, "]:");
    if (t) {
        len = t - (str + 1);
        memcpy(address, str + 1, len);
        address[len] = '\0';
        bl = strstr(t, "/");
        if (bl) {
            strcpy(path, bl);
            memcpy(sport, t + 2, bl - (t + 2));
            sport[bl - (t + 2)] = '\0';
            *port = atoi(sport);
        } else {
            strcpy(path, "/");
            *port = atoi(t + 2);
        }      
        return;
    }

    t = strstr(str, ":");
    if (t) {
        len = t - str;
        memcpy(address, str, len);
        address[len] = '\0';
        bl = strstr(t, "/");
        if (bl) {
            strcpy(path, bl);
            memcpy(sport, t + 1, bl - (t + 1));
            sport[bl - (t + 1)] = '\0';
            *port = atoi(sport);
        } else {
            strcpy(path, "/");
            *port = atoi(t + 1);
        }    
        return;
    }

    t = strstr(str, "/");
    if (t) {
        len = t - str;
        memcpy(address, str, len);
        address[len] = '\0';
        strcpy(path, t);
        return;
    } else {
        strcpy(address, str);
        strcpy(path, "/");
        return;
    }
}

static void get(id client, id link, id user, id callback)
{
    id sck, bid;
    char addr[257];
    char path[8193];
    char url[8193];
    unsigned short port;
    signed len;
    unsigned char ssl;
    signed r;
    http_client_callback *cb;

    /* send */
    ssl = 0;
    read_url(link, addr, &port, path, &ssl);
    socket_new(&sck);
    if (ssl) {
        socket_connect_ssl(sck, addr, port);
    } else {
        socket_connect(sck, addr, port);
    }
    len = sprintf(url, "GET %s HTTP/1.0\r\nHost: %s\r\nConnection: close\r\n\r\n", path, addr);
    debug("http-client: %s\n", url);
    socket_send(sck, url, len, &r);

    if (r == 0) {
        /* read */
        debug("http-client: trying to read\n");
        buffer_new(&bid);
        socket_read(sck, bid);
        release(sck);
        
        /* callback */
        debug("http-client: callback\n");
        bind_get(callback, &cb);
        if (cb->response) {
            cb->response(user, bid);
        }
        release(bid); 
    } else {
        release(sck);
        bind_get(callback, &cb);
        if (cb->error) {
            cb->error(user);
        }
    }
}

void http_client_get(id pid, const char *link, id cid, http_client_callback cb)
{
    struct http_client *raw;
    id job, buf, bnd;

    http_client_fetch(pid, &raw);
    assert(raw != NULL);

    buffer_new(&buf);
    buffer_append(buf, link, strlen(link));

    bind_new(&bnd);
    bind_set_raw(bnd, &cb, sizeof(http_client_callback));

    thread_job_new(&job);
    thread_job_set_callback(job, (thread_job_callback)get);
    thread_job_add_arguments(job, pid);
    thread_job_add_arguments(job, buf);
    thread_job_add_arguments(job, cid);
    thread_job_add_arguments(job, bnd);
    thread_add_job(raw->tid, job);

    release(bnd);
    release(buf);
    release(job);
}

#endif