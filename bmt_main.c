#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"
#include "tusb.h"

#include "bmt_config.h"
#include "display.h"
#include "bmt_main.h"

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

#define STDIN_FILENO (0)
#define STDOUT_FILENO (1)
#define STDERR_FILENO (2)
#define EAGAIN (11)
static int errno = 0;

static ssize_t read(int fd, void *buf, size_t count)
{
	ssize_t ret;
	uint32_t available;

	if (fd != STDIN_FILENO) {
		ret = -1;
		errno = 255;
		goto EXIT;
	}

	available = uart_is_readable(UART_ID) ? 1 : 0;

	if (available == 0) {
		ret = -1;
		errno = EAGAIN;
		goto EXIT;
	}

	if (count == 0) {
		ret = 0;
		goto EXIT;
	}

	for (ret = 0 ; uart_is_readable(UART_ID) && (0 < count) ; ret++, count--) {
		((char *)buf)[ret] = uart_getc(UART_ID);
	}

	if (ret == 0) {
		ret = -1;
		errno = 255;
	}

EXIT:

	return ret;
}

static ssize_t _read(int fd, uint8_t *buffer, size_t buffer_size)
{
	ssize_t sst;
	errno = 0;
	sst = read(fd, buffer, buffer_size);
	if (sst == -1) {
		if (errno == EAGAIN) {
			sst = 0;
		}
	}
	return sst;
}
#define _is_rfd_close(ret) (((ret) == 0) && (errno != EAGAIN))
#define _is_rfd_readable(ret) (((ret) == 0) && (errno != EAGAIN)) // ret = _read(fd, NULL, 0);

static ssize_t write(int fd, void const *buf, size_t count)
{
	ssize_t ret;
	uint32_t available;

	if ((fd != STDOUT_FILENO) && (fd != STDERR_FILENO)) {
		ret = -1;
		errno = 255;
		goto EXIT;
	}

	available = uart_is_writable(UART_ID) ? 1 : 0;

	if (available == 0) {
		ret = -1;
		errno = EAGAIN;
		goto EXIT;
	}

	if (count == 0) {
		ret = 0;
		goto EXIT;
	}

	for (ret = 0 ; uart_is_writable(UART_ID) && (0 < count) ; ret++, count--) {
		uart_putc_raw(UART_ID, ((char *)buf)[ret]);
	}

	if (ret == 0) {
		ret = -1;
		errno = 255;
	}

EXIT:

	return ret;
}

static ssize_t _write(int fd, uint8_t const *buffer, size_t buffer_size)
{
	size_t st;
	ssize_t sst;
	uint32_t start_time;
	errno = 0;
	if (buffer_size == 0) {
		sst = write(fd, NULL, 0);
		if (sst == -1) {
			if (errno == EAGAIN) {
				sst = 0;
			}
		}
		goto EXIT;
	}
	for (
		st = 0, errno = 0, start_time = time_us_32() ;
		(st < buffer_size) && ((time_us_32() - start_time) < BMT_TIMEOUT_US) ;
		st += sst
	) {
		sst = write(fd, buffer + st, buffer_size - st);
		if (sst == -1) {
			if (errno == EAGAIN) {
				sst = 0;
			} else {
				break;
			}
		}
	}
	if (st < buffer_size) {
		sst = -1;
	} else {
		sst = st;
	}
EXIT:
	return sst;
}
#define _is_wfd_writable(ret) (((ret) == 0) && (errno != EAGAIN)) // ret = _write(fd, NULL, 0);

