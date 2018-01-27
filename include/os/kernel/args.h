#ifndef ARGS_H
#define ARGS_H

#define MAX_ARGS 32
#define MAX_ARG_LEN 32

int split_to_args(char *in, char **out);
void clear_args(char **args, int arg_len);

#endif