#include <errno.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "parseconf.h"
#include "movefiles.h"
#include "exphome.h"

void
fileorg(void)
{
    const char confPath[] = "$HOME/.config/fileorg.conf";
    char *confFile;
    FILE *confStream;
    char **srcDirname, **p_srcDirname;
    char **dstDirname, **p_dstDirname;

    if (exphome(confPath, &confFile) == -1) {
        fprintf(stderr, "ERR: $HOME environment variable unset\n");
        exit(EXIT_FAILURE);
    }

    if (!(confStream = fopen(confFile, "r"))) {
        if (errno == ENOENT && initconf(confFile)) 
            exit(EXIT_SUCCESS);

        fprintf(stderr, "ERR: Failed to open config file: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (!movetoHeader("Source", confStream)) {
        fputs("ERR: No source header\n", stderr);
        exit(EXIT_FAILURE);
    }

    if (!getblockInfo(&srcDirname, confStream)) {
        fputs("ERR: Source block has invalid or no tags\n", stderr);
        exit(EXIT_FAILURE);
    }

    if (!movetoHeader("Destination", confStream)) {
        fputs("ERR: No destination header\n", stderr);
        exit(EXIT_FAILURE);
    }

    if (!getblockInfo(&dstDirname, confStream)) {
        fputs("ERR: Destination block has invalid or no tags\n", stderr);
        exit(EXIT_FAILURE);
    }

    if (!parsedirs(srcDirname, dstDirname)) {
        fprintf(stderr, "ERR: Failed to parse or move files: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    free(confFile);
    fclose(confStream);

    p_srcDirname = srcDirname;
    while (*p_srcDirname)
        free(*p_srcDirname++);
    free(srcDirname);

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
