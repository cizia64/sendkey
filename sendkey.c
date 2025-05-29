#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/uinput.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <errno.h>

struct KeyMapping {
    const char *name;
    int code;
    int type;
};

struct KeyMapping key_mappings[] = {
    {"B", BTN_SOUTH, EV_KEY},
    {"A", BTN_EAST, EV_KEY},
    {"X", BTN_WEST, EV_KEY},
    {"Y", BTN_NORTH, EV_KEY},
    {"MENU", BTN_MODE, EV_KEY},
    {"SELECT", BTN_SELECT, EV_KEY},
    {"START", BTN_START, EV_KEY},
    {"L1", BTN_TL, EV_KEY},
    {"R1", BTN_TR, EV_KEY},
    {"L2", ABS_Z, EV_ABS},
    {"R2", ABS_RZ, EV_ABS},
    {"UP", ABS_HAT0Y, EV_ABS},
    {"DOWN", ABS_HAT0Y, EV_ABS},
    {"LEFT", ABS_HAT0X, EV_ABS},
    {"RIGHT", ABS_HAT0X, EV_ABS},
    {NULL, -1, -1}
};

int find_key_code(const char *name, int *code, int *type, int *value) {
    for (int i = 0; key_mappings[i].name != NULL; i++) {
        if (strcmp(name, key_mappings[i].name) == 0) {
            *code = key_mappings[i].code;
            *type = key_mappings[i].type;
            if (strcmp(name, "UP") == 0 || strcmp(name, "LEFT") == 0) {
                *value = -1;
            } else if (strcmp(name, "DOWN") == 0 || strcmp(name, "RIGHT") == 0) {
                *value = 1;
            } else if (strcmp(name, "L2") == 0 || strcmp(name, "R2") == 0) {
                *value = 255;
            } else {
                *value = 1;
            }
            return 0;
        }
    }
    return -1;
}

void emit_event(int fd, int type, int code, int value) {
    struct input_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.type = type;
    ev.code = code;
    ev.value = value;
    gettimeofday(&ev.time, NULL);
    write(fd, &ev, sizeof(ev));
}

void emit_syn(int fd) {
    emit_event(fd, EV_SYN, SYN_REPORT, 0);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s /dev/input/eventX [[KEY] [VALUE], ...]\n", argv[0]);
        fprintf(stderr, "Example: %s /dev/input/event0 A 1 B 1 A 0 B 0\n", argv[0]);
        return 1;
    }

    const char *device = argv[1];
    int fd = open(device, O_WRONLY);
    if (fd < 0) {
        perror("Failed to open input device");
        return 1;
    }

    int i = 2;
    while (i < argc) {
        const char *key = argv[i];
        int code, type, default_value;

        if (find_key_code(key, &code, &type, &default_value) != 0) {
            fprintf(stderr, "Unknown key: %s\n", key);
            i++;
            continue;
        }

        int send_press = default_value;
        int auto_release = 1;

        if ((i + 1) < argc) {
            char *end;
            long val = strtol(argv[i + 1], &end, 10);
            if (*end == '\0') {
                if (val == 0 || val == 1 || val == 255) {
                    emit_event(fd, type, code, val);
                    emit_syn(fd);
                    i += 2;
                    continue;
                } else if (val == 2) {
                    emit_event(fd, type, code, send_press);
                    emit_syn(fd);
                    usleep(20000);
                    emit_event(fd, type, code, (type == EV_ABS) ? 0 : 0);
                    emit_syn(fd);
                    i += 2;
                    continue;
                }
            }
        }

        emit_event(fd, type, code, send_press);
        emit_syn(fd);
        usleep(20000);
        emit_event(fd, type, code, (type == EV_ABS) ? 0 : 0);
        emit_syn(fd);
        i++;
    }

    close(fd);
    return 0;
}
