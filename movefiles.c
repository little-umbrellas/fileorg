#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <unistd.h>

#include "movefiles.h"
#include "exphome.h"

int
movefile(char *src, char *dst)
{
    struct stat metadata;
    ssize_t filesize;
    int srcfile, dstfile;
    int res = 0, sent = 0;

    if (stat(src, &metadata))
        return 0;

    filesize = metadata.st_size;

    if ((srcfile = open(src, O_RDONLY)) == -1)
        return 0;

    if ((dstfile = open(dst, O_WRONLY|O_CREAT|O_EXCL, 0644)) == -1)
        goto clean;

    while (sent < filesize)
        sent = sendfile(dstfile, srcfile, 0, filesize);

    if (remove(src))
        goto clean;

    printf("%s -> %s\n", src, dst);

    res = 1;
    close(dstfile);
 clean:
    close(srcfile);
    return res;
}

void
formatpaths(char *filename, char *src, char *dst, char **srcpath, char **dstpath)
{
    *srcpath = malloc(strlen(src) + strlen(filename) + 2);
    *dstpath = malloc(strlen(dst) + strlen(filename) + 2);

    sprintf(*srcpath, "%s/%s", src, filename);
    sprintf(*dstpath, "%s/%s", dst, filename);
}

int
parsedirs(char **src, char **dst)
{
    DIR *srcdir;
    struct dirent *file;
    char *buf = NULL;
    char **p_src = src;
    char **p_dst = dst;
    char *srcpath, *dstpath;

    while (*p_src) {
        if (strcmp(*p_src, "dir"))
            return 1;
        p_src++;

        if (exphome(*p_src, &buf)) {
            free(*p_src);
            *p_src = buf;
        }

        if (!(srcdir = opendir(*p_src)))
            return 0;

        while ((file = readdir(srcdir))) {
            while (*p_dst) {
                if (strcmp(*p_dst, "ext")) 
                    break;
                p_dst++;

                if (strncmp(*p_dst, file->d_name, strlen(*p_dst))) {
                    (*++p_dst) && (p_dst += 2);
                    continue;
                }
                p_dst++;

                if (!*p_dst)
                    break;

                if (strcmp(*p_dst, "dir"))
                    break;
                p_dst++;

                formatpaths(file->d_name, *p_src, *p_dst, &srcpath, &dstpath);
                if (exphome(dstpath, &buf)) {
                    free(dstpath);
                    dstpath = buf;
                }

                if (!movefile(srcpath, dstpath))
                    return 0;

                free(srcpath);
                free(dstpath);
            }
            p_dst = dst;
        }
        closedir(srcdir);
        p_src++;
    }

    return 1;
}
