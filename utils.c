#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
exphome(const char *path, char **exp)
{
    const char *HOME;
    size_t homelen, pathlen;
    static size_t homesize = sizeof("$HOME") - 1;

    if (!(HOME = getenv("HOME")))
        return -1;

    if (path[0] == '~')
        path++;
    else if (!strncmp("$HOME", path, homesize))
        path += homesize;
    else
        return 0;

    homelen = strlen(HOME);
    pathlen = strlen(path);
    *exp = malloc(homelen + pathlen + 1);

    memcpy(*exp, HOME, homelen);
    memcpy(*exp + homelen, path, pathlen + 1);

    return 1;
}
