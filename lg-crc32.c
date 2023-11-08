/* This is the CRC32 algorithm LG uses for partinfo (and other things). */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define CRC32_POLY (0x04c11db7)

uint32_t calc_lg_crc32(unsigned char *buf, size_t len);

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *path = argv[1];

    FILE *fp = NULL;

    if ((fp = fopen(path, "rb")) == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        perror("fseek");
        return EXIT_FAILURE;
    }

    long pos = -1L;

    if ((pos = ftell(fp)) == -1L) {
        perror("ftell");
        return EXIT_FAILURE;
    }

    size_t len = pos;

    rewind(fp);

    unsigned char *buf = NULL;

    if ((buf = calloc(len, 1)) == NULL) {
        perror("calloc");
        return EXIT_FAILURE;
    }

    size_t read_len = 0;

    if ((read_len = fread(buf, 1, len, fp)) != len) {
        const char *reason = "idk";

        if (ferror(fp) != 0) {
            reason = "error";
        } else if (feof(fp) != 0) {
            reason = "EOF";
        }

        fprintf(stderr, "fread: %s (read %zu)\n", reason, read_len);

        return EXIT_FAILURE;
    }

    fprintf(stderr, "read %zu (%#zx) bytes\n", len, len);

    fclose(fp);

    uint32_t crc = calc_lg_crc32(buf, len);

    free(buf);

    printf("CRC32 = %#08x [%02x %02x %02x %02x]\n", crc, crc & 0xff,
           (crc >> 8) & 0xff, (crc >> 16) & 0xff, (crc >> 24) & 0xff);

    return EXIT_SUCCESS;
}

static uint32_t lg_crc32_table[256];

static void init_lg_crc32(void) {
    for (unsigned int i = 0; i < 256; i++) {
        uint32_t c = i << 24;

        for (unsigned int j = 8; j > 0; j--) {
            c = ((c & 0x80000000) != 0) ? (c << 1) ^ CRC32_POLY : (c << 1);
        }

        lg_crc32_table[i] = c;
    }
}

uint32_t calc_lg_crc32(unsigned char *buf, size_t len) {
    static bool initialized = false;

    if (!initialized) {
        init_lg_crc32();   
        initialized = true;
    }

    uint32_t crc = 0xffffffff;

    for (unsigned char *p = buf; len > 0; p++) {
        crc = (crc << 8) ^ lg_crc32_table[(crc >> 24) ^ (*p)];
        len--;
    }

    return crc;
}
