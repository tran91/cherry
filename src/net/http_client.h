#ifndef __http_client_h
#define __http_client_h

#include "types/id.h"

typedef struct {
    /*
     * @buf: buffer
     */
    void(*response)(id user, id buf);

    void(*error)(id user);
} http_client_callback;

type(http_client);

/*
 * @pid: http_client
 * 
 * example:
 *      static void your_response(id user, id buf)
 *      {
 *          const char *str;
 *          unsigned int len;
 * 
 *          buffer_get_ptr(buf, &str);
 *          buffer_get_length(buf, &len);
 *          ...
 *      }
 * 
 *      static void your_error(id user)
 *      {
 *  
 *      }
 * 
 *      ...
 *      
 *      id user;
 *      
 *      ...
 *  
 *      id client;
 *      http_client_new(&client);
 *      http_client_get(client, "https://www.anysite.com", user, (http_client_callback){
 *          .response = your_response,
 *          .error = your_error
 *      });
 */
void http_client_get(id pid, const char *link, id user, http_client_callback cb);

#endif