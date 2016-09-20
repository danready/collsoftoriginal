// GNU General Public License Agreement
// Copyright (C) 2015-2016 Daniele Berto daniele.fratello@gmail.com
//
// CollSoft is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software Foundation. 
// You must retain a copy of this licence in all copies. 
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE. See the GNU General Public License for more details.
// ---------------------------------------------------------------------------------

//This library is obtained simplifying the libmodbus v.3.0.6 by Stephane Raimbault
//See http://libmodbus.org for information.
//In particular I have converted the library from dynamic link to static link.
//N.B. My version does not not implement all the function contained in the Stephane Raimbault version.
//in particular you have to modify the timout manually. I have moved its definition in DefineGeneral.h.
//So, if you want to change it, you have to modify the definitions in DefineGeneral.h. Than, you have to
//recompile the program with "make clean" and "make"

//In DefineGeneral.h!!!!
//~ #define _RESPONSE_TIMEOUT    500000
//~ 
//~ #define _BYTE_TIMEOUT        500000
//In DefineGeneral.h!!!!

//So, tested functions are:
//modbus_new_rtu
//modbus_free
//modbus_strerror
//modbus_set_slave
//modbus_read_registers
//modbus_write_registers
//modbus_flush
//And modbus_t data type.
//See libmodbus v.3.0.6 for the their documentation.

#ifndef __BASICMODBUSLIBRARYH
#define __BASICMODBUSLIBRARYH

#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
//#include <pthread.h>
//// modified C++
#include <iostream>
#include <thread>
#include <assert.h>

#include <time.h>
#include <unistd.h>

#define MODBUS_MAX_WRITE_REGISTERS         123

/* Function codes */
#define _FC_READ_COILS                0x01
#define _FC_READ_DISCRETE_INPUTS      0x02
#define _FC_READ_HOLDING_REGISTERS    0x03
#define _FC_READ_INPUT_REGISTERS      0x04
#define _FC_WRITE_SINGLE_COIL         0x05
#define _FC_WRITE_SINGLE_REGISTER     0x06
#define _FC_READ_EXCEPTION_STATUS     0x07
#define _FC_WRITE_MULTIPLE_COILS      0x0F
#define _FC_WRITE_MULTIPLE_REGISTERS  0x10
#define _FC_REPORT_SLAVE_ID           0x11
#define _FC_WRITE_AND_READ_REGISTERS  0x17

#define MSG_LENGTH_UNDEFINED -1

//Moved into DefineGeneral.h!!!!
//~ #define _RESPONSE_TIMEOUT    500000
//~ 
//~ #define _BYTE_TIMEOUT        500000
//Moved into DefineGeneral.h!!!!

#define _MODBUS_RTU_HEADER_LENGTH      1

#define _MODBUS_RTU_CHECKSUM_LENGTH    2

#define MODBUS_RTU_MAX_ADU_LENGTH  256

#define _MODBUS_RTU_PRESET_REQ_LENGTH  6
#define _MODBUS_RTU_PRESET_RSP_LENGTH  2


#define _MIN_REQ_LENGTH 12
#define MAX_MESSAGE_LENGTH 260
#define MODBUS_MAX_READ_REGISTERS          125
#define _FC_READ_HOLDING_REGISTERS    0x03

enum {
    MODBUS_EXCEPTION_ILLEGAL_FUNCTION = 0x01,
    MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS,
    MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE,
    MODBUS_EXCEPTION_SLAVE_OR_SERVER_FAILURE,
    MODBUS_EXCEPTION_ACKNOWLEDGE,
    MODBUS_EXCEPTION_SLAVE_OR_SERVER_BUSY,
    MODBUS_EXCEPTION_NEGATIVE_ACKNOWLEDGE,
    MODBUS_EXCEPTION_MEMORY_PARITY,
    MODBUS_EXCEPTION_NOT_DEFINED,
    MODBUS_EXCEPTION_GATEWAY_PATH,
    MODBUS_EXCEPTION_GATEWAY_TARGET,
    MODBUS_EXCEPTION_MAX
};
#define MODBUS_ENOBASE 112345678
//#define EMBXGTAR   (MODBUS_ENOBASE + MODBUS_EXCEPTION_GATEWAY_TARGET)
//#define EMBBADCRC  (EMBXGTAR + 1)

