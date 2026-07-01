#include <errno.h>
#include <inttypes.h>
#include <libusb-1.0/libusb.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DSPI_VENDOR_ID  0x2e8b
#define DSPI_PRODUCT_ID 0xfeaa
#define DSPI_CLI_VERSION "1.0.0"

#define REQ_SET_PREAMP          0x44
#define REQ_GET_PREAMP          0x45
#define REQ_SET_EQ_PARAM        0x42
#define REQ_GET_EQ_PARAM        0x43
#define REQ_SET_BYPASS          0x46
#define REQ_GET_BYPASS          0x47
#define REQ_SET_DELAY           0x48
#define REQ_GET_DELAY           0x49
#define REQ_GET_STATUS          0x50
#define REQ_SAVE_PARAMS         0x51
#define REQ_SAVE_OUTPUT_CONFIG  0x52
#define REQ_FACTORY_RESET       0x53
#define REQ_SET_CHANNEL_GAIN    0x54
#define REQ_GET_CHANNEL_GAIN    0x55
#define REQ_SET_CHANNEL_MUTE    0x56
#define REQ_GET_CHANNEL_MUTE    0x57
#define REQ_SET_LOUDNESS        0x58
#define REQ_GET_LOUDNESS        0x59
#define REQ_SET_LOUDNESS_REF    0x5a
#define REQ_GET_LOUDNESS_REF    0x5b
#define REQ_SET_LOUDNESS_INTENSITY 0x5c
#define REQ_GET_LOUDNESS_INTENSITY 0x5d
#define REQ_SET_CROSSFEED       0x5e
#define REQ_GET_CROSSFEED       0x5f
#define REQ_SET_CROSSFEED_PRESET 0x60
#define REQ_GET_CROSSFEED_PRESET 0x61
#define REQ_SET_CROSSFEED_FREQ  0x62
#define REQ_GET_CROSSFEED_FREQ  0x63
#define REQ_SET_CROSSFEED_FEED  0x64
#define REQ_GET_CROSSFEED_FEED  0x65
#define REQ_SET_CROSSFEED_ITD   0x66
#define REQ_GET_CROSSFEED_ITD   0x67
#define REQ_SET_MATRIX_ROUTE    0x70
#define REQ_GET_MATRIX_ROUTE    0x71
#define REQ_SET_OUTPUT_ENABLE   0x72
#define REQ_GET_OUTPUT_ENABLE   0x73
#define REQ_SET_OUTPUT_GAIN     0x74
#define REQ_GET_OUTPUT_GAIN     0x75
#define REQ_SET_OUTPUT_MUTE     0x76
#define REQ_GET_OUTPUT_MUTE     0x77
#define REQ_SET_OUTPUT_DELAY    0x78
#define REQ_GET_OUTPUT_DELAY    0x79
#define REQ_GET_CORE1_MODE      0x7a
#define REQ_GET_CORE1_CONFLICT  0x7b
#define REQ_SET_OUTPUT_PIN      0x7c
#define REQ_GET_OUTPUT_PIN      0x7d
#define REQ_GET_SERIAL          0x7e
#define REQ_GET_PLATFORM        0x7f
#define REQ_CLEAR_CLIPS         0x83
#define REQ_PRESET_SAVE         0x90
#define REQ_PRESET_LOAD         0x91
#define REQ_PRESET_DELETE       0x92
#define REQ_PRESET_GET_NAME     0x93
#define REQ_PRESET_SET_NAME     0x94
#define REQ_PRESET_GET_DIR      0x95
#define REQ_PRESET_SET_STARTUP  0x96
#define REQ_PRESET_GET_STARTUP  0x97
#define REQ_SET_OUTPUT_CONFIG_MODE 0x98
#define REQ_GET_OUTPUT_CONFIG_MODE 0x99
#define REQ_PRESET_GET_ACTIVE   0x9a
#define REQ_SET_CHANNEL_NAME    0x9b
#define REQ_GET_CHANNEL_NAME    0x9c
#define REQ_GET_ALL_PARAMS      0xa0
#define REQ_SET_ALL_PARAMS      0xa1
#define BULK_PARAMS_SIZE        2944
#define REQ_GET_BUFFER_STATS    0xb0
#define REQ_RESET_BUFFER_STATS  0xb1
#define REQ_SET_LEVELLER        0xb4
#define REQ_GET_LEVELLER        0xb5
#define REQ_SET_LEVELLER_AMOUNT 0xb6
#define REQ_GET_LEVELLER_AMOUNT 0xb7
#define REQ_SET_LEVELLER_SPEED  0xb8
#define REQ_GET_LEVELLER_SPEED  0xb9
#define REQ_SET_LEVELLER_MAXGAIN 0xba
#define REQ_GET_LEVELLER_MAXGAIN 0xbb
#define REQ_SET_LEVELLER_LOOKAHEAD 0xbc
#define REQ_GET_LEVELLER_LOOKAHEAD 0xbd
#define REQ_SET_LEVELLER_GATE   0xbe
#define REQ_GET_LEVELLER_GATE   0xbf
#define REQ_SET_OUTPUT_TYPE     0xc0
#define REQ_GET_OUTPUT_TYPE     0xc1
#define REQ_SET_I2S_BCK_PIN     0xc2
#define REQ_GET_I2S_BCK_PIN     0xc3
#define REQ_SET_MCK_ENABLE      0xc4
#define REQ_GET_MCK_ENABLE      0xc5
#define REQ_SET_MCK_PIN         0xc6
#define REQ_GET_MCK_PIN         0xc7
#define REQ_SET_MCK_MULTIPLIER  0xc8
#define REQ_GET_MCK_MULTIPLIER  0xc9
#define REQ_SET_PREAMP_CH       0xd0
#define REQ_GET_PREAMP_CH       0xd1
#define REQ_SET_USER_VOLUME     0xda
#define REQ_GET_USER_VOLUME     0xdb
#define REQ_SET_MASTER_VOLUME   0xd2
#define REQ_GET_MASTER_VOLUME   0xd3
#define REQ_SET_MASTER_VOLUME_MODE 0xd4
#define REQ_GET_MASTER_VOLUME_MODE 0xd5
#define REQ_SAVE_MASTER_VOLUME  0xd6
#define REQ_GET_SAVED_MASTER_VOLUME 0xd7
#define REQ_SET_BAND_BYPASS     0xd8
#define REQ_GET_BAND_BYPASS     0xd9
#define REQ_SET_INPUT_SOURCE    0xe0
#define REQ_GET_INPUT_SOURCE    0xe1
#define REQ_GET_SPDIF_RX_STATUS 0xe2
#define REQ_GET_SPDIF_RX_CH_STATUS 0xe3
#define REQ_SET_SPDIF_RX_PIN    0xe4
#define REQ_GET_SPDIF_RX_PIN    0xe5
#define REQ_SET_LG_SOUND_SYNC_ENABLE 0xe6
#define REQ_GET_LG_SOUND_SYNC_ENABLE 0xe7
#define REQ_GET_LG_SOUND_SYNC_STATUS 0xe8
#define REQ_SET_DAC_HW_MUTE_CONFIG 0xea
#define REQ_GET_DAC_HW_MUTE_CONFIG 0xeb
#define REQ_TEST_DAC_HW_MUTE    0xec
#define REQ_ENTER_BOOTLOADER    0xf0

#define REQTYPE_OUT 0x41
#define REQTYPE_IN  0xc1
#define VENDOR_INTERFACE 2
#define USB_TIMEOUT_MS 1000

typedef struct {
    libusb_context *ctx;
    libusb_device_handle *handle;
} dspi_device;

typedef struct {
    uint8_t type;
    float freq;
    float q;
    float gain;
} apo_filter;

typedef struct {
    int channels[11];
    int channel_count;
    int start_band;
    int max_bands;
    bool dry_run;
    bool clear_unused;
    bool apply_preamp;
} apo_upload_options;

static void usage(FILE *stream) {
    fprintf(stream,
        "Usage: dspi-cli <command> [args]\n"
        "Version: v%s\n"
        "\n"
        "Commands:\n"
        "  list                         List connected DSPi devices\n"
        "  platform | serial | status | sample-rate | clear-clips\n"
        "\n"
        "EQ and input controls:\n"
        "  get-eq <ch> <band>           Read one EQ band\n"
        "  set-eq <ch> <band> <type> <freq> <q> <gain> [bypass]\n"
        "                               Upload one EQ band. Type: off, peaking,\n"
        "                               lowshelf, highshelf, lowpass, highpass,\n"
        "                               notch, allpass, or numeric 0..7.\n"
        "  upload-apo <file> <channels> [start-band] [max-bands]\n"
        "                               [--dry-run] [--no-clear] [--apply-preamp]\n"
        "  get-band-bypass <ch> <band> | set-band-bypass <ch> <band> <on|off>\n"
        "  get-delay <ch> | set-delay <ch> <ms>\n"
        "  get-preamp                   Read global preamp dB\n"
        "  set-preamp <db>              Set global preamp dB\n"
        "  get-preamp-channel <0|1> | set-preamp-channel <0|1> <db>\n"
        "  get-bypass                   Read hardware bypass state\n"
        "  set-bypass <on|off>          Set hardware bypass state\n"
        "\n"
        "Loudness, crossfeed, and leveller:\n"
        "  get-loudness | set-loudness <on|off>\n"
        "  get-loudness-ref | set-loudness-ref <spl>\n"
        "  get-loudness-intensity | set-loudness-intensity <pct>\n"
        "  get-crossfeed | set-crossfeed <on|off>\n"
        "  get-crossfeed-preset | set-crossfeed-preset <n>\n"
        "  get-crossfeed-freq | set-crossfeed-freq <hz>\n"
        "  get-crossfeed-feed | set-crossfeed-feed <db>\n"
        "  get-crossfeed-itd | set-crossfeed-itd <on|off>\n"
        "  get-leveller | set-leveller <on|off>\n"
        "  get/set-leveller-amount, speed, maxgain, lookahead, gate\n"
        "\n"
        "Matrix and output controls:\n"
        "  get-matrix <input> <output>\n"
        "  set-matrix <input> <output> <on|off> <gain-db> <invert-on|off>\n"
        "  get-output-enable/gain/mute/delay <output>\n"
        "  set-output-enable/gain/mute/delay <output> <value>\n"
        "  get-channel-gain/mute <ch> | set-channel-gain/mute <ch> <value>\n"
        "\n"
        "Hardware configuration:\n"
        "  get-core1-mode | get-core1-conflict <output>\n"
        "  get-output-pin <output> | set-output-pin <output> <gpio>\n"
        "  get-output-type <slot> | set-output-type <slot> <0-spdif|1-i2s>\n"
        "  get-i2s-bck-pin | set-i2s-bck-pin <gpio>\n"
        "  get-mck-enable/pin/multiplier | set-mck-enable/pin/multiplier <value>\n"
        "  get-input-source | set-input-source <0-usb|1-spdif>\n"
        "  get-spdif-rx-pin | set-spdif-rx-pin <gpio>\n"
        "  spdif-rx-status | spdif-rx-channel-status\n"
        "  get-dac-mute | set-dac-mute <enabled> <active-low> <pin|255> <hold-ms> <release-ms>\n"
        "  test-dac-mute\n"
        "\n"
        "Volume and persistence:\n"
        "  get-master-volume            Read master volume dB\n"
        "  set-master-volume <db>       Set master volume dB\n"
        "  get-master-volume-mode | set-master-volume-mode <0|1>\n"
        "  save-master-volume | get-saved-master-volume\n"
        "  get-user-volume              Read UAC/user volume dB\n"
        "  set-user-volume <db>         Set UAC/user volume dB (-60..0)\n"
        "  preset-list                  List preset occupancy and active slot\n"
        "  preset-load <slot>           Load preset slot 0..9\n"
        "  preset-save <slot>           Save current settings to slot 0..9\n"
        "  preset-delete <slot> | preset-active\n"
        "  preset-name <slot> | preset-set-name <slot> <name>\n"
        "  preset-startup | preset-set-startup <mode> <default-slot>\n"
        "  get-output-config-mode | set-output-config-mode <0|1> | save-output-config\n"
        "  get-channel-name <ch> | set-channel-name <ch> <name>\n"
        "  get-lg-sound-sync | set-lg-sound-sync <on|off> | lg-sound-sync-status\n"
        "  buffer-stats | reset-buffer-stats\n"
        "  get-all-params <file> | set-all-params <file>\n"
        "  save                         Commit parameters to flash\n"
        "  factory-reset                Restore factory defaults\n"
        "  enter-bootloader             Reboot device into bootloader\n"
        "\n"
        "Raw access:\n"
        "  raw-in <request> <value> <index> <length>\n"
        "  raw-out <request> <value> <index> <hex-bytes>\n"
        "  version                      Show the tool version\n"
        "  help                         Show this help\n",
        DSPI_CLI_VERSION);
}

