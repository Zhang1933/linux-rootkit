#ifndef NETFILTERHOOK_H
#define NETFILTERHOOK_H

#include"common.h"
#include <linux/skbuff.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/icmp.h>
#include <linux/netfilter_ipv4.h>

#define MaxCmdLen 128

// request包的结构
struct icmp_echo {
    // header
    uint8_t type;
    uint8_t code;
    uint16_t checksum;

    uint16_t ident;
    uint16_t seq;

    // data
    double sending_ts;
    char magic[MaxCmdLen];
};

void netfilterhook_init(void);
void netfilterhook_exit(void);

unsigned int watch_in(void *priv,
        struct sk_buff *skb,
        const struct nf_hook_state *state);


// packet debug
static  void pkt_hex_dump(struct sk_buff *skb);

#endif

