#include <hcc/string.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

HcString HcString_new(const HcChar* content, size_t size) {
    HcString str;
    str.size = size;
    str.data = calloc(size, sizeof(char));
    memcpy(str.data, content, size);
    return str;
}

HcString HcString_new_a(const HcChar* content) {
    return HcString_new(content, strlen(content));
}

HcChar* HcString_toCharPtr(HcString s) {
    HcChar* cptr = calloc(s.size + 1, sizeof(HcChar));
    for (size_t i = 0; i < s.size; i++) {
        cptr[i] = s.data[i];
    }
    return cptr;
}
HcChar* HcString_toCharPtr_p(HcString* s) {
    if (s == NULL) {
        HcChar* dat = calloc(2, sizeof(HcChar));
        dat[0] = ' ';
        return dat;
    }
    return HcString_toCharPtr(*s);
}

HcString HcString_append(HcString a, HcString b) {
    size_t len = a.size + b.size;
    char* data = calloc(len, sizeof(HcChar));
    for (size_t i = 0; i < a.size; i++) {
        data[i] = HcString_getChar(a, i);
    }
    for (size_t i = 0; i < b.size; i++) {
        data[i + a.size] = HcString_getChar(b, i);
    }
    return HcString_new(data, len);
}

HcString HcString_append_s(HcString a, const HcChar* b) {
    return HcString_append(a, HcString_new_a(b));
}

HcString HcString_append_c(HcString a, const HcChar b) {
    return HcString_append(a, HcString_new(&b, 1));
}

HcChar HcString_getChar(HcString str, size_t index) {
    return str.data[index];
}

int HcString_compare(HcString a, HcString b) {
    if ((a.size != b.size) || (a.size == 0 || b.size == 0)) {
        return memcmp(&a.size, &b.size, sizeof(size_t));
    }
    return memcmp(a.data, b.data, a.size);
}

int HcString_compare_s(HcString a, const HcChar* b) {
    return HcString_compare(a, HcString_new_a(b));
}

bool HcString_contains(HcString str, HcString target) {
    return strstr(HcString_toCharPtr(str), HcString_toCharPtr(target)) != NULL;
}

// it's more efficient
bool HcString_contains_c(HcString str, HcChar target) {
    for (size_t i = 0; i < str.size; i++) {
        if (str.data[i] == target) {
            return true;
        }
    }
    return false;
}

HcString HcString_format(const HcChar* fmt, size_t maxSize, ...) {
    va_list ap;
    va_start(ap, maxSize);
    HcChar* dat = (char*) calloc(maxSize, sizeof(char));
    size_t len = snprintf(dat, maxSize, fmt, ap);
    va_end(ap);
    return HcString_new(dat, len);
}
