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

#ifdef ENABLE_FMRADIO

#include <string.h>

#include "app/fm.h"
#include "driver/bk1080.h"
//#include "driver/st7565.h"
#include "printf.h"
#include "misc.h"
#include "settings.h"
#include "ui/fmradio.h"
//#include "ui/helper.h"
#ifndef ENABLE_FMRADIO_BASIC
#include "ui/inputbox.h"
#endif
#include "ui/ui.h"
#include "ui/gui.h"

void UI_DisplayFM(void)
{
    char String[16] = {0};
#ifdef ENABLE_FMRADIO_BASIC
    const char *modeLabel = "FM RADIO";    // 基础FM广播模式
#else
    const char *pPrintStr = String;
#endif

    UI_ClearDisplay();
    UI_SetBlackColor();

    UI_DrawBox(0, 0, 128, 7);
    UI_SetFont(FONT_8B_TR);
    UI_DrawString(UI_TEXT_ALIGN_LEFT, 2, 0, 6, false, false, false, "FM");

    sprintf(String, "%d%s-%dM", 
        BK1080_GetFreqLoLimit(gEeprom.FM_Band)/10,
        gEeprom.FM_Band == 0 ? ".5" : "",
        BK1080_GetFreqHiLimit(gEeprom.FM_Band)/10
        );
    
    UI_SetFont(FONT_5_TR);
    UI_DrawString(UI_TEXT_ALIGN_RIGHT, 0, 126, 6, false, false, false, String);

    //uint8_t spacings[] = {20,10,5};
    //sprintf(String, "%d0k", spacings[gEeprom.FM_Space % 3]);
    //UI_PrintStringSmallNormal(String, 127 - 4*7, 0, 6);

    #ifdef ENABLE_FMRADIO_BASIC
    UI_SetFont(FONT_8B_TR);
    UI_DrawString(UI_TEXT_ALIGN_CENTER, 0, 127, 19, true, false, false, modeLabel);

    sprintf(String, "%3d.%d", gEeprom.FM_FrequencyPlaying / 10, gEeprom.FM_FrequencyPlaying % 10);

    UI_SetFont(FONT_BN_TN);
    UI_DrawString(UI_TEXT_ALIGN_CENTER, 0, 127, 47, true, false, false, String);
    UI_SetFont(FONT_5_TR);
    UI_DrawString(UI_TEXT_ALIGN_CENTER, 0, 127, 63, true, false, false, "UP/DN 0:OFF 1:BAND");   // 上下键调谐 0:关机 1:选波段
    UI_UpdateDisplay();
    return;
    #else
    if (gAskToSave) {
        pPrintStr = "SAVE?";   // 保存?
    } else if (gAskToDelete) {
        pPrintStr = "DEL?";    // 删除?
    } else if (gFM_ScanState == FM_SCAN_OFF) {
        if (gEeprom.FM_IsMrMode) {
            sprintf(String, "MR(CH%02u)", gEeprom.FM_SelectedChannel + 1);
            pPrintStr = String;
        } else {
            pPrintStr = "VFO";
            for (unsigned int i = 0; i < 20; i++) {
                if (gEeprom.FM_FrequencyPlaying == gFM_Channels[i]) {
                    sprintf(String, "VFO(CH%02u)", i + 1);
                    pPrintStr = String;
                    break;
                }
            }
        }
    } else if (gFM_AutoScan) {
        sprintf(String, "A-SCN(%u)", gFM_ChannelPosition + 1);   // 自动扫描
        pPrintStr = String;
    } else {
        pPrintStr = "M-SCN";       // 手动搜索
    }

    UI_SetFont(FONT_8B_TR);
    UI_DrawString(UI_TEXT_ALIGN_CENTER, 0, 127, 19, true, false, false, pPrintStr); // memory, vfo, scan

    memset(String, 0, sizeof(String));
    if (gAskToSave || (gEeprom.FM_IsMrMode && gInputBoxIndex > 0)) {
        sprintf(String, "CH-%02u", gFM_ChannelPosition + 1);
    } else if (gAskToDelete) {
        sprintf(String, "CH-%02u", gEeprom.FM_SelectedChannel + 1);
    } else {
        if (gInputBoxIndex == 0) {
            sprintf(String, "%3d.%d", gEeprom.FM_FrequencyPlaying / 10, gEeprom.FM_FrequencyPlaying % 10);
        } else {
            const char * ascii = INPUTBOX_GetAscii();
            sprintf(String, "%.3s.%.1s",ascii, ascii + 3);
        }

        UI_SetFont(FONT_BN_TN);
        UI_DrawString(UI_TEXT_ALIGN_CENTER, 0, 127, 47, true, false, false, String);  // frequency
        UI_UpdateDisplay();
        return;
    }

    UI_SetFont(FONT_10_TR);
    UI_DrawString(UI_TEXT_ALIGN_CENTER, 0, 127, 44, true, false, false, String);
    #endif

    UI_UpdateDisplay();
}

#endif
