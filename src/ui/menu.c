/* Copyright 2023 Dual Tachyon
 * https://github.com/DualTachyon
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 */

#include <string.h>
#include <stdio.h>

#include "app/dtmf.h"
#include "app/menu.h"
// #include "bitmaps.h"
#include "board.h"
#include "dcs.h"
#include "driver/backlight.h"
#include "driver/bk4819.h"
#include "driver/eeprom.h"
//#include "driver/st7565.h"
#include "printf.h"
#include "frequencies.h"
#include "helper/battery.h"
#include "misc.h"
#include "settings.h"
#ifdef ENABLE_FEAT_F4HWN
#include "version.h"
#endif
// #include "helper.h"
#include "inputbox.h"
#include "menu.h"
#include "ui.h"
#include "ui/gui.h"
#include "ui/welcome.h"


const t_menu_item MenuList[] =
{
    //   text,          menu ID          [CN meaning]
        {"SQ",          MENU_SQL           }, // 静噪
        {"STP",         MENU_STEP          }, // 步进
        {"PWR",         MENU_TXP           }, // 功率
        {"R-DCS",       MENU_R_DCS         }, // 收数字哑音
        {"R-CTC",       MENU_R_CTCS        }, // 收模拟哑音
        {"T-DCS",       MENU_T_DCS         }, // 发数字哑音
        {"T-CTC",       MENU_T_CTCS        }, // 发模拟哑音
        {"TXD",         MENU_SFT_D         }, // 频差方向
        {"TXO",         MENU_OFFSET        }, // 频差频率
        {"W/N",         MENU_W_N           }, // 宽窄带
        {"BSY",         MENU_BCL           }, // 繁忙锁
        {"CMP",         MENU_COMPAND       }, // 压扩
        {"MOD",         MENU_AM            }, // 调制模式
    #ifdef ENABLE_FEAT_F4HWN
        {"TXL",         MENU_TX_LOCK       }, // 发射锁定
    #endif
    #ifdef ENABLE_SCANLIST
        {"SC-A1",       MENU_S_ADD1        }, // 扫描列表1添加
        {"SC-A2",       MENU_S_ADD2        }, // 扫描列表2添加
        {"SC-A3",       MENU_S_ADD3        }, // 扫描列表3添加
    #endif
        {"CHSV",        MENU_MEM_CH        }, // 保存信道
        //{"CHDL",      MENU_DEL_CH        }, // 删除信道
        //{"CHNM",      MENU_MEM_NAME      }, // 信道命名

    #ifdef ENABLE_SCANLIST
        {"SCLST",       MENU_S_LIST        }, // 扫描列表模式
        {"SL1",         MENU_SLIST1        }, // 列表1成员
        {"SL2",         MENU_SLIST2        }, // 列表2成员
        {"SL3",         MENU_SLIST3        }, // 列表3成员
    #endif
        {"SCRV",        MENU_SC_REV        }, // 扫描恢复
    #ifndef ENABLE_FEAT_F4HWN
        #ifdef ENABLE_NOAA
            {"NOAA",    MENU_NOAA_S    }, // NOAA气象扫描
        #endif
    #endif
        {"F1-S",        MENU_F1SHRT        }, // F1键短按
        {"F1-L",        MENU_F1LONG        }, // F1键长按
        {"F2-S",        MENU_F2SHRT        }, // F2键短按
        {"F2-L",        MENU_F2LONG        }, // F2键长按
        {"M-L",         MENU_MLONG         }, // M键长按
        {"KLCK",        MENU_AUTOLK        }, // 自动键盘锁
        {"TOT",         MENU_TOT           }, // 限时计时器
        {"BATSV",       MENU_SAVE          }, // 省电模式
        //{"BATTP",     MENU_BAT_TXT       }, // 电池显示模式
        {"MIC",         MENU_MIC           }, // 麦克风增益
        {"MICB",        MENU_MIC_BAR       }, // 麦克风条
        //{"CHDSP",     MENU_MDF           }, // 信道显示格式
        //{"PON",       MENU_PONMSG        }, // 开机信息
        {"BLT",         MENU_ABR           }, // 背光时间
        {"BLMn",        MENU_ABR_MIN       }, // 背光最暗
        {"BLMx",        MENU_ABR_MAX       }, // 背光最亮
        {"BLTR",        MENU_ABR_ON_TX_RX  }, // 收发背光
        {"BEEP",        MENU_BEEP          }, // 按键音
    #ifdef ENABLE_VOICE
        {"VOICE",       MENU_VOICE         }, // 语音播报
    #endif
        {"ROGER",       MENU_ROGER         }, // 结束音
        {"STE",         MENU_STE           }, // 单音结尾
        {"RPST",        MENU_RP_STE        }, // 尾音消除
        {"1CALL",       MENU_1_CALL        }, // 选呼
    #ifdef ENABLE_ALARM
        {"ALM",         MENU_AL_MOD        }, // 报警方式
    #endif
    #ifdef ENABLE_DTMF_CALLING
        {"ANI",         MENU_ANI_ID        }, // ANI身份码
    #endif
        //{"UPC",       MENU_UPCODE        }, // 上行码
        //{"DWC",       MENU_DWCODE        }, // 下行码
        {"PTTID",       MENU_PTT_ID        }, // PTT身份
        {"DST",         MENU_D_ST          }, // DTMF状态
    #ifdef ENABLE_DTMF_CALLING
        {"DRSP",        MENU_D_RSP         }, // DTMF应答
        {"DHLD",        MENU_D_HOLD        }, // DTMF保持
    #endif
        {"DPRE",        MENU_D_PRE         }, // DTMF预载
    #ifdef ENABLE_DTMF_CALLING
        {"DDEC",        MENU_D_DCD         }, // DTMF解码
        {"DLST",        MENU_D_LIST        }, // DTMF列表
    #endif
        {"DLIV",        MENU_D_LIVE_DEC    }, // 实时DTMF
    #ifndef ENABLE_FEAT_F4HWN
        #ifdef ENABLE_AM_FIX
            {"AMFX",    MENU_AM_FIX        }, // AM修正
        #endif
    #endif
        {"VOX",         MENU_VOX           }, // 声控发射
        {"RXM",         MENU_TDR           }, // 接收模式
    #ifdef ENABLE_FEAT_F4HWN
        {"SETP",        MENU_SET_PWR       }, // 功率等级设置
        {"SETPT",       MENU_SET_PTT       }, // PTT模式设置
        {"SETT",        MENU_SET_TOT       }, // 限时设置
        {"SETE",        MENU_SET_EOT       }, // 限时结束设置
        //{"SETC",      MENU_SET_CTR       }, // 对比度设置
        //{"SETINV",    MENU_SET_INV       }, // 反色设置
        {"SETL",        MENU_SET_LCK       }, // 锁定设置
        //{"SETMET",    MENU_SET_MET       }, // 仪表设置
        //{"SETGUI",    MENU_SET_GUI       }, // 界面设置
        {"SETM",        MENU_SET_TMR       }, // 定时器设置
    #ifdef ENABLE_FEAT_F4HWN_SLEEP
        {"SETOF",       MENU_SET_OFF      }, // 自动关机设置
    #endif
    #ifdef ENABLE_FEAT_F4HWN_NARROWER
        {"SETNF",       MENU_SET_NFM       }, // 窄带设置
    #endif
    #ifdef ENABLE_FEAT_F4HWN_VOL
        {"SETV",        MENU_SET_VOL       }, // 音量设置
    #endif
    #ifdef ENABLE_FEAT_F4HWN_RESCUE_OPS
        {"SETK",        MENU_SET_KEY       }, // 键盘设置
    #endif
    #ifdef ENABLE_NOAA
        {"SETNW",       MENU_NOAA_S    }, // 气象频段设置
    #endif
    #endif
    #ifdef ENABLE_FEAT_F4HWN
        {"ABOUT",       MENU_VOL           }, // 关于/电压
    #else
        {"BATV",        MENU_VOL           }, // 电池电压
    #endif
        // hidden menu items from here on (hidden: hold PTT + upper sidekey at boot)
        {"FLCK",        MENU_F_LOCK        }, // 频段锁定
    #ifndef ENABLE_FEAT_F4HWN
    //    {"TX200",      MENU_200TX         }, // 200MHz TX
    //    {"TX350",      MENU_350TX         }, // 350MHz TX
    //    {"TX500",      MENU_500TX         }, // 500MHz TX
    #endif
    //    {"350EN",      MENU_350EN         }, // 350启用
    #ifdef ENABLE_F_CAL_MENU
        {"FRCAL",       MENU_F_CALI        }, // 频率校准
    #endif
        {"BTCAL",       MENU_BATCAL        }, // 电压校准
        {"BTTYP",       MENU_BATTYP        }, // 电池类型
        {"RST",         MENU_RESET         }, // 复位

        {"",            0xff               }  // end of list
};

