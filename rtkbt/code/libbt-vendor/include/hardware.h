/******************************************************************************
 *
 *  Copyright (C) 2009-2018 Realtek Corporation
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

#ifndef HARDWARE_H
#define HARDWARE_H

//#define TEST_NEW_CHIP
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define cpu_to_le16(d)  (d)
#define cpu_to_le32(d)  (d)
#define le16_to_cpu(d)  (d)
#define le32_to_cpu(d)  (d)
#elif __BYTE_ORDER == __BIG_ENDIAN
#define cpu_to_le16(d)  bswap_16(d)
#define cpu_to_le32(d)  bswap_32(d)
#define le16_to_cpu(d)  bswap_16(d)
#define le32_to_cpu(d)  bswap_32(d)
#else
#error "Unknown byte order"
#endif

#define FIRMWARE_DIRECTORY  "/vendor/firmware/%s"
#define BT_CONFIG_DIRECTORY "/vendor/firmware/%s"
#ifdef  TEST_NEW_CHIP
#define TEST_FIRMWARE_NAME  "rtlbt_fw"
#define TEST_CONFIG_NAME    "rtlbt_config"
#endif

#define PATCH_DATA_FIELD_MAX_SIZE       252
#define RTK_VENDOR_CONFIG_MAGIC         0x8723ab55
#define MAX_PATCH_SIZE_24K            (1024*24 + 529)   //24K
#define MAX_PATCH_SIZE_25K            (1024*25 + 529)   //25K for rtl8822b
#define MAX_PATCH_SIZE_40K            (1024*40 + 529)   //40K
#define MAX_PATCH_SIZE_49_2K          (0xC4CF + 529)   //49.2K 8723f
#define MAX_PATCH_SIZE_69_2K          (0x114D0 + 529)  //69.2K 8852a
#define MAX_PATCH_SIZE_65_2K          (0x104D0 + 529)   //65.2K 8852b
#define MAX_PATCH_SIZE_78K            (1024*78 + 529)   //78K  8852c
#define MAX_PATCH_SIZE_145K           (0x24620)        //145K 8822E

#define MAX_ORG_CONFIG_SIZE     (0x100*14)
#define MAX_ALT_CONFIG_SIZE     (0x100*2)

struct rtk_bt_vendor_config_entry
{
    uint16_t offset;
    uint8_t entry_len;
    uint8_t entry_data[0];
} __attribute__((packed));

struct rtk_bt_vendor_config
{
    uint32_t signature;
    uint16_t data_len;
    struct rtk_bt_vendor_config_entry entry[0];
} __attribute__((packed));

#define HCI_CMD_MAX_LEN             258

#define HCI_VERSION_MASK_10     (1<<0)     //Bluetooth Core Spec 1.0b
#define HCI_VERSION_MASK_11     (1<<1)     //Bluetooth Core Spec 1.1
#define HCI_VERSION_MASK_12     (1<<2)     //Bluetooth Core Spec 1.2
#define HCI_VERSION_MASK_20     (1<<3)     //Bluetooth Core Spec 2.0+EDR
#define HCI_VERSION_MASK_21     (1<<4)     //Bluetooth Core Spec 2.1+EDR
#define HCI_VERSION_MASK_30     (1<<5)     //Bluetooth Core Spec 3.0+HS
#define HCI_VERSION_MASK_40     (1<<6)     //Bluetooth Core Spec 4.0
#define HCI_VERSION_MASK_41     (1<<7)     //Bluetooth Core Spec 4.1
#define HCI_VERSION_MASK_42     (1<<8)     //Bluetooth Core Spec 4.2
#define HCI_VERSION_MASK_ALL    (0xFFFFFFFF)

#define HCI_REVISION_MASK_ALL   (0xFFFFFFFF)

#define LMP_SUBVERSION_NONE     (0x0)
#define LMPSUBVERSION_8723a     (0x1200)

#define CHIPTYPE_NONE           (0x1F)      //Chip Type's range: 0x0 ~ 0xF
#define CHIP_TYPE_MASK_ALL      (0xFFFFFFFF)
#define PROJECT_ID_MASK_ALL     (0xFFFFFFFFFFFFFFFF)    // temp used for unknow project id for a new chip

#define PATCH_OPTIONAL_MATCH_FLAG_CHIPTYPE   (0x1)

#define CONFIG_MAC_OFFSET_GEN_1_2       (0x3C)      //MAC's OFFSET in config/efuse for realtek generation 1~2 bluetooth chip
#define CONFIG_MAC_OFFSET_GEN_3PLUS     (0x44)      //MAC's OFFSET in config/efuse for rtk generation 3+ bluetooth chip
#define CONFIG_MAC_OFFSET_GEN_4PLUS     (0x30)      //MAC's OFFSET in config/efuse for rtk generation 4+ bluetooth chip

#define HCI_EVT_CMD_CMPL_OPCODE_OFFSET          (3)     //opcode's offset in COMMAND Completed Event
#define HCI_EVT_CMD_CMPL_STATUS_OFFSET          (5)     //status's offset in COMMAND Completed Event

#define HCI_EVT_CMD_CMPL_OP1001_HCI_VERSION_OFFSET     (6)     //HCI_Version's offset in COMMAND Completed Event for OpCode 0x1001(Read Local Version Information Command)
#define HCI_EVT_CMD_CMPL_OP1001_HCI_REVISION_OFFSET     (7)     //HCI_Revision's offset in COMMAND Completed Event for OpCode 0x1001(Read Local Version Information Command)
#define HCI_EVT_CMD_CMPL_OP1001_LMP_SUBVERSION_OFFSET  (12)    //LMP Subversion's offset in COMMAND Completed Event for OpCode 0x1001(Read Local Version Information Command)
#define HCI_EVT_CMD_CMPL_OP0C14_LOCAL_NAME_OFFSET      (6)     //Local Name's offset in COMMAND Completed Event for OpCode 0x0C14(Read Local Name Command)
#define HCI_EVT_CMD_CMPL_OP1009_BDADDR_OFFSET    (6)     //BD_ADDR's offset in COMMAND Completed Event for OpCode 0x1009(Read BD_ADDR Command)
#define HCI_EVT_CMD_CMPL_OPFC6D_EVERSION_OFFSET        (6)  //eversion's offset in COMMAND Completed Event for OpCode 0xfc6d(Read eVERSION Vendor Command)
#define HCI_EVT_CMD_CMPL_OPFC61_CHIPTYPE_OFFSET        (6)  //chip type's offset in COMMAND Completed Event for OpCode 0xfc61(Read ChipType Vendor Command)
#define HCI_EVT_CMD_CMPL_OPFC61_KEY_ID_OFFSET          (6)  //chip type's offset in COMMAND Completed Event for OpCode 0xfc61(Read KeyID Vendor Command)
#define HCI_EVT_CMD_CMPL_OPFC61_8761_CHIPTYPE_OFFSET   (8)  //8761B chip type's offset in COMMAND Completed Event for OpCode 0xfc61(Read ChipType Vendor Command)
#define HCI_EVT_CMD_CMPL_OPFC61_CHIPDIE_OFFSET         (7)  //chip die's offset in COMMAND Completed Event for OpCode 0xfc61(Read ChipType Vendor Command)

//#define UPDATE_BAUDRATE_CMD_PARAM_SIZE          (6)
#define HCI_CMD_PREAMBLE_SIZE                   (3)
#define HCI_CMD_READ_CHIP_TYPE_SIZE             (5)
#define HCI_CMD_READ_CHIP_KEY_ID_SIZE           (5)

//#define HCD_REC_PAYLOAD_LEN_BYTE                (2)
//#define BD_ADDR_LEN                             (6)
//#define LOCAL_NAME_BUFFER_LEN                   (32)
//#define LOCAL_BDADDR_PATH_BUFFER_LEN            (256)

#define H5_SYNC_REQ_SIZE (2)
#define H5_SYNC_RESP_SIZE (2)
#define H5_CONF_REQ_SIZE (3)
#define H5_CONF_RESP_SIZE (2)

/******************************************************************************
**  Local type definitions
******************************************************************************/