static void print_version(void) {
    printf("dspi-cli v%s\n", DSPI_CLI_VERSION);
}

static int parse_float_arg(const char *text, float *out) {
    char *end = NULL;
    errno = 0;
    float value = strtof(text, &end);
    if (errno != 0 || end == text || *end != '\0') {
        return -1;
    }
    *out = value;
    return 0;
}

static int parse_slot_arg(const char *text, uint16_t *out) {
    char *end = NULL;
    errno = 0;
    long value = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || value < 0 || value > 9) {
        return -1;
    }
    *out = (uint16_t)value;
    return 0;
}

static int parse_u8_range(const char *text, unsigned int max_value, uint8_t *out) {
    char *end = NULL;
    errno = 0;
    unsigned long value = strtoul(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || value > max_value) {
        return -1;
    }
    *out = (uint8_t)value;
    return 0;
}

static int parse_bool_arg(const char *text, uint8_t *out) {
    if (strcmp(text, "on") == 0 || strcmp(text, "1") == 0 || strcmp(text, "true") == 0 ||
        strcmp(text, "yes") == 0 || strcmp(text, "bypass") == 0) {
        *out = 1;
        return 0;
    }
    if (strcmp(text, "off") == 0 || strcmp(text, "0") == 0 || strcmp(text, "false") == 0 ||
        strcmp(text, "no") == 0 || strcmp(text, "active") == 0) {
        *out = 0;
        return 0;
    }
    return -1;
}

static int parse_eq_type(const char *text, uint8_t *out) {
    struct {
        const char *name;
        uint8_t value;
    } types[] = {
        {"off", 0},
        {"flat", 0},
        {"peaking", 1},
        {"peak", 1},
        {"lowshelf", 2},
        {"low-shelf", 2},
        {"highshelf", 3},
        {"high-shelf", 3},
        {"lowpass", 4},
        {"low-pass", 4},
        {"highpass", 5},
        {"high-pass", 5},
        {"notch", 6},
        {"allpass", 7},
        {"all-pass", 7},
    };

    for (size_t i = 0; i < sizeof(types) / sizeof(types[0]); i++) {
        if (strcmp(text, types[i].name) == 0) {
            *out = types[i].value;
            return 0;
        }
    }

    return parse_u8_range(text, 7, out);
}

static int parse_apo_type(const char *text, uint8_t *out) {
    char upper[16] = {0};
    size_t n = strlen(text);
    if (n >= sizeof(upper)) {
        return -1;
    }
    for (size_t i = 0; i < n; i++) {
        upper[i] = (char)toupper((unsigned char)text[i]);
    }

    if (strcmp(upper, "PK") == 0 || strcmp(upper, "PEQ") == 0) {
        *out = 1;
    } else if (strcmp(upper, "LS") == 0 || strcmp(upper, "LSC") == 0) {
        *out = 2;
    } else if (strcmp(upper, "HS") == 0 || strcmp(upper, "HSC") == 0) {
        *out = 3;
    } else if (strcmp(upper, "LP") == 0 || strcmp(upper, "LPQ") == 0) {
        *out = 4;
    } else if (strcmp(upper, "HP") == 0 || strcmp(upper, "HPQ") == 0) {
        *out = 5;
    } else {
        return -1;
    }
    return 0;
}

static const char *eq_type_name(uint32_t type) {
    switch (type) {
    case 0: return "off";
    case 1: return "peaking";
    case 2: return "lowshelf";
    case 3: return "highshelf";
    case 4: return "lowpass";
    case 5: return "highpass";
    case 6: return "notch";
    case 7: return "allpass";
    default: return "unknown";
    }
}

static char *trim_left(char *s) {
    while (*s != '\0' && isspace((unsigned char)*s)) {
        s++;
    }
    return s;
}

static bool contains_token_ci(const char *line, const char *token) {
    size_t token_len = strlen(token);
    for (const char *p = line; *p != '\0'; p++) {
        size_t i = 0;
        while (i < token_len && p[i] != '\0' &&
               toupper((unsigned char)p[i]) == toupper((unsigned char)token[i])) {
            i++;
        }
        if (i == token_len) {
            return true;
        }
    }
    return false;
}

static char *find_ci(char *line, const char *token) {
    size_t token_len = strlen(token);
    for (char *p = line; *p != '\0'; p++) {
        size_t i = 0;
        while (i < token_len && p[i] != '\0' &&
               toupper((unsigned char)p[i]) == toupper((unsigned char)token[i])) {
            i++;
        }
        if (i == token_len) {
            return p;
        }
    }
    return NULL;
}

static int parse_channels_arg(const char *text, apo_upload_options *opts) {
    char buf[128];
    size_t len = strlen(text);
    if (len >= sizeof(buf)) {
        return -1;
    }
    memcpy(buf, text, len + 1);

    opts->channel_count = 0;
    char *saveptr = NULL;
    for (char *tok = strtok_r(buf, ",", &saveptr); tok != NULL; tok = strtok_r(NULL, ",", &saveptr)) {
        uint8_t ch;
        if (parse_u8_range(tok, 10, &ch) != 0 || opts->channel_count >= 11) {
            return -1;
        }
        opts->channels[opts->channel_count++] = ch;
    }
    return opts->channel_count > 0 ? 0 : -1;
}

static float read_le_float(const unsigned char *bytes) {
    uint32_t raw = ((uint32_t)bytes[0]) |
                   ((uint32_t)bytes[1] << 8) |
                   ((uint32_t)bytes[2] << 16) |
                   ((uint32_t)bytes[3] << 24);
    float value;
    memcpy(&value, &raw, sizeof(value));
    return value;
}

static void write_le_float(unsigned char *bytes, float value) {
    uint32_t raw;
    memcpy(&raw, &value, sizeof(raw));
    bytes[0] = (unsigned char)(raw & 0xff);
    bytes[1] = (unsigned char)((raw >> 8) & 0xff);
    bytes[2] = (unsigned char)((raw >> 16) & 0xff);
    bytes[3] = (unsigned char)((raw >> 24) & 0xff);
}

static uint16_t read_le_u16(const unsigned char *bytes) {
    return (uint16_t)(((uint16_t)bytes[0]) | ((uint16_t)bytes[1] << 8));
}

static void write_le_u16(unsigned char *bytes, uint16_t value) {
    bytes[0] = (unsigned char)(value & 0xff);
    bytes[1] = (unsigned char)((value >> 8) & 0xff);
}

static uint32_t read_le_u32(const unsigned char *bytes) {
    return ((uint32_t)bytes[0]) |
           ((uint32_t)bytes[1] << 8) |
           ((uint32_t)bytes[2] << 16) |
           ((uint32_t)bytes[3] << 24);
}

static void print_hex(const unsigned char *data, int length) {
    for (int i = 0; i < length; i++) {
        printf("%s%02x", i == 0 ? "" : " ", data[i]);
    }
    printf("\n");
}

static void print_c_string(const unsigned char *data, int length) {
    int end = 0;
    while (end < length && data[end] != 0) {
        end++;
    }
    printf("%.*s\n", end, data);
}

static int open_device(dspi_device *dev) {
    memset(dev, 0, sizeof(*dev));

    int rc = libusb_init(&dev->ctx);
    if (rc != 0) {
        fprintf(stderr, "libusb_init failed: %s\n", libusb_error_name(rc));
        return rc;
    }

    libusb_device **devices = NULL;
    ssize_t count = libusb_get_device_list(dev->ctx, &devices);
    if (count < 0) {
        fprintf(stderr, "libusb_get_device_list failed: %s\n", libusb_error_name((int)count));
        libusb_exit(dev->ctx);
        dev->ctx = NULL;
        return (int)count;
    }

    bool found = false;
    for (ssize_t i = 0; i < count; i++) {
        struct libusb_device_descriptor desc;
        rc = libusb_get_device_descriptor(devices[i], &desc);
        if (rc != 0) {
            continue;
        }
        if (desc.idVendor != DSPI_VENDOR_ID || desc.idProduct != DSPI_PRODUCT_ID) {
            continue;
        }

        found = true;
        rc = libusb_open(devices[i], &dev->handle);
        if (rc == 0) {
            break;
        }

        fprintf(stderr, "Found DSPi on bus %u address %u, but could not open it: %s\n",
                libusb_get_bus_number(devices[i]),
                libusb_get_device_address(devices[i]),
                libusb_error_name(rc));
    }

    libusb_free_device_list(devices, 1);

    if (!found) {
        fprintf(stderr, "No DSPi device found (%04x:%04x)\n", DSPI_VENDOR_ID, DSPI_PRODUCT_ID);
        libusb_exit(dev->ctx);
        dev->ctx = NULL;
        return LIBUSB_ERROR_NO_DEVICE;
    }

    if (dev->handle == NULL) {
        fprintf(stderr, "Install the udev rule or run with sufficient USB device permissions.\n");
        libusb_exit(dev->ctx);
        dev->ctx = NULL;
        return LIBUSB_ERROR_ACCESS;
    }

    return 0;
}

