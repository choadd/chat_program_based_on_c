#include <stdio.h>

#include <inf/inf_svr_sock.h>

int inf_svr_init(void);
void inf_svr_cleanup(void);
int inf_svr_send(int fd, void *data, size_t data_size);

static struct inf_svr_device inf_svr_device_pt = {
    .init       = inf_svr_init,
    .cleanup    = inf_svr_cleanup,

    /* operation function */
    .send       = inf_svr_send,
};

int inf_svr_init(void)
{
    int ret = 0;
    printf("server socket init\n");
    return ret;
}

void inf_svr_cleanup(void)
{
    printf("server socket cleanup\n");    
    return ;
}

int inf_svr_send(int fd, void *data, size_t data_size)
{
    int ret = -1;
    printf("server socket send message\n");
    return ret;
}

struct inf_svr_device * inf_svr_device_get_pt(void){
    return &inf_svr_device_pt;
}