#include "../headers/utils.h"

#include <stdlib.h>
#include <string.h>
#include <linux/limits.h>

/*This function will install the required dependencies */
void install_dependencies(void){
    char resolved_path[PATH_MAX];
    char* command = (char*) malloc(sizeof(char)*MAX_INPUT);
    realpath("assets/", resolved_path);
    strcat(command,resolved_path);
    strcat(command,"/./install_binary_dependencies.sh");
    system(command);
    free(command);
}

size_t get_file_board_width(FILE *fp) {
    fseek(fp, 0, SEEK_SET);
    char* line = NULL;
    size_t lineLength = 0;
    size_t width;
    width = getline(&line,&lineLength,fp) - 1; /* Get the length line, ie the width (minus '\n') */
    free(line);
    return width;
}

size_t get_file_board_height(FILE *fp){
    fseek(fp,0, SEEK_SET);
    size_t height = 0;
    char* line = NULL;
    size_t lineLength = 0;
    while((getline(&line, &lineLength, fp)) !=  EOF){
        /* line history delimiter */
        if(strcmp(line, "====\n") == 0)
            break;

        height++;
    }
    free(line);
    return height;
}


bool check_file(FILE *fp, Error* err)
{
    if(fp == NULL) {
        *err = FILE_PTR_ERR;
        return false;
    }

    char* lineBuffer = NULL;
    size_t bufferSize = 0; /* if set to 0, then getline() will allocate the buffer */
    size_t width = getline(&lineBuffer, &bufferSize, fp) - 1; /* Get the length line, ie the width (minus '\n') */
    free(lineBuffer);

    lineBuffer = NULL;
    bufferSize = 0;
    fseek(fp, 0, SEEK_SET); /* Set cursor at the beginning of the file */
    size_t col;
    while((getline(&lineBuffer, &bufferSize, fp)) !=  EOF) {
        /* Line history delimiter, exit while loop */
        if(strcmp(lineBuffer, "====\n") == 0)
            break;

        if(strlen(lineBuffer) - 1 != width) {
            *err = FILE_DIMENSION_ERR;
            return false;
        }

        for (col = 0; col < width ; col++) {
            /* check if char is valid */
            if (lineBuffer[col] != '.' && lineBuffer[col] != 'o' && lineBuffer[col] != '\n') {
                *err = FILE_UNKNOWN_CHAR_ERR;
                free(lineBuffer);
                return false;
            }
        }
    }
    free(lineBuffer);
    return true;

}

void empty_input_buffer(void) {
    while (getchar()!='\n');
}

/*@requires nothing
  @assigns nothing
  @ensures returns randon number between min and max */
int get_random_number(int min, int max)
{
    int random = rand() % (max - min + 1) + min;
    return random;
}