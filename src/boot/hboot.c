#include "config.h"
#include "hal/io.h"
#include "hal/uart_hboot.h"
#include "lcd_hboot.h"
#include "at45db011.h"
#include <avr/eeprom.h>
#include <kern/lock.h>

#define eeprom_wb(addr,val) eeprom_write_byte ((uint8_t *)(addr),(uint8_t)(val))
#define eeprom_rb(addr) eeprom_read_byte ((uint8_t *)(addr))

// UART baud rate
#define BAUD_RATE   19200

#define HW_VER      0x02
#define SW_MAJOR    0x01
#define SW_MINOR    0x0e
#define SIG1        0x1E
#define SIG2        0x97
#define SIG3        0x02
#define PAGE_SIZE   0x80U

void acquire(struct lock *k) {}
int try_acquire(struct lock *k) {return 1;}
void release(struct lock *k) {}
void init_lock(struct lock *k, const char *name) {}

// pointer to the start of the Main App.
void (*appBoot)(void) = 0x0000;

// address 1w -> 2b
union address_union {
    uint16_t word;
    uint8_t  byte[2];
} address;

// length 1w -> 2b
union length_union {
    uint16_t word;
    uint8_t  byte[2];
} length;

// flags
struct flags_struct {
    unsigned eeprom : 1;
    unsigned rampz  : 1;
    unsigned extflash : 1;
} flags;

// download buffer
uint8_t buff[256];
uint8_t address_high;
uint8_t pagesz=0x80;

void bootRecvChars(uint8_t count) {
    uint8_t i,j;
    for(i=0;i<count;i++) {
        while(!(UCSR0A & _BV(RXC0)));
            j = UDR0;
    }
}

void bootSendResp(uint8_t val) {
    if (uart_recv() == ' ') {
        uart_send(0x14);
        uart_send(val);
        uart_send(0x10);
    }
}

void bootSendEmpty(void) {
    if (uart_recv() == ' ') {
        uart_send(0x14);
        uart_send(0x10);
    }
}

