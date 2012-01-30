#ifndef __INCLUDE_PIPE_H__
#define __INCLUDE_PIPE_H__

#include <stdio.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <kern/lock.h>
#include <kern/ring.h>

typedef struct tag_pipe {
    FILE f;
    void (*poll)(struct tag_pipe *);
    int (*flush)(struct tag_pipe *);
    ring_buf rx_buf, tx_buf;
    struct {
        unsigned translate_newlines : 1;
        unsigned flush_newlines : 1;
    } flags;
} pipe;

#define PIPE_INIT(p, tx_cap, rx_cap, pollfunc, flushfunc, translate_nl, flush_nl, dirflag) { \
    FILE f = FDEV_SETUP_STREAM(pipe_putc, pipe_getc, dirflag); \
    p.f = f; \
    p.poll = pollfunc; \
    p.flush = flushfunc; \
    if (rx_cap) ring_init(&p.rx_buf, rx_cap); \
    if (tx_cap) ring_init(&p.tx_buf, tx_cap); \
    p.flags.translate_newlines = translate_nl; \
    p.flags.flush_newlines = flush_nl; \
}

#define PIPE_INIT_RW(p, tx_cap, rx_cap, poll, flush, translate_nl, flush_nl) \
    PIPE_INIT(p, tx_cap, rx_cap, poll, flush, translate_nl, flush_nl, _FDEV_SETUP_RW);
#define PIPE_INIT_RO(p, rx_cap, poll, flush, translate_nl, flush_nl) \
    PIPE_INIT(p, 0, rx_cap, poll, flush, translate_nl, flush_nl, _FDEV_SETUP_READ);
#define PIPE_INIT_WO(p, tx_cap, poll, flush, translate_nl, flush_nl) \
    PIPE_INIT(p, tx_cap, 0, poll, flush, translate_nl, flush_nl, _FDEV_SETUP_WRITE);

#define N_PIPES     4
#define PIPE_NULL   0
#define PIPE_LCD    1
#define PIPE_UART   2
#define PIPE_RF     3

extern pipe std_pipes[4];

#define F_PIPE(p)   (&std_pipes[p].f)
#define F_NULL      F_PIPE(PIPE_NULL)
#define F_LCD       F_PIPE(PIPE_LCD )
#define F_UART      F_PIPE(PIPE_UART)
#define F_RF        F_PIPE(PIPE_RF  )

// Main entry points for printing
#define fprintf( f, fmt, ...)  pipe_fprintf_P(f, PSTR(fmt), ## __VA_ARGS__)
#define fprintf_P( f, fmt, ...)  pipe_fprintf_P(f, fmt, ## __VA_ARGS__)

#define null_printf(fmt, ...)  fprintf(F_NULL, fmt, ## __VA_ARGS__)
#define null_printf_P(fmt, ...)  fprintf_P(F_NULL, fmt, ## __VA_ARGS__)

#define lcd_printf( fmt, ...)  fprintf(F_LCD , fmt, ## __VA_ARGS__)
#define lcd_printf_P( fmt, ...)  fprintf_P(F_LCD , fmt, ## __VA_ARGS__)

#define uart_printf(fmt, ...)  fprintf(F_UART, fmt, ## __VA_ARGS__)
#define uart_printf_P(fmt, ...)  fprintf_P(F_UART, fmt, ## __VA_ARGS__)

#define rf_printf(  fmt, ...)  fprintf(F_RF  , fmt, ## __VA_ARGS__)
#define rf_printf_P(  fmt, ...)  fprintf_P(F_RF  , fmt, ## __VA_ARGS__)

#define printf(     fmt, ...)  uart_printf(    fmt, ## __VA_ARGS__)
#define printf_P(     fmt, ...)  uart_printf_P(    fmt, ## __VA_ARGS__)

int pipe_fprintf_P(FILE *f, PGM_P fmt, ...);
int pipe_vfprintf_P(FILE *f, PGM_P fmt, va_list ap);

// Main entry points for scanning
#define fscanf(f,  fmt, ...)  pipe_fscanf_P(f, PSTR(fmt), ## __VA_ARGS__)
#define null_scanf(fmt, ...)  panic("Cannot scan from NULL")
#define lcd_scanf( fmt, ...)  panic("Cannot scan from LCD")
#define uart_scanf(fmt, ...)  fscanf(F_UART, fmt, ## __VA_ARGS__)
#define rf_scanf(  fmt, ...)  fscanf(F_RF  , fmt, ## __VA_ARGS__)

#define scanf(     fmt, ...)  uart_scanf(    fmt, ## __VA_ARGS__)

int pipe_fscanf_P(FILE *f, PGM_P fmt, ...);
int pipe_vfscanf_P(FILE *f, PGM_P fmt, va_list ap);
void init_pipes();

#define fflush(f) pipe_fflush(f)
#define null_flush() pipe_fflush(F_NULL)
#define lcd_flush() pipe_fflush(F_LCD)
#define uart_flush() pipe_fflush(F_UART)
#define rf_flush() pipe_fflush(F_RF)

#endif
