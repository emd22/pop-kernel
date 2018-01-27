#include <kernel/args.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

int split_to_args(char *in, char **out) {
    int arg_i = 0;
    int sub_i = 0;

    bool in_string = false;

    do {
        switch (*in) {
            case '\'':
                in_string = !in_string;
                continue;
            case ' ':
                if (!in_string) {
                    arg_i++;
                    sub_i = 0;
                    continue;
                }
                break;
        }

        out[arg_i][sub_i++] = *in;
    } while ((*in++) != 0);

    return arg_i+1;
}

void clear_args(char **args, int arg_len) {
    int i;
    for (i = 0; i < arg_len; i++) {
        bzero(args[i], MAX_ARG_LEN);
    }
}
