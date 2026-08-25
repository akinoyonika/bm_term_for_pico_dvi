#ifndef __BMT_MAIN_H__
#define __BMT_MAIN_H__

#define UART_ID (uart0)
// #define UART_BOUD_RATE (921600)
#define UART_BOUD_RATE (3000000)
#define GPIO_UART_TX  (0) // PIN 1
#define GPIO_UART_RX  (1) // PIN 2
#define GPIO_UART_CTS (2) // PIN 4
#define GPIO_UART_RTS (3) // PIN 5

#define BMT_TIMEOUT_US (1000000)

struct bmt_settings {
	char const *font;
	char const *mode;
	char const *lhpx;
	char const *cwpx;
	char const *rows;
	char const *cols;
	char const *user;
	char const *pass;
};

int bmt_main(struct bmt_settings *settings);

#endif /* __BMT_MAIN_H__ */