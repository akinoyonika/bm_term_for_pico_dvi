#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"
#include "tusb.h"

#include "bmt/key.h"

static int _begin(void (*write)(uint8_t const *buffer, size_t buffer_size, void *param), void *write_param);
static void _end(void);
static void _yield(void);

struct bmt_key_ops const bmt_key[] = {{
	_begin,
	_end,
	_yield
}};

#define _KEY_AUTO_REPEAT_DELAY (500000)
#define _KEY_AUTO_REPEAT_INTERVAL (40000)

static char const *_a = NULL;

void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report, uint16_t desc_len)
{
	uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);

	if (itf_protocol != HID_ITF_PROTOCOL_KEYBOARD) goto EXIT;

	tuh_hid_receive_report(dev_addr, instance);

EXIT:

	return;
}

void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance)
{
}

void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len)
{
	int i;
	int j;
	static uint8_t prev_report[8] = {0};
	static char const *k2a[][128] = {{
			NULL      ,  NULL     , NULL      , NULL      , "a"       , "b"       , "c"       , "d"       ,
			"e"       , "f"       , "g"       , "h"       , "i"       , "j"       , "k"       , "l"       ,
			"m"       , "n"       , "o"       , "p"       , "q"       , "r"       , "s"       , "t"       ,
			"u"       , "v"       , "w"       , "x"       , "y"       , "z"       , "1"       , "2"       ,
			"3"       , "4"       , "5"       , "6"       , "7"       , "8"       , "9"       , "0"       ,
			"\n"      , "\x1b"    , "\x08"    , "\t"      , " "       , "-"       , "="       , "["       ,
			"]"       , "\\"      , NULL      , ";"       , "'"       , "`"       , ","       , "."       ,
			"/"       , NULL      , "\x1b[OP" , "\x1b[OQ" , "\x1b[OR" , "\x1b[OS" , "\x1b[15~", "\x1b[17~",
			"\x1b[18~", "\x1b[19~", "\x1b[20~", "\x1b[21~", "\x1b[23~", "\x1b[24~", NULL      , NULL      ,
			NULL      , "\x1b[2~" , "\x1b[H"  , "\x1b[5~" , "\x1b[3~" , "\x1b[F"  , "\x1b[6~" , "\x1b[C"  ,
			"\x1b[D"  , "\x1b[B"  , "\x1b[A"  , NULL      , "/"       , "*"       , "-"       , "+"       ,
			"\n"      , "1"       , "2"       , "3"       , "4"       , "5"       , "6"       , "7"       ,
			"8"       , "9"       , "0"       , "."       , NULL      , NULL      , NULL      , NULL      ,
			NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      ,
			NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      ,
			NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL
		}, {
			NULL      ,  NULL     , NULL      , NULL      , "A"       , "B"       , "C"       , "D"       ,
			"E"       , "F"       , "G"       , "H"       , "I"       , "J"       , "K"       , "L"       ,
			"M"       , "N"       , "O"       , "P"       , "Q"       , "R"       , "S"       , "T"       ,
			"U"       , "V"       , "W"       , "X"       , "Y"       , "Z"       , "!"       , "@"       ,
			"#"       , "$"       , "%"       , "^"       , "&"       , "*"       , "("       , ")"       ,
			"\n"      , "\x1b"    , "\x08"    , "\t"      , " "       , "_"       , "+"       , "{"       ,
			"}"       , "|"       , NULL      ,  ":"      , "\""      , "~"       , "<"       , ">"       ,
			"?"       , NULL      , "\x1b[OP" , "\x1b[OQ" , "\x1b[OR" , "\x1b[OS" , "\x1b[15~", "\x1b[17~",
			"\x1b[18~", "\x1b[19~", "\x1b[20~", "\x1b[21~", "\x1b[23~", "\x1b[24~", NULL      , NULL      ,
			NULL      , "\x1b[2~" , "\x1b[H"  , "\x1b[5~" , "\x1b[3~" , "\x1b[F"  , "\x1b[6~" , "\x1b[C"  ,
			"\x1b[D"  , "\x1b[B"  , "\x1b[A"  , NULL      , "/"       , "*"       , "-"       , "+"       ,
			"\n"      , "\x1b[F"  , "\x1b[B"  , "\x1b[6~" , "\x1b[D"  , "5"       , "\x1b[C"  , "\x1b[H"  ,
			"\x1b[A"  , "\x1b[5~" , "\x1b[2~" , "\x1b[3~" , NULL      , NULL      , NULL      , NULL      ,
			NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      ,
			NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      ,
			NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL
		}, {
			NULL      ,  NULL     , NULL      , NULL      , "\x01"    , "\x02"    , "\x03"    , "\x04"    ,
			"\x05"    , "\x06"    , "\x07"    , "\x08"    , "\x09"    , "\x0a"    , "\x0b"    , "\x0c"    ,
			"\x0d"    , "\x0e"    , "\x0f"    , "\x10"    , "\x11"    , "\x12"    , "\x13"    , "\x14"    ,
			"\x15"    , "\x16"    , "\x17"    , "\x18"    , "\x19"    , "\x1a"    , NULL      , NULL      ,
			NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      ,
			NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      ,
			NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      ,
			NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      ,
			NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      ,
			NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      ,
			NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      ,
			NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      ,
			NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      ,
			NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      ,
			NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      ,
			NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL      , NULL
		}
	};
	uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);
	int is_key_down;
	int is_shift;
	int is_ctrl;

	if (itf_protocol != HID_ITF_PROTOCOL_KEYBOARD) goto EXIT;

	if (len < 8) goto EXIT;

	for (i = 1, is_key_down = 0 ; i < 8 ; i++) {
		if (0 == report[i]) continue;
		is_key_down = 1;
		for (j = 1 ; j < 8 ; j++) if (report[i] == prev_report[j]) break;
		if (j == 8) break;
	}
	if (is_key_down) {
		if (i < 8) {
			is_shift = report[0] & (KEYBOARD_MODIFIER_LEFTSHIFT | KEYBOARD_MODIFIER_RIGHTSHIFT);
			is_ctrl = report[0] & (KEYBOARD_MODIFIER_LEFTCTRL | KEYBOARD_MODIFIER_RIGHTCTRL);
			_a = k2a[is_ctrl ? 2 : (is_shift ? 1 : 0)][report[i]];
		}
	} else {
		_a = NULL;
	}

	memcpy(prev_report, report, 8);

	tuh_hid_receive_report(dev_addr, instance);

EXIT:

	return;
}

static char const *_inkey()
{
	static char const *prev_a = NULL;
	static uint32_t a_time;
	char const *ret = NULL;

	if (_a != NULL) {
		if (prev_a == _a) {
			if (_KEY_AUTO_REPEAT_DELAY <= (time_us_32() - a_time)) {
				ret = _a;
				a_time += _KEY_AUTO_REPEAT_INTERVAL;
			} else {
				ret = NULL;
			}
		} else {
			ret = _a;
			a_time = time_us_32();
		}
	}

	prev_a = _a;
	
	return ret;
}

static void (*_write)(uint8_t const *buffer, size_t buffer_size, void *param);
static void *_write_param;

static bool _key()
{
	tuh_task();
	char const *a = _inkey();
	if (a != NULL) _write((uint8_t const *)a, strlen(a), _write_param);
	return true;
}

static int _begin(void (*write)(uint8_t const *buffer, size_t buffer_size, void *param), void *write_param)
{
	_write = write;
	_write_param = write_param;
	return 0;
}

static void _end()
{
}

static void _yield()
{
	_key();
}