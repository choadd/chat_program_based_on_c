#include <stdio.h>
#include <unistd.h>

#include <inf/inf_svr_sock.h>

void svr_init(void)
{
    printf("[MAIN] server main init function\n");
    INF_SVR_DEV *inf = NULL;

    inf = INF_SVR_DEV_PT();
    inf->init();
}

void svr_cleanup()
{
    printf("[MAIN] server main cleanup function\n");
    INF_SVR_DEV *inf = NULL;

    inf = INF_SVR_DEV_PT();
    inf->cleanup();
}

void svr_option()
{
    printf("[MAIN] server main arguments option processing\n");
}

void svr_signal()
{
    printf("[MAIN] server main signal processing\n");
}

//int argc, char **argv
int main() 
{
    svr_option();
    svr_signal();

    svr_init();

    while(1){ // chat server is run?
        printf("[MAIN] Keepalive\n");
        sleep(3);
    }

    svr_cleanup();

    return 0;
}
