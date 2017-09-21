#ifndef STRUTILS_H
#define STRUTILS_H

#include <string.h>
#include <stdio.h>

char *addToBuffer(char *buffer, size_t *bufferSize, char toAdd);

size_t readline(char **buffer, FILE *fp);

char *stripFirst(char *val);

int digit(char c);

int alpha(char c);

int aldigit(char c);

size_t splitIntoArray(const char *input, char ***output, const char toSplit);

char *getNextLine(char *input, size_t *pointer);

char *str_replace(char *inpt, char *rep, char *with);

int startsWith(const char *pre, const char *str);

char *strpart(char *input, size_t start, size_t end);
#endif
