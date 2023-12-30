#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "parseconf.h"

#define BUF_SIZE 8192

void
writeheader(const char *header, FILE *stream)
{
    fwrite(header, 1, strlen(header), stream);
    puts(header);
    fputc('\n', stream);
}

void
writetag(const char *tag, FILE *stream)
{
    char tagInfo[BUF_SIZE];

    fwrite(tag, 1, strlen(tag), stream);
    printf("%s", tag);

    fgets(tagInfo, BUF_SIZE, stdin);
    fwrite(tagInfo, 1, strlen(tagInfo), stream);
}

void
writeconf(FILE *stream)
{
    const char *SRCHEADER = "[Source]";
    const char *DSTHEADER = "[Destination]";
    const char *DIRTAG = "dir = ";
    const char *EXTTAG = "ext = ";

    writeheader(SRCHEADER, stream);
    writetag(DIRTAG, stream);

    fputc('\n', stream);
    putchar('\n');

    writeheader(DSTHEADER, stream);
    writetag(EXTTAG, stream);
    writetag(DIRTAG, stream);
}

int
initconf(char *file)
{
    FILE *stream;

    if (!(stream = fopen(file, "w")))
        return 0;

    writeconf(stream);

    return 1;
}
