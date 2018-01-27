#include <kernel/args.h>

int split_to_args(char *in, char **out) {
    int arg_i = 0;
    int sub_i = 0;

    do {
        if ((*in) == ' ') {
            arg_i++;
            sub_i = 0;
            continue;
        }
        out[arg_i][sub_i++] = *in;
    } while ((*in++) != 0);

    return arg_i+1;
}
