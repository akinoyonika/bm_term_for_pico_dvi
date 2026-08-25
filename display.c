#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/clocks.h"
#include "hardware/irq.h"
#include "hardware/sync.h"
#include "hardware/gpio.h"
#include "hardware/vreg.h"
#include "hardware/structs/bus_ctrl.h"

#include "dvi.h"
#include "dvi_serialiser.h"
#include "common_dvi_pin_configs.h"

#include "bmt_config.h"
#include "display.h"

// frame buffer: 8bpp, RGB332
uint8_t display_frame_buffer[FRAME_BUFFER_WIDTH * FRAME_BUFFER_HEIGHT];
//__attribute__((aligned(65536))) uint8_t display_frame_buffer[FRAME_BUFFER_WIDTH * FRAME_BUFFER_HEIGHT];

static struct dvi_inst _dvi0;
static uint8_t const *_frame_buffer = NULL;

void sysinit_display()
{
	irq_set_enabled(IO_IRQ_BANK0, false);
	vreg_set_voltage(VREG_VSEL);
	sleep_ms(10);
	set_sys_clock_khz(DVI_TIMING.bit_clk_khz, true);
}

void init_display(uint8_t const *frame_buffer)
{
	_dvi0.timing = &DVI_TIMING;
	_dvi0.ser_cfg = DVI_DEFAULT_SERIAL_CONFIG;
	dvi_init(&_dvi0, next_striped_spin_lock_num(), next_striped_spin_lock_num());

	_frame_buffer = frame_buffer;
}

int yield_display()
{
	static int state = 0;
	static uint y = 0;
	int ret = 0;
	uint8_t const *buf = _frame_buffer + y / 2 * FRAME_BUFFER_WIDTH;

	switch (state) {
	case 0:
		if (queue_try_add_u32(&_dvi0.q_colour_valid, &buf)) {
			state = 1;
		}
		break;
	case 1:
		if (!queue_try_remove_u32(&_dvi0.q_colour_free, &buf)) {
			ret = 1;
			if (++y == FRAME_HEIGHT) {
				y = 0;
				ret = -1;
			}
			state = 0;
		}
		break;
	default:
		break;
	}

	return ret;
}

void display_main()
{
	dvi_register_irqs_this_core(&_dvi0, DMA_IRQ_1);
	while (queue_is_empty(&_dvi0.q_colour_valid)) __wfe();
	dvi_start(&_dvi0);
	dvi_scanbuf_main_8bpp(&_dvi0);
}
