#ifndef __TASK_H__
#define __TASK_H__

enum
{
    FIRMWARE_OFF = 0,
    FIRMWARE_ON,
};

#define FIRMWARE_DIR "/lib/firmware"
#define FIRMWARE_NAME "arduino.elf"
#define FIRMWARE_PATH "/lib/firmware/arduino.elf"
#define REC_FIRMWARE_PATH "/lib/firmware/arduino_old.elf"
#define SYSFS_FIRMWARE "/sys/class/remoteproc/remoteproc0/firmware"
#define SYSFS_STATE "/sys/class/remoteproc/remoteproc0/state"

extern int echo_firmware();
extern int ctrl_firmware(int flag);
extern int run_task(enum TASK_CMD cmd, uint32_t param);

#endif