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

#define ZWSOCK_ERR_NO_ERROR		0x0
#define ZWSOCK_ERR_RAM			0x1
#define ZWSOCK_ERR_SOCKET		0x2
#define ZWSOCK_ERR_PORT			0x3
#define ZWSOCK_ERR_GEN			0x4
#define ZWSOCK_ERR_NO_MSG		0x5

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _zwssock_t zwssock_t;

ZWSOCK_EXPORT zwssock_t* zwssock_new_router(zctx_t *ctx);
ZWSOCK_EXPORT zwssock_t* zwssock_new_router_err(zctx_t *ctx, int* errorCode);

ZWSOCK_EXPORT int zwssock_destroy(zwssock_t **self_p);

ZWSOCK_EXPORT int zwssock_bind(zwssock_t *self, const char *endpoint);
ZWSOCK_EXPORT int zwssock_bind_err(zwssock_t *self, const char *endpoint, int* errorCode);

ZWSOCK_EXPORT int zwssock_send(zwssock_t *self, zmsg_t **msg_p);

ZWSOCK_EXPORT zmsg_t * zwssock_recv(zwssock_t *self);
ZWSOCK_EXPORT zmsg_t * zwssock_recv_err(zwssock_t *self, int* errorCode);

ZWSOCK_EXPORT void* zwssock_handle(zwssock_t *self);
ZWSOCK_EXPORT void* zwssock_handle_err(zwssock_t *self, int* errorCode);

#ifdef __cplusplus
}
#endif

#endif