const uint8_t FIRST_HIDDEN_MENU_ITEM = MENU_F_LOCK;

const char gSubMenu_TXP[][6] =
{
 "USER",    // 用户自定义
 "L1",      // 低功率1
 "L2",      // 低功率2
 "L3",      // 低功率3
 "L4",      // 低功率4
 "L5",      // 低功率5
 "MID",     // 中功率
 "HIGH"     // 高功率
};

const char gSubMenu_SFT_D[][4] =
{
 "OFF",     // 关闭(无差频)
 "+",       // 加差频
 "-"        // 减差频
};

const char gSubMenu_W_N[][7] =
{
 "WIDE",    // 宽带
 "NARROW"   // 窄带
};

const char gSubMenu_OFF_ON[][4] =
{
 "OFF",     // 关
 "ON"       // 开
};

const char gSubMenu_NA[4] =
{
 "N/A"      // 不适用
};

const char* const gSubMenu_RXMode[] =
{
 "A ONLY",  // 仅A路收发
 "A/B RX",  // 双路接收，响应A路
 "CROSS",   // 交叉(A发B收)
 "A TX"     // 固定A发双收
};

#ifdef ENABLE_VOICE
const char gSubMenu_VOICE[][4] =
{
 "OFF",     // 关闭
 "CHN",     // 中文
 "ENG"      // 英文
};
#endif

#ifdef ENABLE_ALARM
const char gSubMenu_AL_MOD[][5] =
{
 "SITE",    // 现场报警
 "TONE"     // 音调报警
};
#endif

#ifdef ENABLE_DTMF_CALLING
const char gSubMenu_D_RSP[][11] =
{
 "NONE",    // 无动作
 "RING",    // 响铃
 "REPLY",   // 回复
 "BOTH"     // 响铃+回复
};
#endif

const char* const gSubMenu_PTT_ID[] =
{
 "OFF",     // 关闭
 "UP",      // 上行码
 "DOWN",    // 下行码
 "UP+DN",   // 上下行
 "APOLLO"   // Apollo模式
};

/*const char gSubMenu_PONMSG[][8] =
{
#ifdef ENABLE_FEAT_F4HWN
 "ALL",
 "SOUND",
#else
 "FULL",
#endif
 "MSG",
 "VOLT",
 "NONE"
};*/

const char gSubMenu_ROGER[][6] =
{
 "OFF",     // 关
 "RGR",     // Roger音
 "MDC"      // MDC音
};

const char gSubMenu_RESET[][4] =
{
 "VFO",     // 仅VFO复位
 "ALL"      // 全部复位
};

const char* const gSubMenu_F_LOCK[] =
{
 "DEF+",    // 默认+
 "FCC",     // FCC业余
#ifdef ENABLE_FEAT_F4HWN_CA
 "CA",      // CA业余
#endif
 "CE",      // CE业余
 "GB",      // GB业余
 "137-430", // 137到430
 "137-438", // 137到438
#ifdef ENABLE_FEAT_F4HWN_PMR
 "PMR446",  // PMR446
#endif
#ifdef ENABLE_FEAT_F4HWN_GMRS_FRS_MURS
 "GMRS+",   // GMRS FRS MURS
#endif
 "DISABL",  // 禁用全部
 "UNLOCK"   // 解锁全部
};

const char gSubMenu_RX_TX[][6] =
{
 "OFF",     // 关
 "TX",      // 仅发射
 "RX",      // 仅接收
 "TX/RX"    // 收+发
};

/*const char gSubMenu_BAT_TXT[][8] =
{
 "NONE",
 "VOLT",
 "PCT"
};
*/

const char gSubMenu_BATTYP[][9] =
{
 "1600mAh", // 1600毫安时
 "2200mAh", // 2200毫安时
 "3500mAh"  // 3500毫安时
};

#ifdef ENABLE_FEAT_F4HWN
const char gSubMenu_SET_PWR[][6] =
{
 "<20mW",   // 小于20毫瓦
 "125mW",   // 125毫瓦
 "250mW",   // 250毫瓦
 "500mW",   // 500毫瓦
 "1W",      // 1瓦
 "2W",      // 2瓦
 "5W"       // 5瓦
};

const char gSubMenu_SET_PTT[][8] =
{
 "CLASSIC", // 经典模式
 "1PUSH"    // 一键模式
};

const char gSubMenu_SET_TOT[][7] =
{
 "OFF",     // 关
 "SOUND",   // 声音
 "VISUAL",  // 视觉
 "ALL"      // 全部
};

const char gSubMenu_SET_LCK[][9] =
{
 "KEYS",    // 仅按键
 "KEY+PTT"  // 按键+PTT
};


#ifdef ENABLE_FEAT_F4HWN_NARROWER
const char gSubMenu_SET_NFM[][9] =
{
 "NARROW",  // 窄带
 "NRROWER"  // 更窄带
};
#endif

#ifdef ENABLE_FEAT_F4HWN_RESCUE_OPS
const char gSubMenu_SET_KEY[][9] =
{
 "MENU",    // 菜单键
 "UP",      // 上键
 "DOWN",    // 下键
 "EXIT",    // 退出键
 "STAR"     // *键
};
#endif
#endif

