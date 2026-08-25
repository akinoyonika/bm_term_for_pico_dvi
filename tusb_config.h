#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif

// 1: ホストモードを有効 / 0: デバイスモード
#define CFG_TUSB_RHPORT0_MODE     OPT_MODE_HOST

// 使用するクラスの定義（キーボードはHID）
#define CFG_TUH_HID               1
#define CFG_TUH_CDC               0
#define CFG_TUH_MSC               0
#define CFG_TUH_VENDOR            0

// HIDデバイスの最大同時接続数
#define CFG_TUH_HID_MAX_INTERFACES 2

#ifdef __cplusplus
}
#endif

#endif
