#ifndef VIOS_STRING_H
#define VIOS_STRING_H

#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>

    char tolower(char s1);
int strlen(const char *ptr);
int strnlen(const char *ptr, int max);
int strnlen_terminator(const char *str, int max, char terminator);
int istrncmp(const char *s1, const char *s2, int n);
int strncmp(const char *str1, const char *str2, int n);
int strcmp(const char *s1, const char *s2);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, int count);
char *strcat(char *dest, const char *src);
bool isdigit(char c);
int tonumericdigit(char c);
char *strtok(char *str, const char *delimiters);
char *strchr(const char *str, int search_str);
int snprintf(char *buffer, size_t size, const char *format, ...);

#endif