#ifndef HCC_STRING_H
#define HCC_STRING_H

#include <stddef.h>
#include <stdbool.h>

typedef char HcChar;

typedef struct HcString_ {
    HcChar* data;
    size_t size;
} HcString;

/**
 * Creates a new HcString.
 * @param content The string to store.
 * @param length The length of the string.
 * @return An HcString with content as content and length as size.
 */
HcString HcString_new(const HcChar* content, size_t length);
/**
 * Creates a new HcString, automatically detecting the size of the string.
 * @param content The string to store.
 * @return An HcString with content as content and size automatically detected.
 */
HcString HcString_new_a(const HcChar* content);

HcChar* HcString_toCharPtr(HcString s);

HcString HcString_append(HcString a, HcString b);
HcString HcString_append_s(HcString a, const HcChar* b);
HcString HcString_append_c(HcString a, HcChar b);

HcChar HcString_getChar(HcString str, size_t index);

int HcString_compare(HcString a, HcString b);
int HcString_compare_s(HcString a, const HcChar* b);

bool HcString_contains(HcString str, HcString target);
bool HcString_contains_c(HcString str, HcChar target);

#endif
