#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>

#include <inf/inf_svr_sock.h>

static bool svr_run = true; 
static int 	svr_signo = 0;

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

void cipc_help(void) {
    printf("Help Message\n");
    printf("\t-h : help message\n");
    printf("\n");
}

void svr_option()
{
    printf("[MAIN] server main arguments option processing\n");
}

void signal_action(int signo)
{
    svr_signo = signo;
    svr_run = false;
}

void svr_signal()
{
    printf("[MAIN] server main signal processing\n");
    struct sigaction sigact;

    sigact.sa_handler = signal_action;

    sigaction(SIGINT, &sigact, NULL);
}

int main(int argc, char **argv) 
{
    svr_option(argc, argv);
    svr_signal();

    svr_init();

    while(svr_run){
        printf("[MAIN] Keepalive\n");
        sleep(3);
    }

    svr_cleanup();
    return 0;
}
