#ifndef STRING_HPP
#define STRING_HPP
#include "../main.hpp"

class string {
    private:
        size_t L;
        char* T;
        char* range_s(size_t start, size_t span) const {
            char* buf = (char*)malloc(sizeof(char) * span);
            memcpy(buf, &(this->T[start]), span);
            return buf;
        };
    public:
        string(const char* chars) {
            this->L = strlen(chars);
            this->T = (char*)malloc(sizeof(char) * this->L);
            memcpy(this->T, chars, this->L);
        };
        string(const string &that) {
            this->L = that.L;
            if(this->L) {
                this->T = this->T = (char *)malloc(sizeof(char) * this->L);
                memcpy(this->T, that.T, this->L);
            } else {
                this->L = NULL;
            }
        };
        string() {
            this->L = 0;
            this->T = NULL;
        };
        ~string() {
            if(this->T != NULL)
                free(this->T);
        };
        char* range(size_t start, size_t end) const {
            if(end < start)
                return NULL;
            if(end >= this->L)
                end = this->L - 1;
            if(start >= end)
                start = end;
            size_t span = end - start + 1;
            char* buf = (char*)malloc(sizeof(char) * (span + 1));
            memcpy(buf, &(this->T[start]), span);
            buf[span] = 0;
            return buf;
        };
        char* direct() const {
            return this->T;
        }
        char* chars() {
            char* buf = (char*)malloc(sizeof(char) * (this->L + 1));
            memcpy(buf, this->T, this->L);
            buf[this->L] = 0;
            return buf;
        }
        string substr(size_t start, size_t end) const {
            if(end < start)
                return NULL;
            if(end >= this->L)
                end = this->L - 1;
            if(start >= end)
                start = end;
            size_t span = end - start + 1;
            string outs = string();
            outs.L = span;
            outs.T = this->range_s(start, span);
            return outs;
        };
        size_t strpos(const string& that) const {
            if(!this->L || !that.L || that.L > this->L)
                return NOWHERE;
            for(size_t i = 0; i < this->L; i++) {
                if(this->T[i] == that.T[0]) {
                    for(size_t j = 0; j < that.L; j++) {
                        if(this->T[i+j] != that.T[j])
                            goto mismatch;
                    }
                    return i;
                    mismatch:
                    continue;
                }
            }
            return NOWHERE;
        };
        char operator[](const size_t i) const {
            if(i >= this->L)
                return 0;
            return this->T[i];
        }
        string operator+(string &that) const {
            if(!this->L && !that.L)
                return string();
            if(!this->L)
                return string(that);
            if(!that.L)
                return string(*this);
            
            string result = string();
            size_t span = this->L + that.L;
            result.L = span;
            result.T = (char*)malloc(sizeof(char) * span);
            memcpy(result.T, this->T, this->L);
            memcpy(&(result.T[this->L]), that.T, that.L);
            return result;
        }
        int operator==(string& that) const {
            if(this->L != that.L)
                return false;
            if(memcmp(this->T, that.T, this->L) == 0) // already does zero-length check
                return true;
            return false;
        }
        int operator==(char* that) const {
            size_t that_len = strlen(that);
            if(this->L != that_len)
                return false;
            if(memcmp(this->T, that, this->L) == 0) // already does zero-length check
                return true;
            return false;
        }
        size_t length() const {
            return this->L;
        }
};
#endif // STRING_HPP
