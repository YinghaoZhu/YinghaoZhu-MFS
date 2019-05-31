#ifndef _LOG_H
#define _LOG_H


#define __PRINTF__ __attribute__((format(printf, 1, 2)));
extern int log_verbose;
extern void log_open();
extern void log_close();
extern void log_puts(char *msg);
extern void log_printf(char *format, ...) __PRINTF__;
extern int  log_error(char *format, ...) __PRINTF__;
extern void log_panic(char *format, ...) __PRINTF__;

#endif
