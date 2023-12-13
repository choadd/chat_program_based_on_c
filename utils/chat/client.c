#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <fcntl.h>

#define BUFF_SIZE 128

static int sock;
static char nickname[BUFF_SIZE];

// TODO: epoll 기반의 클라이언트 코드로 재 작성해야함.

int test(){
    char input[BUFF_SIZE];

    while(1) {
        printf(" [%s] : ", nickname);
        fgets(input, BUFF_SIZE, stdin);
        
        input[strcspn(input, "\n")] = '\0';
        
        if (input[0] == '\0') {
            continue;
        }

        if (strcmp(input, "exit") == 0) {
            printf("Exit command detected\n");
            send(sock, input, strlen(input), 0);
            exit(0);
        }

        if (strlen(nickname) == 0){
            memcpy(nickname, input, sizeof(nickname));
        }

        if (send(sock, input, strlen(input), 0) == -1) {
            perror("Send input error");
        } else {
            continue;
        }
    }

    return 0;    
}

int main() {
    struct sockaddr_in serv_addr;
    char message[BUFF_SIZE];

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Socket creation failed.\n");
        return -1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(3000);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        printf("Connection failed.\n");
        return -1;
    }

    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);

    pthread_t async_th;
    if (pthread_create(&async_th, NULL, (void *)test, NULL) != 0) {
        printf("error create input thread!\n");
        exit(EXIT_FAILURE);
    }

    while(1){
        sleep(1);
        // printf("wait");
        while (strlen(nickname) != 0) {
            int str_len = read(sock, message, sizeof(message) - 1);
            memcpy(message, message, sizeof(message));

            if (str_len <= 0) {
                printf("Server closed connection.\n");
                break;
            }

            message[str_len] = '\0';
            printf("\nMessage from server: %s", message);
        }
    }
    close(sock);
    return 0;
}
