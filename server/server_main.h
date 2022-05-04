#include"common.h"
#include"netfilterhook.h"

int __init client_start(void);

void __exit clinet_end(void);

module_init(client_start);
module_exit(clinet_end);

