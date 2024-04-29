#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>

#define MAX_ARGC 16


void assert_ok(int status) {
    if (status == -1) {
        perror("Error");
        exit(EXIT_FAILURE);
    }
}

typedef struct {
    pid_t pid;
    const char* path;
    const char* argv[MAX_ARGC+1];
    int fd_in;
    int fd_out;
} Subprocess;

Subprocess* create_subprocess1(int argc, const char* argv[]) {
    int arg_index;
    int in_channel[2];
    int out_channel[2];
    Subprocess* subprocess;

    if (argc > MAX_ARGC) {
        return NULL;
    }

    subprocess = malloc(sizeof(Subprocess));
    subprocess->path = argv[0];
    for (arg_index = 0; arg_index < argc; ++arg_index) {
        subprocess->argv[arg_index] = argv[arg_index];
    }
    argv[argc] = NULL;

    assert_ok(pipe(in_channel));
    assert_ok(pipe(out_channel));

    subprocess->pid = fork();
    assert_ok(subprocess->pid);
    
    if (subprocess->pid == 0) {
        dup2(in_channel[0], 0);
        dup2(out_channel[1], 1);
        close(in_channel[0]);
        close(in_channel[1]);
        close(out_channel[0]);
        close(out_channel[1]);     
        execvp(subprocess->path, (char**)subprocess->argv);
        assert_ok(-1);
    }
    close(in_channel[0]);
    close(out_channel[1]);

    subprocess->fd_in = out_channel[0];
    subprocess->fd_out = in_channel[1];

    return subprocess;

}

Subprocess* create_subprocess2(const char* path, ...) {
    const char* argv[MAX_ARGC+1];
    int arg_index;
    const char* next_arg;
    va_list args;

    argv[0] = path;
    va_start(args, path);
    arg_index = 1;
    next_arg = va_arg(args, const char*);
    while (arg_index <= MAX_ARGC && next_arg != NULL) {
        argv[arg_index++] = next_arg;
        next_arg = va_arg(args, const char*);
    }
    va_end(args);

    return create_subprocess1(arg_index, argv);
}

struct {
    int fd;
    int begin;
    int size;
    char* buffer[BUFSIZ];
} Buffered_input;

char consume_character(Buffered_input* input) {
    char front_char = input->buffer[input->begin];
    ++(input->begin);
    --(input->size);
    if (input->begin==BUFSIZ | input->size==0) {
        input->begin = 0;
    }
    return front_char;
}

int readable_segment(Buffered_input* input, char** dst) {
    int end = input->begin + input->size;
    if (end >= input->BUFSIZ) {
        end -= BUFSIZ;
    }
    int available_space = end < input->begin? input->begin - end : BUFSIZ - end;
    *dst = &(input->buffer[end]);
    return available_space
}

int read_line(Buffered_input* input, char* dst, int timeout_ms) {
    int num_read_chars = 0;
    while (1) {
        char c = '\0';
        while (input->buffer_size > 0 && (c=consume_character(input)) != '\n') {
            dst[num_read_chars] = c;
            ++num_read_chars;
        }
        if (c == '\n') {
            break;
        }
        char* segment;
        int max_readable_size = readable_segment(input, &segment);
    }
}

int main() {
    Subprocess* subprocess = create_subprocess2("echo", "hey", NULL);

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(subprocess->fd_in, &readfds);

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    int ready = select(subprocess->fd_in+1, &readfds, NULL, NULL, &timeout);

    printf("Ready: %d\n", ready);

    char buff[80];
    int r = read(subprocess->fd_in, buff, 80);
    buff[r] = '\0';
    printf("%s", buff);


    /*int fd[2];*/
    /*char* argv_subprocess[] = {"Hey", NULL};*/
    /*char buff[80];*/
    /*subprocess("echo", argv_subprocess, fd);*/
    /*int r = read(fd[0], buff, 70);*/
    /*buff[r] = '\0';*/
    /*printf("Read %d bytes: %s\n", r, buff);*/
}