#ifdef ENABLE_SCANLIST
const char gSubMenu_SLIST[][9] =
{
    "NONE",   // 无列表
    "LIST1",  // 列表1
    "LIST2",  // 列表2
    "LIST3",  // 列表3
    "L1-3",   // 列表1-3
    "ALL"     // 全部
};
#endif


const t_sidefunction gSubMenu_SIDEFUNCTIONS[] =
{
    {"NONE",            ACTION_OPT_NONE},        // 无功能
#ifdef ENABLE_FLASHLIGHT
    {"FLASHLT",        ACTION_OPT_FLASHLIGHT},   // 手电筒
#endif
    {"POWER",           ACTION_OPT_POWER},        // 功率切换
    {"MON",             ACTION_OPT_MONITOR},      // 监听
    {"SCAN",            ACTION_OPT_SCAN},         // 扫描
#ifdef ENABLE_VOX
    {"VOX",             ACTION_OPT_VOX},          // 声控
#endif
#ifdef ENABLE_ALARM
    {"ALARM",           ACTION_OPT_ALARM},        // 报警
#endif
#ifdef ENABLE_FMRADIO
    {"FM-R",            ACTION_OPT_FM},           // 调频广播
#endif
#ifdef ENABLE_TX1750
    {"1750",            ACTION_OPT_1750},         // 1750Hz
#endif
#ifdef ENABLE_REGA
    {"REG-AL",         ACTION_OPT_REGA_ALARM},    // REGA报警
    {"REG-TS",         ACTION_OPT_REGA_TEST},     // REGA测试
#endif
    {"KEYLCK",         ACTION_OPT_KEYLOCK},       // 键盘锁
    {"A/B",             ACTION_OPT_A_B},          // VFO A/B切换
    {"VFO/MR",          ACTION_OPT_VFO_MR},       // VFO/信道切换
    {"MOD",             ACTION_OPT_SWITCH_DEMODUL},// 解调模式
#ifdef ENABLE_BLMIN_TMP_OFF
    {"BLMOFF",         ACTION_OPT_BLMIN_TMP_OFF}, // 临时最低背光
#endif
#ifdef ENABLE_FEAT_F4HWN
    {"RXMOD",           ACTION_OPT_RXMODE},       // 接收模式
    {"MAIN",            ACTION_OPT_MAINONLY},     // 仅主路
    {"PTT",             ACTION_OPT_PTT},          // PTT
    {"W/N",             ACTION_OPT_WN},           // 宽窄带
    #if !defined(ENABLE_SPECTRUM) || !defined(ENABLE_FMRADIO)
    {"MUTE",            ACTION_OPT_MUTE},         // 静音
    #endif
    #ifdef ENABLE_FEAT_F4HWN_RESCUE_OPS
        {"PW-HI",       ACTION_OPT_POWER_HIGH},   // 高功率
        {"REM-OF",      ACTION_OPT_REMOVE_OFFSET},// 去除偏移
    #endif
#endif
#ifdef ENABLE_MESSENGER
    {"MSG",             ACTION_OPT_MESSENGER},    // 短信
#endif
};

const uint8_t gSubMenu_SIDEFUNCTIONS_size = ARRAY_SIZE(gSubMenu_SIDEFUNCTIONS);

bool    gIsInSubMenu;
uint8_t gMenuCursor;

void UI_GenerateChannelStringEx(char *pString, const bool bShowPrefix, const uint8_t ChannelNumber)
{
    if (gInputBoxIndex > 0) {
        for (unsigned int i = 0; i < 3; i++) {
            pString[i] = (gInputBox[i] == 10) ? '-' : gInputBox[i] + '0';
        }

        pString[3] = 0;
        return;
    }

    if (bShowPrefix) {
        // BUG here? Prefixed NULLs are allowed
        sprintf(pString, "CH-%03u", ChannelNumber + 1);
    } else if (ChannelNumber == 0xFF) {
        strcpy(pString, "NULL");
    } else {
        sprintf(pString, "%03u", ChannelNumber + 1);
    }
}

int UI_MENU_GetCurrentMenuId() {
    if (gMenuCursor < ARRAY_SIZE(MenuList))
        return MenuList[gMenuCursor].menu_id;

    return MenuList[ARRAY_SIZE(MenuList) - 1].menu_id;
}

uint8_t UI_MENU_GetMenuIdx(uint8_t id)
{
    for (uint8_t i = 0; i < ARRAY_SIZE(MenuList); i++)
        if (MenuList[i].menu_id == id)
            return i;
    return 0;
}

int32_t gSubMenuSelection;
int32_t gSubMenuSelectionOffset = 0;

// edit box
char    edit_original[17]; // a copy of the text before editing so that we can easily test for changes/difference
char    edit[17];
int     edit_index;


UI_SelectionList menuList;
UI_SelectionList subMenuList;

#define MENU_LIST_BUFFER_SIZE 1536

static char gMenuListBuffer[MENU_LIST_BUFFER_SIZE];

static bool UI_MENU_AppendChar(char *buf, size_t cap, size_t *out, char c)
{
    if (*out + 1 >= cap) {
        return false;
    }
    buf[(*out)++] = c;
    buf[*out] = '\0';
    return true;
}

static bool UI_MENU_AppendStr(char *buf, size_t cap, size_t *out, const char *src)
{
    if (src == NULL) {
        return true;
    }
    while (*src != '\0') {
        if (!UI_MENU_AppendChar(buf, cap, out, *src++)) {
            return false;
        }
    }
    return true;
}

static bool UI_MENU_AppendUIntWidth(char *buf, size_t cap, size_t *out, uint32_t v, uint8_t width)
{
    char tmp[10];
    uint8_t len = 0;
    do {
        tmp[len++] = (char)('0' + (v % 10U));
        v /= 10U;
    } while (v && len < (uint8_t)sizeof(tmp));

    if (width < len) {
        width = len;
    }
    if (*out + width >= cap) {
        return false;
    }
    while (width > len) {
        buf[(*out)++] = '0';
        width--;
    }
    while (len > 0) {
        buf[(*out)++] = tmp[--len];
    }
    buf[*out] = '\0';
    return true;
}

static bool UI_MENU_AppendInt(char *buf, size_t cap, size_t *out, int32_t v)
{
    if (v < 0) {
        if (!UI_MENU_AppendChar(buf, cap, out, '-')) {
            return false;
        }
        v = (int32_t)-(int64_t)v;
    }
    return UI_MENU_AppendUIntWidth(buf, cap, out, (uint32_t)v, 0);
}

static bool UI_MENU_AppendOctalWidth(char *buf, size_t cap, size_t *out, uint32_t v, uint8_t width)
{
    char tmp[12];
    uint8_t len = 0;
    do {
        tmp[len++] = (char)('0' + (v & 7U));
        v >>= 3U;
    } while (v && len < (uint8_t)sizeof(tmp));

    if (width < len) {
        width = len;
    }
    if (*out + width >= cap) {
        return false;
    }
    while (width > len) {
        buf[(*out)++] = '0';
        width--;
    }
    while (len > 0) {
        buf[(*out)++] = tmp[--len];
    }
    buf[*out] = '\0';
    return true;
}