/* Hardware Configuration State */
enum
{
    HW_CFG_H5_INIT = 1,
    HW_CFG_READ_LOCAL_VER,
    HW_CFG_READ_ECO_VER,   //eco version
    HW_CFG_READ_CHIP_TYPE,
    HW_CFG_START,
    HW_CFG_SET_UART_BAUD_HOST,//change FW baudrate
    HW_CFG_SET_UART_BAUD_CONTROLLER,//change Host baudrate
    HW_CFG_SET_UART_HW_FLOW_CONTROL,
    HW_CFG_RESET_CHANNEL_CONTROLLER,
    HW_RESET_CONTROLLER,
    HARDWARE_INIT_COMPLETE,
    HW_CFG_DL_FW_PATCH,
    HW_VENDOR_WRITE,
    HW_CFG_READ_KEY_ID,
    HW_CFG_READ_FC61_LMP_SUB,//8822e initialize
    HW_CFG_READ_FC61_HCI_SUB,
    HW_CFG_READ_LMP
};

/* h/w config control block */
typedef struct
{
    uint32_t    max_patch_size;
    uint32_t    baudrate;
    uint16_t    lmp_subversion;
    uint16_t    lmp_subversion_default;
    uint16_t    lmp_sub_current;
    uint8_t     state;          /* Hardware configuration state */
    uint8_t     eversion;
    uint64_t    project_id_mask;
    uint8_t     hci_version;
    uint8_t     hci_revision;
    uint8_t     chip_type;
    uint8_t     dl_fw_flag;
    int         fw_len;          /* FW patch file len */
    size_t      config_len;      /* Config patch file len */
    unsigned int         total_len;       /* FW & config extracted buf len */
    uint8_t     *fw_buf;         /* FW patch file buf */
    uint8_t     *config_buf;     /* Config patch file buf */
    uint8_t     *total_buf;      /* FW & config extracted buf */
    uint16_t     patch_frag_cnt;  /* Patch fragment count download */
    uint16_t     patch_frag_idx;  /* Current patch fragment index */
    uint8_t     patch_frag_len;  /* Patch fragment length */
    uint8_t     patch_frag_tail; /* Last patch fragment length */
    uint8_t     hw_flow_cntrl;   /* Uart flow control, bit7:set, bit0:enable */
    uint16_t    vid;   /* usb vendor id */
    uint16_t    pid;   /* usb product id */
    uint8_t     heartbeat; /*heartbeat*/
    uint8_t     parsing_rule; /* fw merge rule 1: v1, 2: v2 */
    uint8_t     keyid; /* fw merge rule 1: v1, 2: v2 */
    bool        en_pwr_whtl;
} bt_hw_cfg_cb_t;

