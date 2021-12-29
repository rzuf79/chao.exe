#include <dirent.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

const char* OUTPUT_HEADER_NAME = "resources.h";

static const unsigned char encoding_table[65] ="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static char *decoding_table = NULL;
static int mod_table[] = {0, 2, 1};

char *base64_encode(const unsigned char *data, size_t input_length, size_t *output_length) {

	*output_length = 4 * ((input_length + 2) / 3);

	char *encoded_data = malloc(*output_length);
	if (encoded_data == NULL) return NULL;

	for (int i = 0, j = 0; i < input_length;) {

		uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
		uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
		uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

		uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

		encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
	}

	for (int i = 0; i < mod_table[input_length % 3]; i++)
		encoded_data[*output_length - 1 - i] = '=';

	return encoded_data;
}

char* get_file_data(FILE* f, long* out_size) {
	fseek(f, 0, SEEK_END);
	*out_size = ftell(f);
	fseek(f, 0, SEEK_SET);

	char *data = malloc(*out_size + 1);
	fread(data, *out_size, 1, f);
	data[*out_size] = 0;

	return data;
}


int main(){
	printf("Packing resources...\n\n");

	DIR* d;
	struct dirent *dir;
	d = opendir(".");
	if (d) {

		FILE* f;
		FILE* out = fopen(OUTPUT_HEADER_NAME, "w");

		fprintf(out, "#ifndef RESOURCES_H\n#define RESOURCES_H\n");
		fprintf(out, "");


		while ((dir = readdir(d)) != NULL) {
			const char *ext = strrchr(dir->d_name,'.');
			size_t filename_size = strlen(dir->d_name);
			if((!ext) || (ext == dir->d_name) || filename_size < 5) {
				continue;
			} 

			// printf("%d vs %d\n", strlen(dir->d_name), filename_size - 3);
			char* file_name = malloc(filename_size + 1);
			for (int i = 0; i < filename_size; ++i) {
				if (dir->d_name[i] == '.' || dir->d_name[i] == ' ') {
					file_name[i] = '_';
				} else {
					file_name[i] = dir->d_name[i];
				}
			}
			file_name[filename_size] = '\0';

			// memcpy(file_name, dir->d_name, filename_size-3);
			// file_name[filename_size-4] = '\0';

			if (strcmp(ext, ".png") == 0) {
				printf("png found! %s\n", dir->d_name);
				f = fopen(dir->d_name, "rb");

				long data_size;
				char* data = get_file_data(f, &data_size);
				size_t out_len;
				char *encoded = base64_encode(data, data_size, &out_len);
				// printf("data:image/png;base64,%s\n", encoded);
				// fprintf(out, "\nchar* %s = \"data:image/png;base64,%s\";\n", file_name, encoded);
				fprintf(out, "\nconst char* %s = \"", file_name);
				for (int i = 0; i < out_len; ++i) {
					fprintf(out, "%c", encoded[i]);
				}
				fprintf(out, "\";\n", encoded);
				printf("%d\n", out_len);
				free(data);
				fclose(f);
			} else if (strcmp(ext, ".ogg") == 0) {
				// printf("ogg found! %s\n", dir->d_name);
			}

			free(file_name);
		}
		closedir(d);

		fprintf(out, "\n#endif // RESOURCES_H");
		fclose(out);
	}



	printf("\nAll done!\n");
	return 0;
}

void build_decoding_table() {

	decoding_table = malloc(256);

	for (int i = 0; i < 64; i++)
		decoding_table[(unsigned char) encoding_table[i]] = i;
}


unsigned char *base64_decode(const char *data,
							 size_t input_length,
							 size_t *output_length) {

	if (decoding_table == NULL) build_decoding_table();

	if (input_length % 4 != 0) return NULL;

	*output_length = input_length / 4 * 3;
	if (data[input_length - 1] == '=') (*output_length)--;
	if (data[input_length - 2] == '=') (*output_length)--;

	unsigned char *decoded_data = malloc(*output_length);
	if (decoded_data == NULL) return NULL;

	for (int i = 0, j = 0; i < input_length;) {

		uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
		uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
		uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
		uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

		uint32_t triple = (sextet_a << 3 * 6)
		+ (sextet_b << 2 * 6)
		+ (sextet_c << 1 * 6)
		+ (sextet_d << 0 * 6);

		if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
		if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
		if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
	}

	return decoded_data;
}


void base64_cleanup() {
	free(decoding_table);
}