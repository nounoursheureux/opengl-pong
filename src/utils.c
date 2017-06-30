#include "utils.h"
#include <math.h>

char* ReadFile(const char* path)
{
    FILE* fp = fopen(path, "rb");
    if (!fp) return NULL;

    fseek(fp, 0, SEEK_END);
    int size = ftell(fp) + 1;
    rewind(fp);

    char* buf = (char*)malloc(size);
    fread(buf, 1, size, fp);
    buf[size-1] = 0;

    fclose(fp);
    return buf;
}
