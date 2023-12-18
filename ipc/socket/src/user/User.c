#include <stdio.h>
#include <user/User.h>

void create_user(User * user, int fd, char *name)
{
    user->fd = fd;
    user->name = name;
}

void show_user(User * user)
{
    printf("[ user fd : %d , user name : %s ]\n", user->fd, user->name);
}
