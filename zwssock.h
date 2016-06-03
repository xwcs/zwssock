#ifndef __ZWSSOCK_H_INCLUDED__
#define __ZWSSOCK_H_INCLUDED__

#include <czmq.h>


#if defined (__WINDOWS__)
#   if defined zwssock_STATIC
#       define ZWSOCK_EXPORT
#   elif defined zwssock_EXPORTS
#       define ZWSOCK_EXPORT __declspec(dllexport)
#   else
#       define ZWSOCK_EXPORT __declspec(dllimport)
#   endif
#else
#   define ZWSOCK_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _zwssock_t zwssock_t;

ZWSOCK_EXPORT zwssock_t* zwssock_new_router(zctx_t *ctx);

ZWSOCK_EXPORT void zwssock_destroy(zwssock_t **self_p);

ZWSOCK_EXPORT int zwssock_bind(zwssock_t *self, char *endpoint);

ZWSOCK_EXPORT int zwssock_send(zwssock_t *self, zmsg_t **msg_p);

ZWSOCK_EXPORT zmsg_t * zwssock_recv(zwssock_t *self);

ZWSOCK_EXPORT void* zwssock_handle(zwssock_t *self);

#ifdef __cplusplus
}
#endif

#endif