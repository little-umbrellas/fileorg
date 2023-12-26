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

int
startswith(char *prefix, char *str)
{
    int i, j;
    char *p_prefix = prefix;

    for (i = 0; *p_prefix++; i++);
    for (j = 0; *str++ == *prefix++; j++);

    if (i == j)
        return 1;

    return 0;
}

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
    char *srcpath, *dstpath;
    int res = 0;

    while ((file = readdir(dir))) {
        while (*p_dst) {
            if (strcmp(*p_dst++, "ext")) 
                break;

            if (!startswith(*p_dst++, file->d_name)) {
                (*p_dst) && (p_dst += 2);
                continue;
            }

            if (!(*p_dst))
                break;

            if (strcmp(*p_dst++, "dir"))
                break;

            formatpaths(file->d_name, src, *p_dst, &srcpath, &dstpath);

            if (movefile(srcpath, dstpath))
                res = errno;

            free(srcpath);
            free(dstpath);
        }
        p_dst = dst;
    }

    return res;
}
