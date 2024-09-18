#include "../main.hpp"
#include "token.h"
#include <stdlib.h>

char* make_substr(char* source, fpos_t word_start, fpos_t word_end) {
	char* new_word = (char*)malloc(sizeof(char) * (word_end - word_start + 2));
	strncpy(new_word, source + word_start, word_end - word_start + 1);
	new_word[word_end - word_start + 1] = 0;
	return new_word;
}

char* make_literal_output(unsigned long long source, unsigned short bytes) {
	char* o = (char*)malloc(sizeof(char) * bytes);
	o[bytes - 1] = source & 0xff;
	if(bytes >= 2)
		o[bytes - 2] = (source & 0xff00) >> 8;
	if(bytes >= 4) {
		o[bytes - 3] = (source & 0xff0000) >> 16;
		o[bytes - 4] = (source & 0xff000000) >> 24;
	}
	if(bytes == 8) {
		o[bytes - 5] = (source & 0xff00000000) >> 32;
		o[bytes - 6] = (source & 0xff0000000000) >> 40;
		o[bytes - 7] = (source & 0xff000000000000) >> 48;
		o[bytes - 8] = (source & 0xff00000000000000) >> 56;
	}
	// be the sound change you want to see in the world - sandhi
	
	return o;
}

link<token>* add_token(link<token>* last_token, char* raw_text, fpos_t word_start, fpos_t word_end, int token_type, fpos_t number, fpos_t line_number) {
	if(word_end < 0)
		return last_token;
	link<token>* new_token = new link<token>();
	new_token->child = new token();
	new_token->child->offset = word_start;
	new_token->child->number = number;
	new_token->child->line = line_number;
	new_token->child->literal_numeric_value = 0;
	new_token->child->op = NULL;
	new_token->child->output = NULL;

	if(token_type == TK_LIT_STRING) {
		word_start += 1;
		word_end -= 1;
	} else if(token_type == TK_NAME && raw_text[word_end] == ':') {
		token_type = TK_LABEL;
		word_end -= 1;
	}

	new_token->child->text = make_substr(raw_text, word_start, word_end);
	
	if(token_type == TK_NAME) {
		char* text = new_token->child->text; // for brevity
		reg* r = find_register(text);
		if(r != NULL) {
			new_token->child->reg = r;
			token_type = TK_REGNAME;
		} else {
			if(text[0] >= '0' && text[0] <= '9') {
				size_t len = strlen(text);
				if(text[1] == 'x' && text[0] == '0') {
					if(len <= 4) {
						token_type = TK_LIT_BYTE;
						new_token->child->literal_numeric_value = strtoull(text + 2, NULL, 16);
					} else if(len <= 6) {
						token_type = TK_LIT_WORD;
						new_token->child->literal_numeric_value = strtoull(text + 2, NULL, 16);
					} else if(len <= 10) {
						token_type = TK_LIT_DWORD;
						new_token->child->literal_numeric_value = strtoull(text + 2, NULL, 16);
					} else if(len <= 18) {
						token_type = TK_LIT_QWORD;
						new_token->child->literal_numeric_value = strtoull(text + 2, NULL, 16);
					} else {
						fprintf(stderr, "invalid hex literal on line %i: %s\n", line_number, text);
					}
				} else if(text[1] == 'b' && text[0] == '0') {
					if(len <= 10) {
						token_type = TK_LIT_BYTE;
						new_token->child->literal_numeric_value = strtoull(text + 2, NULL, 2);
					} else if(len <= 18) {
						token_type = TK_LIT_WORD;
						new_token->child->literal_numeric_value = strtoull(text + 2, NULL, 2);
					} else if(len <= 34) {
						token_type = TK_LIT_DWORD;
						new_token->child->literal_numeric_value = strtoull(text + 2, NULL, 2);
					} else if(len <= 66) {
						token_type = TK_LIT_QWORD;
						new_token->child->literal_numeric_value = strtoull(text + 2, NULL, 2);
					} else {
						fprintf(stderr, "invalid binary literal on line %i: %s\n", line_number, text);
					}
				} else if(len > 20) {
					fprintf(stderr, "impossibly long numeric literal on line %i: %s\n", line_number, text);
				} else {
					new_token->child->literal_numeric_value = strtoull(text, NULL, 0);
					if(new_token->child->literal_numeric_value <= 0xff) {
						token_type = TK_LIT_BYTE;
					} else if(new_token->child->literal_numeric_value <= 0xffff) {
						token_type = TK_LIT_WORD;
					} else if(new_token->child->literal_numeric_value <= 0xffffffff) {
						token_type = TK_LIT_DWORD;
					} else {
						token_type = TK_LIT_QWORD;
					}
				}
			}
		}
	}

	new_token->child->type = token_type;
	new_token->next = NULL;
	// printf("<%s type %i>\n", new_token->child->text, new_token->child->type); //.chars());

	if(token_type == TK_LIT_STRING) {
		unsigned long long len = word_end - word_start + 1;


		unsigned long long alen = len + 8; // 2 lead for length + no tail
		if(len % 2) ++alen;
		new_token->child->output = (char*)calloc(alen, sizeof(char));
		// new_token->child->output_size = alen;

		// process remaining escape codes:
		unsigned long long li = 0;
		unsigned long long oli = 0;
		while(li < len) {
			new_token->child->output[8 + oli] = new_token->child->text[li];
			if(new_token->child->text[li] == '\\' && new_token->child->text[li+1] == 'n') {
				++li;
				new_token->child->output[8 + oli] = '\n';
			}
			++li;
			++oli;
		}
		
		
		// strncpy(new_token->child->output + 8, new_token->child->text, len);
		new_token->child->output[0] = (oli >> 56) & 0xff;
		new_token->child->output[1] = (oli >> 48) & 0xff;
		new_token->child->output[2] = (oli >> 40) & 0xff;
		new_token->child->output[3] = (oli >> 32) & 0xff;
		new_token->child->output[4] = (oli >> 24) & 0xff;
		new_token->child->output[5] = (oli >> 16) & 0xff;
		new_token->child->output[6] = (oli >>  8) & 0xff;
		new_token->child->output[7] = (oli >>  0) & 0xff;
		new_token->child->output_size = 8 + oli + (oli % 2);
		// size_t retlen = ((new_token->child->output[0]) << 8) + (new_token->child->output[1]);
		printf("created TK_STRING output with content length %i for source string '%s'\n", len, new_token->child->text);
	}
	
	if(last_token != NULL)
		last_token->next = new_token;

	return new_token;
}

