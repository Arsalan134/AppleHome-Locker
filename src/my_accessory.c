/*
 * my_accessory.c
 * Define the accessory in C language using the Macro in characteristics.h
 *
 *  Created on: 2020-05-15
 *      Author: Mixiaoxiao (Wang Bin)
 */

#include <homekit/characteristics.h>
#include <homekit/homekit.h>

typedef enum {
  lock_state_unsecured = 0,
  lock_state_secured = 1,
  lock_state_jammed = 2,
  lock_state_unknown = 3,
} lock_state_t;

homekit_characteristic_t name = HOMEKIT_CHARACTERISTIC_(NAME, "Lock");

homekit_characteristic_t lock_current_state =
    HOMEKIT_CHARACTERISTIC_(LOCK_CURRENT_STATE, lock_state_secured);

void lock_target_state_setter(homekit_value_t value);

homekit_characteristic_t lock_target_state = HOMEKIT_CHARACTERISTIC_(
    LOCK_TARGET_STATE, lock_state_secured, .setter = lock_target_state_setter);

void lock_identify(homekit_value_t _value) {
  printf("Lock identify\n");
  //   xTaskCreate(lock_identify_task, "Lock identify", 128, NULL, 2, NULL);
}

void lock_lock() {
  // relay_write(!relay_open_signal);

  lock_current_state.value = HOMEKIT_UINT8(lock_state_secured);
  homekit_characteristic_notify(&lock_current_state, lock_current_state.value);
}

void lock_unlock() {
  //   relay_write(relay_open_signal);

  lock_current_state.value = HOMEKIT_UINT8(lock_state_unsecured);
  homekit_characteristic_notify(&lock_current_state, lock_current_state.value);
}

void lock_target_state_setter(homekit_value_t value) {

  lock_target_state.value = value;

  if (value.int_value == 0) {
    lock_unlock();
  } else {
    lock_lock();
  }
}

void lock_control_point(homekit_value_t value) {
  printf("lock_control_point\n");
}

homekit_accessory_t *accessories[] = {
    HOMEKIT_ACCESSORY(
            .id = 1, .category = homekit_accessory_category_door_lock,
            .services =
                (homekit_service_t *[]){
                    HOMEKIT_SERVICE(
                        ACCESSORY_INFORMATION,
                        .characteristics =
                            (homekit_characteristic_t *[]){
                                &name,
                                HOMEKIT_CHARACTERISTIC(MANUFACTURER, "Arsalan"),
                                HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "1"),
                                HOMEKIT_CHARACTERISTIC(MODEL, "Basic"),
                                HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION,
                                                       "1.0"),
                                HOMEKIT_CHARACTERISTIC(IDENTIFY, lock_identify),
                                NULL}),
                    HOMEKIT_SERVICE(
                        LOCK_MECHANISM, .primary = true,
                        .characteristics =
                            (homekit_characteristic_t *[]){
                                HOMEKIT_CHARACTERISTIC(NAME, "Lock"),
                                &lock_current_state, &lock_target_state, NULL}),
                    HOMEKIT_SERVICE(
                        LOCK_MANAGEMENT,
                        .characteristics =
                            (homekit_characteristic_t *[]){
                                HOMEKIT_CHARACTERISTIC(
                                    LOCK_CONTROL_POINT,
                                    .setter = lock_control_point),
                                HOMEKIT_CHARACTERISTIC(VERSION, "1"), NULL}),
                    NULL}),
    NULL};

homekit_server_config_t config = {.accessories = accessories,
                                  .password = "111-11-111"};
