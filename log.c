#include<stdlib.h>
#include<stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include "log.h"

int log_verbose = 0;
static FILE *log_file;

#define log_format(ap, format)    \
        va_start(ap, format);            \
        vfprintf(log_file, format, ap);    \
        va_end(ap)

static void high_on()
{
    fprintf(log_file, "\033[1m");
    fflush(log_file);
}

static void high_off()
{
    fprintf(log_file, "\033[m");
    fflush(log_file);
}

void log_open()
{
    log_file = fopen("/dev/tty", "w");
}
void log_close() 
{
    fclose(log_file);
}
void log_puts(char *msg)
{
    if(!log_verbose)
        return;
    fputs("LOG: ", log_file);
    fputs(msg, log_file);
    fputc('\n', log_file);
}

int log_error(char *format, ...)
{
    va_list ap;

    if (!log_verbose)
        return -errno;
    high_on();
    fputs("ERR: ", log_file);
    log_format(ap, format);
    high_off();
    return -errno;
}


void log_printf(char *format, ...)
{
    va_list ap;

    if (!log_verbose)
        return;
    fputs("LOG: ", log_file);
    log_format(ap, format);
}

void log_panic(char *format, ...)
{ 
    va_list ap;

    high_on();
    fputs("ERR: ", log_file);
    log_format(ap, format);
    high_off();
    exit(-1);
}


