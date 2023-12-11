#ifndef __INF_SVR_SOCK_H__
#define __INF_SVR_SOCK_H__

typedef struct inf_svr_device {
    int (*init)(void);
    void (*cleanup)(void);

    /* operation function */
    int (*send)(int fd, void *data, size_t data_size);
} INF_SVR_DEV;

#define INF_SVR_DEV_PT inf_svr_device_get_pt
struct inf_svr_device * inf_svr_device_get_pt(void);

#endif /* __INF_SVR_SOCK_H__ */