static const char* UI_MENU_GetMenuListLines(void)
{
    static char s_menuLines[ARRAY_SIZE(MenuList) *
                                (sizeof(MenuList[0].name) + 1U) +
                            1U] = {0};

    size_t out = 0;
    const size_t cap = sizeof(s_menuLines);

    for (uint8_t i = 0; i < gMenuListCount && MenuList[i].name[0] != '\0'; i++) {
        const char* name = MenuList[i].name;
        const size_t len = strnlen(name, sizeof(MenuList[i].name));

        if (len == 0) {
            continue;
        }

        if (out + len + 1 >= cap) {
            break;
        }

        memcpy(&s_menuLines[out], name, len);
        out += len;

        if ((i + 1) < gMenuListCount && out + 1 < cap) {
            s_menuLines[out++] = '\n';
        }
    }

    s_menuLines[out] = '\0';
    return s_menuLines;
}

#ifdef ENABLE_SCANLIST
static void UI_MENU_DrawScanListPopup(void)
{
    const uint8_t popup_x = 36;
    const uint8_t popup_y = 6;
    const uint8_t popup_w = 90;
    const uint8_t popup_h = 52;
    const uint8_t text_x1 = 40;
    const uint8_t text_x2 = 122;
    const uint8_t line_h = 10;
    char line[32];
    const int32_t selection = gSubMenuSelection;
    const uint8_t list_index = (uint8_t)(UI_MENU_GetCurrentMenuId() - MENU_SLIST1);

    UI_DrawPopupWindow(popup_x, popup_y, popup_w, popup_h, UI_SelectionList_GetStringLine(&menuList));

    UI_SetFont(FONT_8B_TR);
    if (selection < 0) {
        strcpy(line, "NULL");
    } else {
        UI_GenerateChannelStringEx(line, true, selection);
    }
    UI_DrawString(UI_TEXT_ALIGN_CENTER, text_x1, text_x2, popup_y + 14, true, false, false, line);

    UI_SetFont(FONT_8_TR);
    if (selection < 0) {
        strcpy(line, "--");
    } else {
        SETTINGS_FetchChannelName(line, selection);
        if (line[0] == '\0') {
            strcpy(line, "--");
        }
    }
    UI_DrawString(UI_TEXT_ALIGN_CENTER, text_x1, text_x2, (uint8_t)(popup_y + 14 + line_h), true, false, false, line);

    if (selection >= 0 && gEeprom.SCAN_LIST_ENABLED[list_index]) {
        for (uint8_t pri = 1; pri <= 2; ++pri) {
            const uint8_t channel = (pri == 1) ? gEeprom.SCANLIST_PRIORITY_CH1[list_index] : gEeprom.SCANLIST_PRIORITY_CH2[list_index];
            if (IS_MR_CHANNEL(channel)) {
                snprintf(line, sizeof(line), "PRI%u:%u", pri, (unsigned int)channel + 1U);
                UI_DrawString(UI_TEXT_ALIGN_CENTER, text_x1, text_x2,
                    (uint8_t)(popup_y + 14 + (line_h * (pri + 1))), true, false, false, line);
            }
        }
    }
}
#endif

static const char* squelchStr = "OFF\n1\n2\n3\n4\n5\n6\n7\n8\n9";

static const char* UI_MENU_JoinFixedList(const char* arr, size_t elem_size, size_t count)
{
    char *buffer = gMenuListBuffer;
    const size_t cap = sizeof(gMenuListBuffer);
    size_t out = 0;

    for (size_t i = 0; i < count; ++i) {
        const char* item = arr + (i * elem_size);
        const size_t len = strnlen(item, elem_size);

        if (len == 0) {
            continue;
        }

        if (out + len + 1 >= cap) {
            break;
        }

        memcpy(&buffer[out], item, len);
        out += len;

        if ((i + 1) < count && out + 1 < cap) {
            buffer[out++] = '\n';
        }
    }

    buffer[out] = '\0';
    return buffer;
}

static const char* UI_MENU_GetStepList(void)
{
    char *buffer = gMenuListBuffer;
    const size_t cap = sizeof(gMenuListBuffer);
    size_t out = 0;

    for (uint8_t i = 0; i < STEP_N_ELEM; ++i) {
        const STEP_Setting_t step_idx = FREQUENCY_GetStepIdxFromSortedIdx(i);
        const uint16_t step = gStepFrequencyTable[step_idx];
        const uint16_t whole = step / 100;
        const uint16_t frac = step % 100;
        if (!UI_MENU_AppendUIntWidth(buffer, cap, &out, whole, 0) ||
            !UI_MENU_AppendChar(buffer, cap, &out, '.') ||
            !UI_MENU_AppendUIntWidth(buffer, cap, &out, frac, 2) ||
            !UI_MENU_AppendStr(buffer, cap, &out, "kHz")) {
            buffer[cap - 1] = '\0';
            break;
        }

        if ((i + 1) < STEP_N_ELEM && out + 1 < cap) {
            buffer[out++] = '\n';
            buffer[out] = '\0';
        }
    }

    return buffer;
}

static const char* UI_MENU_JoinPtrList(const char* const* arr, size_t count)
{
    char *buffer = gMenuListBuffer;
    const size_t cap = sizeof(gMenuListBuffer);
    size_t out = 0;

    gSubMenuSelectionOffset = 0;

    for (size_t i = 0; i < count; ++i) {
        const char* item = arr[i];
        if (item == NULL) {
            continue;
        }
        const size_t len = strlen(item);
        if (len == 0) {
            continue;
        }

        if (out + len + 1 >= cap) {
            break;
        }

        memcpy(&buffer[out], item, len);
        out += len;

        if ((i + 1) < count && out + 1 < cap) {
            buffer[out++] = '\n';
        }
    }

    buffer[out] = '\0';
    return buffer;
}

static const char* UI_MENU_JoinSideFunctions(void)
{
    char *buffer = gMenuListBuffer;
    const size_t cap = sizeof(gMenuListBuffer);
    size_t out = 0;

    for (size_t i = 0; i < gSubMenu_SIDEFUNCTIONS_size; ++i) {
        const char* item = gSubMenu_SIDEFUNCTIONS[i].name;
        const size_t len = strlen(item);

        if (len == 0) {
            continue;
        }

        if (out + len + 1 >= cap) {
            break;
        }

        memcpy(&buffer[out], item, len);
        out += len;

        if ((i + 1) < gSubMenu_SIDEFUNCTIONS_size && out + 1 < cap) {
            buffer[out++] = '\n';
        }
    }

    buffer[out] = '\0';
    return buffer;
}

typedef enum {
    MENU_LIST_NUMERIC,
    MENU_LIST_TIME_MMSS,
    MENU_LIST_TIME_HM,
    MENU_LIST_MIC_DB,
    MENU_LIST_SCAN_REV,
    MENU_LIST_DCS,
    MENU_LIST_CTCSS
} UI_MENU_ListMode;