/* low power mode parameters */
typedef struct
{
    uint8_t sleep_mode;                     /* 0(disable),1(UART),9(H5) */
    uint8_t host_stack_idle_threshold;      /* Unit scale 300ms/25ms */
    uint8_t host_controller_idle_threshold; /* Unit scale 300ms/25ms */
    uint8_t bt_wake_polarity;               /* 0=Active Low, 1= Active High */
    uint8_t host_wake_polarity;             /* 0=Active Low, 1= Active High */
    uint8_t allow_host_sleep_during_sco;
    uint8_t combine_sleep_mode_and_lpm;
    uint8_t enable_uart_txd_tri_state;      /* UART_TXD Tri-State */
    uint8_t sleep_guard_time;               /* sleep guard time in 12.5ms */
    uint8_t wakeup_guard_time;              /* wakeup guard time in 12.5ms */
    uint8_t txd_config;                     /* TXD is high in sleep state */
    uint8_t pulsed_host_wake;               /* pulsed host wake if mode = 1 */
} bt_lpm_param_t;

#define ROM_LMP_NONE                0x0000
#define ROM_LMP_8723a               0x1200
#define ROM_LMP_8723b               0x8723
#define ROM_LMP_8821a               0X8821
#define ROM_LMP_8761a               0X8761
#define ROM_LMP_8761b               0X8761
#define ROM_LMP_8703a               0x8723
#define ROM_LMP_8763a               0x8763
#define ROM_LMP_8703b               0x8703
#define ROM_LMP_8723c               0x8703
#define ROM_LMP_8822b               0x8822
#define ROM_LMP_8723d               0x8723
#define ROM_LMP_8821c               0x8821
#define ROM_LMP_8822c               0x8822
#define ROM_LMP_8852a               0x8852
#define ROM_LMP_8723f               0x8723
#define ROM_LMP_8852b               0x8852
#define ROM_LMP_8763c               0x8763
#define ROM_LMP_8773b               0x8773
#define ROM_LMP_8762a               0x8762
#define ROM_LMP_8762b               0x8762
#define ROM_LMP_8852c               0x8852
#define ROM_LMP_8851a               0x8852
#define ROM_LMP_8852bp              0x8852
#define ROM_LMP_8851b               0x8851
#define ROM_LMP_8822e               0x8822

#define HCI_VERSION_5_3             0x000C
#define HCI_VERSION_5_2             0x000B
#define HCI_VERSION_5_1             0x000A
#define HCI_VERSION_5_0             0x0009
#define HCI_VERSION_4_2             0x0008
#define HCI_VERSION_4_1             0x0007
#define HCI_VERSION_4_0             0x0006
#define HCI_VERSION_2_1             0x0004

struct rtk_epatch_entry
{
    uint16_t chip_id;
    uint16_t patch_length;
    uint32_t patch_offset;
    uint32_t svn_version;
    uint32_t coex_version;
} __attribute__((packed));

struct rtk_epatch
{
    uint8_t signature[8];
    uint32_t fw_version;
    uint16_t number_of_patch;
    struct rtk_epatch_entry entry[0];
} __attribute__((packed));

struct rtk_epatch_ota
{
    uint8_t chip_id;
    uint8_t enable;
    uint16_t reserve;
} __attribute__((packed));

struct rtk_epatch_fragment
{
    uint8_t chip_id;
    uint8_t priority;
    uint8_t key_id;
    uint8_t reserve;
    uint32_t length;
    uint8_t  data[0];
} __attribute__((packed));

struct rtk_epatch_section
{
    uint32_t opcode;
    uint32_t length;
    uint16_t number_of_fragment;
    uint16_t reserve;
    struct rtk_epatch_fragment fragment[0];
} __attribute__((packed));

struct rtk_epatch_v2
{
    uint8_t signature[8];
    uint32_t fw_version;
    uint32_t fw_version_sub;
    uint32_t number_of_section;
    struct rtk_epatch_section section[0];
} __attribute__((packed));

struct rtk_epatch_fragment_linklist
{
    struct rtk_epatch_fragment *fragment;
    struct rtk_epatch_fragment_linklist *next;
} __attribute__((packed));

#endif
