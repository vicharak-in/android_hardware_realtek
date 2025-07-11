/******************************************************************************
 *
 *  Copyright (C) 2009-2018 Realtek Corporation.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/
#define LOG_TAG "rtk_heartbeat"
#define RTKBT_RELEASE_NAME "20201130_BT_ANDROID_12.0"

#include <utils/Log.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <termios.h>
#include <sys/syscall.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>
#include <cutils/properties.h>
#include <stdlib.h>
#include "bt_hci_bdroid.h"
#include "bt_vendor_rtk.h"
#include "userial.h"
#include "userial_vendor.h"
#include "rtk_btservice.h"
#include "rtk_poll.h"
#include "upio.h"
#include <unistd.h>
#include <sys/eventfd.h>
#include <semaphore.h>
#include <endian.h>
#include <byteswap.h>
#include <sys/un.h>
#include <stddef.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "bt_vendor_lib.h"

#define RTKBT_HEARTBEAT_CONF_FILE         "/vendor/etc/bluetooth/rtkbt_heartbeat.conf"

#define HCI_EVT_HEARTBEAT_STATUS_OFFSET          (5)
#define HCI_EVT_HEARTBEAT_SEQNUM_OFFSET_L          (6)
#define HCI_EVT_HEARTBEAT_SEQNUM_OFFSET_H          (7)
#define RTK_HANDLE_EVENT

static const uint32_t DEFALUT_HEARTBEAT_TIMEOUT_MS = 1000; //send a per sercond
int heartBeatLog = -1;
static int heartBeatTimeout = -1;
static bool heartbeatFlag = false;
static int heartbeatCount = 0;
volatile uint32_t heartbeatCmdCount = 0;
static uint16_t nextSeqNum = 1;
static uint16_t cleanupFlag = 0;
static pthread_mutex_t heartbeat_mutex;

typedef struct Rtk_Service_Data
{
    uint16_t        opcode;
    uint8_t         parameter_len;
    uint8_t         *parameter;
    void (*complete_cback)(void *);
} Rtk_Service_Data;

extern void Rtk_Service_Vendorcmd_Hook(Rtk_Service_Data *RtkData, int client_sock);
extern uint8_t get_heartbeat_from_hardware();
extern void userial_send_cmd_to_controller(unsigned char *recv_buffer, int total_length);

static char *rtk_trim(char *str)
{
    while (isspace(*str))
    {
        ++str;
    }

    if (!*str)
    {
        return str;
    }

    char *end_str = str + strlen(str) - 1;
    while (end_str > str && isspace(*end_str))
    {
        --end_str;
    }

    end_str[1] = '\0';
    return str;
}


static void load_rtkbt_heartbeat_conf()
{
    char *split;
    FILE *fp = fopen(RTKBT_HEARTBEAT_CONF_FILE, "rt");
    if (!fp)
    {
        ALOGE("%s unable to open file '%s': %s", __func__, RTKBT_HEARTBEAT_CONF_FILE, strerror(errno));
        return;
    }
    int line_num = 0;
    char line[1024];
    //char value[1024];
    while (fgets(line, sizeof(line), fp))
    {
        char *line_ptr = rtk_trim(line);
        ++line_num;

        // Skip blank and comment lines.
        if (*line_ptr == '\0' || *line_ptr == '#' || *line_ptr == '[')
        {
            continue;
        }

        split = strchr(line_ptr, '=');
        if (!split)
        {
            ALOGE("%s no key/value separator found on line %d.", __func__, line_num);
            continue;
        }

        *split = '\0';
        char *endptr;
        if (!strcmp(rtk_trim(line_ptr), "HeartBeatTimeOut"))
        {
            heartBeatTimeout = strtol(rtk_trim(split + 1), &endptr, 0);
        }
        else if (!strcmp(rtk_trim(line_ptr), "HeartBeatLog"))
        {
            heartBeatLog = strtol(rtk_trim(split + 1), &endptr, 0);
        }
    }

    fclose(fp);

}

static void rtkbt_heartbeat_send_hw_error(uint8_t status, uint16_t seqnum, uint16_t next_seqnum,
                                          int heartbeatCnt)
{
    if (!heartbeatFlag)
    {
        return;
    }
    unsigned char p_buf[100];
    int length;
    p_buf[0] = HCIT_TYPE_EVENT;//event
    p_buf[1] = HCI_VSE_SUBCODE_DEBUG_INFO_SUB_EVT;//firmwre event log
    p_buf[3] = 0x01;// host log opcode
    length = snprintf((char *)&p_buf[4], 96, "host stack: heartbeat hw error: %d:%d:%d:%d \n",
                      status, seqnum, next_seqnum, heartbeatCnt);
    p_buf[2] = length + 2;//len
    length = length + 1 + 4;
    userial_recv_rawdata_hook(p_buf, length);

    length = 4;
    p_buf[0] = HCIT_TYPE_EVENT;//event
    p_buf[1] = HCI_HARDWARE_ERROR_EVT;//hardware error
    p_buf[2] = 0x01;//len
    p_buf[3] = HEARTBEAT_HWERR_CODE_RTK;//heartbeat error code
    userial_recv_rawdata_hook(p_buf, length);
}

void rtkbt_heartbeat_cmpl_cback(void *p_params)
{
    uint8_t  status = 0;
    uint16_t seqnum = 0;
    uint8_t *pp_params = (uint8_t *)p_params;

    if (!heartbeatFlag)
    {
        return;
    }

    if (p_params != NULL)
    {
        status = pp_params[HCI_EVT_HEARTBEAT_STATUS_OFFSET];
        seqnum = pp_params[HCI_EVT_HEARTBEAT_SEQNUM_OFFSET_H] << 8 |
                 pp_params[HCI_EVT_HEARTBEAT_SEQNUM_OFFSET_L];
    }
    ALOGD("rtkbt_heartbeat_cmpl_cback: Current SeqNum = %d,should SeqNum=%d, status = %d", seqnum,
          nextSeqNum, status);
    if (status == 0 && (seqnum >= nextSeqNum  && seqnum <= heartbeatCmdCount))
    {
        if (seqnum == 1)
        {
            heartbeatCmdCount = 1;
        }
        nextSeqNum = (seqnum + 1);
        pthread_mutex_lock(&heartbeat_mutex);
        heartbeatCount = 0;
        pthread_mutex_unlock(&heartbeat_mutex);
    }
    else
    {
        ALOGE("rtkbt_heartbeat_cmpl_cback: Current SeqNum = %d,should SeqNum=%d, status = %d", seqnum,
              nextSeqNum, status);
        ALOGE("heartbeat event missing:  restart bluedroid stack\n");
        usleep(1000);
        rtkbt_heartbeat_send_hw_error(status, seqnum, nextSeqNum, heartbeatCount);
    }

}


static void heartbeat_timed_out()//(union sigval arg)
{
    int count;
    uint8_t heartbeat_cmd[4] = {0x01, 0x94, 0xfc, 0x00};
    if (!heartbeatFlag)
    {
        return;
    }
    pthread_mutex_lock(&heartbeat_mutex);
    heartbeatCount++;
    if (heartbeatCount >= 3)
    {
        if (cleanupFlag == 1)
        {
            ALOGW("Already cleanup, ignore.");
            pthread_mutex_unlock(&heartbeat_mutex);
            return;
        }
        ALOGE("heartbeat_timed_out: heartbeatCount = %d, expected nextSeqNum = %d", heartbeatCount,
              nextSeqNum);
        ALOGE("heartbeat_timed_out, controller may be suspend! Now restart bluedroid stack\n");
        count = heartbeatCount;
        pthread_mutex_unlock(&heartbeat_mutex);
        usleep(1000);
        rtkbt_heartbeat_send_hw_error(0, 0, nextSeqNum, count);

        //kill(getpid(), SIGKILL);
        return;
    }
    pthread_mutex_unlock(&heartbeat_mutex);
    if (heartbeatFlag)
    {

        heartbeatCmdCount++;
        ALOGD("heartbeat_timed_out: heartbeatCmdCount = %d, expected nextSeqNum = %d", heartbeatCmdCount,
              nextSeqNum);
        userial_send_cmd_to_controller(heartbeat_cmd, 4);

        poll_timer_flush();
    }
}


static void rtkbt_heartbeat_beginTimer_func(void)
{
    uint8_t heartbeat_cmd[4] = {0x01, 0x94, 0xfc, 0x00};
    pthread_mutex_lock(&heartbeat_mutex);
    if ((heartBeatTimeout != -1) && (heartBeatLog != -1))
    {
        poll_init(heartbeat_timed_out, heartBeatTimeout);
    }
    else
    {
        heartBeatLog = 0;
        poll_init(heartbeat_timed_out, DEFALUT_HEARTBEAT_TIMEOUT_MS);
    }
    poll_enable(TRUE);

    userial_send_cmd_to_controller(heartbeat_cmd, 4);
    heartbeatCmdCount++;

    poll_timer_flush();
    pthread_mutex_unlock(&heartbeat_mutex);
}

void Heartbeat_cleanup()
{
    if (!heartbeatFlag)
    {
        return;
    }
    heartbeatFlag = false;
    nextSeqNum = 1;
    heartbeatCount = 0;
    heartbeatCmdCount = 0;
    cleanupFlag = 1;

    pthread_mutex_lock(&heartbeat_mutex);
    poll_enable(FALSE);
    poll_cleanup();
    pthread_mutex_unlock(&heartbeat_mutex);
}

void Heartbeat_init()
{
    if (heartbeatFlag)
    {
        ALOGD("Heartbeat_init already start");
        return;
    }
    int res;
    ALOGD("Heartbeat_init start");
    Heartbeat_cleanup();
    load_rtkbt_heartbeat_conf();
    pthread_mutex_init(&heartbeat_mutex, NULL);
    heartbeatFlag = true;
    heartbeatCount = 0;
    cleanupFlag = 0;
    res = get_heartbeat_from_hardware();
    ALOGD("Heartbeat_init res = %x", res);
    if (res == 1)
    {
        rtkbt_heartbeat_beginTimer_func();
    }
    else
    {
        Heartbeat_cleanup();
    }
    ALOGD("Heartbeat_init end");
}

