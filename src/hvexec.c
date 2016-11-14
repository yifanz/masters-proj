#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <Hypervisor/hv.h>
#include <Hypervisor/hv_vmx.h>

#include "vm_mem.h"
#include "vcpu.h"
#include "macho-parser.h"
#include "loader.h"
#include "conf.h"

int
main(int argc, char **argv)
{
    int ret_val = 0;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: hvexec boot basic.o\n");
        return 1;
    }

	if (hv_vm_create(HV_VM_DEFAULT))
    {
        fprintf(stderr, "main(): hv_vm_create failed\n");
        ret_val = 1;
        goto VM_DESTROY;
	}
    
    if (vm_mem_init(CONF_INIT_NUM_PAGES))
    {
        fprintf(stderr, "main(): vm_mem_init failed\n");
        ret_val = 1;
        goto VM_MEM_DESTROY;
    }

    hv_vcpuid_t vcpu;

    if (hv_vcpu_create(&vcpu, HV_VCPU_DEFAULT))
    {
        fprintf(stderr, "main(): hv_vcpu_create failed\n");
        ret_val = 1;
        goto VCPU_DESTROY;
    }

    if (vcpu_setup_ia32(vcpu))
    {
        fprintf(stderr, "main(): vcpu_setup_ia32 failed\n");
        ret_val = 1;
        goto VCPU_DESTROY;
    }

    // Load the kernel
    if (load_raw(argv[1], CONF_START_ADDR))
    {
        fprintf(stderr, "main(): load_raw failed\n");
        goto VCPU_DESTROY;
    }

    uint64_t entry_gva;

    // Load the application
    if (load_mach_obj(argv[2], &entry_gva))
    {
        fprintf(stderr, "main(): load_mach_obj failed\n");
        goto VCPU_DESTROY;
    }

    // Put address of application entry point on the stack
    push64(vcpu, entry_gva);

    vm_mem_dump();

    /*
    {
#define DYLD_ADDR 0x0000000000200000
#define DYLD_PATH "/usr/lib/dyld"
        int f = open(DYLD_PATH, O_RDONLY);
        struct stat st;
        stat(DYLD_PATH, &st);
        void *buf = mmap(NULL, st.st_size - 0x1000,
                PROT_READ, MAP_PRIVATE, f, 0x1000);
        if (wmem(DYLD_ADDR, buf, st.st_size - 0x1000) != st.st_size - 0x1000)
        {
            fprintf(stderr, "main(): loading %s failed\n", DYLD_PATH);
        }
        munmap(buf, st.st_size - 0x1000);
        close(f);
    }

    {
#define START_ADDR_EXE 0x0000000000400000
#define KSTACK_ADDR 0x0000000000020000
        int f = open(argv[2], O_RDONLY);
        struct stat st;
        stat(argv[2], &st);
        void *buf = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, f, 0);
        if (wmem(START_ADDR_EXE, buf, st.st_size) != st.st_size)
        {
            fprintf(stderr, "main(): loading %s failed\n", argv[2]);
        }
        munmap(buf, st.st_size);
        close(f);
    }

    {
        FILE *f = fopen(argv[2], "r");
        struct entry_point_command entry_point;
        if (!read_entry_point_cmd(f, &entry_point)) {
            printf("entry point: %llx\t stack size: %llx\n", entry_point.entryoff, entry_point.stacksize);
            // *((uint64_t*) ((uint64_t) vm_mem + KSTACK_ADDR - 8)) = entry_point.entryoff;
            // *((uint64_t*) ((uint64_t) vm_mem + KSTACK_ADDR - 8)) = 0x7fff5fc01000;
            // *((uint64_t*) ((uint64_t) vm_mem + KSTACK_ADDR - 16)) = 0x100000000;


            // *((uint64_t*) ((uint64_t) vm_mem + DYLD_ADDR + 0x22bee)) = 0xC1010f;
            wmem64(KSTACK_ADDR - 8, 0x7fff5fc01000);
            wmem64(KSTACK_ADDR - 16, 0x100000000);
            wmem32(DYLD_ADDR + 0x22bee, 0xC1010f);
        }
        fclose(f);
    }
*/
    printf("Ready to launch\n");
    vcpu_dump(vcpu);

    vcpu_run(vcpu);

VCPU_DESTROY:
    if (hv_vcpu_destroy(vcpu))
    {
        fprintf(stderr, "main(): hv_vcpu_destroy failed\n");
        ret_val = 1;
    }

VM_MEM_DESTROY:
    vm_mem_destroy();

VM_DESTROY:
	if (hv_vm_destroy())
    {
        fprintf(stderr, "main(): hv_vm_destroy failed\n");
        ret_val = 1;
	}

	return ret_val;
}
