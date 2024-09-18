#include "main.hpp"
#include "type/opcode.h"
#include "type/token.h"
#include "type/symbol.h"
#include "type/register.h"

size_t base_address = START_ADDRESS;

char* get_file_contents(char* filename) {
	FILE* f = fopen(filename, "r");
    if(f == NULL) {
		fprintf(stderr, " -- could not open file: %s\n", filename);
		return NULL;
	} else {
        fseek(f, 0, SEEK_END);
		fpos_t pos;
		fgetpos64(f, &pos);
		printf(" -- file length: %i\n", pos);
		char* buffer = (char*)malloc(sizeof(char) * (pos + 1));
        fseek(f, 0, 0);

		pos = 0;
		char ch;
		while ((ch = fgetc(f)) != EOF) {
			//if(ch != 0x0d)
				buffer[pos++] = ch;
			// putchar(ch);
		}
		buffer[pos] = 0;
		fclose(f);
		return buffer;
    }
}

int process(char* filename, char* output_filename) {
	char* buffer;
	link<token>* tokens;
	buffer = get_file_contents(filename);
    if(buffer != NULL) {
		tokens = tokenize(buffer);
	} else {
		fprintf(stderr, " -- failed to load file %s\n", filename);
		return 1;
	}

	if(tokens != NULL) {
		int failures = collect_labels(tokens);
		if(failures)
			return failures;
	} else {
		fprintf(stderr, " -- failed to collect labels in file %s\n", filename);
		return 1;
	}

	if(labels != NULL) {
		int failures = symbolize(tokens);
		if(failures)
			return failures;
	} else {
		fprintf(stderr, " -- failed to tokenize file %s\n", filename);
		return 1;
	}

	int patch_failures = apply_patches();
	if(patch_failures)
		return patch_failures;
	
	fprintf(stderr, " -- symbolize() succeeded; writing to %s now?\n", output_filename);

	int export_failures = write_emissions(output_filename);

	return export_failures;
}

int main(int argc, char* argv[]) {
	fprintf(stderr, "chas " VERSION " built " __DATE__ " " __TIME__ "\n");
	init_registers();
	init_opcodes();

	char* output_filename = NULL;

	int failures = 0;
	int argi = 1;
	while(argi < argc) {
		int argj = 0;
		if(argv[argi][0] == ':')
			argj = 1;
		else if(argv[argi][0] == '-' && argv[argi][1] == '-')
			argj = 2;
		
		string testp = string(argv[argi] + argj);
		char* chars = testp.chars();
		if(argj) {
			if(strcmp(chars, "print-opcodes") == 0) {
				link<opcode>* o = opcodes;
				while(o != NULL) {
					printf("%s ", o->child->text);
					o = o->next;
				}
				printf("\n");
			} else if(strcmp(chars, "base") == 0) {
				++argi;
				if(argi < argc) {
					base_address = strtoull(argv[argi], NULL, 0);
					if(base_address < 0) {
						fprintf(stderr, " -- illegal or malformed base address: %s\n", argv[argi]);
						failures += 1;
					} else {
						fprintf(stderr, " -- base address set to: 0x%016llx\n", base_address);
					}
				} else {
					fprintf(stderr, " -- %s: please specify a base address\n", chars);
					failures += 1;
				}
			} else if((strcmp(chars, "o") == 0) || (strcmp(chars, "output") == 0)) {
				++argi;
				if(argi < argc) {
					output_filename = argv[argi];
				} else {
					fprintf(stderr, " -- %s: please specify an output filename\n", chars);
				}
			} else {
				fprintf(stderr, " -- unknown parameter: %s\n", chars);
				failures += 1;
			}
		} else {
			fprintf(stderr, " -- processing file: %s\n", chars);
			if(output_filename == NULL) {
				output_filename = (char*)malloc(strlen(argv[argi]) + 3);
				strcpy(output_filename, argv[argi]);
				strcat(output_filename, ".o");
				// just the worst string processing
			}
			failures += process(argv[argi], output_filename);
		}
		delete chars;
		if(failures) {
			fprintf(stderr, " -- aborted\n");
			return failures;
		}
		++argi;
	}
	return failures;
}
