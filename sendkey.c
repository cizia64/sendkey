#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/uinput.h>
#include <unistd.h>
#include <linux/input.h>

struct {
    const char *name;
    int code;
} key_table[] = {
    {"A", BTN_EAST},
    {"B", BTN_SOUTH},
    {"X", BTN_WEST},
    {"Y", BTN_NORTH},
    {"MENU", BTN_MODE},
    {"SELECT", BTN_SELECT},
    {"START", BTN_START},
    {"L1", BTN_TL},
    {"R1", BTN_TR},
    {NULL, -1}
};

int get_key_code(const char *name) {
    for (int i = 0; key_table[i].name != NULL; i++) {
        if (strcasecmp(name, key_table[i].name) == 0) {
            return key_table[i].code;
        }
    }
    return -1;
}

void send_event(int fd, int code, int value) {
    struct input_event ev = {0};
    gettimeofday(&ev.time, NULL);
    ev.type = EV_KEY;
    ev.code = code;
    ev.value = value;
    write(fd, &ev, sizeof(ev));

    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    ev.value = 0;
    write(fd, &ev, sizeof(ev));
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s /dev/input/eventX [[KEY] [VALUE], ...]\n", argv[0]);
        fprintf(stderr, "Example: %s /dev/input/event0 A 1 B 1 A 0 B 0\n", argv[0]);
        return 1;
    }

    const char *device = argv[1];
    int input_fd = open(device, O_WRONLY);
    if (input_fd < 0) {
        perror("Failed to open input device");
        return 1;
    }

    int i = 2;
    while (i < argc) {
        int code = get_key_code(argv[i]);
        if (code == -1) {
            fprintf(stderr, "Unknown key: %s\n", argv[i]);
            i++;
            continue;
        }

        int value = 2; // Default: press and release
        if (i + 1 < argc && strlen(argv[i + 1]) == 1 &&
            (argv[i + 1][0] == '0' || argv[i + 1][0] == '1' || argv[i + 1][0] == '2')) {
            value = atoi(argv[i + 1]);
            i += 2;
        } else {
            i += 1;
        }

        if (value == 2) {
            send_event(input_fd, code, 1);
            send_event(input_fd, code, 0);
        } else {
            send_event(input_fd, code, value);
        }
    }

    close(input_fd);
    return 0;
}
