/*
 * Copyright (c) 2016, Yi-Fan Zhang.  All rights reserved.
 * Copyrights licensed under the BSD License.
 * See the accompanying LICENSE file for terms.
 * See the accompanying ATTRIB file for attributions/credits.
 */
#include <unistd.h>
#include <string.h>
#include <libgen.h>

#include "vm_mem.h"
#include "vcpu.h"
#include "loader.h"
#include "conf.h"
#include "logging.h"
#include "emu_syscall.h"

static char* _basename = "hvexec";

static void
usage()
{
    fprintf(stderr,
            "Usage: %s [-i] [-l <log level>] [-o <log file>] [-s <policy agent>]\n"
            "       %*s -k <kernel> executable [args ...] \n"
            "       -i: pause for input on each interposition event\n"
            "       -l: log level can be one of DEBUG, INFO, WARN, ERROR or SILENT (default DEBUG)\n"
            "       -o: log output file path (default stdout)\n"
            "       -s: optional script for defining custom interposition behavior\n"
            "       -k: required path to a raw binary that is executed in the VM before the target executable\n"
            ,_basename, (int)strlen(_basename), "");
}

int
main(int argc, char **argv)
{
    int ret_val = 0;

    enum LOG_LEVEL log_level = DEBUG;
    FILE *log_output = stdout;
    char *kernel_path = NULL;
    int optf;

    _basename = basename(argv[0]);

    while ((optf = getopt(argc, argv, "il:o:k:")) != -1)
    {
        switch (optf)
        {
            case 'i':
                syscall_interactive = 1;
                break;
            case 'l':
                if (!strcmp(optarg, "DEBUG"))
                    log_level = DEBUG;
                else if (!strcmp(optarg, "INFO"))
                    log_level = INFO;
                else if (!strcmp(optarg, "WARN"))
                    log_level = WARN;
                else if (!strcmp(optarg, "ERROR"))
                    log_level = ERROR;
                else if (!strcmp(optarg, "SILENT"))
                    log_level = SILENT;
                else
                {
                    fprintf(stderr, "Unknown logging level %s\n", optarg);
                    usage();
                    return 1;
                }
                break;
            case 'o':
                if ((log_output = fopen(optarg, "a")) == NULL)
                {
                    fprintf(stderr, "Opening log file failed %s", optarg);
                    usage();
                    return 1;
                }
                break;
            case 'k':
                kernel_path = optarg;
                break;
            default:
                fprintf(stderr, "Unknown argument %s\n", optarg);
                usage();
                return 1;
        }
    }

    argc -= optind;
    argv += optind;

    if (kernel_path == NULL)
    {
        fprintf(stderr, "-k <kernel> is a required argument\n");
        usage();
        return 1;
    }

    if (argc < 1)
    {
        fprintf(stderr, "no target executable\n");
        usage();
        return 1;
    }

    char *app_path = argv[0];

    argc--;
    argv++;

    init_logging(log_level, log_output);

	if (hv_vm_create(HV_VM_DEFAULT))
    {
        ELOG("hv_vm_create failed");
        ret_val = 1;
        goto VM_DESTROY;
	}
    
    if (vm_mem_init(CONF_INIT_NUM_PAGES))
    {
        ELOG("vm_mem_init failed");
        ret_val = 1;
        goto VM_MEM_DESTROY;
    }

    hv_vcpuid_t vcpu;

    if (hv_vcpu_create(&vcpu, HV_VCPU_DEFAULT))
    {
        ELOG("hv_vcpu_create failed");
        ret_val = 1;
        goto VCPU_DESTROY;
    }

    if (vcpu_setup_ia32(vcpu))
    {
        ELOG("vcpu_setup_ia32 failed");
        ret_val = 1;
        goto VCPU_DESTROY;
    }

    // Load the kernel
    if (load_raw(kernel_path, CONF_START_ADDR))
    {
        ELOG("load_raw failed");
        goto VCPU_DESTROY;
    }

    uint64_t entry_gva;

    // Load the application
    if (load_mach_obj(app_path, &entry_gva))
    {
        ELOG("load_mach_obj failed");
        goto VCPU_DESTROY;
    }

    // Setup args to the application's main
    uint64_t argv_addr = CONF_ARGV_ADDR;
    uint64_t args_addr = CONF_ARGS_ADDR;

    for (int i = 0; i < argc; i++)
    {
        char *str = argv[i];
        size_t nbytes;

        nbytes = sizeof args_addr;

        if (nbytes != vm_mem_write(argv_addr, &args_addr, nbytes))
        {
            ELOG("Setup main args failed");
            goto VCPU_DESTROY;
        }

        argv_addr += nbytes;
        nbytes = strlen(str) + 1;

        if (nbytes != vm_mem_write(args_addr, str, nbytes))
        {
            ELOG("Setup main args failed");
            goto VCPU_DESTROY;
        }

        args_addr += nbytes;
    }

    // Put application's argv on the stack
    push64(vcpu, CONF_ARGV_ADDR);

    // Put application's argc on the stack
    push64(vcpu, argc);

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
    DLOG("Ready to launch");
    vcpu_dump(vcpu);

    vcpu_run(vcpu);

VCPU_DESTROY:
    if (hv_vcpu_destroy(vcpu))
    {
        ELOG("hv_vcpu_destroy failed");
        ret_val = 1;
    }

VM_MEM_DESTROY:
    vm_mem_destroy();

VM_DESTROY:
	if (hv_vm_destroy())
    {
        ELOG("hv_vm_destroy failed");
        ret_val = 1;
	}

	return ret_val;
}
