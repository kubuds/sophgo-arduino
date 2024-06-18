#include "mailbox.h"

struct valid_t {
  uint8_t linux_valid;
  uint8_t rtos_valid;
} __attribute__((packed));

typedef union resv_t {
  struct valid_t valid;
  unsigned short mstime;  // 0 : noblock, -1 : block infinite
} resv_t;

typedef struct cmdqu_t cmdqu_t;
/* cmdqu size should be 8 bytes because of mailbox buffer size */
struct cmdqu_t {
  uint8_t ip_id;
  uint8_t cmd_id : 7;
  uint8_t block : 1;
  union resv_t resv;
  unsigned int param_ptr;
} __attribute__((packed)) __attribute__((aligned(0x8)));

void showmsg(MailboxMsg msg) {
  cmdqu_t *cmdq;
  Serial2.print("Get Msg: ");
  Serial2.println(*(msg.data), HEX);
  cmdq = (cmdqu_t *)msg.data;
  Serial2.printf("cmdq->ip_id = %d\r\n", cmdq->ip_id);
  Serial2.printf("cmdq->cmd_id = %x\r\n", cmdq->cmd_id);
  Serial2.printf("cmdq->block = %d\r\n", cmdq->block);
  Serial2.printf("cmdq->para_ptr = %x\r\n", cmdq->param_ptr);
  *(msg.data) = 0;
}

void setup() {
  // put your setup code here, to run once:
  Serial2.begin(115200);
  mailbox_init(false);
  mailbox_register(0, showmsg);
  mailbox_enable_receive(0);
  Serial2.println("Mailbox Start");
}

void loop() {
  // put your main code here, to run repeatedly:
}

