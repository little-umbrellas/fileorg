#include <errno.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wordexp.h>

#include "parseconf.h"
#include "movefiles.h"

void
fileorg(void)
{
    wordexp_t homexp;
    char *confFile = "$HOME/.config/fileorg.conf";
    FILE *confStream;
    char *srcDirname;
    DIR *srcDir;
    char **dstDirname, **p_dstDirname;

    wordexp(confFile, &homexp, 0);
    confFile = *homexp.we_wordv;

    if (!(confStream = fopen(confFile, "r"))) {
        fprintf(stderr, "ERR: Failed to open config file: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (!movetoHeader(confStream, "Source")) {
        fputs("ERR: No source header\n", stderr);
        exit(EXIT_FAILURE);
    }

    if (!gettagInfo(confStream, "dir", &srcDirname)) {
        fputs("ERR: No source directory\n", stderr);
        exit(EXIT_FAILURE);
    }

    if (!(srcDir = opendir(srcDirname))) {
        fprintf(stderr, "ERR: Failed to open source directory: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (!movetoHeader(confStream, "Destination")) {
        fputs("ERR: No destination header\n", stderr);
        exit(EXIT_FAILURE);
    }

    if (!getblockInfo(confStream, &dstDirname)) {
        fputs("ERR: No destination directory\n", stderr);
        exit(EXIT_FAILURE);
    }

    if (parsedir(srcDir, srcDirname, dstDirname)) {
        fprintf(stderr, "ERR: Failed to move file: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    free(srcDirname);

    p_dstDirname = dstDirname;
    while (*p_dstDirname)
        free(*p_dstDirname++);
    free(dstDirname);

    wordfree(&homexp);

    fclose(confStream);
    closedir(srcDir);
}

int
main(void)
{
    fileorg();

    return 0;
}
