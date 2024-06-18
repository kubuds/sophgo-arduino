#!/usr/bin/env python
# -*- coding:utf-8 -*-

__version__ = "0.3"

import threading
import time
import sys
import os
import argparse
import serial
import zlib
import queue

MAX_PACKET_SIZE = 64

NONE_CMD = "AA 55 00 00 00 00 00 00"
CONNECT_CMD = "AA 55 01 00 00 00 00 00"
STOP_CMD = "AA 55 02 00 00 00 00 00"
START_CMD = "AA 55 03 00 00 00 00 00"
UPDATE_CMD = "AA 55 04 00 00 00 00 00"
DATAEND_CMD = "00 00 00 00 FF FF FF FF"

ser = None
firmware = None     
            
data_event = threading.Event()
recv_event = threading.Event()

recv_stop = True

def recv_date():
    global ser
    global recv_event
    global data_event
    while not recv_event.is_set():
        while ser.inWaiting() > 0:
            raw_data=ser.read(8)
            relydata = []
            if len(raw_data) > 0:
                for data in raw_data:
                    relydata.append(data)
                #print(relydata, flush=True)
            if relydata[4] == 0:
                data_event.set()
            else:
                data_event.clear()

def send_data(arg, result_q):
    global ser
    try:
        total = 0
        chunk_size = 64
        with open(arg, 'rb') as file:
            data = file.read()
            chunk_size = int(len(data)/10)
            for i in range(0, len(data), chunk_size):
                chunk = data[i:i+chunk_size]
                count = ser.write(chunk)
                total = total + count
                print('send data... {:.2%}'.format(total/len(data)), flush=True)
                time.sleep(0.5)
        result_q.put(True)
    except Exception as e:
        print("error:", e)

def get_file_crc32(fileName):
    crc32 = 0
    for line in open(fileName,  "rb"):
        crc32 = zlib.crc32(line, crc32)
    return "%X"%(crc32 & 0xFFFFFFFF)

def get_firmware_crc32():
    global firmware
    global DATAEND_CMD
    if firmware != None:
        hex_num = get_file_crc32(firmware)
        hex_num = hex_num.replace('0x','').zfill(8)
        result = [hex_num[i:i+2] for i in range(0, len(hex_num), 2)]
        update_list = DATAEND_CMD.split(" ")
        update_list[0] = result[3]
        update_list[1] = result[2]
        update_list[2] = result[1]
        update_list[3] = result[0]
        DATAEND_CMD=' '.join(update_list)
        return DATAEND_CMD
    return 0

def get_firmware_size():
    global firmware
    global UPDATE_CMD
    if firmware != None: 
        filesizes=os.path.getsize(firmware)
        hex_num = hex(filesizes)
        hex_num = hex_num.replace('0x','').zfill(8)
        result = [hex_num[i:i+2] for i in range(0, len(hex_num), 2)]
        update_list = UPDATE_CMD.split(" ")
        update_list[4] = result[3]
        update_list[5] = result[2]
        update_list[6] = result[1]
        update_list[7] = result[0]
        UPDATE_CMD=' '.join(update_list)
        return UPDATE_CMD
    return 0


def send_cmd(arg1,result_q):
    global ser
    global data_event
    global UPDATE_CMD
    global DATAEND_CMD
    try:
        if arg1 == UPDATE_CMD:
            arg1 = get_firmware_size()
        if arg1 == DATAEND_CMD:
            arg1 = get_firmware_crc32()
        SEND_CMDS = bytes.fromhex(arg1)
        count = ser.write(SEND_CMDS)
        data_event.wait(timeout = 5)
        if data_event.is_set():
            result_q.put(True)
        else:
            result_q.put(False)
    except Exception as e:
        print("error:", e)

def sendbin(port,initial_baud=115200):
    try:
        if ser.is_open:
            result_q = queue.Queue()
            print("\nstart upload...", flush=True)
            global data_event
            global NONE_CMD
            print("\ncommunication and testing...", flush=True)
            data_event.clear()
            send_thread=threading.Thread(target=send_cmd, args=(NONE_CMD,result_q))
            send_thread.start()
            send_thread.join()
            if not result_q.get():
                return False
            global CONNECT_CMD
            print("\nconnecting...", flush=True)
            data_event.clear()
            send_thread=threading.Thread(target=send_cmd, args=(CONNECT_CMD,result_q))
            send_thread.start()
            send_thread.join()
            if not result_q.get():
                print("\nconnection failed...", flush=True)
                return False
            else:
                print("\nconnection successful...", flush=True)
            global STOP_CMD
            print("stop firmware...", flush=True)
            data_event.clear()
            send_thread=threading.Thread(target=send_cmd, args=(STOP_CMD,result_q))
            send_thread.start()
            send_thread.join()
            global UPDATE_CMD
            print("\nready to update firmware...", flush=True)
            data_event.clear()
            send_thread=threading.Thread(target=send_cmd, args=(UPDATE_CMD,result_q))
            send_thread.start()
            send_thread.join()
            global firmware
            print("start to update firmware...", flush=True)
            send_thread=threading.Thread(target=send_data, args=(firmware,result_q))
            send_thread.start()
            send_thread.join()
            global DATAEND_CMD
            data_event.clear()
            send_thread=threading.Thread(target=send_cmd, args=(DATAEND_CMD,result_q))
            send_thread.start()
            send_thread.join()
            print("update firmware done...", flush=True)
            global START_CMD
            data_event.clear()
            print("\nrestart firmware...", flush=True)
            send_thread=threading.Thread(target=send_cmd, args=(START_CMD,result_q))
            send_thread.start()
            send_thread.join()
            print("\nupload and runing success...", flush=True)
            return True
    except Exception as e:
        print("error:", e)

def start(argv=None):
    """
    Main function for burntool

    """
    parser = argparse.ArgumentParser(
        description="burntool.py v%s -  Burn Firmware Tool"
        % __version__,
        prog="burntool",
    )

    parser.add_argument(
        "--chip",
        "-c",
        help="Target chip type",
        type=str,
        default=os.environ.get("BURNTOOL_CHIP", "auto"),
    )

    parser.add_argument(
        "--port",
        "-p",
        help="Serial port device",
        type=str,
        default=os.environ.get("BURNTOOL_PORT", "/dev/ttyACM0"),
    )

    parser.add_argument(
        "--baud",
        "-b",
        help="Serial port baud rate used when flashing/reading",
        type=int,
        default=os.environ.get("BURNTOOL_BAUD", 9600),
    )

    parser.add_argument(
        "--firmware",
        "-f",
        help="Firmware Files",
        type=str,
        default=os.environ.get("BURNTOOL_FIRMWARE", None),
    )

    args = parser.parse_args()

    print('\nburntool.py v%s' % __version__, flush=True)

    global firmware
    firmware = args.firmware

    print('\nupload firmware %s' % firmware, flush=True)

    global ser
    try:
        ser=serial.Serial(args.port, args.baud, timeout=3)
        if ser is None:
            raise Exception("serial null!")
    except Exception as e:
        print("serial err:", str(e))

    recv_thread=threading.Thread(target=recv_date)
    recv_thread.start()

    result = sendbin(args.port, args.baud)

    if not result:
        print("connect failed...", flush=True)

    recv_event.set()
    recv_thread.join()
    ser.close()

if __name__ == '__main__':
    start(sys.argv[1:])
    sys.exit(0)
