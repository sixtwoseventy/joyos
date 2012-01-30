#include <kern/pipe.h>
#include <lcd.h>
#include <hal/uart.h>
#include <rf.h>
#include <kern/thread.h>

pipe std_pipes[N_PIPES];

int pipe_putc(char ch, FILE *f);
int pipe_getc(FILE *f);

void null_poll(pipe *p) {
    int n = p->rx_buf.capacity;
    if ((n = ring_size(&p->tx_buf)) != 0)
        ring_read(&p->tx_buf, NULL, n);
    return;
}

void init_pipes() {
    PIPE_INIT_WO(std_pipes[PIPE_NULL], 8,      null_poll,  NULL, 0, 0);
    PIPE_INIT_WO(std_pipes[PIPE_LCD ], 32,     lcd_poll,   NULL, 0, 0);
    PIPE_INIT_RW(std_pipes[PIPE_UART], 64, 64, uart_poll,  NULL, 1, 0);
    PIPE_INIT_RW(std_pipes[PIPE_RF  ], 64, 64, rf_poll,    rf_fflush, 1, 1);
}

int pipe_fprintf_P(FILE *f, PGM_P fmt, ...) {
    va_list ap;
    int count;

    va_start(ap, fmt);
    count = pipe_vfprintf_P(f, fmt, ap);
    va_end(ap);

    return count;
}

int pipe_vfprintf_P(FILE *f, PGM_P fmt, va_list ap) {
    pipe *p = (pipe *)f;
    int count;

    acquire(&p->tx_buf.lock);
    count = vfprintf_P(f, fmt, ap);
    release(&p->tx_buf.lock);

    return count;
}

int pipe_fscanf_P(FILE *f, PGM_P fmt, ...) {
    va_list ap;
    int count;

    va_start(ap, fmt);
    count = pipe_vfscanf_P(f, fmt, ap);
    va_end(ap);

    return count;
}

int pipe_vfscanf_P(FILE *f, PGM_P fmt, va_list ap) {
    pipe *p = (pipe *)f;
    int count;

    acquire(&p->rx_buf.lock);
    count = vfscanf_P(f, fmt, ap);
    release(&p->rx_buf.lock);

    return count;
}

int pipe_putc(char ch, FILE *f) {
    pipe *p = (pipe *)f;
    if (ch == '\n' && p->flags.translate_newlines) {
        uint8_t cr = '\r';
        while (!ring_write(&p->tx_buf, &cr, 1))
            yield();
    }
    while (!ring_write(&p->tx_buf, (uint8_t*)&ch, 1))
        yield();
    if (ch == '\n' && p->flags.flush_newlines && p->flush)
        p->flush(p);
    return ch;
}

int pipe_getc(FILE *f) {
    pipe *p = (pipe *)f;
    uint8_t ch;
    if (!ring_read(&p->rx_buf, &ch, 1))
        return EOF;
    return ch;
}

int pipe_fflush(FILE *f) {
    pipe *p = (pipe *)f;
    if (p->flush)
        return p->flush(p);
    return 0;
}

void pipe_poll() {
    for (uint8_t i=0; i<N_PIPES; i++)
        std_pipes[i].poll(&std_pipes[i]);
}
