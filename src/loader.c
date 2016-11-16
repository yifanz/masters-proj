#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "loader.h"
#include "vm_mem.h"
#include "macho_parser.h"
#include "conf.h"
#include "logging.h"

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
        ELOG("open %s failed", path);
        goto ERROR;
    }

    struct stat st;

    if (stat(path, &st) < 0)
    {
        ELOG("stat %s failed", path);
        goto ERROR_CLOSE;
    }

    void *buf = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, f, 0);

    if (buf == NULL)
    {
        goto ERROR_CLOSE;
    }

    if (vm_mem_write(gpa, buf, st.st_size) != st.st_size)
    {
        ELOG("writing %s to vm mem failed", path);
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
            DLOG("entry point: %llx\t stack size: %llx",
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
            ELOG("mmap failed");
            goto ERROR;
        }

        size_t num_pages = st.st_size / CONF_PAGE_SIZE;

        if (st.st_size % CONF_PAGE_SIZE > 0)
        {
            num_pages++;
        }

        if (map_gpa(gpa, num_pages, hva))
        {
            ELOG("map_gpa failed");
            goto ERROR;
        }

        if (map_gva(gva, num_pages, gpa))
        {
            ELOG("map_gva failed");
            goto ERROR;
        }
    }

    return 0;

ERROR:
    return -1;
}
