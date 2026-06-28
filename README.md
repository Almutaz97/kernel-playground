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

## 2. Repository Information

The project repository is located on the Ubuntu host at `~/kernel-playground`. For building and testing, the repository is mounted inside the Podman development container at `/opt/kernel-playground`. Most build commands in this project are executed from inside the container using the `/opt/kernel-playground` path.

The main implementation file of this project is:

```text
kernel/modules/snf_lkm.c
```

Screenshots and evidence images, when needed, can be stored under:

```text
docs/images/
```

## 3. Testing and Development Environment

The project was developed and tested using the environment provided by `kernel-playground`. This environment is designed for Linux kernel development and allows kernel modules to be built and tested without loading experimental code directly on the host system.

The development workflow used two isolated layers:

1. A Podman container for building the kernel module.
2. A QEMU virtual machine for loading and testing the kernel module.

The module was built inside the container environment. This container includes the required build tools, kernel headers, compiler, and project files needed to compile the out-of-tree kernel module.

The actual module execution was done inside a QEMU VM. The compiled `.ko` kernel module was loaded inside the VM kernel using `insmod`. All runtime tests, including HTTP traffic detection and blacklist-based packet dropping, were performed inside this virtual machine.

This setup is important because Linux kernel modules run inside kernel space. A bug in kernel-space code can crash the running kernel. By testing inside a QEMU VM, the experiment is isolated from the host operating system. If the module causes a crash, only the VM is affected, while the host system and development container remain safe.

For the network experiment, the QEMU VM used user-mode networking with port forwarding. The relevant VM startup file was:

```text id="duwqvo"
tests/vm/run.sh
```

The QEMU network option was configured as follows:

```bash id="1fqbez"
-netdev user,host=10.0.2.10,id=mynet0,hostfwd=tcp::10022-:22,hostfwd=tcp::10080-:80
```

This configuration created a virtual network between the container side and the QEMU VM.

The option `host=10.0.2.10` made forwarded connections appear inside the VM as coming from the QEMU-side address `10.0.2.10`. This is why the kernel module logs HTTP packets with source IP `10.0.2.10`.

The forwarding rules exposed selected VM ports outside the VM:

```text id="y0sitd"
hostfwd=tcp::10022-:22    forwards port 10022 to VM port 22
hostfwd=tcp::10080-:80    forwards port 10080 to VM port 80
```

The SSH forwarding rule was useful for accessing the VM. The HTTP forwarding rule was necessary for the experiment because it allowed HTTP requests sent to port `10080` from outside the VM to reach port `80` inside the VM.

Since the kernel module detects HTTP traffic by checking TCP destination port `80`, this forwarding rule made it possible to trigger and test the Netfilter hook.

During testing, the observed VM-side network values were:

```text id="lvms9j"
VM IP address:        10.0.2.15
QEMU source address:  10.0.2.10
HTTP port:            80
```

The source IP address `10.0.2.10` was later added to the runtime blacklist to verify that the module could drop HTTP traffic from a selected source.

The source IP address `10.0.2.10` was later added to the runtime blacklist to verify that the module could drop HTTP traffic from a selected source.
