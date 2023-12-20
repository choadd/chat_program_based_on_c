#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#include <inf/inf_svr_sock.h>
#include <svr_macro.h>
#include <linkedList/LinkedList.h>
#include <user/User.h>
// #include <user/UserService.h>

#define BUFF_SIZE 128
#define EPOLL_SIZE 10

THREAD_DEFINE(inf_svr);

int inf_svr_init(void);
void inf_svr_cleanup(void);
int inf_svr_send(int fd, void *data, size_t data_size);

static struct inf_svr_device inf_svr_device_pt = {
    .init       = inf_svr_init,
    .cleanup    = inf_svr_cleanup,

    /* operation function */
    .send       = inf_svr_send,
};

struct inf_svr_device * inf_svr_device_get_pt(void){
    return &inf_svr_device_pt;
}

int inf_svr_send(int fd, void *data, size_t data_size)
{
    int ret = -1;
    if(fd > 0){
        ret = send(fd, data, data_size, 0);
    }
    printf("[THREAD] inf_svr -- send message\n");
    return ret;
}

User *add_user(int fd, char *name) {
    User *user = (User *)malloc(sizeof(User));

    if (user != NULL) {
        user->name = (char *)malloc(strlen(name) + 1);
        if (user->name != NULL) {
            name[sizeof(name)-1] = '\0';
            strcpy(user->name, name);
            create_user(user, fd, user->name);
        } else {
            free(user);
            user = NULL;
        }
    }

    return user;
}
void handle_user_req(List list, User *user, char msg[BUFF_SIZE])
{
    if(LFirst(&list, &user)){
        show_user(user);
        while(LNext(&list, &user)){
            show_user(user);
        }
    }
}

THREAD_FUNC(inf_svr){
    int svr_sock, c_sock;
    struct sockaddr_in svr_addr, c_addr;
    socklen_t c_addr_size;
    int str_len;
    char buf[BUFF_SIZE];
    char nick_buf[BUFF_SIZE];

    struct epoll_event *ep_events;
    struct epoll_event event;
    int ep_fd, event_cnt;

    // LinkedList
    List list;
    ListInit(&list);
    User * user;

    svr_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&svr_addr, 0, sizeof(svr_addr));
    svr_addr.sin_family = AF_INET;
    svr_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    svr_addr.sin_port = htons(3000);

    if(svr_sock == -1){
        printf("[THREAD] inf_svr -- server socket creation failed\n");
    }

    if(bind(svr_sock, (struct sockaddr*)&svr_addr, sizeof(svr_addr)) == -1){
        printf("[THREAD] inf_svr -- server socket bind error!\n");
    }

    if(listen(svr_sock, 5) == -1){
        printf("[THREAD] inf_svr -- server socket listen error!\n");
    }

    ep_fd = epoll_create(EPOLL_SIZE);
    ep_events = MALLOC(sizeof(struct epoll_event) *EPOLL_SIZE);

    event.events = EPOLLIN;
    event.data.fd = svr_sock;

    if (epoll_ctl(ep_fd, EPOLL_CTL_ADD, svr_sock, &event)){
        printf("[THREAD] inf_svr -- server socket epoll_ctl_add error!\n");
    }

    THREAD_RUN(inf_svr);
    printf("[THREAD] inf_svr -- server socket thread start!\n");

    while(THREAD_CHECK(inf_svr)){
        event_cnt = epoll_wait(ep_fd, ep_events, EPOLL_SIZE, -1);
        if(event_cnt == -1){
            printf("[THREAD] inf_svr -- epoll wait error!\n");
            continue;
        }

        // thread exit
        if(!THREAD_CHECK(inf_svr)){
            if(LFirst(&list, &user)){
                LRemove(&list);
                while(LNext(&list, &user)){
                    LRemove(&list);
                }
            }
        }

        for (int i = 0; i < event_cnt; i++){
            if(ep_events[i].data.fd == svr_sock){
                c_addr_size = sizeof(c_addr);

                c_sock = accept(svr_sock, (struct sockaddr*)&c_addr, &c_addr_size);
                event.events = EPOLLIN;
                event.data.fd = c_sock;

                if (epoll_ctl(ep_fd, EPOLL_CTL_ADD, c_sock, &event)){
                    close(c_sock);
                    continue;
                } else{
                    read(event.data.fd, nick_buf, BUFF_SIZE);

                    user = add_user(c_sock, nick_buf);
                    LInsert(&list, user);

                    printf("[THREAD] inf_svr -- connected user name : %s\n", user->name);
                }
            } else {
                str_len = read(ep_events[i].data.fd, buf, BUFF_SIZE);
                if(str_len == 0){
                    epoll_ctl(ep_fd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
                    close(ep_events[i].data.fd);

                    // 접속 종료시 클라이언트 삭제
                    if(LFirst(&list, &user)){
                        if(user->fd == ep_events[i].data.fd){
                            LRemove(&list);
                        }while(LNext(&list, &user)){
                            if(user->fd == ep_events[i].data.fd){
                                LRemove(&list);
                            }
                        }
                    }

                    printf("[THREAD] inf_svr -- closed client : %d\n", ep_events[i].data.fd);
                }else{
                    buf[str_len] = '\0';
                    printf("[THREAD] inf_svr -- Received message from client: %s\n", buf);

                    // 접속중인 유저 리스트 조회
                    printf("\n[THREAD] inf_svr -- USER LIST \n\n");
                    if(LFirst(&list, &user)){
                        show_user(user);
                        while(LNext(&list, &user)){
                            show_user(user);
                        }
                    }
                    printf("[THREAD] inf_svr -- USER LIST END \n");

                    // 메세지가 온 클라이언트를 제외하고 브로드캐스트
                    if(LFirst(&list, &user)){
                        if(user->fd != ep_events[i].data.fd){
                            inf_svr_send(user->fd, buf, str_len); 
                        }while(LNext(&list, &user)){
                            if(user->fd != ep_events[i].data.fd){
                                inf_svr_send(user->fd, buf, str_len);
                            } 
                        }
                    }
                }
            } // epoll client connect
        } // epoll for end
    } // epoll while end

    close(svr_sock);
    return;
}

int inf_svr_init(void)
{
    int ret = 0;

    printf("[THREAD] inf_svr -- init\n");
    THREAD_CREATE(inf_svr, NULL);
    
    return ret;
}

void inf_svr_cleanup(void)
{
    printf("[THREAD] inf_svr -- cleanup\n");    

    THREAD_EXIT(inf_svr);

    return;
}
