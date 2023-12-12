#ifndef __SVR_MACRO_H__
#define __SVR_MACRO_H__

#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/* memory */
#define MALLOC(x)   malloc(x)
#define FREE(x)     free(x); x = NULL

/* pthread status */
typedef enum pthread_status {
    THREAD_STATUS_NONE      = 0,
    THREAD_STATUS_WAIT      = 1,
    THREAD_STATUS_RUNNING   = 2,
    THREAD_STATUS_EXIT      = 3,
} THREAD_STATUS;

/* thread */
#define THREAD_DEFINE(x)        pthread_t _svr_sock_##x##_th;\
                                THREAD_STATUS _svr_sock_##x##_th_status = THREAD_STATUS_NONE
#define THREAD_CREATE(x, y)     _svr_sock_##x##_th_status = THREAD_STATUS_WAIT;\
                                pthread_create(&_svr_sock_##x##_th, NULL, (void *)_svr_sock_##x##_th_func, #y);\
                                while(_svr_sock_##x##_th_status == THREAD_STATUS_WAIT) { usleep(10000);}
#define THREAD_FUNC(x)          void _svr_sock_##x##_th_func(void *data)
#define THREAD_CHECK(x)         (_svr_sock_##x##_th_status == THREAD_STATUS_RUNNING)
#define THREAD_RUN(x)           _svr_sock_##x##_th_status = THREAD_STATUS_RUNNING
#define THREAD_EXIT(x)          _svr_sock_##x##_th_status = THREAD_STATUS_EXIT;\
                                pthread_join(_svr_sock_##x##_th, NULL)

#endif /*__SVR_MACRO_H__*/