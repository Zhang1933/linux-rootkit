#ifndef HOOK_SYSCALL_H
#define HOOK_SYSCALL_H

#include"common.h"
#include <linux/ftrace.h>
#include <linux/linkage.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include "show_hide.h"

struct ftrace_hook {
    const char *name;
    void *function;
    void *original;

    unsigned long address;
    struct ftrace_ops ops;
};

int hook_syscall_init(void);
void hook_syscall_exit(void);


#define HOOK(_name, _hook, _orig)   \
{                   \
    .name = (_name),        \
    .function = (_hook),        \
    .original = (_orig),        \
}


#endif