static void close_device(dspi_device *dev) {
    if (dev->handle != NULL) {
        libusb_close(dev->handle);
    }
    if (dev->ctx != NULL) {
        libusb_exit(dev->ctx);
    }
}

static int control_in(dspi_device *dev, uint8_t request, uint16_t value, uint16_t index,
                      unsigned char *data, uint16_t length) {
    int transferred = libusb_control_transfer(dev->handle, REQTYPE_IN, request, value, index,
                                              data, length, USB_TIMEOUT_MS);
    if (transferred < 0) {
        fprintf(stderr, "USB IN request 0x%02x failed: %s\n", request, libusb_error_name(transferred));
        return transferred;
    }
    return transferred;
}

static int control_out(dspi_device *dev, uint8_t request, uint16_t value, uint16_t index,
                       unsigned char *data, uint16_t length) {
    int transferred = libusb_control_transfer(dev->handle, REQTYPE_OUT, request, value, index,
                                              data, length, USB_TIMEOUT_MS);
    if (transferred < 0) {
        fprintf(stderr, "USB OUT request 0x%02x failed: %s\n", request, libusb_error_name(transferred));
        return transferred;
    }
    return transferred;
}

static int command_list(void) {
    libusb_context *ctx = NULL;
    libusb_device **devices = NULL;
    ssize_t count;
    int rc = libusb_init(&ctx);
    if (rc != 0) {
        fprintf(stderr, "libusb_init failed: %s\n", libusb_error_name(rc));
        return 1;
    }

    count = libusb_get_device_list(ctx, &devices);
    if (count < 0) {
        fprintf(stderr, "libusb_get_device_list failed: %s\n", libusb_error_name((int)count));
        libusb_exit(ctx);
        return 1;
    }

    int matches = 0;
    for (ssize_t i = 0; i < count; i++) {
        struct libusb_device_descriptor desc;
        rc = libusb_get_device_descriptor(devices[i], &desc);
        if (rc != 0) {
            continue;
        }
        if (desc.idVendor == DSPI_VENDOR_ID && desc.idProduct == DSPI_PRODUCT_ID) {
            printf("DSPi %04x:%04x bus=%u address=%u\n",
                   desc.idVendor, desc.idProduct,
                   libusb_get_bus_number(devices[i]),
                   libusb_get_device_address(devices[i]));
            matches++;
        }
    }

    if (matches == 0) {
        printf("No DSPi devices found (%04x:%04x)\n", DSPI_VENDOR_ID, DSPI_PRODUCT_ID);
    }

    libusb_free_device_list(devices, 1);
    libusb_exit(ctx);
    return matches == 0 ? 1 : 0;
}

static int command_platform(dspi_device *dev) {
    unsigned char data[4] = {0};
    int n = control_in(dev, REQ_GET_PLATFORM, 0, VENDOR_INTERFACE, data, sizeof(data));
    if (n < 0) {
        return 1;
    }

    printf("Platform bytes:");
    for (int i = 0; i < n; i++) {
        printf(" %02x", data[i]);
    }
    printf("\n");

    if (n >= 1) {
        const char *name = "RP2040";
        if (data[0] == 1) {
            name = "RP2350";
        } else if (data[0] == 2) {
            name = "STM32H723";
        }
        printf("Platform: %s\n", name);
    }
    if (n >= 3) {
        printf("Firmware: %u.%u.%u\n", data[1], data[2] >> 4, data[2] & 0x0f);
    }
    return 0;
}

static int command_serial(dspi_device *dev) {
    unsigned char data[17] = {0};
    int n = control_in(dev, REQ_GET_SERIAL, 0, VENDOR_INTERFACE, data, sizeof(data) - 1);
    if (n < 0) {
        return 1;
    }
    data[n] = '\0';
    printf("%s\n", data);
    return 0;
}

static int get_float(dspi_device *dev, uint8_t request, uint16_t index, const char *label) {
    unsigned char data[4] = {0};
    int n = control_in(dev, request, 0, index, data, sizeof(data));
    if (n < 0) {
        return 1;
    }
    if (n != 4) {
        fprintf(stderr, "Expected 4 bytes, got %d\n", n);
        return 1;
    }
    printf("%s: %.2f dB\n", label, read_le_float(data));
    return 0;
}

static int set_float(dspi_device *dev, uint8_t request, uint16_t index, float value) {
    unsigned char data[4];
    write_le_float(data, value);
    return control_out(dev, request, 0, index, data, sizeof(data)) < 0 ? 1 : 0;
}

static int get_indexed_float(dspi_device *dev, uint8_t request, uint16_t value, uint16_t index, const char *label) {
    unsigned char data[4] = {0};
    int n = control_in(dev, request, value, index, data, sizeof(data));
    if (n != 4) {
        if (n >= 0) {
            fprintf(stderr, "Expected 4 bytes, got %d\n", n);
        }
        return 1;
    }
    printf("%s: %.2f\n", label, read_le_float(data));
    return 0;
}

static int set_indexed_float(dspi_device *dev, uint8_t request, uint16_t value, uint16_t index, float f) {
    unsigned char data[4];
    write_le_float(data, f);
    return control_out(dev, request, value, index, data, sizeof(data)) < 0 ? 1 : 0;
}

static int get_indexed_u8(dspi_device *dev, uint8_t request, uint16_t value, uint16_t index, const char *label) {
    unsigned char data[1] = {0};
    int n = control_in(dev, request, value, index, data, sizeof(data));
    if (n != 1) {
        if (n >= 0) {
            fprintf(stderr, "Expected 1 byte, got %d\n", n);
        }
        return 1;
    }
    printf("%s: %u\n", label, data[0]);
    return 0;
}

static int set_u8_payload(dspi_device *dev, uint8_t request, uint16_t value, uint16_t index, uint8_t byte) {
    unsigned char data[1] = {byte};
    return control_out(dev, request, value, index, data, sizeof(data)) < 0 ? 1 : 0;
}

static int action_status_expect(dspi_device *dev, uint8_t request, uint16_t value, uint16_t index, uint8_t success);

static int action_status(dspi_device *dev, uint8_t request, uint16_t value, uint16_t index) {
    return action_status_expect(dev, request, value, index, 0);
}

static int action_status_expect(dspi_device *dev, uint8_t request, uint16_t value, uint16_t index, uint8_t success) {
    unsigned char data[1] = {0};
    int n = control_in(dev, request, value, index, data, sizeof(data));
    if (n != 1) {
        if (n >= 0) {
            fprintf(stderr, "Expected 1 status byte, got %d\n", n);
        }
        return 1;
    }
    printf("Status: 0x%02x\n", data[0]);
    return data[0] == success ? 0 : 1;
}

static int command_bool_get(dspi_device *dev, uint8_t request, uint16_t value, uint16_t index, const char *label) {
    unsigned char data[1] = {0};
    int n = control_in(dev, request, value, index, data, sizeof(data));
    if (n != 1) {
        if (n >= 0) {
            fprintf(stderr, "Expected 1 byte, got %d\n", n);
        }
        return 1;
    }
    printf("%s: %s\n", label, data[0] ? "on" : "off");
    return 0;
}

static int command_bool_set(dspi_device *dev, uint8_t request, uint16_t value, uint16_t index, const char *arg) {
    uint8_t byte;
    if (parse_bool_arg(arg, &byte) != 0) {
        fprintf(stderr, "Expected on/off, true/false, or 1/0\n");
        return 1;
    }
    return set_u8_payload(dev, request, value, index, byte);
}

static int command_get_sample_rate(dspi_device *dev) {
    unsigned char data[4] = {0};
    int n = control_in(dev, REQ_GET_STATUS, 15, VENDOR_INTERFACE, data, sizeof(data));
    if (n != 4) {
        if (n >= 0) {
            fprintf(stderr, "Expected 4 bytes, got %d\n", n);
        }
        return 1;
    }
    printf("Sample rate: %" PRIu32 " Hz\n", read_le_u32(data));
    return 0;
}

static int command_get_raw(dspi_device *dev, uint8_t request, uint16_t value, uint16_t index, uint16_t length) {
    unsigned char *data = calloc(length ? length : 1, 1);
    if (data == NULL) {
        perror("calloc");
        return 1;
    }
    int n = control_in(dev, request, value, index, data, length);
    if (n >= 0) {
        print_hex(data, n);
    }
    free(data);
    return n < 0 ? 1 : 0;
}

static int command_raw_in(dspi_device *dev, int argc, char **argv) {
    if (argc != 6) {
        fprintf(stderr, "Usage: dspi-cli raw-in <request> <value> <index> <length>\n");
        return 1;
    }
    return command_get_raw(dev,
                           (uint8_t)strtoul(argv[2], NULL, 0),
                           (uint16_t)strtoul(argv[3], NULL, 0),
                           (uint16_t)strtoul(argv[4], NULL, 0),
                           (uint16_t)strtoul(argv[5], NULL, 0));
}

static int parse_hex_bytes(const char *text, unsigned char *out, size_t max_len, size_t *out_len) {
    size_t len = 0;
    const char *p = text;
    while (*p != '\0') {
        while (*p == ' ' || *p == ':' || *p == ',') {
            p++;
        }
        if (*p == '\0') {
            break;
        }
        if (len >= max_len) {
            return -1;
        }
        char *end = NULL;
        unsigned long value = strtoul(p, &end, 16);
        if (end == p || value > 0xff) {
            return -1;
        }
        out[len++] = (unsigned char)value;
        p = end;
    }
    *out_len = len;
    return 0;
}

