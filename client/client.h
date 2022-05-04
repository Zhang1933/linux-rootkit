#include<stdio.h>
#include<time.h>
#include<string.h>
#include<errno.h>
#include<arpa/inet.h>
#include<stdint.h>
#include<string.h>
#include<netinet/ip_icmp.h>
#include<sys/time.h>
#include<unistd.h>
#include<signal.h>
#include <stdlib.h>


#define DEBUG(format,arg...)  \
        printf("FILE : %s , LINE : %d" format,__FILE__,__LINE__,##arg);


#define ICMPNORESPONSE 0

//最长命令长度
#define  MaxLen 128

#define MTU 1500
// 1s
#define RECV_TIMEOUT_USEC 100000

#define MaxCmdLen 128


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