static int _indexed_to_rgb(int idx)
{
	int ret;
	int const code[] = {0, 95, 135, 175, 215, 255};

	switch (idx) {
	case 0:
		ret = 0x000000;
		break;
	case 1:
		ret = 0x800000;
		break;
	case 2:
		ret = 0x008000;
		break;
	case 3:
		ret = 0x808000;
		break;
	case 4:
		ret = 0x000080;
		break;
	case 5:
		ret = 0x800080;
		break;
	case 6:
		ret = 0x008080;
		break;
	case 7:
		ret = 0xc0c0c0;
		break;
	case 8:
		ret = 0x808080;
		break;
	case 9:
		ret = 0xff0000;
		break;
	case 10:
		ret = 0x00ff00;
		break;
	case 11:
		ret = 0xffff00;
		break;
	case 12:
		ret = 0x0000ff;
		break;
	case 13:
		ret = 0xff00ff;
		break;
	case 14:
		ret = 0x00ffff;
		break;
	case 15:
		ret = 0xffffff;
		break;
	case 232:
		ret = 0x080808;
		break;
	case 233:
		ret = 0x121212;
		break;
	case 234:
		ret = 0x1c1c1c;
		break;
	case 235:
		ret = 0x262626;
		break;
	case 236:
		ret = 0x303030;
		break;
	case 237:
		ret = 0x3a3a3a;
		break;
	case 238:
		ret = 0x444444;
		break;
	case 239:
		ret = 0x4e4e4e;
		break;
	case 240:
		ret = 0x585858;
		break;
	case 241:
		ret = 0x626262;
		break;
	case 242:
		ret = 0x6c6c6c;
		break;
	case 243:
		ret = 0x767676;
		break;
	case 244:
		ret = 0x808080;
		break;
	case 245:
		ret = 0x8a8a8a;
		break;
	case 246:
		ret = 0x949494;
		break;
	case 247:
		ret = 0x9e9e9e;
		break;
	case 248:
		ret = 0xa8a8a8;
		break;
	case 249:
		ret = 0xb2b2b2;
		break;
	case 250:
		ret = 0xbcbcbc;
		break;
	case 251:
		ret = 0xc6c6c6;
		break;
	case 252:
		ret = 0xd0d0d0;
		break;
	case 253:
		ret = 0xdadada;
		break;
	case 254:
		ret = 0xe4e4e4;
		break;
	case 255:
		ret = 0xeeeeee;
		break;
	default:
		ret = idx - 16;
		ret = (code[ret / 36] << 16) | (code[(ret % 36) / 6] << 8) | code[ret % 6];
		break;
	}

	return ret;
}

struct _term_arg {
	struct bmt_settings *settings;
	char font[64];
	int mode;
	int lhpx;
	int cwpx;
	int rows;
	int cols;
	int user;
	int pass;
};

