#pragma once

//Implement as much of string.h as possible
//https://www.tutorialspoint.com/c_standard_library/string_h.htm

int strlen(char[]);
void strcpy(char*,char*);
void strncpy(char*,char*,int);
int strcmp(char*,char*);
int strncmp(char*,char*,int);
char *strchr(char *,char);
void strcat(char*,char*);
char *strstr(char*,char*);
void reverse(char[]);
void itoa(int, char[]);
void itoh(int, char[]);

