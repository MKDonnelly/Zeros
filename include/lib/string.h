#pragma once
#include <lib/types.h>
//Implement as much of string.h as possible
//https://www.tutorialspoint.com/c_standard_library/string_h.htm

void memcpy(void *dest, void *src, size_t len);
void memset(void *dest, size_t len, int val);

size_t strlen(char input_str[]);
void strcpy(char *str1, char *str2);
void strncpy(char *str1, char *srt2, size_t len);
int strcmp(char *str1, char *str2);
int strncmp(char *str1, char *str2, size_t len);
char *strchr(char *str, char c);
void strcat(char *str1, char *str2);
char *strstr(char *source, char *substr);
void reverse(char str[]);
int is_in(char c, char *str);
int atoi(char *str);
void itoa(int number, char *buf);
void itoh(int, char[]);