#define EMBXILFUN  (MODBUS_ENOBASE + MODBUS_EXCEPTION_ILLEGAL_FUNCTION)
#define EMBXILADD  (MODBUS_ENOBASE + MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS)
#define EMBXILVAL  (MODBUS_ENOBASE + MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE)
#define EMBXSFAIL  (MODBUS_ENOBASE + MODBUS_EXCEPTION_SLAVE_OR_SERVER_FAILURE)
#define EMBXACK    (MODBUS_ENOBASE + MODBUS_EXCEPTION_ACKNOWLEDGE)
#define EMBXSBUSY  (MODBUS_ENOBASE + MODBUS_EXCEPTION_SLAVE_OR_SERVER_BUSY)
#define EMBXNACK   (MODBUS_ENOBASE + MODBUS_EXCEPTION_NEGATIVE_ACKNOWLEDGE)
#define EMBXMEMPAR (MODBUS_ENOBASE + MODBUS_EXCEPTION_MEMORY_PARITY)
#define EMBXGPATH  (MODBUS_ENOBASE + MODBUS_EXCEPTION_GATEWAY_PATH)
#define EMBXGTAR   (MODBUS_ENOBASE + MODBUS_EXCEPTION_GATEWAY_TARGET)

/* Native libmodbus error codes */
#define EMBBADCRC  (EMBXGTAR + 1)
#define EMBBADDATA (EMBXGTAR + 2)
#define EMBBADEXC  (EMBXGTAR + 3)
#define EMBUNKEXC  (EMBXGTAR + 4)
#define EMBMDATA   (EMBXGTAR + 5)

#define MODBUS_BROADCAST_ADDRESS    0

//6: FALSE MODBUS_ERROR_RECOVERY_NONE _RESPONSE_TIMEOUT _BYTE_TIMEOUT
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* 3 steps are used to parse the query */
typedef enum {
    _STEP_FUNCTION,
    _STEP_META,
    _STEP_DATA
} _step_t;

typedef enum {
    /* Request message on the server side */
    MSG_INDICATION,
    /* Request message on the client side */
    MSG_CONFIRMATION
} msg_type_t;

typedef enum
{
    MODBUS_ERROR_RECOVERY_NONE          = 0,
    MODBUS_ERROR_RECOVERY_LINK          = (1<<1),
    MODBUS_ERROR_RECOVERY_PROTOCOL      = (1<<2),
} modbus_error_recovery_mode;

typedef enum {
    _MODBUS_BACKEND_TYPE_RTU=0,
    _MODBUS_BACKEND_TYPE_TCP
} modbus_bakend_type_t;

typedef struct _sft {
    int slave;
    int function;
    int t_id;
} sft_t;

typedef struct _modbus_rtu {
    /* Device: "/dev/ttyS0", "/dev/ttyUSB0" or "/dev/tty.USA19*" on Mac OS X for
       KeySpan USB<->Serial adapters this string had to be made bigger on OS X
       as the directory+file name was bigger than 19 bytes. Making it 67 bytes
       for now, but OS X does support 256 byte file names. May become a problem
       in the future. */
#if defined(__APPLE_CC__)
    char device[64];
#else
    char device[16];
#endif
    /* Bauds: 9600, 19200, 57600, 115200, etc */
    int baud;
    /* Data bit */
    uint8_t data_bit;
    /* Stop bit */
    uint8_t stop_bit;
    /* Parity: 'N', 'O', 'E' */
    char parity;
#if defined(_WIN32)
    struct win32_ser w_ser;
    DCB old_dcb;
#else
    /* Save old termios settings */
    struct termios old_tios;
#endif
#if HAVE_DECL_TIOCSRS485
    int serial_mode;
#endif
} modbus_rtu_t;

typedef struct _modbus modbus_t;