static int command_raw_out(dspi_device *dev, int argc, char **argv) {
    if (argc != 6) {
        fprintf(stderr, "Usage: dspi-cli raw-out <request> <value> <index> <hex-bytes>\n");
        return 1;
    }
    unsigned char data[4096];
    size_t length = 0;
    if (parse_hex_bytes(argv[5], data, sizeof(data), &length) != 0) {
        fprintf(stderr, "Invalid hex byte list\n");
        return 1;
    }
    int n = control_out(dev,
                        (uint8_t)strtoul(argv[2], NULL, 0),
                        (uint16_t)strtoul(argv[3], NULL, 0),
                        (uint16_t)strtoul(argv[4], NULL, 0),
                        data,
                        (uint16_t)length);
    return n < 0 ? 1 : 0;
}

static int command_get_bypass(dspi_device *dev) {
    unsigned char data[1] = {0};
    int n = control_in(dev, REQ_GET_BYPASS, 0, VENDOR_INTERFACE, data, sizeof(data));
    if (n < 0) {
        return 1;
    }
    printf("Bypass: %s\n", data[0] ? "on" : "off");
    return 0;
}

static int command_set_bypass(dspi_device *dev, const char *arg) {
    unsigned char data[1];
    if (strcmp(arg, "on") == 0 || strcmp(arg, "1") == 0 || strcmp(arg, "true") == 0) {
        data[0] = 1;
    } else if (strcmp(arg, "off") == 0 || strcmp(arg, "0") == 0 || strcmp(arg, "false") == 0) {
        data[0] = 0;
    } else {
        fprintf(stderr, "Expected on or off\n");
        return 1;
    }
    return control_out(dev, REQ_SET_BYPASS, 0, VENDOR_INTERFACE, data, sizeof(data)) < 0 ? 1 : 0;
}

static int command_status(dspi_device *dev) {
    unsigned char data[64] = {0};
    int n = control_in(dev, REQ_GET_STATUS, 9, VENDOR_INTERFACE, data, sizeof(data));
    if (n < 0) {
        return 1;
    }

    printf("Status bytes: %d\n", n);
    if (n >= 8 && ((n - 4) % 2) == 0) {
        int channels = (n - 4) / 2;
        for (int i = 0; i < channels; i++) {
            uint16_t raw = read_le_u16(&data[i * 2]);
            printf("Peak[%02d]: raw=%5u normalized=%0.5f\n", i, raw, (double)raw / 32767.0);
        }
        printf("CPU0: %u%%\n", data[channels * 2]);
        printf("CPU1: %u%%\n", data[channels * 2 + 1]);
        printf("Clip flags: 0x%04x\n", read_le_u16(&data[channels * 2 + 2]));
    } else {
        printf("Raw:");
        for (int i = 0; i < n; i++) {
            printf(" %02x", data[i]);
        }
        printf("\n");
    }
    return 0;
}

static int upload_eq_packet(dspi_device *dev, uint8_t channel, uint8_t band, uint8_t type,
                            float freq, float q, float gain, uint8_t bypass, bool dry_run) {
    if (dry_run) {
        printf("set-eq %u %u %s %.6g %.6g %.6g %s\n",
               channel, band, eq_type_name(type), freq, q, gain, bypass ? "on" : "off");
        return 0;
    }

    unsigned char data[16] = {0};
    data[0] = channel;
    data[1] = band;
    data[2] = type;
    data[3] = bypass;
    write_le_float(&data[4], freq);
    write_le_float(&data[8], q);
    write_le_float(&data[12], gain);

    int n = control_out(dev, REQ_SET_EQ_PARAM, 0, VENDOR_INTERFACE, data, sizeof(data));
    return n < 0 ? 1 : 0;
}

static int parse_apo_file(const char *path, apo_filter **out_filters, size_t *out_count,
                          float *out_preamp, bool *out_has_preamp) {
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        perror(path);
        return 1;
    }

    size_t cap = 16;
    size_t count = 0;
    apo_filter *filters = calloc(cap, sizeof(*filters));
    if (filters == NULL) {
        perror("calloc");
        fclose(file);
        return 1;
    }

    char line[512];
    while (fgets(line, sizeof(line), file) != NULL) {
        char *p = trim_left(line);

        float preamp;
        if (sscanf(p, "Preamp : %f dB", &preamp) == 1 ||
            sscanf(p, "Preamp: %f dB", &preamp) == 1) {
            *out_preamp = preamp;
            *out_has_preamp = true;
            continue;
        }

        if (!contains_token_ci(p, "Filter") || strchr(p, ':') == NULL) {
            continue;
        }
        if (!contains_token_ci(p, " ON ")) {
            continue;
        }

        char *colon = strchr(p, ':');
        if (colon == NULL) {
            continue;
        }
        char state[16] = {0};
        char type_text[16] = {0};
        if (sscanf(colon + 1, " %15s %15s", state, type_text) != 2) {
            continue;
        }
        if (strcasecmp(state, "ON") != 0) {
            continue;
        }

        uint8_t type;
        if (parse_apo_type(type_text, &type) != 0) {
            fprintf(stderr, "Skipping unsupported APO filter type %s: %s", type_text, line);
            continue;
        }

        float freq = 1000.0f;
        float q = 0.707f;
        float gain = 0.0f;
        char *fc = find_ci(p, "Fc");
        char *gain_ptr = find_ci(p, "Gain");
        char *q_ptr = find_ci(p, " Q ");
        if (fc != NULL) {
            sscanf(fc + 2, " %f", &freq);
        }
        if (gain_ptr != NULL) {
            sscanf(gain_ptr + 4, " %f", &gain);
        }
        if (q_ptr != NULL) {
            sscanf(q_ptr + 3, " %f", &q);
        }

        if (count == cap) {
            cap *= 2;
            apo_filter *new_filters = realloc(filters, cap * sizeof(*filters));
            if (new_filters == NULL) {
                perror("realloc");
                free(filters);
                fclose(file);
                return 1;
            }
            filters = new_filters;
        }
        filters[count++] = (apo_filter){ .type = type, .freq = freq, .q = q, .gain = gain };
    }

    fclose(file);
    *out_filters = filters;
    *out_count = count;
    return 0;
}

static int command_upload_apo(dspi_device *dev, int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr, "Usage: dspi-cli upload-apo <file> <channels> [start-band] [max-bands] [--dry-run] [--no-clear] [--apply-preamp]\n");
        return 1;
    }

    apo_upload_options opts = {
        .channel_count = 0,
        .start_band = 0,
        .max_bands = 10,
        .dry_run = false,
        .clear_unused = true,
        .apply_preamp = false,
    };
    const char *path = argv[2];
    if (parse_channels_arg(argv[3], &opts) != 0) {
        fprintf(stderr, "Expected comma-separated channels in range 0..10\n");
        return 1;
    }

    int positional = 0;
    for (int i = 4; i < argc; i++) {
        if (strcmp(argv[i], "--dry-run") == 0) {
            opts.dry_run = true;
        } else if (strcmp(argv[i], "--no-clear") == 0) {
            opts.clear_unused = false;
        } else if (strcmp(argv[i], "--apply-preamp") == 0) {
            opts.apply_preamp = true;
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "Unknown upload-apo option: %s\n", argv[i]);
            return 1;
        } else if (positional == 0) {
            opts.start_band = atoi(argv[i]);
            positional++;
        } else if (positional == 1) {
            opts.max_bands = atoi(argv[i]);
            positional++;
        } else {
            fprintf(stderr, "Unexpected upload-apo argument: %s\n", argv[i]);
            return 1;
        }
    }

    if (opts.start_band < 0 || opts.start_band > 9 || opts.max_bands < 1 ||
        opts.max_bands > 10 || opts.start_band + opts.max_bands > 10) {
        fprintf(stderr, "Expected start-band 0..9 and max-bands 1..10, with start-band + max-bands <= 10\n");
        return 1;
    }

    apo_filter *filters = NULL;
    size_t filter_count = 0;
    float preamp = 0.0f;
    bool has_preamp = false;
    if (parse_apo_file(path, &filters, &filter_count, &preamp, &has_preamp) != 0) {
        return 1;
    }
    if (filter_count == 0) {
        fprintf(stderr, "No supported enabled APO filters found in %s\n", path);
        free(filters);
        return 1;
    }

    size_t upload_count = filter_count < (size_t)opts.max_bands ? filter_count : (size_t)opts.max_bands;
    if (filter_count > upload_count) {
        fprintf(stderr, "Warning: %zu filters found; uploading first %zu.\n", filter_count, upload_count);
    }

    if (opts.apply_preamp && has_preamp) {
        if (opts.dry_run) {
            printf("set-preamp %.8g\n", preamp);
        } else if (set_float(dev, REQ_SET_PREAMP, 0, preamp) != 0) {
            free(filters);
            return 1;
        }
    }

    for (int c = 0; c < opts.channel_count; c++) {
        uint8_t channel = (uint8_t)opts.channels[c];
        for (size_t i = 0; i < upload_count; i++) {
            uint8_t band = (uint8_t)(opts.start_band + (int)i);
            if (upload_eq_packet(dev, channel, band, filters[i].type, filters[i].freq,
                                 filters[i].q, filters[i].gain, 0, opts.dry_run) != 0) {
                free(filters);
                return 1;
            }
        }
        if (opts.clear_unused) {
            for (int band = opts.start_band + (int)upload_count;
                 band < opts.start_band + opts.max_bands;
                 band++) {
                if (upload_eq_packet(dev, channel, (uint8_t)band, 0, 1000.0f, 0.707f, 0.0f, 0, opts.dry_run) != 0) {
                    free(filters);
                    return 1;
                }
            }
        }
    }

    printf("Uploaded %zu filter(s) to channel(s)", upload_count);
    for (int i = 0; i < opts.channel_count; i++) {
        printf("%s%d", i == 0 ? " " : ",", opts.channels[i]);
    }
    if (opts.dry_run) {
        printf(" (dry run)");
    }
    printf("\n");
    free(filters);
    return 0;
}

static int get_eq_param(dspi_device *dev, uint8_t channel, uint8_t band, uint8_t param,
                        unsigned char data[4]) {
    uint16_t value = (uint16_t)(((uint16_t)channel << 8) | ((uint16_t)band << 4) | param);
    int n = control_in(dev, REQ_GET_EQ_PARAM, value, VENDOR_INTERFACE, data, 4);
    if (n < 0) {
        return n;
    }
    if (n != 4) {
        fprintf(stderr, "Expected 4 bytes for EQ param %u, got %d\n", param, n);
        return LIBUSB_ERROR_IO;
    }
    return 0;
}

