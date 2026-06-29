## 1. M3 — HTTP Packet Logger - Project Overview

This project implements **M3 — HTTP Packet Logger**, a Linux kernel module developed inside the `kernel-playground` repository.

The goal of this module is to inspect network traffic at kernel level and detect HTTP packets. HTTP traffic normally uses TCP destination port `80`, so the module checks IPv4 TCP packets and identifies packets whose destination port is `80`.

When an HTTP packet is detected, the module writes information about the packet to the Linux kernel log. The logged information includes:

* source IP address
* destination IP address
* source TCP port
* destination TCP port

The module is named `snf_lkm` and is implemented as an **out-of-tree kernel module**. This means it is built separately from the main Linux kernel source tree and then loaded into the running kernel as a `.ko` module.

The module uses the Linux **Netfilter** framework to receive packets while they pass through the kernel networking stack. It does not modify or block traffic. Every packet is accepted using `NF_ACCEPT`.

## 2. Implemented Scope

The original M3 project can be divided into three possible levels:

| Level        | Requirement                                                 | Status          |
| ------------ | ----------------------------------------------------------- | --------------- |
| Basic        | Detect HTTP packets on port `80`                            | Implemented     |
| Intermediate | Log HTTP packets' source IPs to the kernel log              | Implemented     |
| Advanced     | Drop HTTP packets based on an IP blacklist loaded from file | Not implemented |

For this delivery, I decided to implement only the **Basic** and **Intermediate** levels.

The final version of the module detects HTTP packets by checking whether an IPv4 TCP packet has destination port `80`. When such a packet is detected, the module logs packet information to the kernel log.

The logged information includes:

* source IP address
* destination IP address
* source TCP port
* destination TCP port

The **Advanced** level was intentionally removed from the final version to keep the project simpler, easier to explain, and focused on the selected scope.

Therefore, the final module:

* does not use an IP blacklist
* does not create `/proc/snf_blacklist`
* does not load IP addresses from a file
* does not drop or block packets
* accepts all packets using `NF_ACCEPT`

This means the module is only a packet logger. It observes HTTP traffic and writes information to the kernel log, but it does not change the behavior of the network connection.
