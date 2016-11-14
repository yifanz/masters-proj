#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "loader.h"
#include "vm_mem.h"
#include "macho-parser.h"
#include "conf.h"

int
load_raw(const char *path, uint64_t gpa)
{
    if (path == NULL)
    {
        goto ERROR;
    }

    int f = open(path, O_RDONLY);

    if (f == -1)
    {
        fprintf(stderr, "load_raw(): open %s failed\n", path);
        goto ERROR;
    }

    struct stat st;

    if (stat(path, &st) < 0)
    {
        fprintf(stderr, "load_raw(): stat %s failed\n", path);
        goto ERROR_CLOSE;
    }

    void *buf = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, f, 0);

    if (buf == NULL)
    {
        goto ERROR_CLOSE;
    }

    if (vm_mem_write(gpa, buf, st.st_size) != st.st_size)
    {
        fprintf(stderr, "load_raw(): writing %s to vm mem failed\n", path);
        goto ERROR_MUNMAP;
    }

    munmap(buf, st.st_size);
    close(f);

    return 0;

ERROR_MUNMAP:
    munmap(buf, st.st_size);
ERROR_CLOSE:
    close(f);
ERROR:
    return -1;
}

int
load_mach_obj(const char *path, uint64_t *entryoff)
{
    uint64_t gpa = 0x100000000;
    uint64_t gva = gpa;

    if (path == NULL)
    {
        goto ERROR;
    }

    {
        FILE *f = fopen(path, "r");
        struct entry_point_command entry_point;

        if (!read_entry_point_cmd(f, &entry_point)) {
            printf("load_mach_obj(): entry point: %llx\t stack size: %llx\n",
                    entry_point.entryoff, entry_point.stacksize);
            *entryoff = entry_point.entryoff + gva;
        }

        fclose(f);
    }

    {
        int f = open(path, O_RDONLY);

        struct stat st;
        stat(path, &st);

        void *hva = mmap(NULL, st.st_size, PROT_READ|PROT_WRITE, MAP_PRIVATE, f, 0);

        if (hva == NULL)
        {
            fprintf(stderr, "load_mach_obj(): mmap failed\n");
            goto ERROR;
        }

        size_t num_pages = st.st_size / CONF_PAGE_SIZE;

        if (st.st_size % CONF_PAGE_SIZE > 0)
        {
            num_pages++;
        }

        if (map_gpa(gpa, num_pages, hva))
        {
            fprintf(stderr, "load_mach_obj(): map_gpa failed\n");
            goto ERROR;
        }

        if (map_gva(gva, num_pages, gpa))
        {
            fprintf(stderr, "load_mach_obj(): map_gva failed\n");
            goto ERROR;
        }
    }

    return 0;

ERROR:
    return -1;
}