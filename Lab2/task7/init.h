/* UART serial communication */
void uart_init(unsigned long);
void uart_transmit(unsigned char);
unsigned char uart_receive(void);

/* Interrupt */
void interrupt_setup();
void pin_change_setup();

/* Input/Output */
void io_init();