static const char* UI_MENU_BuildList(UI_MENU_ListMode mode, int32_t min, int32_t max,
    int32_t base_offset, uint32_t step,
    const char* first_item, const char* last_item,
    const char* prefix, const char* suffix)
{
    size_t out = 0;

    if (min > max) {
        gMenuListBuffer[0] = '\0';
        return gMenuListBuffer;
    }

    gSubMenuSelectionOffset = (min > 0) ? -min : 0;

    if (prefix == NULL) {
        prefix = "";
    }
    if (suffix == NULL) {
        suffix = "";
    }

    for (int32_t value = min; value <= max; ++value) {
        const char* item_text = NULL;
        const bool allow_item_text = (mode == MENU_LIST_NUMERIC ||
            mode == MENU_LIST_TIME_MMSS ||
            mode == MENU_LIST_TIME_HM);

        if (allow_item_text) {
            if (value == min && first_item != NULL) {
                item_text = first_item;
            }
            else if (value == max && last_item != NULL) {
                item_text = last_item;
            }
        }

        if (item_text != NULL) {
            if (!UI_MENU_AppendStr(gMenuListBuffer, sizeof(gMenuListBuffer), &out, item_text)) {
                gMenuListBuffer[sizeof(gMenuListBuffer) - 1] = '\0';
                break;
            }
        } else {
            switch (mode) {
            case MENU_LIST_NUMERIC:
                if (!UI_MENU_AppendStr(gMenuListBuffer, sizeof(gMenuListBuffer), &out, prefix) ||
                    !UI_MENU_AppendInt(gMenuListBuffer, sizeof(gMenuListBuffer), &out, value) ||
                    !UI_MENU_AppendStr(gMenuListBuffer, sizeof(gMenuListBuffer), &out, suffix)) {
                    gMenuListBuffer[sizeof(gMenuListBuffer) - 1] = '\0';
                    value = max;
                }
                break;
            case MENU_LIST_TIME_MMSS:
            {
                const uint32_t seconds = (uint32_t)(value + base_offset) * step;
                const uint32_t minutes = seconds / 60U;
                const uint32_t secs = seconds % 60U;
                if (!UI_MENU_AppendUIntWidth(gMenuListBuffer, sizeof(gMenuListBuffer), &out, minutes, 2) ||
                    !UI_MENU_AppendChar(gMenuListBuffer, sizeof(gMenuListBuffer), &out, 'm') ||
                    !UI_MENU_AppendChar(gMenuListBuffer, sizeof(gMenuListBuffer), &out, ':') ||
                    !UI_MENU_AppendUIntWidth(gMenuListBuffer, sizeof(gMenuListBuffer), &out, secs, 2) ||
                    !UI_MENU_AppendChar(gMenuListBuffer, sizeof(gMenuListBuffer), &out, 's')) {
                    gMenuListBuffer[sizeof(gMenuListBuffer) - 1] = '\0';
                    value = max;
                }
                break;
            }
            case MENU_LIST_TIME_HM:
            {
                const uint32_t total_minutes = (uint32_t)value * step;
                const uint32_t hours = total_minutes / 60U;
                const uint32_t minutes = total_minutes % 60U;
                if (!UI_MENU_AppendUIntWidth(gMenuListBuffer, sizeof(gMenuListBuffer), &out, hours, 0) ||
                    !UI_MENU_AppendChar(gMenuListBuffer, sizeof(gMenuListBuffer), &out, 'h') ||
                    !UI_MENU_AppendChar(gMenuListBuffer, sizeof(gMenuListBuffer), &out, ':') ||
                    !UI_MENU_AppendUIntWidth(gMenuListBuffer, sizeof(gMenuListBuffer), &out, minutes, 2) ||
                    !UI_MENU_AppendChar(gMenuListBuffer, sizeof(gMenuListBuffer), &out, 'm')) {
                    gMenuListBuffer[sizeof(gMenuListBuffer) - 1] = '\0';
                    value = max;
                }
                break;
            }
            case MENU_LIST_MIC_DB:
            {
                const uint8_t mic = gMicGain_dB2[value];
                if (!UI_MENU_AppendChar(gMenuListBuffer, sizeof(gMenuListBuffer), &out, '+') ||
                    !UI_MENU_AppendUIntWidth(gMenuListBuffer, sizeof(gMenuListBuffer), &out, mic / 2U, 0) ||
                    !UI_MENU_AppendChar(gMenuListBuffer, sizeof(gMenuListBuffer), &out, '.') ||
                    !UI_MENU_AppendUIntWidth(gMenuListBuffer, sizeof(gMenuListBuffer), &out, mic % 2U, 1) ||
                    !UI_MENU_AppendStr(gMenuListBuffer, sizeof(gMenuListBuffer), &out, "dB")) {
                    gMenuListBuffer[sizeof(gMenuListBuffer) - 1] = '\0';
                    value = max;
                }
                break;
            }
            case MENU_LIST_SCAN_REV:
                if (value == 0) {
                    if (!UI_MENU_AppendStr(gMenuListBuffer, sizeof(gMenuListBuffer), &out, "STOP")) {
                        gMenuListBuffer[sizeof(gMenuListBuffer) - 1] = '\0';
                        value = max;
                    }
                } else if (value < 81) {
                    const uint32_t ms = (uint32_t)value * 250U;
                    const uint32_t seconds = ms / 1000U;
                    const uint32_t millis = ms % 1000U;
                    if (!UI_MENU_AppendStr(gMenuListBuffer, sizeof(gMenuListBuffer), &out, "CAR ") ||
                        !UI_MENU_AppendUIntWidth(gMenuListBuffer, sizeof(gMenuListBuffer), &out, seconds, 2) ||
                        !UI_MENU_AppendChar(gMenuListBuffer, sizeof(gMenuListBuffer), &out, 's') ||
                        !UI_MENU_AppendChar(gMenuListBuffer, sizeof(gMenuListBuffer), &out, ':') ||
                        !UI_MENU_AppendUIntWidth(gMenuListBuffer, sizeof(gMenuListBuffer), &out, millis, 3) ||
                        !UI_MENU_AppendStr(gMenuListBuffer, sizeof(gMenuListBuffer), &out, "ms")) {
                        gMenuListBuffer[sizeof(gMenuListBuffer) - 1] = '\0';
                        value = max;
                    }
                } else {
                    const uint32_t seconds = (uint32_t)(value - 80) * 5U;
                    const uint32_t minutes = seconds / 60U;
                    const uint32_t secs = seconds % 60U;
                    if (!UI_MENU_AppendStr(gMenuListBuffer, sizeof(gMenuListBuffer), &out, "OUT ") ||
                        !UI_MENU_AppendUIntWidth(gMenuListBuffer, sizeof(gMenuListBuffer), &out, minutes, 2) ||
                        !UI_MENU_AppendChar(gMenuListBuffer, sizeof(gMenuListBuffer), &out, 'm') ||
                        !UI_MENU_AppendChar(gMenuListBuffer, sizeof(gMenuListBuffer), &out, ':') ||
                        !UI_MENU_AppendUIntWidth(gMenuListBuffer, sizeof(gMenuListBuffer), &out, secs, 2) ||
                        !UI_MENU_AppendChar(gMenuListBuffer, sizeof(gMenuListBuffer), &out, 's')) {
                        gMenuListBuffer[sizeof(gMenuListBuffer) - 1] = '\0';
                        value = max;
                    }
                }
                break;
            case MENU_LIST_DCS:
            {
                const uint32_t count = ARRAY_SIZE(DCS_Options);
                if (value == 0) {
                    if (!UI_MENU_AppendStr(gMenuListBuffer, sizeof(gMenuListBuffer), &out, gSubMenu_OFF_ON[0])) {
                        gMenuListBuffer[sizeof(gMenuListBuffer) - 1] = '\0';
                        value = max;
                    }
                } else if ((uint32_t)value <= count) {
                    if (!UI_MENU_AppendChar(gMenuListBuffer, sizeof(gMenuListBuffer), &out, 'D') ||
                        !UI_MENU_AppendOctalWidth(gMenuListBuffer, sizeof(gMenuListBuffer), &out,
                                                  DCS_Options[value - 1], 3) ||
                        !UI_MENU_AppendChar(gMenuListBuffer, sizeof(gMenuListBuffer), &out, 'N')) {
                        gMenuListBuffer[sizeof(gMenuListBuffer) - 1] = '\0';
                        value = max;
                    }
                } else if ((uint32_t)(value - (int32_t)count) <= count) {
                    if (!UI_MENU_AppendChar(gMenuListBuffer, sizeof(gMenuListBuffer), &out, 'D') ||
                        !UI_MENU_AppendOctalWidth(gMenuListBuffer, sizeof(gMenuListBuffer), &out,
                                                  DCS_Options[value - 1 - (int32_t)count], 3) ||
                        !UI_MENU_AppendChar(gMenuListBuffer, sizeof(gMenuListBuffer), &out, 'I')) {
                        gMenuListBuffer[sizeof(gMenuListBuffer) - 1] = '\0';
                        value = max;
                    }
                }
                break;
            }
            case MENU_LIST_CTCSS:
            {
                const uint32_t count = ARRAY_SIZE(CTCSS_Options);
                if (value == 0) {
                    if (!UI_MENU_AppendStr(gMenuListBuffer, sizeof(gMenuListBuffer), &out, gSubMenu_OFF_ON[0])) {
                        gMenuListBuffer[sizeof(gMenuListBuffer) - 1] = '\0';
                        value = max;
                    }
                } else if ((uint32_t)value <= count) {
                    const uint16_t tone = CTCSS_Options[value - 1];
                    if (!UI_MENU_AppendUIntWidth(gMenuListBuffer, sizeof(gMenuListBuffer), &out, tone / 10U, 0) ||
                        !UI_MENU_AppendChar(gMenuListBuffer, sizeof(gMenuListBuffer), &out, '.') ||
                        !UI_MENU_AppendUIntWidth(gMenuListBuffer, sizeof(gMenuListBuffer), &out, tone % 10U, 1) ||
                        !UI_MENU_AppendStr(gMenuListBuffer, sizeof(gMenuListBuffer), &out, "Hz")) {
                        gMenuListBuffer[sizeof(gMenuListBuffer) - 1] = '\0';
                        value = max;
                    }
                }
                break;
            }
            default:
                break;
            }
        }

        if (out + 1 >= sizeof(gMenuListBuffer)) {
            gMenuListBuffer[sizeof(gMenuListBuffer) - 1] = '\0';
            break;
        }

        if (value < max && out + 1 < sizeof(gMenuListBuffer)) {
            gMenuListBuffer[out++] = '\n';
            gMenuListBuffer[out] = '\0';
        }
    }

    return gMenuListBuffer;
}