static int _pterm(int in_fd, int out_fd, struct _term_arg *arg)
{
	int is_running = 1;
	ssize_t sst;
	uint8_t buffer[8];
	size_t buffer_size = 0;
	char line[80];
	size_t line_len = 0;
	int term = 0;
	int lgin = 0;

	arg->font[0] = 0;
	arg->mode    = 0;
	arg->lhpx    = 0;
	arg->cwpx    = 0;
	arg->rows    = 0;
	arg->cols    = 0;
	arg->user    = 0;
	arg->pass    = 0;

	while (!lgin && is_running) {
		sst = _read(in_fd, buffer, sizeof(buffer));
		if ((sst == -1) || _is_rfd_close(sst)) {
			sst = 0;
			is_running = 0;
		}
		buffer_size = sst;

		if (
			(term         == 0) ||
			(arg->font[0] == 0) ||
			(arg->mode    == 0) ||
			(arg->lhpx    == 0) ||
			(arg->cwpx    == 0) ||
			(arg->rows    == 0) ||
			(arg->cols    == 0) ||
			(arg->user    == 0) ||
			(arg->pass    == 0) ||
			(lgin         == 0)
		) {
			for (size_t st = 0 ; st < buffer_size ; st++) {
				line[line_len++] = buffer[st];
				if (line[line_len - 1] == '\n') {
					line[line_len - 1] = '\0';
					if (term == 0) {
						if (strncmp(line, "TERM: ", 6) == 0) {
							term = 1;
						}
						if (term != 0) line_len = 0;
					}
					if (arg->font[0] == 0) {
						if (strncmp(line, "FONT: ", 6) == 0) {
							arg->font[sizeof(arg->font) - 1] = 0;
#							pragma GCC diagnostic push
#							pragma GCC diagnostic ignored "-Wstringop-truncation"
							strncpy(arg->font, line + 6, sizeof(arg->font));
#							pragma GCC diagnostic pop
							if (arg->font[sizeof(arg->font) - 1] != 0) {
								arg->font[0] = 0;
							}
						}
						if (arg->font[0] != 0) line_len = 0;
					}
					if (arg->mode == 0) {
						if (sscanf(line, "MODE: %d", &(arg->mode)) != 1) {
							arg->mode = 0;
						}
						if (arg->mode < 0) arg->mode = 0;
						if (arg->mode != 0) line_len = 0;
					}
					if (arg->lhpx == 0) {
						if (sscanf(line, "LHPX: %d", &(arg->lhpx)) != 1) {
							arg->lhpx = 0;
						}
						if (arg->lhpx < 0) arg->lhpx = 0;
						if (arg->lhpx != 0) line_len = 0;
					}
					if (arg->cwpx == 0) {
						if (sscanf(line, "CWPX: %d", &(arg->cwpx)) != 1) {
							arg->cwpx = 0;
						}
						if (arg->cwpx < 0) arg->cwpx = 0;
						if (arg->cwpx != 0) line_len = 0;
					}
					if (arg->rows == 0) {
						if (sscanf(line, "ROWS: %d", &(arg->rows)) != 1) {
							arg->rows = 0;
						}
						if (arg->rows < 0) arg->rows = 0;
						if (arg->rows != 0) line_len = 0;
					}
					if (arg->cols == 0) {
						if (sscanf(line, "COLS: %d", &(arg->cols)) != 1) {
							arg->cols = 0;
						}
						if (arg->cols < 0) arg->cols = 0;
						if (arg->cols != 0) line_len = 0;
					}
					if (arg->user == 0) {
						if (strncmp(line, "USER: ", 6) == 0) {
							arg->user = 1;
						}
						if (arg->user != 0) line_len = 0;
					}
					if (arg->pass == 0) {
						if (strncmp(line, "PASS: ", 6) == 0) {
							arg->pass = 1;
						}
						if (arg->pass != 0) line_len = 0;
					}
					if (lgin == 0) {
						if (strncmp(line, "login", 5) == 0) {
							lgin = 1;
						}
						if (lgin != 0) line_len = 0;
					}
					if (0 < line_len) {
						fprintf(stderr, "%s\n", line);
						line_len = 0;
					}
				} else if (line[line_len - 1] < '\x20') {
					line_len--;
				} else if ((arg->font[0] == 0) && (line_len == 6) && (strncmp(line, "FONT: ", 6) == 0)) {
					if (arg->settings->font == NULL) {
						if (_write(out_fd, (uint8_t *)"\n", 1) == -1) {
							is_running = 0;
						}
					} else {
						if (
							(_write(out_fd, (uint8_t *)(arg->settings->font), strlen(arg->settings->font)) == -1) ||
							(_write(out_fd, (uint8_t *)"\n", 1) == -1)
						) {
							is_running = 0;
						}
					}
				} else if ((arg->mode    == 0) && (line_len == 6) && (strncmp(line, "MODE: ", 6) == 0)) {
					if (arg->settings->mode == NULL) {
						if (_write(out_fd, (uint8_t *)"\n", 1) == -1) {
							is_running = 0;
						}
					} else {
						if (
							(_write(out_fd, (uint8_t *)(arg->settings->mode), strlen(arg->settings->mode)) == -1) ||
							(_write(out_fd, (uint8_t *)"\n", 1) == -1)
						) {
							is_running = 0;
						}
					}
				} else if ((arg->lhpx    == 0) && (line_len == 6) && (strncmp(line, "LHPX: ", 6) == 0)) {
					if (arg->settings->lhpx == NULL) {
						if (_write(out_fd, (uint8_t *)"\n", 1) == -1) {
							is_running = 0;
						}
					} else {
						if (
							(_write(out_fd, (uint8_t *)(arg->settings->lhpx), strlen(arg->settings->lhpx)) == -1) ||
							(_write(out_fd, (uint8_t *)"\n", 1) == -1)
						) {
							is_running = 0;
						}
					}
				} else if ((arg->cwpx    == 0) && (line_len == 6) && (strncmp(line, "CWPX: ", 6) == 0)) {
					if (arg->settings->cwpx == NULL) {
						if (_write(out_fd, (uint8_t *)"\n", 1) == -1) {
							is_running = 0;
						}
					} else {
						if (
							(_write(out_fd, (uint8_t *)(arg->settings->cwpx), strlen(arg->settings->cwpx)) == -1) ||
							(_write(out_fd, (uint8_t *)"\n", 1) == -1)
						) {
							is_running = 0;
						}
					}
				} else if ((arg->rows    == 0) && (line_len == 6) && (strncmp(line, "ROWS: ", 6) == 0)) {
					if (arg->settings->rows == NULL) {
						if (_write(out_fd, (uint8_t *)"\n", 1) == -1) {
							is_running = 0;
						}
					} else {
						if (
							(_write(out_fd, (uint8_t *)(arg->settings->rows), strlen(arg->settings->rows)) == -1) ||
							(_write(out_fd, (uint8_t *)"\n", 1) == -1)
						) {
							is_running = 0;
						}
					}
				} else if ((arg->cols    == 0) && (line_len == 6) && (strncmp(line, "COLS: ", 6) == 0)) {
					if (arg->settings->cols == NULL) {
						if (_write(out_fd, (uint8_t *)"\n", 1) == -1) {
							is_running = 0;
						}
					} else {
						if (
							(_write(out_fd, (uint8_t *)(arg->settings->cols), strlen(arg->settings->cols)) == -1) ||
							(_write(out_fd, (uint8_t *)"\n", 1) == -1)
						) {
							is_running = 0;
						}
					}
				} else if ((arg->user    == 0) && (line_len == 6) && (strncmp(line, "USER: ", 6) == 0)) {
					if (arg->settings->user == NULL) {
						if (_write(out_fd, (uint8_t *)"\n", 1) == -1) {
							is_running = 0;
						}
					} else {
						if (
							(_write(out_fd, (uint8_t *)(arg->settings->user), strlen(arg->settings->user)) == -1) ||
							(_write(out_fd, (uint8_t *)"\n", 1) == -1)
						) {
							is_running = 0;
						}
					}
				} else if ((arg->pass    == 0) && (line_len == 6) && (strncmp(line, "PASS: ", 6) == 0)) {
					if (arg->settings->pass == NULL) {
						if (_write(out_fd, (uint8_t *)"\n", 1) == -1) {
							is_running = 0;
						}
					} else {
						if (
							(_write(out_fd, (uint8_t *)(arg->settings->pass), strlen(arg->settings->pass)) == -1) ||
							(_write(out_fd, (uint8_t *)"\n", 1) == -1)
						) {
							is_running = 0;
						}
					}
				}
				if (sizeof(line) <= line_len) line_len = sizeof(line) - 1;
			}
		}

		buffer_size = 0;
	}

	return is_running;
}

