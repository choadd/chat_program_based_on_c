#ifndef __USER_H__
#define __USER_H__

typedef struct _user
{
    int fd;
    char *name;
} User;

void create_user(User * user, int fd, char *name);
void show_user(User * user);

// whisper
// void find_username_by_sockfd(User * user, int fd); 

#endif /* __USER_H__ */