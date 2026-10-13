#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"
//#include "pico/multicore.h"
#include "hardware/irq.h"
#include "tusb.h"

#include "bmt_config.h"
//#include "display.h"
#include "bmt/disp.h"
#include "bmt/main.h"

#if   FRAME_WIDTH == 640
#	define CWPX "8"
#	define COLS "40"
#elif FRAME_WIDTH == 720
#	define CWPX "8"
#	define COLS "45"
#elif FRAME_WIDTH == 800
#	define CWPX "8"
#	define COLS "50"
#elif FRAME_WIDTH == 960
#	define CWPX "8"
#	define COLS "60"
#elif FRAME_WIDTH == 1280
#	define CWPX "8"
#	define COLS "80"
#else
#	error
#endif

#if   FRAME_HEIGHT == 480
#	define LHPX "16"
#	define ROWS "15"
#elif FRAME_HEIGHT == 600
#	define LHPX "15"
#	define ROWS "20"
#elif FRAME_HEIGHT == 540
#	define LHPX "15"
#	define ROWS "18"
#elif FRAME_HEIGHT == 720
#	define LHPX "15"
#	define ROWS "24"
#endif

/*static bool __not_in_flash_func(repeating_timer_callback)(struct repeating_timer *t)
{
	(void)t;
	yield_display();
	return true;
}
static void core1_main()
{
	struct repeating_timer timer;
	init_display(display_frame_buffer);
	add_repeating_timer_us(1, repeating_timer_callback, NULL, &timer);
	display_main();
}*/

int main()
{
	int ret = 0;
	uint32_t u;

	struct bmt_settings settings[1] = {{
		/* font = */ FONT,
		/* mode = */ MODE,
		/* lhpx = */ LHPX,
		/* cwpx = */ CWPX,
		/* rows = */ ROWS,
		/* cols = */ COLS,
		/* user = */ USER,
		/* pass = */ PASS
	}};

	/*for (u = 0 ; u < (FRAME_BUFFER_HEIGHT * FRAME_BUFFER_WIDTH) ; u++) {
		display_frame_buffer[u] = (u % FRAME_BUFFER_WIDTH) & 0xff;
	}*/

	//sysinit_display();
	//multicore_launch_core1(core1_main);
	bmt_disp->init();

	stdio_init_all();

	// uart initialize
	uart_init(UART_ID, UART_BOUD_RATE);
	gpio_set_function(GPIO_UART_TX, GPIO_FUNC_UART);
	gpio_set_function(GPIO_UART_RX, GPIO_FUNC_UART);
	gpio_set_function(GPIO_UART_CTS, GPIO_FUNC_UART);
	gpio_set_function(GPIO_UART_RTS, GPIO_FUNC_UART);
	uart_set_hw_flow(UART_ID, true, true); 
	uart_set_format(UART_ID, 8, 1, UART_PARITY_NONE);
	uart_set_fifo_enabled(UART_ID, true);
	for (u = time_us_32() ; (time_us_32() - u) < 2000000 ; ) {
		if (uart_is_readable(UART_ID)) (void)uart_getc(UART_ID);
	}

	tusb_init();

	bmt_main(settings);

	return ret;
}