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

User * add_user(int fd, char *name)
{
    User * user = (User *)malloc(sizeof(User));
    create_user(user, fd, name);

    return user;    
}

THREAD_FUNC(inf_svr){
    int svr_sock, c_sock;
    struct sockaddr_in svr_addr, c_addr;
    socklen_t c_addr_size;
    int str_len;
    char buf[BUFF_SIZE];

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
            break;
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
                    printf("[THREAD] inf_svr -- connected client : %d\n", c_sock);
                    read(event.data.fd, buf, BUFF_SIZE);
                    
                    // 유저 생성 (fd값, 유저이름)
                    user = add_user(c_sock, buf);
                    LInsert(&list, user);


                }
            } else {
                str_len = read(ep_events[i].data.fd, buf, BUFF_SIZE);
                if(str_len == 0){
                    epoll_ctl(ep_fd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
                    close(ep_events[i].data.fd);


                    // TODO: 클라이언트 삭제하는 함수로 분리해야함
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
                    //  send func
                    buf[str_len] = '\0';
                    printf("[THREAD] inf_svr -- Received message from client: %s\n", buf);

                    // TODO: 전체 유저 조회 함수 만들기 -> 클라이언트가 명령으로 list입력시
                    // if(LFirst(&list, &user)){
                    //     show_user(user);
                    //     while(LNext(&list, &user)){
                    //         show_user(user);
                    //     }
                    // }

                    // TODO: 함수 분리해야함 -> broadcase 함수
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
            }
        }
    }

    // if(ep_events != NULL){
    //     FREE(ep_events);
    // }

    // if(svr_sock >= 0){
    //     close(svr_sock);
    //     svr_sock = -1;
    // }
    // printf("server socket thread End!\n");
    // return;
// error:
//     if(ep_events != NULL){
//         FREE(ep_events);
//     }

//     if(svr_sock >= 0){
//         close(svr_sock);
//         svr_sock = -1;
//     }
//     printf("server socket thread error!\n");
//     return;
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

    return ;
}