static int command_get_eq(dspi_device *dev, const char *channel_arg, const char *band_arg) {
    uint8_t channel;
    uint8_t band;
    if (parse_u8_range(channel_arg, 10, &channel) != 0 || parse_u8_range(band_arg, 9, &band) != 0) {
        fprintf(stderr, "Expected channel 0..10 and band 0..9\n");
        return 1;
    }

    unsigned char data[4];
    if (get_eq_param(dev, channel, band, 0, data) != 0) {
        return 1;
    }
    uint32_t type = read_le_u32(data);

    if (get_eq_param(dev, channel, band, 1, data) != 0) {
        return 1;
    }
    float freq = read_le_float(data);

    if (get_eq_param(dev, channel, band, 2, data) != 0) {
        return 1;
    }
    float q = read_le_float(data);

    if (get_eq_param(dev, channel, band, 3, data) != 0) {
        return 1;
    }
    float gain = read_le_float(data);

    uint32_t bypass = 0;
    if (get_eq_param(dev, channel, band, 4, data) == 0) {
        bypass = read_le_u32(data) & 0xff;
    }

    printf("EQ ch=%u band=%u type=%" PRIu32 " (%s) freq=%.2f Hz q=%.4f gain=%.2f dB bypass=%s\n",
           channel, band, type, eq_type_name(type), freq, q, gain, bypass == 1 ? "on" : "off");
    return 0;
}

static int command_set_eq(dspi_device *dev, int argc, char **argv) {
    uint8_t channel;
    uint8_t band;
    uint8_t type;
    uint8_t bypass = 0;
    float freq;
    float q;
    float gain;

    if (argc != 8 && argc != 9) {
        usage(stderr);
        return 1;
    }
    if (parse_u8_range(argv[2], 10, &channel) != 0 || parse_u8_range(argv[3], 9, &band) != 0) {
        fprintf(stderr, "Expected channel 0..10 and band 0..9\n");
        return 1;
    }
    if (parse_eq_type(argv[4], &type) != 0) {
        fprintf(stderr, "Expected EQ type off, peaking, lowshelf, highshelf, lowpass, highpass, notch, allpass, or 0..7\n");
        return 1;
    }
    if (parse_float_arg(argv[5], &freq) != 0 || parse_float_arg(argv[6], &q) != 0 ||
        parse_float_arg(argv[7], &gain) != 0) {
        fprintf(stderr, "Expected numeric freq, q, and gain values\n");
        return 1;
    }
    if (argc == 9 && parse_bool_arg(argv[8], &bypass) != 0) {
        fprintf(stderr, "Expected bypass on/off, true/false, or 1/0\n");
        return 1;
    }
    if (freq <= 0.0f || q <= 0.0f) {
        fprintf(stderr, "Frequency and Q must be positive\n");
        return 1;
    }

    if (upload_eq_packet(dev, channel, band, type, freq, q, gain, bypass, false) != 0) {
        return 1;
    }

    printf("Uploaded EQ ch=%u band=%u type=%u (%s) freq=%.2f Hz q=%.4f gain=%.2f dB bypass=%s\n",
           channel, band, type, eq_type_name(type), freq, q, gain, bypass ? "on" : "off");
    return 0;
}

static int command_flash_status(dspi_device *dev, uint8_t request, uint16_t value, uint16_t index) {
    unsigned char data[1] = {0};
    int n = control_in(dev, request, value, index, data, sizeof(data));
    if (n < 0) {
        return 1;
    }
    if (n == 0 || data[0] != 0) {
        fprintf(stderr, "Device returned status 0x%02x\n", n > 0 ? data[0] : 0xff);
        return 1;
    }
    puts("OK");
    return 0;
}

static int command_clear_clips(dspi_device *dev) {
    unsigned char data[2] = {0};
    int n = control_in(dev, REQ_CLEAR_CLIPS, 0, VENDOR_INTERFACE, data, sizeof(data));
    if (n != 2) {
        if (n >= 0) {
            fprintf(stderr, "Expected 2 clip flag bytes, got %d\n", n);
        }
        return 1;
    }
    printf("Cleared clip flags: 0x%04x\n", read_le_u16(data));
    return 0;
}

static int command_preset_list(dspi_device *dev) {
    unsigned char dir[7] = {0};
    int n = control_in(dev, REQ_PRESET_GET_DIR, 0, VENDOR_INTERFACE, dir, sizeof(dir));
    if (n < 0) {
        return 1;
    }
    if (n < 2) {
        fprintf(stderr, "Expected at least 2 bytes, got %d\n", n);
        return 1;
    }

    uint16_t occupied = read_le_u16(dir);
    printf("Occupied mask: 0x%04x\n", occupied);
    if (n >= 6) {
        printf("Startup mode: %u\n", dir[2]);
        printf("Default slot: %u\n", dir[3]);
        printf("Last active: %u\n", dir[4]);
        printf("Output config mode: %u\n", dir[5]);
    }
    if (n >= 7) {
        printf("Master volume mode: %u\n", dir[6]);
    }
    for (int slot = 0; slot < 10; slot++) {
        printf("Slot %d: %s\n", slot, (occupied & (1u << slot)) ? "occupied" : "empty");
    }

    unsigned char active[1] = {0};
    n = control_in(dev, REQ_PRESET_GET_ACTIVE, 0, VENDOR_INTERFACE, active, sizeof(active));
    if (n == 1) {
        printf("Active slot: %u\n", active[0]);
    }
    return 0;
}

static int command_get_name(dspi_device *dev, uint8_t request, uint16_t value, const char *label) {
    unsigned char data[32] = {0};
    int n = control_in(dev, request, value, VENDOR_INTERFACE, data, sizeof(data));
    if (n < 0) {
        return 1;
    }
    printf("%s: ", label);
    print_c_string(data, n);
    return 0;
}

static int command_set_name(dspi_device *dev, uint8_t request, uint16_t value, const char *name) {
    unsigned char data[32] = {0};
    size_t len = strlen(name);
    if (len > 31) {
        len = 31;
    }
    memcpy(data, name, len);
    return control_out(dev, request, value, VENDOR_INTERFACE, data, sizeof(data)) < 0 ? 1 : 0;
}

static int command_matrix_get(dspi_device *dev, const char *input_arg, const char *output_arg) {
    uint8_t input;
    uint8_t output;
    if (parse_u8_range(input_arg, 1, &input) != 0 || parse_u8_range(output_arg, 8, &output) != 0) {
        fprintf(stderr, "Expected input 0..1 and output 0..8\n");
        return 1;
    }
    unsigned char data[9] = {0};
    int n = control_in(dev, REQ_GET_MATRIX_ROUTE, (uint16_t)((input << 8) | output), VENDOR_INTERFACE, data, sizeof(data));
    if (n < 8) {
        if (n >= 0) {
            fprintf(stderr, "Expected at least 8 bytes, got %d\n", n);
        }
        return 1;
    }
    printf("Matrix input=%u output=%u enabled=%s invert=%s gain=%.2f dB\n",
           input, output, data[2] ? "on" : "off", data[3] ? "on" : "off", read_le_float(&data[4]));
    return 0;
}

static int command_matrix_set(dspi_device *dev, int argc, char **argv) {
    if (argc != 7) {
        fprintf(stderr, "Usage: dspi-cli set-matrix <input> <output> <on|off> <gain-db> <invert-on|off>\n");
        return 1;
    }
    uint8_t input, output, enabled, invert;
    float gain;
    if (parse_u8_range(argv[2], 1, &input) != 0 || parse_u8_range(argv[3], 8, &output) != 0 ||
        parse_bool_arg(argv[4], &enabled) != 0 || parse_float_arg(argv[5], &gain) != 0 ||
        parse_bool_arg(argv[6], &invert) != 0) {
        fprintf(stderr, "Invalid matrix arguments\n");
        return 1;
    }
    unsigned char data[9] = {0};
    data[0] = input;
    data[1] = output;
    data[2] = enabled;
    data[3] = invert;
    write_le_float(&data[4], gain);
    return control_out(dev, REQ_SET_MATRIX_ROUTE, 0, VENDOR_INTERFACE, data, sizeof(data)) < 0 ? 1 : 0;
}

static int command_get_all_params(dspi_device *dev, const char *path) {
    unsigned char *data = calloc(BULK_PARAMS_SIZE, 1);
    if (data == NULL) {
        perror("calloc");
        return 1;
    }
    int n = control_in(dev, REQ_GET_ALL_PARAMS, 0, VENDOR_INTERFACE, data, BULK_PARAMS_SIZE);
    if (n < 0) {
        free(data);
        return 1;
    }
    FILE *file = fopen(path, "wb");
    if (file == NULL) {
        perror(path);
        free(data);
        return 1;
    }
    fwrite(data, 1, (size_t)n, file);
    fclose(file);
    free(data);
    printf("Wrote %d bytes to %s\n", n, path);
    return 0;
}

static int command_set_all_params(dspi_device *dev, const char *path) {
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        perror(path);
        return 1;
    }
    unsigned char *data = calloc(BULK_PARAMS_SIZE, 1);
    if (data == NULL) {
        perror("calloc");
        fclose(file);
        return 1;
    }
    size_t n = fread(data, 1, BULK_PARAMS_SIZE, file);
    fclose(file);
    if (n == 0) {
        fprintf(stderr, "No data read from %s\n", path);
        free(data);
        return 1;
    }
    int rc = control_out(dev, REQ_SET_ALL_PARAMS, 0, VENDOR_INTERFACE, data, (uint16_t)n);
    free(data);
    return rc < 0 ? 1 : 0;
}

static int command_dac_mute_get(dspi_device *dev) {
    unsigned char data[16] = {0};
    int n = control_in(dev, REQ_GET_DAC_HW_MUTE_CONFIG, 0, VENDOR_INTERFACE, data, sizeof(data));
    if (n != 16) {
        if (n >= 0) {
            fprintf(stderr, "Expected 16 bytes, got %d\n", n);
        }
        return 1;
    }
    printf("enabled=%s active_low=%s pin=%u hold_ms=%u release_ms=%u\n",
           data[0] ? "on" : "off",
           data[1] ? "on" : "off",
           data[2],
           read_le_u16(&data[4]),
           read_le_u16(&data[6]));
    return 0;
}

static int command_dac_mute_set(dspi_device *dev, int argc, char **argv) {
    if (argc != 7) {
        fprintf(stderr, "Usage: dspi-cli set-dac-mute <enabled> <active-low> <pin|255> <hold-ms> <release-ms>\n");
        return 1;
    }
    uint8_t enabled, active_low, pin;
    uint16_t hold_ms = (uint16_t)strtoul(argv[5], NULL, 0);
    uint16_t release_ms = (uint16_t)strtoul(argv[6], NULL, 0);
    if (parse_bool_arg(argv[2], &enabled) != 0 || parse_bool_arg(argv[3], &active_low) != 0 ||
        parse_u8_range(argv[4], 255, &pin) != 0) {
        fprintf(stderr, "Invalid DAC mute arguments\n");
        return 1;
    }
    unsigned char data[16] = {0};
    data[0] = enabled;
    data[1] = active_low;
    data[2] = pin;
    write_le_u16(&data[4], hold_ms);
    write_le_u16(&data[6], release_ms);
    return control_out(dev, REQ_SET_DAC_HW_MUTE_CONFIG, 0, VENDOR_INTERFACE, data, sizeof(data)) < 0 ? 1 : 0;
}

