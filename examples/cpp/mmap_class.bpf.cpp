// SPDX-License-Identifier: GPL-2.0
// Copyright (c) 2019 Facebook

#include <linux/bpf.h>
#include <stdint.h>
#include <bpf/bpf_helpers.h>

extern "C" {

#include "classdef.h"

char _license[] SEC("license") = "GPL";

typedef char raw_X[sizeof(class X)];

struct {
	__uint(type, BPF_MAP_TYPE_ARRAY);
	__uint(map_flags, BPF_F_MMAPABLE);
	__type(key, __u32);
	__type(value, raw_X);
} data_map SEC(".maps");

int my_pid = 0;

SEC("raw_tracepoint/sys_enter")
int test_mmap(void *ctx)
{
    int zero = 0;
    X *p;
	
    /* Only procced when syscall comes from the userspace prog */
    int pid = bpf_get_current_pid_tgid() >> 32;
    if (pid != my_pid)
        return 0;

    /* Accessing first element of the array */
	p = reinterpret_cast<X *>(bpf_map_lookup_elem(&data_map, &zero));
	
    if (p) {
        bpf_printk("Addr: %p\n", p);
        bpf_printk("values at index %d is: %d\n", zero, p->getX());

        /* Accessing pointer from userspace */
        bpf_printk("Pointer addr: %p\n", p->getP());

        /* Verifier pukes */
        bpf_printk("Pointer val: %d\n", *p->getP());
	}

	return 0;
}

} // extern "C"
