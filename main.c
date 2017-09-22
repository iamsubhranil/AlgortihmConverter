#include "strutils.h"
#include <stdio.h>
#include <stdlib.h>

static char *getindent(char **input){
    char *indent = NULL;
    size_t pointer = 0, len = strlen(*input), temp = 0;
    while(temp < len && ((*input)[temp] == ' ' || (*input)[temp] == '\t')){
        indent = addToBuffer(indent, &pointer, (*input)[temp]);
        temp++;
    }
    indent = addToBuffer(indent, &pointer, '\0');
    //printf("\n[getindent] indent [%s] input [%s] \n", indent, (*input));
    (*input) = strpart(*input, temp, strlen(*input));
    return indent;
}

static int getnextend(char **line, char *indent, int pointer, int count, int step){
    int ret = step;
    while(pointer < count - 1){
        char *temp = strdup(line[pointer]);
        if(strcmp(getindent(&temp), indent) == 0){
            if(strcasecmp(temp, "EndWhile") == 0
                    || strcasecmp(temp, "EndFor") == 0){
                //printf("\nMatched for indent [%s] and pointer %d Ret : %d \n", indent, start, ret);
                return ret;
            }
            else
                ret--;
        }
        else if(strcasecmp(temp, "EndIf") == 0 ||
                strcasecmp(temp, "EndWhile") == 0
             || strcasecmp(temp, "EndFor") == 0)
            ret--;
        free(temp);
        ret++;
        pointer++;
    }
    return -1;
}

void printspace(FILE *fp, int c){
    while(c--)
        fprintf(fp, " ");
}

int main(int argc, char **argv){
    if(argc != 3){
        printf("\n[Error] Wrong arguments!\nUsage : ./algoconv input_file output_file\n");
        return 1;
    }
    char *source = argv[1];
    char *dest = argv[2];
    FILE *ifile = fopen(source, "rb");
    FILE *ofile = fopen(dest, "w");
    if(!ifile || !ofile){
        printf("\n[Error] Unable to access one or more files!\n");
        fclose(ifile);
        fclose(ofile);
        return 2;
    }
    char **line = NULL;
    int count = 1;
    size_t lsize = 0;
    while(lsize != 1){
        line = (char **)realloc(line, sizeof(char **) * count);
        lsize = readline(&line[count - 1], ifile);
        //printf("%lu ", lsize);
        count++;
    }
    int pointer = 0, isnested = 0, step = 0;
    while(pointer < (count - 2)){
        //printf("\nCurrent line : [%s]", line[pointer]);
        char *indent = getindent(&(line[pointer]));
        //printf("\nLine after indent : [%s]", line[pointer]);
       // printf("\nCurrent line : [%s]", line[pointer]);
        if(strcasecmp(line[pointer], "EndIf") == 0){
            //printf(" IsEndIf : true \n");
            printspace(ofile, 10);
            fprintf(ofile, "%s", indent);
            fprintf(ofile, "[End of if structure]\n");
        }
        else if(strcasecmp(line[pointer], "EndFor") == 0 || strcasecmp(line[pointer], "EndWhile") == 0){
            //printf(" IsEndFor : true \n");
            printspace(ofile, 10);
            fprintf(ofile, "%s", indent);
            fprintf(ofile, "[End of %sfor loop]\n", isnested > 1 ? "inner " : "");
            isnested--;
        }
        else{
            //printf(" isStatement : true");
            fprintf(ofile, "Step %2d : ", step + 1);
            fprintf(ofile, "%s", indent);
            char *nextline = strdup(line[pointer + 1]);
            //printf(" NextLine : [%s] ", nextline);
            char *nextindent = getindent(&nextline);
            //printf(" NextIndent : [%s] \n", nextindent);
            if(startsWith("While", nextline)){ // While (a < b)
                //printf("\n[While in next line]");
                char *condition = str_replace(nextline, "While", ""), **conparts = NULL; // [ (a < b) ]
                // Replace condition with our identifer
                if(strstr(condition, "<=") != NULL)
                    condition = str_replace(condition, "<=", "#");
                else if(strstr(condition, ">=") != NULL)
                    condition = str_replace(condition, ">=", "#");
                else if(strstr(condition, "<") != NULL)
                    condition = str_replace(condition, "<", "#");
                else if(strstr(condition, ">") != NULL)
                    condition = str_replace(condition, ">", "#");
                // Pivot from the identifier            
                size_t parts = splitIntoArray(condition, &conparts, '#'); // [ (a, b) ]
                condition = conparts[parts - 1]; // b)
                if(condition[strlen(condition) - 1] == ')') 
                    condition = strpart(condition, 0, strlen(condition) - 1); // b
                getindent(&condition);
                char *init = str_replace(line[pointer], "Set ", "");
                int end = getnextend(line, nextindent, pointer, count, step);
                fprintf(ofile, "Repeat through step %d to step %d for %s to %s\n", step + 2, end, init, condition);
                pointer++;
                isnested++;
            }
            else{
                fprintf(ofile, "%s\n", line[pointer]); 
            }
            free(nextline);
            free(nextindent);
            step++;
        }
        //printf("\n");
        pointer++;
    }
    fclose(ifile);
    fclose(ofile);
    return 0;
}