typedef struct _modbus_backend {
    unsigned int backend_type;
    unsigned int header_length;
    unsigned int checksum_length;
    unsigned int max_adu_length;
    int (*set_slave) (modbus_t *ctx, int slave);
    int (*build_request_basis) (modbus_t *ctx, int function, int addr,
                                int nb, uint8_t *req);
    int (*build_response_basis) (sft_t *sft, uint8_t *rsp);
    int (*prepare_response_tid) (const uint8_t *req, int *req_length);
    int (*send_msg_pre) (uint8_t *req, int req_length);
    ssize_t (*send) (modbus_t *ctx, const uint8_t *req, int req_length);
    ssize_t (*recv) (modbus_t *ctx, uint8_t *rsp, int rsp_length);
    int (*check_integrity) (modbus_t *ctx, uint8_t *msg,
                            const int msg_length);
    int (*pre_check_confirmation) (modbus_t *ctx, const uint8_t *req,
                                   const uint8_t *rsp, int rsp_length);
    int (*connect) (modbus_t *ctx);
    void (*close) (modbus_t *ctx);
    int (*flush) (modbus_t *ctx);
    int (*select) (modbus_t *ctx, fd_set *rfds, struct timeval *tv, int msg_length);
    int (*filter_request) (modbus_t *ctx, int slave);
} modbus_backend_t;

struct _modbus {
    /* Slave address */
    int slave;
    /* Socket or file descriptor */
    int s;
    int debug;
    int error_recovery;
    struct timeval response_timeout;
    struct timeval byte_timeout;
    const modbus_backend_t *backend;
    void *backend_data;
};

void modbus_close(modbus_t *ctx);

void modbus_free(modbus_t *ctx);

//5:_modbus_init_common
void _modbus_init_common(modbus_t *ctx);

static int _modbus_set_slave(modbus_t *ctx, int slave);

static int _modbus_rtu_build_request_basis(modbus_t *ctx, int function,
                                           int addr, int nb,
                                           uint8_t *req);
                                           
static int _modbus_rtu_build_response_basis(sft_t *sft, uint8_t *rsp);

static uint16_t crc16(uint8_t *buffer, uint16_t buffer_length);

int _modbus_rtu_prepare_response_tid(const uint8_t *req, int *req_length);

int _modbus_rtu_send_msg_pre(uint8_t *req, int req_length);                                           

ssize_t _modbus_rtu_send(modbus_t *ctx, const uint8_t *req, int req_length);

ssize_t _modbus_rtu_recv(modbus_t *ctx, uint8_t *rsp, int rsp_length);

int _modbus_rtu_flush(modbus_t *ctx);


int _modbus_rtu_check_integrity(modbus_t *ctx, uint8_t *msg,
                                const int msg_length);

static int _modbus_rtu_connect(modbus_t *ctx);

void _modbus_rtu_close(modbus_t *ctx);

int _modbus_rtu_select(modbus_t *ctx, fd_set *rfds,
                       struct timeval *tv, int length_to_read);

int _modbus_rtu_filter_request(modbus_t *ctx, int slave);

size_t strlcpy(char *dest, const char *src, size_t dest_size);

int modbus_set_slave(modbus_t *ctx, int slave);

const char *modbus_strerror(int errnum);

int modbus_connect(modbus_t *ctx);

//const modbus_backend_t _modbus_rtu_backend;

modbus_t* modbus_new_rtu(const char *device, int baud, char parity,
                         int data_bit, int stop_bit);

int modbus_flush(modbus_t *ctx);

int _sleep_and_flush(modbus_t *ctx);

void _error_print(modbus_t *ctx, const char *context);                         

static int send_msg(modbus_t *ctx, uint8_t *msg, int msg_length);

static uint8_t compute_meta_length_after_function(int function,
                                                  msg_type_t msg_type);

static int compute_data_length_after_meta(modbus_t *ctx, uint8_t *msg,
                                          msg_type_t msg_type);
                                          
static int receive_msg(modbus_t *ctx, uint8_t *msg, msg_type_t msg_type);

static unsigned int compute_response_length_from_request(modbus_t *ctx, uint8_t *req);

static int check_confirmation(modbus_t *ctx, uint8_t *req,
                              uint8_t *rsp, int rsp_length);
                              
static int read_registers(modbus_t *ctx, int function, int addr, int nb,
                          uint16_t *dest);
                          
int modbus_read_registers(modbus_t *ctx, int addr, int nb, uint16_t *dest);                                                        

int modbus_write_registers(modbus_t *ctx, int addr, int nb, const uint16_t *src);

#endif
