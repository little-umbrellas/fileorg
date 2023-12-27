#include <errno.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parseconf.h"
#include "movefiles.h"
#include "utils.h"

void
fileorg(void)
{
    const char confPath[] = "$HOME/.config/fileorg.conf";
    char *confFile;
    FILE *confStream;
    char *buf, *srcDirname;
    DIR *srcDir;
    char **dstDirname, **p_dstDirname;

    if (exphome(confPath, &confFile) == -1) {
        fprintf(stderr, "ERR: $HOME environment variable unset\n");
        exit(EXIT_FAILURE);
    }

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

    if (exphome(srcDirname, &buf)) {
        free(srcDirname);
        srcDirname = buf;
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
        fputs("ERR: Block has invalid or no tags\n", stderr);
        exit(EXIT_FAILURE);
    }

    if (parsedir(srcDir, srcDirname, dstDirname)) {
        fprintf(stderr, "ERR: Failed to move file: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    free(confFile);
    fclose(confStream);
    free(srcDirname);
    closedir(srcDir);

    p_dstDirname = dstDirname;
    while (*p_dstDirname)
        free(*p_dstDirname++);
    free(dstDirname);

}

int
main(void)
{
    fileorg();

    return 0;
}
