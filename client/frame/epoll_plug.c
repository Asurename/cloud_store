#include <func.h>
#include "epoll_plug.h"
//·â×°ºÃµÄepoll
int epoll_mod(int epollfd, int op, int events, int fd) {
    struct epoll_event epev;
    epev.events = events;
    epev.data.fd = fd;
    epoll_ctl(epollfd, op, fd, &epev);
    return 0;
}