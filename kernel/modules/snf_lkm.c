#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/in.h>
#include <linux/skbuff.h>
#include <net/net_namespace.h>

#define HTTP_PORT 80

static struct nf_hook_ops snf_nf_hook_ops;

/*
 * Netfilter callback function.
 *
 * This function is called when an IPv4 packet reaches the PRE_ROUTING hook.
 * The module checks whether the packet is TCP and whether its destination
 * port is 80. If yes, it logs the source IP address to the kernel log.
 */
static unsigned int snf_nf_callback(void *priv, struct sk_buff *skb,
                                    const struct nf_hook_state *state)
{
        struct iphdr *iph;
        struct tcphdr *tcph;
        unsigned int ip_hdr_len;

        if (!skb)
                return NF_ACCEPT;

        if (!pskb_may_pull(skb, sizeof(struct iphdr)))
                return NF_ACCEPT;

        iph = ip_hdr(skb);

        if (iph->version != 4)
                return NF_ACCEPT;

        if (iph->protocol != IPPROTO_TCP)
                return NF_ACCEPT;

        ip_hdr_len = iph->ihl * 4;

        if (ip_hdr_len < sizeof(struct iphdr))
                return NF_ACCEPT;

        if (!pskb_may_pull(skb, ip_hdr_len + sizeof(struct tcphdr)))
                return NF_ACCEPT;

        iph = ip_hdr(skb);
        tcph = (struct tcphdr *)((unsigned char *)iph + ip_hdr_len);

        /*
         * Basic requirement:
         * Detect HTTP packets by checking TCP destination port 80.
         *
         * Intermediate requirement:
         * Log the source IP address of the HTTP packet.
         */
        if (tcph->dest == htons(HTTP_PORT)) {
                printk(KERN_INFO
                       "snf_lkm: HTTP packet detected: src=%pI4 dst=%pI4 sport=%u dport=%u\n",
                       &iph->saddr,
                       &iph->daddr,
                       ntohs(tcph->source),
                       ntohs(tcph->dest));
        }

        return NF_ACCEPT;
}

/*
 * Module initialization.
 * This runs when the module is loaded with insmod.
 */
static int __init snf_init(void)
{
        int rc;

        snf_nf_hook_ops.hook = snf_nf_callback;
        snf_nf_hook_ops.hooknum = NF_INET_PRE_ROUTING;
        snf_nf_hook_ops.pf = PF_INET;
        snf_nf_hook_ops.priority = NF_IP_PRI_FIRST;

        rc = nf_register_net_hook(&init_net, &snf_nf_hook_ops);
        if (rc) {
                printk(KERN_ERR "snf_lkm: failed to register Netfilter hook\n");
                return rc;
        }

        printk(KERN_INFO "snf_lkm: HTTP packet logger module loaded\n");
        return 0;
}

/*
 * Module cleanup.
 * This runs when the module is removed with rmmod.
 */
static void __exit snf_exit(void)
{
        nf_unregister_net_hook(&init_net, &snf_nf_hook_ops);

        printk(KERN_INFO "snf_lkm: HTTP packet logger module unloaded\n");
}

module_init(snf_init);
module_exit(snf_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrea Mayer / modified for M3 HTTP packet logging");
MODULE_DESCRIPTION("Linux Netfilter module for detecting IPv4 TCP HTTP packets and logging source IP addresses");
MODULE_VERSION("1.0.0");
