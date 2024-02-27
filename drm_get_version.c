#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#include "drm.h"
#include "xf86drmMode.h"
#include "xf86drm.h"

int main(int argc, char **argv)
{
    drmDevicePtr devices[2];
    int i;
    int drm_node;
    int fd;
    int drm_count;
    drmVersionPtr version;

    drm_count = drmGetDevices2(0, devices, 2);

    if (drm_count < 0) {
        printf("drmGetDevices2() returned an error %d\n", drm_count);
        return 0;
    }

    for (i=0; i<drm_count; i++) {
        if (devices[i]->bustype != DRM_BUS_PCI)
            continue;
        int drm_node = DRM_NODE_RENDER;
        fd = -1;
        if (devices[i]->available_nodes & 1 << drm_node)
            fd = open(devices[i]->nodes[drm_node], O_RDWR | O_CLOEXEC);
        printf("drm node: %s\n", devices[i]->nodes[drm_node]);
        if (fd < 0) {
             printf("fd < 0\n");
             continue;
        }

        version = drmGetVersion(fd);
        printf("drm Version: %s\n", version->name);
        if (!version) {
            printf("Warning: Cannot get version for %s. Error is %s \n", devices[i]->nodes[drm_node], strerror(errno));
            close(fd);
            continue;
        }
        drmFreeVersion(version);
    }

    drmFreeDevices(devices, drm_count);

    return 0;
}