link<token>* tokenize(char* raw_text) {
	fpos_t ws = -1;
	fpos_t we = -1;
	fpos_t ls = 0;
	fpos_t mode = 0; // 0 = code, 1 = double string, 2 = comment, 3 = single string
	link<token>* first_token = new link<token>();
	first_token->child = new token();
	first_token->child->type = TK_BEGIN;

	link<token>* last_token = first_token;
	fpos_t n = 1;
	fpos_t ln = 1;
	
	for(fpos_t i = 0; raw_text[i] != '\0'; ++i) {
		char d = raw_text[i + 1];
		char c = raw_text[i];

		// printf("Chars: %c%c\n", c, d);

		if(c == '\r' && d == '\n') {
			++i;
		}
		
		if(mode == 0) {
			if(c == '"') {
				if(we - ws >= 0) {
					last_token = add_token(last_token, raw_text, ws, we, TK_NAME, n++, ln);
				}
				ws = i;
				mode = 1;
			} else if(c == '\'') {
				if(we - ws >= 0) {
					last_token = add_token(last_token, raw_text, ws, we, TK_NAME, n++, ln);
				}
				ws = i;
				mode = 3;
			} else if(c == ',') {
				if(we - ws >= 0) {
					last_token = add_token(last_token, raw_text, ws, we, TK_NAME, n++, ln);
				}
				last_token = add_token(last_token, raw_text, i, i, TK_COMMA, n++, ln);
				we = ws = -1;
			} else if(c == '+') {
				if(we - ws >= 0) {
					last_token = add_token(last_token, raw_text, ws, we, TK_NAME, n++, ln);
				}
				last_token = add_token(last_token, raw_text, i, i, TK_PLUS, n++, ln);
				we = ws = -1;
			/*} else if(c == '-') {
				if(we - ws >= 0) {
					last_token = add_token(last_token, raw_text, ws, we, TK_NAME, n++, ln);
				}
				last_token = add_token(last_token, raw_text, i, i, TK_MINUS, n++, ln);
				we = ws = -1; */
			} else if(c == ';') {
				if(we - ws >= 0) {
					last_token = add_token(last_token, raw_text, ws, we, TK_NAME, n++, ln);
				}
				mode = 2;
				we = ws = -1;
			} else if(c < 33) {
				if(we - ws >= 0) {
					last_token = add_token(last_token, raw_text, ws, we, TK_NAME, n++, ln);
				}
				we = ws = -1;
				if(c == '\n' || c == '\r') {
					++ln;
				}
			} else {
				if(ws == -1)
					ws = i;
				we = i;
			}
		} else if(mode == 1) {
			if(c == '\\' && d == '\\') {
				++i;
			} else if(c == '\\' && d == '"') {
				++i;
			} else if(c == '"') {
				we = i;
				last_token = add_token(last_token, raw_text, ws, we, TK_LIT_STRING, n++, ln);
				mode = 0;
				ws = we = -1;
			}
		} else if(mode == 3) {
			if(c == '\\' && d == '\\') {
				++i;
			} else if(c == '\\' && d == '\'') {
				++i;
			} else if(c == '\'') {
				we = i;
				last_token = add_token(last_token, raw_text, ws, we, TK_LIT_STRING, n++, ln);
				mode = 0;
				ws = we = -1;
			}
		} else if(mode == 2) {
			if(c == '\r' || c == '\n') {
				mode = 0;
				ws = we = -1;
			}
		}

		/*
		if(c == 0x0a || c == 0x0d) {
			char* line_text = make_substr(raw_text, ls, i);
			printf("\nline %i: %s\ntokens: ", ln, line_text);
			free(line_text);
			ls = i;
			++ln;
		} */
	}
	link<token>* t = first_token->next;
	fpos_t j = 0;
	ln = 0;
	while(t != NULL) {
		if(t->child != NULL) {
			/*if(t->child->line != ln)
				printf("\nLine %i: ", t->child->line); */
			ln = t->child->line;
			// printf("<%s>", t->child->text);
		}
		t = t->next;
	}
	fprintf(stderr, " -- tokenized successfully\n");
	return first_token;
}