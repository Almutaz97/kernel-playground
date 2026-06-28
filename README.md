# M3 HTTP Packet Logger and Blacklist Kernel Module

## 1. Project Overview

This project implements an out-of-tree Linux kernel module for the M3 Software Defined Networking assignment. This means that the module is not part of the main Linux kernel source code. Instead, it is developed separately, compiled into a loadable `.ko` file, and inserted into the running kernel at runtime. This approach allows us to extend kernel behavior without modifying or rebuilding the full Linux kernel.

The implemented module is called `snf_lkm`. It uses the Linux Netfilter framework to inspect IPv4 TCP packets before they are delivered to the local system. When a TCP packet is addressed to destination port `80`, the module classifies it as HTTP traffic. If the source IP address is not blacklisted, the packet is accepted and a kernel log message is produced. If the source IP address exists in the runtime blacklist, the packet is dropped and the drop event is logged.

The blacklist is controlled at runtime through a `/proc` interface:

```text
/proc/snf_blacklist
```

This means the module does not need to be recompiled or reloaded every time the blacklist changes. A user can write an IP address into `/proc/snf_blacklist`, and the module will immediately use that address to decide whether HTTP traffic should be accepted or dropped.

The main implementation file is:

```text
kernel/modules/snf_lkm.c
```

The module was built inside the provided container-based development environment and tested inside a QEMU virtual machine. This is important because kernel modules run with high privileges inside the kernel. Testing the module inside a VM keeps the host system safe: if the module crashes the kernel, only the virtual machine is affected, not the host operating system.

## 2. Repository and Branch Information

The work was developed inside the `kernel-playground` repository, which is the repository required for Linux kernel/module projects in this assignment.

The working repository path inside the development environment is:

```text
/opt/kernel-playground
```

The project is developed on the following fork:

```text
https://github.com/Almutaz97/kernel-playground
```

The working branch for this module is:

```text
m3-http-packet-logger
```

The main implementation file is:

```text
kernel/modules/snf_lkm.c
```

The final documentation is written directly in the root `README.md` file. No separate documentation file is used for this project. Screenshots and evidence images, when needed, can be stored under:

```text
docs/images/
```

The intended final delivery state is that this branch contains the implemented kernel module, the full root README documentation, and the evidence screenshots. After verification, the branch can be merged into the `main` branch of the fork.

