#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <net/sock.h>
#include <linux/socket.h>
#include <linux/net.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/skbuff.h>
#include <linux/delay.h>

#include <linux/netdevice.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>


#define NIPQUAD(addr) \
    ((unsigned char *)&addr)[0], \
    ((unsigned char *)&addr)[1], \
    ((unsigned char *)&addr)[2], \
    ((unsigned char *)&addr)[3]

#define NETLINK_USER 29
#define MYGRP 2 // 用户空间的进程和内核所在的组号

/*
 * 用来存放5元组信息的结构体
 * s_ip 源ip地址
 * d_ip 目的ip地址
 * s_port 源端口地址
 * d_port 目的端口地址
 * proto 网络协议类型
*/
struct quintuple {
    char s_ip[20];
    char d_ip[20];
    char s_port[10];
    char d_port[10];
    char proto[10];
};


struct sock *nl_sk = NULL;

static struct nf_hook_ops nfho;
struct iphdr *iph = NULL;
unsigned int sport, dport;
struct tcphdr *tcp_header = NULL;
struct udphdr *udp_header = NULL;

struct sk_buff *sock_buff = NULL;


static void 
send_msg_to_user(struct quintuple *data) {
    printk(KERN_INFO "--------send_msg_to_user---------");
    struct sk_buff *skb;
    struct nlmsghdr *nlh;
    char   s[100];
    char   str[100];
    char   temp[100];
    char   port[100];
    char   temp_port[100];
    int    str_len;
    int    len;
    int    res;
    
    memset(s, '\0', sizeof(s));
    memset(str, '\0', sizeof(str));
    memset(temp, '\0', sizeof(temp));
    memset(port, '\0', sizeof(port));
    memset(temp_port, '\0', sizeof(temp_port));

    sprintf(s, "protocol: %s", data->proto);
    
    sprintf(str, "source ip: %s", data->s_ip);
    sprintf(temp, "dest ip: %s", data->d_ip);
    strcat(str, temp);

    sprintf(port, "source port: %s", data->s_port);
    sprintf(temp_port, "dest port: %s", data->d_port);
    strcat(port, temp_port);

    strcat(str, port);

    strcat(s, str);

    printk(KERN_INFO "%s", s);

    str_len = strlen(s);
    len = str_len + 1;

    skb = nlmsg_new(len, GFP_KERNEL);
    if (!skb) {
        printk(KERN_ERR "Failed to allocate new skb\n");
        return ;
    }

    nlh = nlmsg_put(skb, 0, 7438, 0, len, 0);
    if (!nlh) {
        printk(KERN_ERR "nlmsg_put: couldn't put nlmsghdr\n");
        kfree_skb(skb);

        return ;
    }
    strncpy(NLMSG_DATA(nlh), s, len);

    res = nlmsg_multicast(nl_sk, skb, 0, MYGRP, 0);

    if (res < 0) {
        printk(KERN_INFO "Error while sending bak to user, err id: %d\n", res);
    }
}

unsigned int hook_func(unsigned int hooknum,
                       struct sk_buff **skb,
                       const struct net_device *in,
                       const struct net_device *out,
                       int (*okfn)(struct sk_buff *)
    ) {
    printk(KERN_INFO "--------hook_func---------");

    struct quintuple quint;

    sock_buff = skb;
    memset(&quint, '\0', sizeof(struct quintuple));

    if (!sock_buff) {
        return NF_ACCEPT;
    }

    iph = (struct iphdr *)skb_network_header(sock_buff);
    if (!iph) {
        return NF_ACCEPT;
    }

    if (iph->protocol == IPPROTO_TCP) {
        sprintf(quint.proto, "%s\n", "TCP");
        sprintf(quint.s_ip, "%d.%d.%d.%d\n", NIPQUAD(iph->saddr));
        sprintf(quint.d_ip, "%d.%d.%d.%d\n", NIPQUAD(iph->daddr));

        tcp_header = tcp_hdr(sock_buff);
        sport = htons((unsigned short int) tcp_header->source);
        dport = htons((unsigned short int) tcp_header->dest);
        sprintf(quint.s_port, "%d\n", sport);
        sprintf(quint.d_port, "%d\n", dport);

        send_msg_to_user(&quint);

        return NF_ACCEPT;
    }

    if (iph->protocol == IPPROTO_UDP) {
        sprintf(quint.proto, "%s\n", "UDP");
        sprintf(quint.s_ip, "%d.%d.%d.%d\n", NIPQUAD(iph->saddr));
        sprintf(quint.d_ip, "%d.%d.%d.%d\n", NIPQUAD(iph->daddr));

        udp_header = udp_hdr(sock_buff);
        sport = htons((unsigned short int) udp_header->source);
        dport = htons((unsigned short int) udp_header->dest);
        sprintf(quint.s_port, "%d\n", sport);
        sprintf(quint.d_port, "%d\n", dport);

        send_msg_to_user(&quint);

        return NF_ACCEPT;
    }

    if(iph->protocol == IPPROTO_ICMP) {
        sprintf(quint.proto, "%s\n", "ICMP");
        sprintf(quint.s_ip, "%d.%d.%d.%d\n", NIPQUAD(iph->saddr));
        sprintf(quint.d_ip, "%d.%d.%d.%d\n", NIPQUAD(iph->daddr));

        send_msg_to_user(&quint);

        return NF_ACCEPT;
    }

    return NF_ACCEPT;
}

static int __init
getpacket_init(void) {
    int ret;

    struct netlink_kernel_cfg cfg = {
        .groups = MYGRP,
    };
    printk("Entering: %s\n", __FUNCTION__);

    nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);
    if (!nl_sk) {
        printk(KERN_ALERT "Error creating socket.\n");
        return -10;
    }

    nfho.hook      = hook_func;
    nfho.hooknum   = NF_INET_LOCAL_OUT;
    nfho.pf        = PF_INET;
    nfho.priority  = NF_IP_PRI_FIRST;

    ret = nf_register_hook(&nfho);
    if (ret < 0) {
        printk("Register Error\n");
        return ret;
    }

    return 0;
}

static void __exit
getpacket_exit(void) {
    printk(KERN_INFO "exiting hello module\n\n\n\n\n\n\n");
    netlink_kernel_release(nl_sk);
    nf_unregister_hook(&nfho);
}

module_init(getpacket_init);
module_exit(getpacket_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("huanghantao");
MODULE_DESCRIPTION("Get the packet's five-tuple");
MODULE_VERSION("0.0.1");