#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_EVENTS 10
#define MAX_MSG_LEN 100

static char name[MAX_MSG_LEN];

void *receive_thread(void *arg) {
    int sockfd = *((int *)arg);
    char buffer[MAX_MSG_LEN];

    // FIXME: max buffer size 조절해야함
    while (1) {
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

    // TODO: Enter exception -> func '\n' ? while : return;
    while (1) {
        fgets(input, MAX_MSG_LEN, stdin);
        size_t len = strlen(name);
        if (len > 0 && name[len - 1] == '\n') {
            name[len - 1] = '\0'; // 개행 문자를 널 문자로 대체
        }
        snprintf(msg, sizeof(msg), "[%s]:%s", name, input);
        send(sockfd, msg, strlen(msg), 0);
    }

    close(sockfd);
    pthread_exit(NULL);
}


int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    struct epoll_event event, events[MAX_EVENTS];

    // 서버 소켓 생성
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        return EXIT_FAILURE;
    }

    // 서버 주소 설정
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(3000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 서버에 연결
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        close(sockfd);
        return EXIT_FAILURE;
    }

    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("Epoll creation failed");
        close(sockfd);
        return EXIT_FAILURE;
    }

    event.events = EPOLLIN | EPOLLET; // 수신 및 에지 트리거 설정
    event.data.fd = sockfd;

    // epoll에 소켓 등록
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &event) == -1) {
        perror("Epoll control failed");
        close(epoll_fd);
        close(sockfd);
        return EXIT_FAILURE;
    }

    // TODO: name 정규식 검사
    name[MAX_MSG_LEN];
    printf("your name : ");
    fgets(name, MAX_MSG_LEN, stdin);
    send(sockfd, name, strlen(name), 0);
    name[MAX_MSG_LEN-1] = '\0';
    printf("\n");

    // TODO: 콘솔 클리어 후 로고 출력 함수 따로 만들기
    system("clear");
    fflush(stdout);
    printf("====================================================\n");
    printf("            welcome to chat application!            \n");
    printf("====================================================\n");

    pthread_t recv_thread, send_thread;
    pthread_create(&recv_thread, NULL, receive_thread, (void *)&sockfd);
    pthread_create(&send_thread, NULL, sended_thread, (void *)&sockfd);

    pthread_join(recv_thread, NULL);
    pthread_join(send_thread, NULL);

    // while(1){
    //     sleep(1);
    // }

    close(epoll_fd);
    close(sockfd);
    return EXIT_SUCCESS;
}
