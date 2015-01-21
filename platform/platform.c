#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
#ifdef __APPLE__
    printf("%s", "Platform is Mac OS\n");
#elif defined __linux__
    printf("%s", "Platform is Linux\n");
#elif defined __FreeBSD__
    printf("%s", "Platform is FreeBSD\n");
#elif defined _WIN32 || defined _WIN64
    printf("%s", "Platform is Windows\n");
#else
    printf("%s", "Platform is Unknown\n");
#endif
}