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
#include "utils.h"

int
movefile(char *src, char *dst)
{
    struct stat metadata;
    ssize_t filesize;
    int srcfile, dstfile;
    int res = 0;

    if (stat(src, &metadata))
        return errno;

    filesize = metadata.st_size;

    if ((srcfile = open(src, O_RDONLY)) == -1)
        res = errno;

    if ((dstfile = open(dst, O_WRONLY|O_CREAT|O_EXCL, 0644)) == -1) {
        res = errno;
        goto clean;
    } 

    while (res < filesize)
        res = sendfile(dstfile, srcfile, 0, filesize);

    if (remove(src))
        res = errno;

    printf("%s -> %s\n", src, dst);

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
parsedir(DIR *dir, char *src, char **dst)
{
    struct dirent *file;
    char **p_dst = dst;
    char *buf, *srcpath, *dstpath;
    int res = 0;

    while ((file = readdir(dir))) {
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

            formatpaths(file->d_name, src, *p_dst, &srcpath, &dstpath);
            if (exphome(dstpath, &buf)) {
                free(dstpath);
                dstpath = buf;
            }

            if (movefile(srcpath, dstpath))
                res = errno;

            free(srcpath);
            free(dstpath);
        }
        p_dst = dst;
    }

    return res;
}
