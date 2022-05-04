#include"server_main.h"

int __init client_start(void)
{
    printk("Loading module...\n");
    netfilterhook_init();
    return 0;
}

void __exit clinet_end(void)
{
    netfilterhook_exit();
    printk("Goodbye Mr.\n");
}