static const char* UI_MENU_GetOptionLinesForId(int menuId)
{
    gSubMenuSelectionOffset = 0;

    int32_t        mMin;
    int32_t        mMax;
    char* buf = gMenuListBuffer;
    MENU_GetLimits(menuId, &mMin, &mMax);

    switch (menuId)
    {
    case MENU_SQL:
        return squelchStr;
    case MENU_STEP:
        return UI_MENU_GetStepList();
    case MENU_AM:
        return UI_MENU_JoinFixedList((const char*)gModulationStr, sizeof(gModulationStr[0]), ARRAY_SIZE(gModulationStr));
    case MENU_TXP:
        return UI_MENU_JoinFixedList((const char*)gSubMenu_TXP, sizeof(gSubMenu_TXP[0]), ARRAY_SIZE(gSubMenu_TXP));
    case MENU_SFT_D:
        return UI_MENU_JoinFixedList((const char*)gSubMenu_SFT_D, sizeof(gSubMenu_SFT_D[0]), ARRAY_SIZE(gSubMenu_SFT_D));
    case MENU_W_N:
        return UI_MENU_JoinFixedList((const char*)gSubMenu_W_N, sizeof(gSubMenu_W_N[0]), ARRAY_SIZE(gSubMenu_W_N));
    case MENU_COMPAND:
    case MENU_ABR_ON_TX_RX:
        return UI_MENU_JoinFixedList((const char*)gSubMenu_RX_TX, sizeof(gSubMenu_RX_TX[0]), ARRAY_SIZE(gSubMenu_RX_TX));
    case MENU_TDR:
        return UI_MENU_JoinPtrList(gSubMenu_RXMode, ARRAY_SIZE(gSubMenu_RXMode));
    case MENU_R_DCS:
    case MENU_T_DCS:
        return UI_MENU_BuildList(MENU_LIST_DCS, mMin, mMax, 0, 0U, NULL, NULL, NULL, NULL);
    case MENU_R_CTCS:
    case MENU_T_CTCS:
        return UI_MENU_BuildList(MENU_LIST_CTCSS, mMin, mMax, 0, 0U, NULL, NULL, NULL, NULL);
    case MENU_SC_REV:
        return UI_MENU_BuildList(MENU_LIST_SCAN_REV, mMin, mMax, 0, 0U, NULL, NULL, NULL, NULL);
    case MENU_TOT:
        return UI_MENU_BuildList(MENU_LIST_TIME_MMSS, mMin, mMax, 1, 5U, NULL, NULL, NULL, NULL);
    case MENU_MIC:
        return UI_MENU_BuildList(MENU_LIST_MIC_DB, mMin, mMax, 0, 0U, NULL, NULL, NULL, NULL);
    case MENU_ABR:
        return UI_MENU_BuildList(MENU_LIST_TIME_MMSS, mMin, mMax, 0, 5U, gSubMenu_OFF_ON[0], gSubMenu_OFF_ON[1], NULL, NULL);
    case MENU_ABR_MIN:
    case MENU_ABR_MAX:
        if (gIsInSubMenu) {
            BACKLIGHT_SetBrightness(gSubMenuSelection);
        }
        return UI_MENU_BuildList(MENU_LIST_NUMERIC, mMin, mMax, 0, 0U, NULL, NULL, NULL, NULL);
#ifdef ENABLE_FEAT_F4HWN_SLEEP
    case MENU_SET_OFF:
        return UI_MENU_BuildList(MENU_LIST_TIME_HM, mMin, mMax, 0, 1U, gSubMenu_OFF_ON[0], NULL, NULL, NULL);
#endif
#ifdef ENABLE_VOICE
    case MENU_VOICE:
        return UI_MENU_JoinFixedList((const char*)gSubMenu_VOICE, sizeof(gSubMenu_VOICE[0]), ARRAY_SIZE(gSubMenu_VOICE));
#endif
    case MENU_ROGER:
        return UI_MENU_JoinFixedList((const char*)gSubMenu_ROGER, sizeof(gSubMenu_ROGER[0]), ARRAY_SIZE(gSubMenu_ROGER));
        /*case MENU_PONMSG:
            return UI_MENU_JoinFixedList((const char*)gSubMenu_PONMSG, sizeof(gSubMenu_PONMSG[0]), ARRAY_SIZE(gSubMenu_PONMSG));*/
    case MENU_RESET:
        return UI_MENU_JoinFixedList((const char*)gSubMenu_RESET, sizeof(gSubMenu_RESET[0]), ARRAY_SIZE(gSubMenu_RESET));
    case MENU_F_LOCK:
        return UI_MENU_JoinPtrList(gSubMenu_F_LOCK, ARRAY_SIZE(gSubMenu_F_LOCK));
        /*case MENU_BAT_TXT:
            return UI_MENU_JoinFixedList((const char*)gSubMenu_BAT_TXT, sizeof(gSubMenu_BAT_TXT[0]), ARRAY_SIZE(gSubMenu_BAT_TXT));*/
    case MENU_BATTYP:
        return UI_MENU_JoinFixedList((const char*)gSubMenu_BATTYP, sizeof(gSubMenu_BATTYP[0]), ARRAY_SIZE(gSubMenu_BATTYP));
#ifdef ENABLE_ALARM
    case MENU_AL_MOD:
        return UI_MENU_JoinFixedList((const char*)gSubMenu_AL_MOD, sizeof(gSubMenu_AL_MOD[0]), ARRAY_SIZE(gSubMenu_AL_MOD));
#endif
#ifdef ENABLE_DTMF_CALLING
    case MENU_D_RSP:
        return UI_MENU_JoinFixedList((const char*)gSubMenu_D_RSP, sizeof(gSubMenu_D_RSP[0]), ARRAY_SIZE(gSubMenu_D_RSP));
#endif

    case MENU_MIC_BAR:
#ifdef ENABLE_AUDIO_BAR
        return UI_MENU_JoinFixedList((const char*)gSubMenu_OFF_ON, sizeof(gSubMenu_OFF_ON[0]), ARRAY_SIZE(gSubMenu_OFF_ON));
#else
        return gSubMenu_NA;
#endif
    case MENU_PTT_ID:
        return UI_MENU_JoinPtrList(gSubMenu_PTT_ID, ARRAY_SIZE(gSubMenu_PTT_ID));
    case MENU_VOL:
        snprintf(buf, sizeof(gMenuListBuffer), "%s\n%s\n%s\n%u.%02uV %u%%",
            AUTHOR_STRING_2,
            VERSION_STRING_2,
            EDITION_STRING,
            gBatteryVoltageAverage / 100, gBatteryVoltageAverage % 100,
            BATTERY_VoltsToPercent(gBatteryVoltageAverage)
        );
        return buf;
#ifdef ENABLE_FEAT_F4HWN
    case MENU_SET_PWR:
        return UI_MENU_JoinFixedList((const char*)gSubMenu_SET_PWR, sizeof(gSubMenu_SET_PWR[0]), ARRAY_SIZE(gSubMenu_SET_PWR));
    case MENU_SET_PTT:
        return UI_MENU_JoinFixedList((const char*)gSubMenu_SET_PTT, sizeof(gSubMenu_SET_PTT[0]), ARRAY_SIZE(gSubMenu_SET_PTT));
    case MENU_SET_TOT:
    case MENU_SET_EOT:
        return UI_MENU_JoinFixedList((const char*)gSubMenu_SET_TOT, sizeof(gSubMenu_SET_TOT[0]), ARRAY_SIZE(gSubMenu_SET_TOT));
    case MENU_SET_LCK:
        return UI_MENU_JoinFixedList((const char*)gSubMenu_SET_LCK, sizeof(gSubMenu_SET_LCK[0]), ARRAY_SIZE(gSubMenu_SET_LCK));
#ifdef ENABLE_FEAT_F4HWN_NARROWER
    case MENU_SET_NFM:
        return UI_MENU_JoinFixedList((const char*)gSubMenu_SET_NFM, sizeof(gSubMenu_SET_NFM[0]), ARRAY_SIZE(gSubMenu_SET_NFM));
#endif
#ifdef ENABLE_FEAT_F4HWN_RESCUE_OPS
    case MENU_SET_KEY:
        return UI_MENU_JoinFixedList((const char*)gSubMenu_SET_KEY, sizeof(gSubMenu_SET_KEY[0]), ARRAY_SIZE(gSubMenu_SET_KEY));
#endif
#endif
#ifdef ENABLE_SCANLIST
    case MENU_S_LIST:
        return UI_MENU_JoinFixedList((const char*)gSubMenu_SLIST, sizeof(gSubMenu_SLIST[0]), ARRAY_SIZE(gSubMenu_SLIST));
#endif
    case MENU_F1SHRT:
    case MENU_F1LONG:
    case MENU_F2SHRT:
    case MENU_F2LONG:
    case MENU_MLONG:
        return UI_MENU_JoinSideFunctions();

#ifndef ENABLE_FEAT_F4HWN
#ifdef ENABLE_AM_FIX
    case MENU_AM_FIX:
#endif
#endif
    case MENU_BCL:
    case MENU_BEEP:
#ifdef ENABLE_SCANLIST
    case MENU_S_ADD1:
    case MENU_S_ADD2:
    case MENU_S_ADD3:
#endif
    case MENU_STE:
    case MENU_D_ST:
#ifdef ENABLE_DTMF_CALLING
    case MENU_D_DCD:
#endif
    case MENU_D_LIVE_DEC:
#ifdef ENABLE_NOAA
    case MENU_NOAA_S:
#endif
#ifndef ENABLE_FEAT_F4HWN
    //case MENU_350TX:
    //case MENU_200TX:
    //case MENU_500TX:
#endif
    //case MENU_350EN:

#ifdef ENABLE_FEAT_F4HWN
    case MENU_SET_TMR:
#endif

    case MENU_TX_LOCK:
        return UI_MENU_JoinFixedList((const char*)gSubMenu_OFF_ON, sizeof(gSubMenu_OFF_ON[0]), ARRAY_SIZE(gSubMenu_OFF_ON));
    default:
        break;
    }

    // ************************************************************************************
    //

    switch (menuId)
    {

    case MENU_OFFSET:
        if (!gIsInSubMenu || gInputBoxIndex == 0)
        {
            const uint32_t frac = (gSubMenuSelection < 0)
                ? (uint32_t)(-gSubMenuSelection)
                : (uint32_t)gSubMenuSelection;
            snprintf(buf, sizeof(gMenuListBuffer), "%3d.%.4u MHz", gSubMenuSelection / 100000, frac % 100000U);
        }
        else
        {
            const char* ascii = INPUTBOX_GetAscii();
            snprintf(buf, sizeof(gMenuListBuffer), "%.3s.%.3s MHz", ascii, ascii + 3);
        }
        break;
    case MENU_AUTOLK:
        return UI_MENU_BuildList(MENU_LIST_TIME_MMSS, mMin, mMax, 0, 15U, gSubMenu_OFF_ON[0], NULL, NULL, NULL);
    case MENU_SAVE:
        return UI_MENU_BuildList(MENU_LIST_NUMERIC, mMin, mMax, 0, 0U, gSubMenu_OFF_ON[0], NULL, "1:", NULL);
        break;
    case MENU_RP_STE:
        return UI_MENU_BuildList(MENU_LIST_NUMERIC, mMin, mMax, 0, 0U, gSubMenu_OFF_ON[0], NULL, NULL, "*100ms");
        break;
    case MENU_VOX:
#ifdef ENABLE_VOX
        snprintf(buf, sizeof(gMenuListBuffer), gSubMenuSelection == 0 ? gSubMenu_OFF_ON[0] : "%u", gSubMenuSelection);
#else
        return gSubMenu_NA;
#endif
        break;
    case MENU_MEM_CH:
    case MENU_1_CALL:
        //case MENU_DEL_CH:
    {
        const bool valid = RADIO_CheckValidChannel(gSubMenuSelection, false, 0);
        UI_GenerateChannelStringEx(buf, valid, gSubMenuSelection);
        size_t len = strnlen(buf, sizeof(gMenuListBuffer));
        if (valid && !gAskForConfirmation) {
            const uint32_t frequency = SETTINGS_FetchChannelFrequency(gSubMenuSelection);
            len += snprintf(&buf[len], sizeof(gMenuListBuffer) - len,
                            "\n%u.%05u", frequency / 100000, frequency % 100000);
        }
        char* name = NULL;
        SETTINGS_FetchChannelName(name, gSubMenuSelection);
        if (name != NULL) {
            len += snprintf(&buf[len], sizeof(gMenuListBuffer) - len, "\n%.8s", name);
        }
    }
    break;

    /*case MENU_MEM_NAME:
    {
        const bool valid = RADIO_CheckValidChannel(gSubMenuSelection, false, 0);
        UI_GenerateChannelStringEx(buf, valid, gSubMenuSelection);

    }
    break;*/

    case MENU_UPCODE:
        memcpy(buf, gEeprom.DTMF_UP_CODE, 8);
        buf[8] = '\n';
        memcpy(buf + 9, gEeprom.DTMF_UP_CODE + 8, 8);
        buf[17] = '\0';
        break;

    case MENU_DWCODE:
        memcpy(buf, gEeprom.DTMF_DOWN_CODE, 8);
        buf[8] = '\n';
        memcpy(buf + 9, gEeprom.DTMF_DOWN_CODE + 8, 8);
        buf[17] = '\0';
        break;

    case MENU_D_PRE:
        return UI_MENU_BuildList(MENU_LIST_NUMERIC, mMin, mMax, 0, 0U, NULL, NULL, NULL, "*10ms");
        break;
#ifdef ENABLE_SCANLIST
    case MENU_SLIST1:
    case MENU_SLIST2:
    case MENU_SLIST3:
        if (gSubMenuSelection < 0) {
            strcpy(buf, "NULL");
        }
        else {
            UI_GenerateChannelStringEx(buf, true, gSubMenuSelection);
        }
        break;
#endif
        // ************************************************************************************
    default:
        snprintf(buf, sizeof(gMenuListBuffer), "* %ld *", (long)gSubMenuSelection);
        break;
    }

    return buf;
}

