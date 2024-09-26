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

link<token>* all_tokens = NULL;
link<token>* last_token = NULL;

int process(char* filename) {
	char* buffer;
	link<token>* tokens;
	buffer = get_file_contents(filename);
    if(buffer != NULL) {
		tokens = tokenize(buffer);
		if(tokens != NULL) {
			if(all_tokens == NULL)
				all_tokens = tokens;
			else {
				last_token->next = tokens;
				printf(" -- ligated token lists\n");
			}

			link<token>* tk = tokens;
			while(tk->next != NULL) {
				tk->child->filename = filename;
				tk = tk->next;
			}
			tk->child->filename = filename;
			last_token = tk;
			return 0;
		} else {
			fprintf(stderr, " -- failed to tokenize file %s\n", filename);
			return 1;
		}
	} else {
		fprintf(stderr, " -- failed to load file %s\n", filename);
		return 1;
	}
}

int output(char* output_filename) {
	if(all_tokens != NULL) {
		int failures = collect_labels(all_tokens);
		if(failures)
			return failures;
	} else {
		fprintf(stderr, " -- failed to collect labels\n");
		return 1;
	}

	if(labels != NULL) {
		int failures = symbolize(all_tokens);
		if(failures)
			return failures;
	} else {
		fprintf(stderr, " -- failed to decorate\n");
		return 1;
	}

	int patch_failures = apply_patches();
	if(patch_failures)
		return patch_failures;
	
	fprintf(stderr, " -- code generation succeeded; writing output to %s\n", output_filename);

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
			} else if(strcmp(chars, "optable") == 0) {
				link<opcode>* o = opcodes;
				const char* empty_string = "";
				const char regname[16][7] = {
					"a", "b", "c", "d",
					"e", "f", "g", "h",
					"pc", "ra", "sb", "sp",
					"db", "cb", "pi", "status"
				};
				char bstr[8];
				char functext[16];
				char r0, r1, r2, vp, ap, skip, w;
				unsigned short b;
				unsigned long long v;
				r0 = 0;
				r1 = 0;
				r2 = 0;
				vp = ap = 0;
				v = 0x0;
				b = 0x0;
				skip = w = 0;
				while(o != NULL) {
					unsigned long long F = o->child->format;
					if(F & O_ALIAS) {
						o = o->next;
						continue;
					}

					unsigned short base_emission = o->child->emission;

					strcpy(functext, o->child->text);
					int ht = strlen(o->child->text);
					int hi = 0;
					int tail_valid = 0; // keep tail? (these instruction variants can't differentiated by optable attributes alone)
					int tail_start = -1;
					while(hi < ht) {
						char c = o->child->text[hi];
						if(c == '.') {
							tail_start = hi;
						} else if(~tail_start) {
							if(c != 'q' && c != 'd' && c != 'r' && c != 'w' && c != 'i' && c != 's')
								tail_valid = 1;
						}
						++hi;
					}

					if(base_emission & 0xff00 == 0x1800) // rep.* codes
						tail_valid = 1;
					else if(strcmp(functext, "ccmp.de") == 0 || strcmp(functext, "ccmp.we") == 0)
						strcpy(functext, "ccmp_e");
					
					if(~tail_start)
						functext[tail_start] = (tail_valid ? '_' : '\0');
					
					/*
						each opcode is implemented as:

						void op_????(char r0, char r1, char r2, char b, regt v, bool vp, bool ap, char skip, char w) {

						}
					*/

					#define PC 0x08

					skip = w = 0;

					if(F & O_LIT_64) {
						skip = 4;
						r1 = PC;
					} else if(F & O_LIT_32) {
						skip = 2;
						r1 = PC;
					} else if(F & O_LIT_16) {
						skip = 1;
						r1 = PC;
					} else {
						skip = 0;
					}

					if(F & O_PROC_64) {
						w = 4;
					} else if(F & O_PROC_32) {
						w = 2;
					} else if(F & O_PROC_16) {
						w = 1;
					} else {
						w = 0;
					}

					if(F & O_REF_ANY_MASK)
						ap = 1;
					else
						ap = 0;

					#define EMIT_OPTABLE(final_emission, rr0, rr1, rr2, rb, rvp, rap, rskip, rw, rsub0, rsub1, rsub2) \
					printf("\t{0x%04llx, &op_%s,\t%3d, %3d, %3d, 0x%02x, %d, %d, %d, %d, 0x%016llx}, // %s %s %s %s \n", \
						final_emission, functext, rr0, rr1, rr2, rb, rvp, rap, rskip, rw, F, o->child->text, rsub0, rsub1, rsub2);
					
					if(F & O_MONO) {
						for(r0 = 0; r0 < 16; r0++) {
							if((F & O_STATUS_FORBIDDEN) && r0 == 15)
								continue;
							
							if(F & O_DUO) {
								for(r1 = 0; r1 < 16; r1++) {
									if((F & O_STATUS_FORBIDDEN_2) && r1 == 15)
										continue;
									EMIT_OPTABLE(base_emission | (r1 << 4) | r0, r0, r1, 0xff, 0xff, vp, ap, skip, w, regname[r0], regname[r1], empty_string);
								}
							} else if(F & O_FLAG) {
								for(b = 0; b < 64; b++) {
									itoa(b, bstr, 10);
									EMIT_OPTABLE(base_emission | (b << 4) | r0, r0, 0xff, 0xff, b, vp, ap, skip, w, regname[r0], bstr, empty_string);
								}
							} else {
								EMIT_OPTABLE(base_emission | r0, r0, 0xff, 0xff, 0xff, vp, ap, skip, w, regname[r0], empty_string, empty_string);
							}
						}
					} else if(F & O_MONO_OR_LIT) {
						for(r0 = 0; r0 < 15; r0++) {
							EMIT_OPTABLE(base_emission | r0, r0, 0xff, 0xff, 0xff, 0, 0, 0, w, regname[r0], empty_string, empty_string);
						}
						EMIT_OPTABLE(base_emission | 0xf, r1, 0xff, 0xff, 0xff, 0, 0, skip, w, "<literal>", empty_string, empty_string);

					} else if(F & O_MONO_AND_LIT) {
						for(r0 = 0; r0 < 16; r0++) {
							EMIT_OPTABLE(base_emission | r0, r0, PC, 0xff, 0xff, vp, ap, skip, w, regname[r0], "<literal>", empty_string);
						}
					} else if(F & O_DUO_ONLY) {
						for(r1 = 0; r1 < 16; r1++) {
							if((F & O_STATUS_FORBIDDEN_2) && r1 == 15)
								continue;
							EMIT_OPTABLE(base_emission | (r1 << 4), 0xff, r1, 0xff, 0xff, vp, ap, skip, w, regname[r1], empty_string, empty_string);
						}
					} else if(F & O_OPT_IMM_8) {
						for(b = 0; b < 256; b++) {
							itoa(b, bstr, 10);
							EMIT_OPTABLE(base_emission | b, 0xff, 0xff, 0xff, b, 0, 0, 0, 0, bstr, empty_string, empty_string);
						}

					} else if(F & O_ARITHMETIC_TRIO) {
						for(r2 = 0; r2 < 8; r2++)
							for(r1 = 0; r1 < 8; r1++)
								for(r0 = 0; r0 < 8; r0++) {
									EMIT_OPTABLE(base_emission | (r2 << 6) | (r1 << 3) | r0, r0, r1, r2, 0xff, vp, ap, skip, w, regname[r0], regname[r1], regname[r2]);
								}

					} else if(F & O_ARITHMETIC_DUO) {
						for(r1 = 0; r1 < 8; r1++)
							for(r0 = 0; r0 < 8; r0++) {
								EMIT_OPTABLE(base_emission | (r1 << 3) | r0, r0, r1, 0xff, 0xff, vp, ap, skip, w, regname[r0], regname[r1], empty_string);
							}

					} else if(F & O_ARITHMETIC_LIT) {
						for(r0 = 0; r0 < 8; r0++) {
							EMIT_OPTABLE(base_emission | r0, r0, PC, 0xff, 0xff, vp, ap, skip, w, regname[r0], "<literal>", empty_string);
						}

					} else if(F == O_ZERO) {
						r0 = r1 = r2 = 0;
						EMIT_OPTABLE(base_emission, 0xff, 0xff, 0xff, 0xff, 0, 0, skip, w, empty_string, empty_string, empty_string);
					}

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
			failures += process(argv[argi]);
		}
		delete chars;
		if(failures) {
			fprintf(stderr, " -- aborted\n");
			return failures;
		}
		++argi;
	}
	if(!failures)
		failures += output(output_filename);
	return failures;
}
