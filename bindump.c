#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>

const char *DEFAULT = "\033[0m";
const char *PRIMARY = "\033[0m";
const char *SECONDARY = "\033[0;2;31m";
const char *FADED = "\033[0;2;90m";

void fprint_bytes(FILE *out, char *data, int start, int length, int bytes_per_line);

int main(int argc, char **argv) {
	if (argc < 2) return printf("Usage: %s [file]\n", argv[0]), 1;
	char *fname = argv[1];
	FILE *f = fopen(fname, "r");
	fseek(f, 0L, SEEK_END);
	int fsize = ftell(f);
	fseek(f, 0L, SEEK_SET);

	char *data = malloc(fsize);
	fread(data, fsize, 1, f);
	
	int start = 0x0;
	int length = fsize;
	int bytes_per_line = 64;
	fprint_bytes(stdout, data, start, length, bytes_per_line);

	free(data);
	fclose(f);
	return 0;
}


void fprint_bytes(FILE *out, char *data, int start, int length, int bytes_per_line) {
	int line_count = (length + bytes_per_line - 1) / bytes_per_line;
	char *has_only_zeros = malloc(line_count);
	int *zero_count = calloc(line_count, sizeof(*zero_count));
	memset(has_only_zeros, 1, line_count);
	for (int line = 0; line < line_count; ++line) {
		for (int i = 0; i < bytes_per_line; ++i) {
			int k = start + i + line * bytes_per_line;
			if (k >= length)
				break;
			if (data[k] != 0) {
				has_only_zeros[line] = 0;
				break;
			}
		}
	}
	if (line_count > 0 && has_only_zeros[0]) zero_count[0] = 1;
	for (int line = 1; line < line_count; ++line) {
		if (has_only_zeros[line])
			zero_count[line] = 1 + zero_count[line - 1];
	}

	for (int line = 0; line < line_count; ++line) {
		if (has_only_zeros[line])
			continue;
		if (line > 0 && has_only_zeros[line-1] && !has_only_zeros[line]) {
			fprintf(out, "\n      %s... (%i lines of zeros) ...%s\n\n", SECONDARY, zero_count[line - 1], DEFAULT);
		}
		int line_addr = start + line * bytes_per_line;
		fprintf(out, "%s%04X:  %s", SECONDARY, line_addr, DEFAULT);
		for (int i = 0; i < bytes_per_line; ++i) {
			if (i % 2 == 0 && i > 0) fprintf(out, " ");
			if (i % 8 == 0 && i > 0) fprintf(out, " ");
			if (line_addr + i >= length) {
				fprintf(out, "  ");
				continue;
			}
			char byte = data[line_addr + i];
			fprintf(out, "%s%02X%s", (byte ? PRIMARY : FADED), (byte & 0xFF), DEFAULT);
		}
		fprintf(out, "  ");
		for (int i = 0; i < bytes_per_line; ++i) {
			if (line_addr + i >= length) {
				fprintf(out, " ");
				continue;
			}
			char byte = data[line_addr + i];
			fprintf(out, "%s", (byte ? PRIMARY : FADED));
			if (iscntrl(byte)) {
				fprintf(out, ".");
			} else {
				fprintf(out, "%c", (byte & 0xFF));
			}
			fprintf(out, "%s", DEFAULT);
		}
		fprintf(out, "\n");
	}

	free(has_only_zeros);
	free(zero_count);
}