const char* getCurrentOption() {
    const char* lines = UI_MENU_GetOptionLinesForId(UI_MENU_GetCurrentMenuId());
    if (u8x8_GetStringLineCnt(lines) == 1)
        return lines;
    int32_t index = gSubMenuSelection + gSubMenuSelectionOffset;
    if (index < 0) {
        index = 0;
    }
    return UI_GetStrValue(lines, (uint8_t)index);
}

void setSubMenu(void) {
    const char* lines = UI_MENU_GetOptionLinesForId(UI_MENU_GetCurrentMenuId());
    const uint8_t total = u8x8_GetStringLineCnt(lines);
    const uint8_t visible = MIN(total, 5);
    UI_SelectionList_Set(&subMenuList, gSubMenuSelection + gSubMenuSelectionOffset, visible > 0 ? visible : 1, 122, lines, NULL);
}

void UI_DisplayMenu(void)
{
    const int current_menu_id = UI_MENU_GetCurrentMenuId();

    // clear the screen
    UI_ClearDisplay();
    // redraw the menu
    UI_SetBlackColor();

    UI_DrawBox(0, 0, 128, 7);

    UI_SetFont(FONT_8B_TR);
    UI_DrawString(UI_TEXT_ALIGN_LEFT, 2, 0, 6, false, false, false, "MENU");
    UI_DrawStringf(UI_TEXT_ALIGN_RIGHT, 0, 126, 6, false, false, false, "%02u / %02u", 1 + gMenuCursor, gMenuListCount);

    UI_SetBlackColor();
    UI_SelectionList_SetCurrentPos(&menuList, gMenuCursor);

    UI_SelectionList_Draw(&menuList, 15, getCurrentOption());

    if ((current_menu_id == MENU_RESET ||
        current_menu_id == MENU_MEM_CH/* ||
        UI_MENU_GetCurrentMenuId() == MENU_MEM_NAME ||
        UI_MENU_GetCurrentMenuId() == MENU_DEL_CH*/) && gAskForConfirmation)
    {   // display confirmation

        UI_DrawPopupWindow(20, 20, 88, 24, "Info");
        UI_SetFont(FONT_8B_TR);
        UI_DrawString(UI_TEXT_ALIGN_CENTER, 22, 106, 36, true, false, false, (gAskForConfirmation == 1) ? "SURE?" : "WAIT!");

    }
    else if (gIsInSubMenu) {
        if (current_menu_id == MENU_VOL) {
            UI_DisplayWelcome();
            return;
        }
    #ifdef ENABLE_SCANLIST
        if (current_menu_id == MENU_SLIST1 ||
            current_menu_id == MENU_SLIST2 ||
            current_menu_id == MENU_SLIST3) {
            UI_MENU_DrawScanListPopup();
        }
        else
    #endif
        {
            UI_DrawPopupWindow(36, 6, 90, 52, UI_SelectionList_GetStringLine(&menuList));
            setSubMenu();
            UI_SelectionList_Draw(&subMenuList, 20, NULL);
        }
    }

    UI_UpdateDisplay();
}

void UI_MenuInit(void)
{
    UI_SelectionList_Init(&menuList);
    UI_SelectionList_Set(&menuList, gMenuCursor, 6, 127, UI_MENU_GetMenuListLines(), NULL);

    UI_SelectionList_Init(&subMenuList);
    //UI_SelectionList_SetMaxWidth(&subMenuList, 90);
    UI_SelectionList_SetShowLineNumbers(&subMenuList, false);
    UI_SelectionList_SetStartXPos(&subMenuList, 40);
}