static int _bterm(int in_fd, int out_fd, struct _term_arg *arg)
{
	int is_running = 0;
	int const mode = arg->mode;
	int const line_height = arg->lhpx;
	int const col_width = arg->cwpx;
	int const rows = arg->rows;
	int const cols = arg->cols;
	uint8_t buffer[4]; // 4 = maximum command length
	size_t buffer_size = 0;
	size_t expect_buffer_size = 0;
	ssize_t sst;
	uint8_t bitmap[line_height * col_width * 3];
	size_t bitmap_size = 0;
	int x = 0;
	int y = 0;
	int l = 0;
	uint32_t start_time;
	int command_len(uint8_t command, int mode)
	{
		int ret;
		switch (command & 0b10000000) {
		case 0b00000000:
			switch (mode & 0b11) {
			case 1:
				ret = 2;
				break;
			case 2:
				ret = 2;
				break;
			default:
				ret = 4;
				break;
			}
			break;
		default:
			switch (command & 0b11000000) {
			case 0b10000000:
				ret = 3;
				break;
			default:
				switch (command & 0b11100000) {
				case 0b11000000:
					ret = 1;
					break;
				default:
					switch (command & 0b11110000) {
					case 0b11100000:
						ret = 0;
						break;
					default:
						switch (command & 0b11111000) {
						case 0b11110000:
							ret = 0;
							break;
						default:
							switch (command & 0b11111100) {
							case 0b11111000:
								ret = 0;
								break;
							default:
								switch (command & 0b11111110) {
								case 0b11111100:
									ret = 0;
									break;
								default:
									switch (command & 0b11111111) {
									case 0b11111110:
										ret = 1;
										break;
									default:
										ret = 1;
										break;
									}
								}
								break;
							}
							break;
						}
						break;
					}
					break;
				}
				break;
			}
			break;
		}
		return ret;
	}
	void do_command(
		uint8_t const *buffer,
		int mode,
		int cols, int rows,
		int line_height, int col_width,
		int *y, int *x, int *l,
		uint8_t *bitmap, size_t *bitmap_size
	)
	{
		int i;
		int j;
		uint8_t r;
		uint8_t g;
		uint8_t b;
		switch (buffer[0] & 0b10000000) {
		case 0b00000000:
			*l = (((*l) << 7) | ((int)(buffer[0]) & 0b01111111)) + 1;
			if ((*l < 0) || ((rows * cols * line_height * col_width) < *l)) *l = 0;
			switch (mode & 0b11) {
			case 1:
				r = buffer[1];
				g = buffer[1];
				b = buffer[1];
				break;
			case 2:
				i = _indexed_to_rgb((int)buffer[1]);
				r = (i >> 16) & 0xff;
				g = (i >>  8) & 0xff;
				b = (i >>  0) & 0xff;
				break;
			default:
				r = buffer[1];
				g = buffer[2];
				b = buffer[3];
				break;
			}
			for (i = 0 ; i < *l ; i++) {
				j = (*bitmap_size % line_height) * col_width + *bitmap_size / line_height;
				bitmap[j] = (r & 0b11100000) | ((g >> 3) & 0b00011100) | (b >> 6);
				(*bitmap_size)++;
				if (*bitmap_size == (line_height * col_width)) {
					for (j = 0 ; j < line_height ; j++) {
						memcpy(
							display_frame_buffer + (*x * col_width + (*y * line_height + j) * col_width * cols),
							bitmap + (j * col_width),
							col_width
						);
					}
					*bitmap_size = 0;
					(*x)++;
					if (cols <= *x) {
						*x = 0;
						(*y)++;
						if (rows <= *y) {
							*y = 0;
						}
					}
				}
			}
			*l = 0;
			break;
		default:
			switch (buffer[0] & 0b11000000) {
			case 0b10000000:
				*bitmap_size = 0;
				i = (((int)(buffer[0]) & 0b00111000) << 5) | (int)(buffer[1]);
				j = (((int)(buffer[0]) & 0b00000111) << 8) | (int)(buffer[2]);
				if ((0 <= i) && (i < rows) && (0 <= j) && (j < cols)) {
					*y = i;
					*x = j;
				}
				break;
			default:
				switch (buffer[0] & 0b11100000) {
				case 0b11000000:
					*l = ((*l) << 5) | ((int)buffer[0] & 0b00011111);
					break;
				default:
					switch (buffer[0] & 0b11110000) {
					case 0b11100000:
						break;
					default:
						switch (buffer[0] & 0b11111000) {
						case 0b11110000:
							break;
						default:
							switch (buffer[0] & 0b11111100) {
							case 0b11111000:
								break;
							default:
								switch (buffer[0] & 0b11111110) {
								case 0b11111100:
									break;
								default:
									switch (buffer[0] & 0b11111111) {
									case 0b11111110:
										// TODO: buzz!
										break;
									default:
										if (0 < *bitmap_size) {
											*bitmap_size = 0;
											(*x)++;
											if (cols <= *x) {
												*x = 0;
												(*y)++;
												if (rows <= *y) {
													*y = 0;
												}
											}
										}
										break;
									}
									break;
								}
								break;
							}
							break;
						}
						break;
					}
					break;
				}
				break;
			}
			break;
		}
	}

	is_running = 1;

	while (is_running) {
		sst = _read(in_fd, NULL, 0);
		if ((buffer_size == 0) || _is_rfd_readable(sst)) {
			start_time = time_us_32();
		} else {
			if (BMT_TIMEOUT_US <= (time_us_32() - start_time)) {
				buffer_size = 0;
				expect_buffer_size = 0;
				start_time = time_us_32();
			}
		}

		if (buffer_size == 0) {
			sst = _read(in_fd, buffer, 1);
			if ((sst == -1) || _is_rfd_close(sst)) {
				sst = 0;
				is_running = 0;
			}
			buffer_size += sst;
		}

		if ((expect_buffer_size == 0) && (buffer_size == 1)) {
			expect_buffer_size = command_len(buffer[0], mode);
			if (expect_buffer_size == 0) {
				buffer_size = 0;
			}
		}

		if ((0 < expect_buffer_size) && (buffer_size < expect_buffer_size)) {
			sst = _read(in_fd, buffer + buffer_size, expect_buffer_size - buffer_size);
			if ((sst == -1) || _is_rfd_close(sst)) {
				sst = 0;
				is_running = 0;
			}
			buffer_size += sst;
		}

		if ((0 < expect_buffer_size) && (buffer_size == expect_buffer_size)) {
			do_command(
				buffer,
				mode,
				cols, rows,
				line_height, col_width,
				&y, &x, &l,
				bitmap, &bitmap_size
			);
			buffer_size = 0;
			expect_buffer_size = 0;
		}

		{
			tuh_task();
			char const *a = _inkey();
			if (a != NULL) _write(out_fd, (uint8_t const *)a, strlen(a));
		}
	}

	return is_running;
}

int bmt_main(struct bmt_settings *settings)
{
	int ret = 0;
	struct _term_arg arg[1] = {{0}};

	arg->settings = settings;
	if (!_pterm(STDIN_FILENO, STDOUT_FILENO, arg)) {
		ret = 1;
		goto PTERM_ABORT_EXIT;
	}

	_bterm(STDIN_FILENO, STDOUT_FILENO, arg);

PTERM_ABORT_EXIT:
	return ret;
}