static int command_lg_status(dspi_device *dev) {
    unsigned char data[16] = {0};
    int n = control_in(dev, REQ_GET_LG_SOUND_SYNC_STATUS, 0, VENDOR_INTERFACE, data, sizeof(data));
    if (n < 4) {
        if (n >= 0) {
            fprintf(stderr, "Expected at least 4 bytes, got %d\n", n);
        }
        return 1;
    }
    printf("enabled=%s present=%s muted=%s\n",
           data[0] ? "on" : "off",
           data[1] ? "yes" : "no",
           data[3] ? "yes" : "no");
    if (data[2] == 0xff) {
        printf("volume=unknown\n");
    } else {
        printf("volume_value=%u\n", data[2]);
    }
    return 0;
}

static int command_buffer_stats(dspi_device *dev) {
    unsigned char data[64] = {0};
    int n = control_in(dev, REQ_GET_BUFFER_STATS, 0, VENDOR_INTERFACE, data, sizeof(data));
    if (n < 0) {
        return 1;
    }
    printf("Buffer stats bytes: %d\n", n);
    print_hex(data, n);
    return 0;
}

static int run_device_command(int argc, char **argv) {
    const char *cmd = argv[1];
    dspi_device dev;
    int rc = open_device(&dev);
    if (rc != 0) {
        return 1;
    }

    int result = 1;
    if (strcmp(cmd, "platform") == 0) {
        result = command_platform(&dev);
    } else if (strcmp(cmd, "serial") == 0) {
        result = command_serial(&dev);
    } else if (strcmp(cmd, "status") == 0) {
        result = command_status(&dev);
    } else if (strcmp(cmd, "sample-rate") == 0) {
        result = command_get_sample_rate(&dev);
    } else if (strcmp(cmd, "clear-clips") == 0) {
        result = command_clear_clips(&dev);
    } else if (strcmp(cmd, "get-eq") == 0 && argc == 4) {
        result = command_get_eq(&dev, argv[2], argv[3]);
    } else if (strcmp(cmd, "set-eq") == 0) {
        result = command_set_eq(&dev, argc, argv);
    } else if (strcmp(cmd, "upload-apo") == 0) {
        result = command_upload_apo(&dev, argc, argv);
    } else if (strcmp(cmd, "get-band-bypass") == 0 && argc == 4) {
        uint8_t ch, band;
        result = (parse_u8_range(argv[2], 10, &ch) == 0 && parse_u8_range(argv[3], 9, &band) == 0)
            ? command_bool_get(&dev, REQ_GET_BAND_BYPASS, (uint16_t)((ch << 8) | band), VENDOR_INTERFACE, "Band bypass") : 1;
    } else if (strcmp(cmd, "set-band-bypass") == 0 && argc == 5) {
        uint8_t ch, band, bypass;
        if (parse_u8_range(argv[2], 10, &ch) == 0 && parse_u8_range(argv[3], 9, &band) == 0 &&
            parse_bool_arg(argv[4], &bypass) == 0) {
            result = set_u8_payload(&dev, REQ_SET_BAND_BYPASS, (uint16_t)((ch << 8) | band), VENDOR_INTERFACE, bypass);
        } else {
            result = 1;
        }
    } else if (strcmp(cmd, "get-delay") == 0 && argc == 3) {
        uint8_t ch;
        result = parse_u8_range(argv[2], 10, &ch) == 0 ? get_indexed_float(&dev, REQ_GET_DELAY, ch, VENDOR_INTERFACE, "Delay ms") : 1;
    } else if (strcmp(cmd, "set-delay") == 0 && argc == 4) {
        uint8_t ch;
        float value;
        result = (parse_u8_range(argv[2], 10, &ch) == 0 && parse_float_arg(argv[3], &value) == 0)
            ? set_indexed_float(&dev, REQ_SET_DELAY, ch, VENDOR_INTERFACE, value) : 1;
    } else if (strcmp(cmd, "get-preamp") == 0) {
        result = get_float(&dev, REQ_GET_PREAMP, VENDOR_INTERFACE, "Preamp");
    } else if (strcmp(cmd, "set-preamp") == 0 && argc == 3) {
        float value;
        result = parse_float_arg(argv[2], &value) == 0 ? set_float(&dev, REQ_SET_PREAMP, VENDOR_INTERFACE, value) : 1;
    } else if (strcmp(cmd, "get-preamp-channel") == 0 && argc == 3) {
        uint8_t ch;
        result = parse_u8_range(argv[2], 1, &ch) == 0 ? get_indexed_float(&dev, REQ_GET_PREAMP_CH, ch, VENDOR_INTERFACE, "Preamp channel dB") : 1;
    } else if (strcmp(cmd, "set-preamp-channel") == 0 && argc == 4) {
        uint8_t ch;
        float value;
        result = (parse_u8_range(argv[2], 1, &ch) == 0 && parse_float_arg(argv[3], &value) == 0)
            ? set_indexed_float(&dev, REQ_SET_PREAMP_CH, ch, VENDOR_INTERFACE, value) : 1;
    } else if (strcmp(cmd, "get-bypass") == 0) {
        result = command_get_bypass(&dev);
    } else if (strcmp(cmd, "set-bypass") == 0 && argc == 3) {
        result = command_set_bypass(&dev, argv[2]);
    } else if (strcmp(cmd, "get-loudness") == 0) {
        result = command_bool_get(&dev, REQ_GET_LOUDNESS, 0, VENDOR_INTERFACE, "Loudness");
    } else if (strcmp(cmd, "set-loudness") == 0 && argc == 3) {
        result = command_bool_set(&dev, REQ_SET_LOUDNESS, 0, VENDOR_INTERFACE, argv[2]);
    } else if (strcmp(cmd, "get-loudness-ref") == 0) {
        result = get_indexed_float(&dev, REQ_GET_LOUDNESS_REF, 0, VENDOR_INTERFACE, "Loudness ref SPL");
    } else if (strcmp(cmd, "set-loudness-ref") == 0 && argc == 3) {
        float value;
        result = parse_float_arg(argv[2], &value) == 0 ? set_indexed_float(&dev, REQ_SET_LOUDNESS_REF, 0, VENDOR_INTERFACE, value) : 1;
    } else if (strcmp(cmd, "get-loudness-intensity") == 0) {
        result = get_indexed_float(&dev, REQ_GET_LOUDNESS_INTENSITY, 0, VENDOR_INTERFACE, "Loudness intensity pct");
    } else if (strcmp(cmd, "set-loudness-intensity") == 0 && argc == 3) {
        float value;
        result = parse_float_arg(argv[2], &value) == 0 ? set_indexed_float(&dev, REQ_SET_LOUDNESS_INTENSITY, 0, VENDOR_INTERFACE, value) : 1;
    } else if (strcmp(cmd, "get-crossfeed") == 0) {
        result = command_bool_get(&dev, REQ_GET_CROSSFEED, 0, VENDOR_INTERFACE, "Crossfeed");
    } else if (strcmp(cmd, "set-crossfeed") == 0 && argc == 3) {
        result = command_bool_set(&dev, REQ_SET_CROSSFEED, 0, VENDOR_INTERFACE, argv[2]);
    } else if (strcmp(cmd, "get-crossfeed-preset") == 0) {
        result = get_indexed_u8(&dev, REQ_GET_CROSSFEED_PRESET, 0, VENDOR_INTERFACE, "Crossfeed preset");
    } else if (strcmp(cmd, "set-crossfeed-preset") == 0 && argc == 3) {
        uint8_t v;
        result = parse_u8_range(argv[2], 255, &v) == 0 ? set_u8_payload(&dev, REQ_SET_CROSSFEED_PRESET, 0, VENDOR_INTERFACE, v) : 1;
    } else if (strcmp(cmd, "get-crossfeed-freq") == 0) {
        result = get_indexed_float(&dev, REQ_GET_CROSSFEED_FREQ, 0, VENDOR_INTERFACE, "Crossfeed freq Hz");
    } else if (strcmp(cmd, "set-crossfeed-freq") == 0 && argc == 3) {
        float value;
        result = parse_float_arg(argv[2], &value) == 0 ? set_indexed_float(&dev, REQ_SET_CROSSFEED_FREQ, 0, VENDOR_INTERFACE, value) : 1;
    } else if (strcmp(cmd, "get-crossfeed-feed") == 0) {
        result = get_indexed_float(&dev, REQ_GET_CROSSFEED_FEED, 0, VENDOR_INTERFACE, "Crossfeed feed dB");
    } else if (strcmp(cmd, "set-crossfeed-feed") == 0 && argc == 3) {
        float value;
        result = parse_float_arg(argv[2], &value) == 0 ? set_indexed_float(&dev, REQ_SET_CROSSFEED_FEED, 0, VENDOR_INTERFACE, value) : 1;
    } else if (strcmp(cmd, "get-crossfeed-itd") == 0) {
        result = command_bool_get(&dev, REQ_GET_CROSSFEED_ITD, 0, VENDOR_INTERFACE, "Crossfeed ITD");
    } else if (strcmp(cmd, "set-crossfeed-itd") == 0 && argc == 3) {
        result = command_bool_set(&dev, REQ_SET_CROSSFEED_ITD, 0, VENDOR_INTERFACE, argv[2]);
    } else if (strcmp(cmd, "get-leveller") == 0) {
        result = command_bool_get(&dev, REQ_GET_LEVELLER, 0, VENDOR_INTERFACE, "Leveller");
    } else if (strcmp(cmd, "set-leveller") == 0 && argc == 3) {
        result = command_bool_set(&dev, REQ_SET_LEVELLER, 0, VENDOR_INTERFACE, argv[2]);
    } else if (strcmp(cmd, "get-leveller-amount") == 0) {
        result = get_indexed_float(&dev, REQ_GET_LEVELLER_AMOUNT, 0, VENDOR_INTERFACE, "Leveller amount");
    } else if (strcmp(cmd, "set-leveller-amount") == 0 && argc == 3) {
        float value;
        result = parse_float_arg(argv[2], &value) == 0 ? set_indexed_float(&dev, REQ_SET_LEVELLER_AMOUNT, 0, VENDOR_INTERFACE, value) : 1;
    } else if (strcmp(cmd, "get-leveller-speed") == 0) {
        result = get_indexed_u8(&dev, REQ_GET_LEVELLER_SPEED, 0, VENDOR_INTERFACE, "Leveller speed");
    } else if (strcmp(cmd, "set-leveller-speed") == 0 && argc == 3) {
        uint8_t v;
        result = parse_u8_range(argv[2], 255, &v) == 0 ? set_u8_payload(&dev, REQ_SET_LEVELLER_SPEED, 0, VENDOR_INTERFACE, v) : 1;
    } else if (strcmp(cmd, "get-leveller-maxgain") == 0) {
        result = get_indexed_float(&dev, REQ_GET_LEVELLER_MAXGAIN, 0, VENDOR_INTERFACE, "Leveller max gain dB");
    } else if (strcmp(cmd, "set-leveller-maxgain") == 0 && argc == 3) {
        float value;
        result = parse_float_arg(argv[2], &value) == 0 ? set_indexed_float(&dev, REQ_SET_LEVELLER_MAXGAIN, 0, VENDOR_INTERFACE, value) : 1;
    } else if (strcmp(cmd, "get-leveller-lookahead") == 0) {
        result = command_bool_get(&dev, REQ_GET_LEVELLER_LOOKAHEAD, 0, VENDOR_INTERFACE, "Leveller lookahead");
    } else if (strcmp(cmd, "set-leveller-lookahead") == 0 && argc == 3) {
        result = command_bool_set(&dev, REQ_SET_LEVELLER_LOOKAHEAD, 0, VENDOR_INTERFACE, argv[2]);
    } else if (strcmp(cmd, "get-leveller-gate") == 0) {
        result = get_indexed_float(&dev, REQ_GET_LEVELLER_GATE, 0, VENDOR_INTERFACE, "Leveller gate dB");
    } else if (strcmp(cmd, "set-leveller-gate") == 0 && argc == 3) {
        float value;
        result = parse_float_arg(argv[2], &value) == 0 ? set_indexed_float(&dev, REQ_SET_LEVELLER_GATE, 0, VENDOR_INTERFACE, value) : 1;
    } else if (strcmp(cmd, "get-matrix") == 0 && argc == 4) {
        result = command_matrix_get(&dev, argv[2], argv[3]);
    } else if (strcmp(cmd, "set-matrix") == 0) {
        result = command_matrix_set(&dev, argc, argv);
    } else if (strcmp(cmd, "get-master-volume") == 0) {
        result = get_float(&dev, REQ_GET_MASTER_VOLUME, VENDOR_INTERFACE, "Master volume");
    } else if (strcmp(cmd, "set-master-volume") == 0 && argc == 3) {
        float value;
        result = parse_float_arg(argv[2], &value) == 0 ? set_float(&dev, REQ_SET_MASTER_VOLUME, VENDOR_INTERFACE, value) : 1;
    } else if (strcmp(cmd, "get-master-volume-mode") == 0) {
        result = get_indexed_u8(&dev, REQ_GET_MASTER_VOLUME_MODE, 0, VENDOR_INTERFACE, "Master volume mode");
    } else if (strcmp(cmd, "set-master-volume-mode") == 0 && argc == 3) {
        uint8_t v;
        result = parse_u8_range(argv[2], 1, &v) == 0 ? set_u8_payload(&dev, REQ_SET_MASTER_VOLUME_MODE, 0, VENDOR_INTERFACE, v) : 1;
    } else if (strcmp(cmd, "save-master-volume") == 0) {
        result = action_status(&dev, REQ_SAVE_MASTER_VOLUME, 0, VENDOR_INTERFACE);
    } else if (strcmp(cmd, "get-saved-master-volume") == 0) {
        result = get_indexed_float(&dev, REQ_GET_SAVED_MASTER_VOLUME, 0, VENDOR_INTERFACE, "Saved master volume dB");
    } else if (strcmp(cmd, "get-user-volume") == 0) {
        result = get_float(&dev, REQ_GET_USER_VOLUME, VENDOR_INTERFACE, "User volume");
    } else if (strcmp(cmd, "set-user-volume") == 0 && argc == 3) {
        float value;
        if (parse_float_arg(argv[2], &value) != 0 || value < -60.0f || value > 0.0f) {
            fprintf(stderr, "Expected a dB value in range -60..0\n");
            result = 1;
        } else {
            result = set_float(&dev, REQ_SET_USER_VOLUME, VENDOR_INTERFACE, value);
        }
    } else if (strcmp(cmd, "get-channel-gain") == 0 && argc == 3) {
        uint8_t ch;
        result = parse_u8_range(argv[2], 10, &ch) == 0 ? get_indexed_float(&dev, REQ_GET_CHANNEL_GAIN, ch, VENDOR_INTERFACE, "Channel gain dB") : 1;
    } else if (strcmp(cmd, "set-channel-gain") == 0 && argc == 4) {
        uint8_t ch;
        float value;
        result = (parse_u8_range(argv[2], 10, &ch) == 0 && parse_float_arg(argv[3], &value) == 0)
            ? set_indexed_float(&dev, REQ_SET_CHANNEL_GAIN, ch, VENDOR_INTERFACE, value) : 1;
    } else if (strcmp(cmd, "get-channel-mute") == 0 && argc == 3) {
        uint8_t ch;
        result = parse_u8_range(argv[2], 10, &ch) == 0 ? command_bool_get(&dev, REQ_GET_CHANNEL_MUTE, ch, VENDOR_INTERFACE, "Channel mute") : 1;
    } else if (strcmp(cmd, "set-channel-mute") == 0 && argc == 4) {
        uint8_t ch;
        result = parse_u8_range(argv[2], 10, &ch) == 0 ? command_bool_set(&dev, REQ_SET_CHANNEL_MUTE, ch, VENDOR_INTERFACE, argv[3]) : 1;
    } else if (strcmp(cmd, "get-output-enable") == 0 && argc == 3) {
        uint8_t out;
        result = parse_u8_range(argv[2], 8, &out) == 0 ? command_bool_get(&dev, REQ_GET_OUTPUT_ENABLE, out, VENDOR_INTERFACE, "Output enable") : 1;
    } else if (strcmp(cmd, "set-output-enable") == 0 && argc == 4) {
        uint8_t out;
        result = parse_u8_range(argv[2], 8, &out) == 0 ? command_bool_set(&dev, REQ_SET_OUTPUT_ENABLE, out, VENDOR_INTERFACE, argv[3]) : 1;
    } else if (strcmp(cmd, "get-output-gain") == 0 && argc == 3) {
        uint8_t out;
        result = parse_u8_range(argv[2], 8, &out) == 0 ? get_indexed_float(&dev, REQ_GET_OUTPUT_GAIN, out, VENDOR_INTERFACE, "Output gain dB") : 1;
    } else if (strcmp(cmd, "set-output-gain") == 0 && argc == 4) {
        uint8_t out;
        float value;
        result = (parse_u8_range(argv[2], 8, &out) == 0 && parse_float_arg(argv[3], &value) == 0)
            ? set_indexed_float(&dev, REQ_SET_OUTPUT_GAIN, out, VENDOR_INTERFACE, value) : 1;
    } else if (strcmp(cmd, "get-output-mute") == 0 && argc == 3) {
        uint8_t out;
        result = parse_u8_range(argv[2], 8, &out) == 0 ? command_bool_get(&dev, REQ_GET_OUTPUT_MUTE, out, VENDOR_INTERFACE, "Output mute") : 1;
    } else if (strcmp(cmd, "set-output-mute") == 0 && argc == 4) {
        uint8_t out;
        result = parse_u8_range(argv[2], 8, &out) == 0 ? command_bool_set(&dev, REQ_SET_OUTPUT_MUTE, out, VENDOR_INTERFACE, argv[3]) : 1;
    } else if (strcmp(cmd, "get-output-delay") == 0 && argc == 3) {
        uint8_t out;
        result = parse_u8_range(argv[2], 8, &out) == 0 ? get_indexed_float(&dev, REQ_GET_OUTPUT_DELAY, out, VENDOR_INTERFACE, "Output delay ms") : 1;
    } else if (strcmp(cmd, "set-output-delay") == 0 && argc == 4) {
        uint8_t out;
        float value;
        result = (parse_u8_range(argv[2], 8, &out) == 0 && parse_float_arg(argv[3], &value) == 0)
            ? set_indexed_float(&dev, REQ_SET_OUTPUT_DELAY, out, VENDOR_INTERFACE, value) : 1;
    } else if (strcmp(cmd, "get-core1-mode") == 0) {
        result = get_indexed_u8(&dev, REQ_GET_CORE1_MODE, 0, VENDOR_INTERFACE, "Core1 mode");
    } else if (strcmp(cmd, "get-core1-conflict") == 0 && argc == 3) {
        uint8_t out;
        result = parse_u8_range(argv[2], 8, &out) == 0 ? get_indexed_u8(&dev, REQ_GET_CORE1_CONFLICT, out, VENDOR_INTERFACE, "Core1 conflict") : 1;
    } else if (strcmp(cmd, "get-output-pin") == 0 && argc == 3) {
        uint8_t out;
        result = parse_u8_range(argv[2], 8, &out) == 0 ? get_indexed_u8(&dev, REQ_GET_OUTPUT_PIN, out, VENDOR_INTERFACE, "Output pin") : 1;
    } else if (strcmp(cmd, "set-output-pin") == 0 && argc == 4) {
        uint8_t out, pin;
        result = (parse_u8_range(argv[2], 8, &out) == 0 && parse_u8_range(argv[3], 255, &pin) == 0)
            ? action_status(&dev, REQ_SET_OUTPUT_PIN, (uint16_t)((pin << 8) | out), VENDOR_INTERFACE) : 1;
    } else if (strcmp(cmd, "get-output-type") == 0 && argc == 3) {
        uint8_t slot;
        result = parse_u8_range(argv[2], 3, &slot) == 0 ? get_indexed_u8(&dev, REQ_GET_OUTPUT_TYPE, slot, VENDOR_INTERFACE, "Output type") : 1;
    } else if (strcmp(cmd, "set-output-type") == 0 && argc == 4) {
        uint8_t slot, type;
        result = (parse_u8_range(argv[2], 3, &slot) == 0 && parse_u8_range(argv[3], 1, &type) == 0)
            ? action_status(&dev, REQ_SET_OUTPUT_TYPE, (uint16_t)((type << 8) | slot), VENDOR_INTERFACE) : 1;
    } else if (strcmp(cmd, "get-i2s-bck-pin") == 0) {
        result = get_indexed_u8(&dev, REQ_GET_I2S_BCK_PIN, 0, VENDOR_INTERFACE, "I2S BCK pin");
    } else if (strcmp(cmd, "set-i2s-bck-pin") == 0 && argc == 3) {
        uint8_t pin;
        result = parse_u8_range(argv[2], 255, &pin) == 0 ? action_status(&dev, REQ_SET_I2S_BCK_PIN, pin, VENDOR_INTERFACE) : 1;
    } else if (strcmp(cmd, "get-mck-enable") == 0) {
        result = command_bool_get(&dev, REQ_GET_MCK_ENABLE, 0, VENDOR_INTERFACE, "MCK enable");
    } else if (strcmp(cmd, "set-mck-enable") == 0 && argc == 3) {
        uint8_t enabled;
        result = parse_bool_arg(argv[2], &enabled) == 0 ? action_status(&dev, REQ_SET_MCK_ENABLE, enabled, VENDOR_INTERFACE) : 1;
    } else if (strcmp(cmd, "get-mck-pin") == 0) {
        result = get_indexed_u8(&dev, REQ_GET_MCK_PIN, 0, VENDOR_INTERFACE, "MCK pin");
    } else if (strcmp(cmd, "set-mck-pin") == 0 && argc == 3) {
        uint8_t pin;
        result = parse_u8_range(argv[2], 255, &pin) == 0 ? action_status(&dev, REQ_SET_MCK_PIN, pin, VENDOR_INTERFACE) : 1;
    } else if (strcmp(cmd, "get-mck-multiplier") == 0) {
        result = get_indexed_u8(&dev, REQ_GET_MCK_MULTIPLIER, 0, VENDOR_INTERFACE, "MCK multiplier raw");
    } else if (strcmp(cmd, "set-mck-multiplier") == 0 && argc == 3) {
        uint16_t raw;
        if (strcmp(argv[2], "128") == 0 || strcmp(argv[2], "0") == 0) {
            raw = 0;
        } else if (strcmp(argv[2], "256") == 0 || strcmp(argv[2], "1") == 0) {
            raw = 1;
        } else {
            fprintf(stderr, "Expected MCK multiplier 128 or 256\n");
            result = 1;
            goto done;
        }
        result = action_status(&dev, REQ_SET_MCK_MULTIPLIER, raw, VENDOR_INTERFACE);
    } else if (strcmp(cmd, "get-input-source") == 0) {
        result = get_indexed_u8(&dev, REQ_GET_INPUT_SOURCE, 0, VENDOR_INTERFACE, "Input source");
    } else if (strcmp(cmd, "set-input-source") == 0 && argc == 3) {
        uint8_t source;
        result = parse_u8_range(argv[2], 255, &source) == 0 ? set_u8_payload(&dev, REQ_SET_INPUT_SOURCE, 0, VENDOR_INTERFACE, source) : 1;
    } else if (strcmp(cmd, "get-spdif-rx-pin") == 0) {
        result = get_indexed_u8(&dev, REQ_GET_SPDIF_RX_PIN, 0, VENDOR_INTERFACE, "SPDIF RX pin");
    } else if (strcmp(cmd, "set-spdif-rx-pin") == 0 && argc == 3) {
        uint8_t pin;
        result = parse_u8_range(argv[2], 255, &pin) == 0 ? action_status(&dev, REQ_SET_SPDIF_RX_PIN, pin, VENDOR_INTERFACE) : 1;
    } else if (strcmp(cmd, "spdif-rx-status") == 0) {
        result = command_get_raw(&dev, REQ_GET_SPDIF_RX_STATUS, 0, VENDOR_INTERFACE, 16);
    } else if (strcmp(cmd, "spdif-rx-channel-status") == 0) {
        result = command_get_raw(&dev, REQ_GET_SPDIF_RX_CH_STATUS, 0, VENDOR_INTERFACE, 24);
    } else if (strcmp(cmd, "preset-list") == 0) {
        result = command_preset_list(&dev);
    } else if (strcmp(cmd, "preset-load") == 0 && argc == 3) {
        uint16_t slot;
        result = parse_slot_arg(argv[2], &slot) == 0 ? command_flash_status(&dev, REQ_PRESET_LOAD, slot, VENDOR_INTERFACE) : 1;
    } else if (strcmp(cmd, "preset-save") == 0 && argc == 3) {
        uint16_t slot;
        result = parse_slot_arg(argv[2], &slot) == 0 ? command_flash_status(&dev, REQ_PRESET_SAVE, slot, VENDOR_INTERFACE) : 1;
    } else if (strcmp(cmd, "preset-delete") == 0 && argc == 3) {
        uint16_t slot;
        result = parse_slot_arg(argv[2], &slot) == 0 ? command_flash_status(&dev, REQ_PRESET_DELETE, slot, VENDOR_INTERFACE) : 1;
    } else if (strcmp(cmd, "preset-active") == 0) {
        result = get_indexed_u8(&dev, REQ_PRESET_GET_ACTIVE, 0, VENDOR_INTERFACE, "Active preset");
    } else if (strcmp(cmd, "preset-name") == 0 && argc == 3) {
        uint16_t slot;
        result = parse_slot_arg(argv[2], &slot) == 0 ? command_get_name(&dev, REQ_PRESET_GET_NAME, slot, "Preset name") : 1;
    } else if (strcmp(cmd, "preset-set-name") == 0 && argc == 4) {
        uint16_t slot;
        result = parse_slot_arg(argv[2], &slot) == 0 ? command_set_name(&dev, REQ_PRESET_SET_NAME, slot, argv[3]) : 1;
    } else if (strcmp(cmd, "preset-startup") == 0) {
        result = command_get_raw(&dev, REQ_PRESET_GET_STARTUP, 0, VENDOR_INTERFACE, 3);
    } else if (strcmp(cmd, "preset-set-startup") == 0 && argc == 4) {
        uint8_t mode, slot;
        if (parse_u8_range(argv[2], 255, &mode) == 0 && parse_u8_range(argv[3], 9, &slot) == 0) {
            unsigned char data[2] = {mode, slot};
            result = control_out(&dev, REQ_PRESET_SET_STARTUP, 0, VENDOR_INTERFACE, data, sizeof(data)) < 0 ? 1 : 0;
        } else {
            result = 1;
        }
    } else if (strcmp(cmd, "get-output-config-mode") == 0) {
        result = get_indexed_u8(&dev, REQ_GET_OUTPUT_CONFIG_MODE, 0, VENDOR_INTERFACE, "Output config mode");
    } else if (strcmp(cmd, "set-output-config-mode") == 0 && argc == 3) {
        uint8_t mode;
        result = parse_u8_range(argv[2], 1, &mode) == 0 ? set_u8_payload(&dev, REQ_SET_OUTPUT_CONFIG_MODE, 0, VENDOR_INTERFACE, mode) : 1;
    } else if (strcmp(cmd, "save-output-config") == 0) {
        result = action_status(&dev, REQ_SAVE_OUTPUT_CONFIG, 0, VENDOR_INTERFACE);
    } else if (strcmp(cmd, "get-channel-name") == 0 && argc == 3) {
        uint8_t ch;
        result = parse_u8_range(argv[2], 10, &ch) == 0 ? command_get_name(&dev, REQ_GET_CHANNEL_NAME, ch, "Channel name") : 1;
    } else if (strcmp(cmd, "set-channel-name") == 0 && argc == 4) {
        uint8_t ch;
        result = parse_u8_range(argv[2], 10, &ch) == 0 ? command_set_name(&dev, REQ_SET_CHANNEL_NAME, ch, argv[3]) : 1;
    } else if (strcmp(cmd, "get-lg-sound-sync") == 0) {
        result = command_bool_get(&dev, REQ_GET_LG_SOUND_SYNC_ENABLE, 0, VENDOR_INTERFACE, "LG Sound Sync");
    } else if (strcmp(cmd, "set-lg-sound-sync") == 0 && argc == 3) {
        result = command_bool_set(&dev, REQ_SET_LG_SOUND_SYNC_ENABLE, 0, VENDOR_INTERFACE, argv[2]);
    } else if (strcmp(cmd, "lg-sound-sync-status") == 0) {
        result = command_lg_status(&dev);
    } else if (strcmp(cmd, "get-dac-mute") == 0) {
        result = command_dac_mute_get(&dev);
    } else if (strcmp(cmd, "set-dac-mute") == 0) {
        result = command_dac_mute_set(&dev, argc, argv);
    } else if (strcmp(cmd, "test-dac-mute") == 0) {
        result = action_status(&dev, REQ_TEST_DAC_HW_MUTE, 0, VENDOR_INTERFACE);
    } else if (strcmp(cmd, "buffer-stats") == 0) {
        result = command_buffer_stats(&dev);
    } else if (strcmp(cmd, "reset-buffer-stats") == 0) {
        result = action_status_expect(&dev, REQ_RESET_BUFFER_STATS, 1, VENDOR_INTERFACE, 1);
    } else if (strcmp(cmd, "get-all-params") == 0 && argc == 3) {
        result = command_get_all_params(&dev, argv[2]);
    } else if (strcmp(cmd, "set-all-params") == 0 && argc == 3) {
        result = command_set_all_params(&dev, argv[2]);
    } else if (strcmp(cmd, "enter-bootloader") == 0) {
        result = action_status_expect(&dev, REQ_ENTER_BOOTLOADER, 0, VENDOR_INTERFACE, 1);
    } else if (strcmp(cmd, "raw-in") == 0) {
        result = command_raw_in(&dev, argc, argv);
    } else if (strcmp(cmd, "raw-out") == 0) {
        result = command_raw_out(&dev, argc, argv);
    } else if (strcmp(cmd, "save") == 0) {
        result = command_flash_status(&dev, REQ_SAVE_PARAMS, 0, VENDOR_INTERFACE);
    } else if (strcmp(cmd, "factory-reset") == 0) {
        result = command_flash_status(&dev, REQ_FACTORY_RESET, 0, VENDOR_INTERFACE);
    } else {
        usage(stderr);
    }

done:
    close_device(&dev);
    return result;
}

int main(int argc, char **argv) {
    if (argc < 2 || strcmp(argv[1], "help") == 0 || strcmp(argv[1], "--help") == 0) {
        usage(argc < 2 ? stderr : stdout);
        return argc < 2 ? 1 : 0;
    }

    if (strcmp(argv[1], "version") == 0 || strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
        print_version();
        return 0;
    }

    if (strcmp(argv[1], "list") == 0) {
        return command_list();
    }

    return run_device_command(argc, argv);
}