// main bootloader
int main(void) {
    uint8_t ch, ch2;
    uint16_t w;

    io_init();
//    LED_PWR(1);
    LED_COMM(1);
    lcd_init();

    // init UART then wait
    asm volatile("nop\n\t");
    // check we actually have a main app
    if(pgm_read_byte_near(0x0000) != 0xFF) {
        if (!SWITCH_STOP())
            appBoot();
    }
    lcd_print("Happyboot v0.1");
    uart_init(BAUD_RATE);
    uart_send('\0');
    // bootloader loop
    while (1) {
        ch = uart_recv();

        if(ch=='0') {        // Hello is anyone home?
            bootSendEmpty();
        }
        // Request programmer ID
        else if(ch=='1') {
            if (uart_recv() == ' ') {
                uart_send(0x14);
                uart_send('A');    //Not using PROGMEM string due to boot block in m128 being beyond 64kB boundry
                uart_send('V');    //Would need to selectively manipulate RAMPZ, and it's only 9 characters anyway so who cares.
                uart_send('R');
                uart_send(' ');
                uart_send('I');
                uart_send('S');
                uart_send('P');
                uart_send(0x10);
            }
        }
        else if(ch=='@') {        // AVR ISP/STK500 board commands  DON'T CARE so default bootSendEmpty
            ch2 = uart_recv();
            if (ch2>0x85) uart_recv();
            bootSendEmpty();
        }
        else if(ch=='A') {        // AVR ISP/STK500 board requests
            ch2 = uart_recv();
            if(ch2==0x80) bootSendResp(HW_VER);        // Hardware version
            else if(ch2==0x81) bootSendResp(SW_MAJOR);    // Software major version
            else if(ch2==0x82) bootSendResp(SW_MINOR);    // Software minor version
            else if(ch2==0x98) bootSendResp(0x03);        // Unknown but seems to be required by avr studio 3.56
            else bootSendResp(0x00);                    // Covers various unnecessary responses we don't care about
        }
        else if(ch=='B') {        // Device Parameters  DON'T CARE, DEVICE IS FIXED
            bootRecvChars(20);
            bootSendEmpty();
        }
        else if(ch=='E') {        // Parallel programming stuff  DON'T CARE
            bootRecvChars(5);
            bootSendEmpty();
        }
        else if(ch=='P') {        // Enter programming mode
            bootSendEmpty();
        }
        else if(ch=='Q') {        // Leave programming mode
            bootSendEmpty();
        }
        else if(ch=='R') {        // Erase device, don't care as we will erase one page at a time anyway.
            bootSendEmpty();
        }
        else if(ch=='U') {        //Set address, little endian. EEPROM in bytes, FLASH in words
                                //Perhaps extra address bytes may be added in future to support > 128kB FLASH.
                                //This might explain why little endian was used here, big endian used everywhere else.
            address.byte[0] = uart_recv();
            address.byte[1] = uart_recv();
            bootSendEmpty();
        }
        else if(ch=='V') {        // Universal SPI programming command, disabled.  Would be used for fuses and lock bits.
            bootRecvChars(4);
            bootSendResp(0x00);
        }
        else if(ch=='d') {        // Write memory, length is big endian and is in bytes
            length.byte[1] = uart_recv();
            length.byte[0] = uart_recv();
            flags.eeprom = 0;
            unsigned char mt = uart_recv();
            if (mt == 'E') flags.eeprom = 1;
            if (mt == 'X') flags.extflash = 1;
            for (w=0;w<length.word;w++) {
              buff[w] = uart_recv();    // Store data in buffer, can't keep up with serial data stream whilst programming pages
            }
            if (uart_recv() == ' ') {
                if (flags.eeprom) {        //Write to EEPROM one byte at a time
                    for(w=0;w<length.word;w++) {
                        eeprom_wb(address.word,buff[w]);
                        address.word++;
                    }
                }
                else if (flags.extflash) {
                    at45db_fill_buffer(address.word, buff, length.word);
                    at45db_store_buffer(address.word);
                    address.word += length.word;
                }
                else {                    //Write to FLASH one page at a time
                    if (address.byte[1]>127) address_high = 0x01;    //Only possible with m128, m256 will need 3rd address byte. FIXME
                    else address_high = 0x00;
                    RAMPZ = address_high;
                    address.word = address.word << 1;    //address * 2 -> byte location
                    if ((length.byte[0] & 0x01)) length.word++;    //Even up an odd number of bytes
                    cli();                                    //Disable interrupts, just to be sure
                    while(bit_is_set(EECR,EEWE));            //Wait for previous EEPROM writes to complete
                    asm volatile("clr   r17\n\t" //page_word_count
                                 "lds   r30,address\n\t" //Address of FLASH location (in bytes)
                                 "lds   r31,address+1\n\t"
                                 "ldi   r28,lo8(buff)\n\t" //Start of buffer array in RAM
                                 "ldi   r29,hi8(buff)\n\t"
                                 "lds   r24,length\n\t" //Length of data to be written (in bytes)
                                 "lds   r25,length+1\n\t"
                                 "length_loop:\n\t" //Main loop, repeat for number of words in block
                                 "cpi   r17,0x00\n\t" //If page_word_count=0 then erase page
                                 "brne  no_page_erase\n\t"
                                 "wait_spm1:\n\t"
                                 "lds   r16,%0\n\t" //Wait for previous spm to complete
                                 "andi  r16,1\n\t"
                                 "cpi   r16,1\n\t"
                                 "breq  wait_spm1\n\t"
                                 "ldi   r16,0x03\n\t" //Erase page pointed to by Z
                                 "sts   %0,r16\n\t"
                                 "spm\n\t"
                                 "wait_spm2:\n\t"
                                 "lds   r16,%0\n\t" //Wait for previous spm to complete
                                 "andi  r16,1\n\t"
                                 "cpi   r16,1\n\t"
                                 "breq  wait_spm2\n\t"

                                 "ldi   r16,0x11\n\t" //Re-enable RWW section
                                 "sts   %0,r16\n\t"
                                 "spm\n\t"
                                 "no_page_erase:\n\t"
                                 "ld    r0,Y+\n\t" //Write 2 bytes into page buffer
                                 "ld    r1,Y+\n\t"

                                 "wait_spm3:\n\t"
                                 "lds   r16,%0\n\t" //Wait for previous spm to complete
                                 "andi  r16,1\n\t"
                                 "cpi   r16,1\n\t"
                                 "breq  wait_spm3\n\t"
                                 "ldi   r16,0x01\n\t" //Load r0,r1 into FLASH page buffer
                                 "sts   %0,r16\n\t"
                                 "spm\n\t"

                                 "inc   r17\n\t" //page_word_count++
                                 "cpi r17,%1\n\t"
                                 "brlo  same_page\n\t" //Still same page in FLASH
                                 "write_page:\n\t"
                                 "clr   r17\n\t" //New page, write current one first
                                 "wait_spm4:\n\t"
                                 "lds   r16,%0\n\t" //Wait for previous spm to complete
                                 "andi  r16,1\n\t"
                                 "cpi   r16,1\n\t"
                                 "breq  wait_spm4\n\t"
                                 "ldi   r16,0x05\n\t" //Write page pointed to by Z
                                 "sts   %0,r16\n\t"
                                 "spm\n\t"
                                 "wait_spm5:\n\t"
                                 "lds   r16,%0\n\t" //Wait for previous spm to complete
                                 "andi  r16,1\n\t"
                                 "cpi   r16,1\n\t"
                                 "breq  wait_spm5\n\t"
                                 "ldi   r16,0x11\n\t" //Re-enable RWW section
                                 "sts   %0,r16\n\t"
                                 "spm\n\t"
                                 "same_page:\n\t"
                                 "adiw  r30,2\n\t" //Next word in FLASH
                                 "sbiw  r24,2\n\t" //length-2
                                 "breq  final_write\n\t" //Finished
                                 "rjmp  length_loop\n\t"
                                 "final_write:\n\t"
                                 "cpi   r17,0\n\t"
                                 "breq  block_done\n\t"
                                 "adiw  r24,2\n\t" //length+2, fool above check on length after short page write
                                 "rjmp  write_page\n\t"
                                 "block_done:\n\t"
                                 "clr   __zero_reg__\n\t" //restore zero register
                                 : "=m" (SPMCR) : "M" (PAGE_SIZE) : "r0","r16","r17","r24","r25","r28","r29","r30","r31");

    /* Should really add a wait for RWW section to be enabled, don't actually need it since we never */
    /* exit the bootloader without a power cycle anyhow */
                }
                uart_send(0x14);
                uart_send(0x10);
            }
        }
        else if(ch=='t') {        //Read memory block mode, length is big endian.
            length.byte[1] = uart_recv();
            length.byte[0] = uart_recv();
            if (address.word>0x7FFF) flags.rampz = 1;        // No go with m256, FIXME
            else flags.rampz = 0;
            if (uart_recv() == 'E') flags.eeprom = 1;
            else {
                flags.eeprom = 0;
                address.word = address.word << 1;    //address * 2 -> byte location
            }
            if (uart_recv() == ' ') {        // Command terminator
                uart_send(0x14);
                for (w=0;w < length.word;w++) {        // Can handle odd and even lengths okay
                    if (flags.eeprom) {    // Byte access EEPROM read
                        uart_send(eeprom_rb(address.word));
                        address.word++;
                    }
                    else {
                        if (!flags.rampz) uart_send(pgm_read_byte_near(address.word));
                        else uart_send(pgm_read_byte_far(address.word + 0x10000));    // Hmmmm, yuck  FIXME when m256 arrvies
                        address.word++;
                    }
                }
                uart_send(0x10);
            }
        }
        else if(ch=='u') {        // Get device signature bytes
            if (uart_recv() == ' ') {
                uart_send(0x14);
                uart_send(SIG1);
                uart_send(SIG2);
                uart_send(SIG3);
                uart_send(0x10);
            }
        }
        else if(ch=='v') {        // Read oscillator calibration byte
            bootSendResp(0x00);
        }
    }
}
