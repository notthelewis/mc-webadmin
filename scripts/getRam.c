/*
 * C program to print the system statistics like system uptime, 
 * total RAM space, free RAM space, process count, page size
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/sysinfo.h> 


int main() {
    int days, hours, mins;

    struct sysinfo info;

    if (sysinfo(&info) != 0)
        perror("sysinfo: error reading system statistics");

    days = info.uptime / 86400;
    hours = (info.uptime / 3600) - (days * 24);
    mins = (info.uptime / 60) - (days * 1440) - (hours * 60);

    printf("Uptime: %ddays, %dhours, %dminutes, %ldseconds\n", days, hours, mins, info.uptime % 60);

    printf("Total RAM: %ld MB\n", info.totalram/1024/1024);
    printf("Free RAM: %ld MB\n", (info.totalram-info.freeram)/1024/1024);
    printf("Process count: %d\n", info.procs);
    printf("Page size: %ld bytes\n", sysconf(_SC_PAGESIZE));

    return 0;
}
