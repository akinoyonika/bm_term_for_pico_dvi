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
#include "bmt/disp.h"

static void _init(void);
static int _begin(int w, int h);
static void _end(void);
static void _update(int x, int y, int w, int h, uint8_t *bitmap);
static void _present(void);
static void _ring(void);

struct bmt_disp_ops const bmt_disp[] = {{
	_init,
	_begin,
	_end,
	_update,
	_present,
	_ring
}};

// frame buffer: 8bpp, RGB332
static uint8_t _frame_buffer[FRAME_BUFFER_WIDTH * FRAME_BUFFER_HEIGHT];

static struct dvi_inst _dvi0[1];

static bool __not_in_flash_func(_repeating_timer_callback)(struct repeating_timer *t)
{
	(void)t;
	static int state = 0;
	static uint y = 0;
	uint8_t const *buf = _frame_buffer + y / 2 * FRAME_BUFFER_WIDTH;

	switch (state) {
	case 0:
		if (queue_try_add_u32(&(_dvi0->q_colour_valid), &buf)) {
			state = 1;
		}
		break;
	case 1:
		if (!queue_try_remove_u32(&(_dvi0->q_colour_free), &buf)) {
			if (++y == FRAME_HEIGHT) {
				y = 0;
			}
			state = 0;
		}
		break;
	default:
		break;
	}

	return true;
}

static void _core1_main()
{
	struct repeating_timer timer[1];
	_dvi0->timing = &DVI_TIMING;
	_dvi0->ser_cfg = DVI_DEFAULT_SERIAL_CONFIG;
	dvi_init(_dvi0, next_striped_spin_lock_num(), next_striped_spin_lock_num());
	add_repeating_timer_us(5, _repeating_timer_callback, NULL, timer);
	dvi_register_irqs_this_core(_dvi0, DMA_IRQ_1);
	while (queue_is_empty(&(_dvi0->q_colour_valid))) __wfe();
	dvi_start(_dvi0);
	dvi_scanbuf_main_8bpp(_dvi0);
}

static void _init()
{
	int i;
	for (i = 0 ; i < (FRAME_BUFFER_HEIGHT * FRAME_BUFFER_WIDTH) ; i++) {
		_frame_buffer[i] = (i % FRAME_BUFFER_WIDTH) & 0xff;
	}
	irq_set_enabled(IO_IRQ_BANK0, false);
	vreg_set_voltage(VREG_VSEL);
	sleep_ms(10);
	set_sys_clock_khz(DVI_TIMING.bit_clk_khz, true);
	multicore_launch_core1(_core1_main);
}

static int _begin(int w, int h)
{
	int ret = 0;
	if ((w != FRAME_BUFFER_WIDTH) || (h != FRAME_BUFFER_HEIGHT)) ret = 1;
	return ret;
}

static void _end()
{
}

static void _update(int x, int y, int w, int h, uint8_t *bitmap)
{
	int i;
	for (i = 0 ; i < h ; i++) {
		memcpy(
			_frame_buffer + (x + ((y + i) * FRAME_BUFFER_WIDTH)),
			bitmap + (i * w),
			w
		);
	}
}

static void _present()
{
}

static void _ring()
{
}