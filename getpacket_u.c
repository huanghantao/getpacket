#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<linux/netlink.h>
#include<sys/types.h>
#include<unistd.h>

#define MYPROTO 29
#define MYGRP 2 // 用户空间的进程和内核所在的组号

int create_nl_socket()
{
    int sock = socket(AF_NETLINK, SOCK_RAW, MYPROTO);
    struct sockaddr_nl addr;

    memset((void *)&addr, 0, sizeof(addr));

    if (sock<0) {
        return sock;
    }

    addr.nl_family = AF_NETLINK;
    addr.nl_pid = getpid();
    addr.nl_groups = MYGRP;

    if (bind(sock,(struct sockaddr *)&addr,sizeof(addr))<0) {
        return -1;
    }

    return sock;
}

int read_msg(int sock, char *buf, int len)
{
    int ret = 0;
    struct nlmsghdr *nlh = NULL;
    struct sockaddr_nl addr;
    struct iovec iov;
    struct msghdr msg;

    memset(&addr, 0, sizeof(struct sockaddr_nl));
    memset(&iov, 0, sizeof(struct iovec));
    memset(&msg, 0, sizeof(struct msghdr));

    nlh = (struct nlmsghdr*)malloc(NLMSG_SPACE(len));
    if(nlh == NULL)
        return -1;
    memset(nlh, 0, sizeof(NLMSG_SPACE(len)));

    iov.iov_base    = nlh; /* iov_base指向一个缓冲区(这里是用来存放数据) */
    iov.iov_len     = NLMSG_SPACE(len);

    msg.msg_name        = &addr;
    msg.msg_namelen     = sizeof(struct sockaddr_nl);
    msg.msg_iov         = &iov; /* 多io缓冲区的地址 */
    msg.msg_iovlen      = 1; /* 缓冲区的个数 */
    msg.msg_control     = NULL;  /* 辅助数据的地址 */ 
    msg.msg_controllen  = 0; /* 辅助数据的长度 */
    msg.msg_flags       = 0; /* 接收消息的标识 */

    ret = recvmsg(sock, &msg, 0);
    memcpy(buf, NLMSG_DATA(nlh), len);

    if(nlh) {
        free(nlh);
    }

    return ret;
}

int main(int argc, char *argv[])
{
    char buf[2048] = {0};

    int nls = create_nl_socket();
    if (nls < 0) {
        printf("Error creating socket.\n");
        
        return 0;
    }

    while (1) {
        read_msg(nls, buf, sizeof(buf));
        printf("%s\n", buf);
    }

    return 0;
}