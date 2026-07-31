#ifndef UART_READER_H
#define UART_READER_H

void uart_parse();

extern char data[30];

extern int status;
extern int first_ir;
extern int second_ir;
extern char color;


#endif