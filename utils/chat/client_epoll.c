#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <signal.h>

#define MAX_EVENTS 10
#define MAX_MSG_LEN 100

static char name[MAX_MSG_LEN];
static bool cli_run = true; 
static int 	cli_signo = 0;

void *receive_thread(void *arg) {
    int sockfd = *((int *)arg);
    char buffer[MAX_MSG_LEN];

    while (cli_run) {
        int bytes_received = recv(sockfd, buffer, MAX_MSG_LEN, 0);
        if (bytes_received <= 0) {
            perror("Connection closed or recv error");
            break;
        }
        buffer[bytes_received] = '\0';
        printf("%s", buffer);
    }

    close(sockfd);
    pthread_exit(NULL);
}

void *sended_thread(void *arg) {
    int sockfd = *((int *)arg);
    char input[MAX_MSG_LEN];
    char msg[202];

    while (cli_run) {
        fgets(input, MAX_MSG_LEN, stdin);
        
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }

        if( input[0] == '\0'){
            continue;
        }

        if ( strcmp(input, "exit") == 0){
            cli_run = false;
            exit(0);
        }

        snprintf(msg, sizeof(msg), "[%s]:%s", name, input);
        send(sockfd, msg, strlen(msg), 0);
    }

    close(sockfd);
    pthread_exit(NULL);
}


void set_name(int sockfd) {
    while(1){
        printf("your name: ");
        fgets(name, MAX_MSG_LEN, stdin);

        size_t len = strlen(name);

        if( name[0] == '\0'){
            continue;
        }
        
        if (len > 0 && name[len - 1] == '\n') {
            name[len - 1] = '\0';
        }

        send(sockfd, name, strlen(name), 0);
        break;
    }
}

void chat_access()
{
    system("clear");
    fflush(stdout);
    printf("====================================================\n");
    printf("            welcome to chat application!            \n");
    printf("====================================================\n");
}

void cli_option(int argc, char **argv)
{
    printf("[MAIN] server main arguments option processing\n");
}

void signal_action(int signo)
{
    cli_signo = signo;
    cli_run = false;
}

void cli_signal()
{
    printf("[MAIN] server main signal processing\n");
    struct sigaction sigact;

    sigact.sa_handler = signal_action;

    sigaction(SIGINT, &sigact, NULL);
}

int main(int argc, char **argv) {
    int sockfd;
    struct sockaddr_in server_addr;
    struct epoll_event event, events[MAX_EVENTS];

    // 서버 소켓 생성
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) 
    {
        perror("Socket creation failed");
        return EXIT_FAILURE;
    }

    // 서버 주소 설정
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(3000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 서버에 연결
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) 
    {
        perror("Connection failed");
        close(sockfd);
        return EXIT_FAILURE;
    }

    // epoll 생성
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) 
    {
        perror("Epoll creation failed");
        close(sockfd);
        return EXIT_FAILURE;
    }

    event.events = EPOLLIN | EPOLLET; // 수신 및 에지 트리거 설정
    event.data.fd = sockfd;

    // epoll에 소켓 등록
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &event) == -1) 
    {
        perror("Epoll control failed");
        close(epoll_fd);
        close(sockfd);
        return EXIT_FAILURE;
    }

    // 유저 닉네임 등록
    set_name(sockfd);

    // 채팅 어플리케이션 접속
    chat_access();

    // send, recv thread 실행
    pthread_t recv_thread, send_thread;
    pthread_create(&recv_thread, NULL, receive_thread, (void *)&sockfd);
    pthread_create(&send_thread, NULL, sended_thread, (void *)&sockfd);

    pthread_join(recv_thread, NULL);
    pthread_join(send_thread, NULL);

    cli_option(argc, argv);
    cli_signal();

    // clean up function
    close(epoll_fd);
    close(sockfd);

    return EXIT_SUCCESS;
}
