#include "strutils.h"
#include <stdlib.h>

/*
 * Adds the given character to the buffer. Since it modifies
 * the buffer itself, it returns the pointer in any case.
 * However, the pointer may not be reassigned to a new address
 * after calling realloc(). Also, there is no typical NULL
 * check after realloc, because it will practically never happen.
 * 
 * Arguments => buffer : The buffer to extend
 * 		bufferSize : A pointer to the present size of the buffer
 * 		add : The character to add
 * Returns   => The newly relocated buffer
 */
char *addToBuffer(char *buffer, size_t *bufferSize, char add) {
    //char *backup = buffer;
    buffer = (char *) realloc(buffer, ++(*bufferSize)); //Call realloc to extend the buffer to bufferSize+1
    //if(backup!=buffer && *bufferSize>1)
    //	free(backup);
    buffer[*bufferSize - 1] = add; //Add the character to the newly available position
    return buffer;
}

/*
 * Limited getline() implementation for non POSIX(read Windows)
 * systems. This method reads a line of arbitrary length from stdin, 
 * stores that in buffer, returning the number of characters read.
 * 
 * Arguments => buffer : The buffer to store the line, terminated with
 * 			EOF or '\n' as applicable
 * Returns => The number of characters read from stdin
 */
size_t readline(char **buffer, FILE *fp) {
    size_t read_size = 0; // The read counter
    (*buffer) = (char *) malloc(sizeof(char)); // Allocate atleast one char of memory
    int c = 1; // Temporary character to store stdin read

    while (c != EOF && c != '\n') { // Continue until the end of line
        c = getc(fp); // Read a character from stdin
        (*buffer) = addToBuffer((*buffer), &read_size,
                                (c == '\n' || c == EOF || c == '\r') ? '\0' : c); // Add it to the buffer
        if (c == '\r') // Windows
            c = getc(fp);
    }
    return read_size; // Return the amount of characters read
}

char *stripFirst(char *val) {
    char *buffer = NULL;
    size_t len = strlen(val);
    size_t i = 1;
    size_t dummy = 0;
    while (i < len) {
        buffer = addToBuffer(buffer, &dummy, val[i]);
        i++;
    }
    if (buffer[dummy - 1] != '\0')
        buffer = addToBuffer(buffer, &dummy, '\0');
    return buffer;
}

int alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int digit(char c) {
    return (c >= '0' && c <= '9');
}

int aldigit(char c) {
    return alpha(c) || digit(c);
}

/* Splits the input string into words using the given delimiter.
 * This method is purely dynamic in nature, meaning it'll 
 * automatically add all words in the given string separated by 
 * the given delimiter into a string array. The size of the array 
 * will be returned after the separation is complete.
 *
 *
 * Arguments => input : the input string to split
 *              output : pointer to the string array in which
 *                      the parts will be stored
 *              toSplit : the character to be used as the delimiter
 *
 * Returns => number of parts in the output part array
 *
 */
size_t splitIntoArray(const char *input, char ***output, const char toSplit){
    if(input==NULL)
        return 0;
    size_t count = 0, dummy = 0;
    size_t len = strlen(input), i = 0;
    char *buffer = NULL;
    *output = NULL;
    i = 0;
    while(i < len){
        char presentChar = input[i];
        if(presentChar == toSplit){
            buffer = addToBuffer(buffer, &dummy, '\0');
            *output = (char **)realloc(*output, (sizeof(char *)*(count+1)));
            (*output)[count] = buffer;
            count++;
            buffer = NULL;
            dummy = 0;
        }
        else{
            buffer = addToBuffer(buffer, &dummy, presentChar);
        }
        i++;
    }
    if(buffer != NULL && buffer[0] != '\0'){
        buffer = addToBuffer(buffer, &dummy, '\0');
        *output = (char **)realloc(*output, sizeof(char *) * (count+1));
        (*output)[count] = buffer;
        count++;
    }
    return count;
}

/* Returns the next line of a particular input stream.
 *
 * If you have embedded some file into your program or
 * just want to split into lines a string input, this
 * method will do just that. It'll return the next line,
 * i.e. the part of string from index pointed by pointer
 * to the first newline or line feed character. The pointer
 * will be automatically updated to point to the next
 * character after \n or \r, and hence can be passed between
 * subsequent function calls without modifications.
 *
 * Arguments => input - the input stream
 *              pointer - the start index
 *
 * Returns => the next line, NULL terminated
 *
 */

char *getNextLine(char *input, size_t *pointer){
    char *buffer = NULL;
    size_t i = *pointer;
    size_t dummy = 0;
    char c = input[i];
    while(i<strlen(input) && c!='\n' && c!='\r'){
        buffer = addToBuffer(buffer, &dummy, c);
        i++;
        (*pointer)++;
        c = input[i];
    }
    (*pointer)++;
    if(c=='\r')
        (*pointer)++;
    buffer = addToBuffer(buffer, &dummy, '\0');
    return buffer;
}

// Replaces a substring(rep) of a string(orig) by another substring(with)
// From :
// https://stackoverflow.com/questions/779875/what-is-the-function-to-replace-string-in-c
// You must free the result if result is non-NULL.
char *str_replace(char *orig, char *rep, char *with) {
    char *result; // the return string
    char *ins;    // the next insert point
    char *tmp;    // varies
    int len_rep;  // length of rep (the string to remove)
    int len_with; // length of with (the string to replace rep with)
    int len_front; // distance between rep and end of last rep
    int count;    // number of replacements

    // sanity checks and initialization
    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL; // empty rep causes infinite loop during count
    if (!with)
        with = strdup("");
    len_with = strlen(with);

    // count the number of replacements needed
    ins = orig;
    for (count = 0; (tmp = strstr(ins, rep)); ++count) {
        ins = tmp + len_rep;
    }

    tmp = result = (char *)malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}

// Function to check if a string startswith another substring
int startsWith(const char *pre, const char *str){
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? 0 : strncmp(pre, str, lenpre) == 0;
}

// Returns a part of a input string from `start` to `end`
char *strpart(char *input, size_t start, size_t end){
    size_t len = strlen(input), pointer = 0;
    if(start == 0 && end == len)
        return strdup(input);
    char *ret = NULL;
    while(start < len && start < end){
        ret = addToBuffer(input, &pointer, input[start]);
        start++;
    }
    ret = addToBuffer(ret, &pointer, '\0');
    return ret;
}
