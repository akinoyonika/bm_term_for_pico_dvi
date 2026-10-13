#ifndef __BMT_DISP_H__
#define __BMT_DISP_H__

#if defined(MODE_640x480_60Hz)
	// DVDD 1.2V (1.1V seems ok too)
#	define FRAME_WIDTH 640
#	define FRAME_HEIGHT 480
#	define VREG_VSEL VREG_VOLTAGE_1_20
#	define DVI_TIMING dvi_timing_640x480p_60hz
#elif defined(MODE_720x480_60Hz)
	// DVDD 1.2V
#	define FRAME_WIDTH 720
#	define FRAME_HEIGHT 480
#	define VREG_VSEL VREG_VOLTAGE_1_20
#	define DVI_TIMING dvi_timing_720x480p_60hz
#elif defined(MODE_800x480_60Hz)
	// DVDD 1.3V
#	define FRAME_WIDTH 800
#	define FRAME_HEIGHT 480
#	define VREG_VSEL VREG_VOLTAGE_1_30
#	define DVI_TIMING dvi_timing_800x480p_60hz
#elif defined(MODE_800x600_60Hz)
	// DVDD 1.3V, going downhill with a tailwind
#	define FRAME_WIDTH 800
#	define FRAME_HEIGHT 600
#	define VREG_VSEL VREG_VOLTAGE_1_30
#	define DVI_TIMING dvi_timing_800x600p_60hz
#elif defined(MODE_960x540p_60Hz)
	// DVDD 1.25V (slower silicon may need the full 1.3, or just not work)
#	define FRAME_WIDTH 960
#	define FRAME_HEIGHT 540
#	define VREG_VSEL VREG_VOLTAGE_1_25
#	define DVI_TIMING dvi_timing_960x540p_60hz
#elif defined(MODE_1280x720_30Hz)
	// 1280x720p 30 Hz (nonstandard)
	// DVDD 1.25V (slower silicon may need the full 1.3, or just not work)
#	define FRAME_WIDTH 1280
#	define FRAME_HEIGHT 720
#	define VREG_VSEL VREG_VOLTAGE_1_25
#	define DVI_TIMING dvi_timing_1280x720p_30hz
#else
#	error "Select a video mode!"
#endif

#define FRAME_BUFFER_WIDTH (FRAME_WIDTH / 2)
#define FRAME_BUFFER_HEIGHT (FRAME_HEIGHT / 2)

struct bmt_disp_ops {
	void (*init)(void);
	int (*begin)(int w, int h);
	void (*end)(void);
	void (*update)(int x, int y, int w, int h, uint8_t *bitmap);
	void (*present)(void);
	void (*ring)(void);
};

extern struct bmt_disp_ops const bmt_disp[];

#endif /* __BMT_DISP_H__ */
