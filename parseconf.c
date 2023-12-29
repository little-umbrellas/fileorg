#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parseconf.h"

#define BUF_SIZE 8192

int
movetoHeader(char *header, FILE *stream)
{
    char c;
    char buf[BUF_SIZE], *p_buf = buf;
    int isHeader = 0;

    for (int nc = 0; nc < BUF_SIZE && (c = fgetc(stream)) != EOF;) {
        if (c == '[')
            isHeader = 1;
        else if (isHeader && c != ']') {
            *p_buf++ = c;
            nc++;
        } else if (isHeader) {
            *p_buf = '\0';
            if (!strcmp(buf, header))
                return 1;

            p_buf = buf;
            nc = 0, isHeader = 0;
        }
    }

    return 0;
}

int
getblockInfo(char ***blockInfo, FILE *stream)
{
    int nc, nl;
    char c;
    char buf[BUF_SIZE], *p_buf = buf;
    int isTag = 0;
    char *tag, *tagInfo;
    char **p_blockInfo;

    *blockInfo = malloc(sizeof(char*) * (BUF_SIZE+1));
    p_blockInfo = *blockInfo;

    for (nc = 1, nl = 1; nc <= BUF_SIZE && nl <= BUF_SIZE && (c = fgetc(stream)) != EOF && (*p_buf++ = c); nc++) {
        if (c == '=' && !isTag) {
            p_buf -= 2, nc--;

            while (*p_buf == ' ' || *p_buf == '\t')
                p_buf--, nc--;
            p_buf++;

            *p_buf = '\0';

            if (!buf[0])
                return 0;

            tag = malloc(nc + 1);
            strcpy(tag, buf);
            *p_blockInfo++ = tag;

            if ((c = fgetc(stream)) != EOF && c == '\n')
                return 0;

            p_buf = buf;
            nc = 0, nl++;
            isTag = 1;
        } else if (c == '\n' && isTag) {
            *--p_buf = '\0';

            tagInfo = malloc(nc + 1);
            strcpy(tagInfo, buf);
            *p_blockInfo++ = tagInfo;

            p_buf = buf;
            nc = 0, nl++;
            isTag = 0;
        } else if (c == '\n') {
            p_buf = buf;
            nc = 0;
        } else if (c == '[') {
            fseek(stream, -1L, SEEK_CUR);
            break;
        }
    }

    *p_blockInfo = NULL;

    if (nc > BUF_SIZE || nl > BUF_SIZE)
        return 0;

    return 1;
}
