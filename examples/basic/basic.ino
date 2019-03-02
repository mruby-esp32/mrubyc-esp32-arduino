#include <mrubyc_for_ESP32_Arduino.h>

extern const uint8_t code[];

#define MEMSIZE (1024*30)
static uint8_t mempool[MEMSIZE];

void setup() {
  delay(100);
  mrbc_init(mempool, MEMSIZE);
  mrbc_define_user_class();
  if(NULL == mrbc_create_task( code, 0 )){
    Serial.println("mrbc_create_task error");
    return;
  }
  mrbc_run();
}

void loop() {
  delay(1000);
}
