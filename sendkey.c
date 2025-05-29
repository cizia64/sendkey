#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <errno.h>
#include <sys/time.h>

// Correspondence table between names and codes
struct key_entry {
    const char *name;
    int code;
};

struct key_entry key_table[] = {
    {"A", BTN_SOUTH},
    {"B", BTN_EAST},
    {"X", BTN_WEST},
    {"Y", BTN_NORTH},
    {"MENU", BTN_MODE},
    {"SELECT", BTN_SELECT},
    {"START", BTN_START},
    {"L1", BTN_TL},
    {"R1", BTN_TR},
    {NULL, -1}
};

int get_key_code(const char *keyname) {
    for (int i = 0; key_table[i].name != NULL; ++i) {
        if (strcasecmp(keyname, key_table[i].name) == 0) {
            return key_table[i].code;
        }
    }
    // If it's a direct numeric code
    if (keyname[0] >= '0' && keyname[0] <= '9')
        return atoi(keyname);

    fprintf(stderr, "Invalid key: %s\n", keyname);
    return -1;
}

int main(int argc, char *argv[])
{
    // Checking the number of arguments
    if (argc < 4 || (argc - 2) % 2 != 0) {
        fprintf(stderr, "Usage: %s /dev/input/eventX [[KEY] [VALUE], ...]\n", argv[0]);
        fprintf(stderr, "Exemple: %s /dev/input/event0 A 1 B 1 A 0 B 0\n", argv[0]);
        return 1;
    }

    // Input file (dev/input/eventX)
    char *event_file = argv[1];

    // Open input file
    int input_fd = open(event_file, O_WRONLY);
    if (input_fd < 0) {
        perror("Error opening input file");
        return 1;
    }

    printf("Using %s to send events\n", event_file);

    // Loop to process events
    for (int i = 2; i < argc; i += 2) {
        int code = get_key_code(argv[i]);
        int value = atoi(argv[i + 1]);

        if (code == -1 || value < 0 || value > 2) {
            fprintf(stderr, "Invalid event: %s %s\n", argv[i], argv[i+1]);
            continue;
        }

        struct input_event ev;
        gettimeofday(&ev.time, NULL);
        ev.type = EV_KEY;
        ev.code = code;
        ev.value = value;

        // Write the event
        if (write(input_fd, &ev, sizeof(ev)) < 0) {
            perror("Error write()");
        }

        // Send synchronization event
        struct input_event syn;
        gettimeofday(&syn.time, NULL);
        syn.type = EV_SYN;
        syn.code = SYN_REPORT;
        syn.value = 0;
        write(input_fd, &syn, sizeof(syn));
    }

    // Close input file
    close(input_fd);
    return 0;
}
