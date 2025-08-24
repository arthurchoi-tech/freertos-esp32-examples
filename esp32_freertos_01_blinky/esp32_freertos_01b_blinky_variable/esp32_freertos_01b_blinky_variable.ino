// Add ESP32 Arduino core URL to File > Preferences > Additional boards manager URLs:
//   https://dl.espressif.com/dl/package_esp32_index.json
// Then go to Tools > Board > Boards Manager, search for "esp32" and install the latest package
// (e.g., 3.3.0 as of Aug 2025).

// Select which core to run the task on.
// - On single-core builds, only core 0 exists (CONFIG_FREERTOS_UNICORE = 1).
// - On dual-core ESP32 (e.g., Adafruit HUZZAH32), both cores exist and by default
//   the firmware runs in dual-core mode, unless explicitly configured for unicore.
// For this demo, we pin the task to core 1 to avoid interference with Wi-Fi/system tasks.
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Goal: Toggles the LED at different rates using separate tasks.

// LED rates
static const int rate_1 = 500;  // ms
static const int rate_2 = 160;  // ms
static const int rate_3 = 375;  // ms

// Pins
static const int led_pin = LED_BUILTIN; // Built-in LED 

// Task: blink an LED at rate_1
// Note: vTaskDelay() expects ticks, not ms. Use portTICK_PERIOD_MS to convert ms to ticks.
// On ESP32 (Arduino-ESP32), portTICK_PERIOD_MS = 1 (configTICK_RATE_HZ = 1000),
// meaning 1 tick = 1 ms. On other FreeRTOS ports, it may differ (e.g., 1 tick = 10 ms).
void toggleLED_1(void *parameter) {
  while(1) {
    digitalWrite(led_pin, HIGH);
    vTaskDelay(rate_1 / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(rate_1 / portTICK_PERIOD_MS);
  }
}

// Task: blink an LED at rate_2
void toggleLED_2(void *parameter) {
  while(1) {
    digitalWrite(led_pin, HIGH);
    vTaskDelay(rate_2 / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(rate_2 / portTICK_PERIOD_MS);
  }
}

// Task: blink an LED at rate_3
void toggleLED_3(void *parameter) {
  while(1) {
    digitalWrite(led_pin, HIGH);
    vTaskDelay(rate_3 / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(rate_3 / portTICK_PERIOD_MS);
  }
}

void setup() {
  // Configure pin
  pinMode(led_pin, OUTPUT);

  // Create task 1 with rate_1 toggling
  xTaskCreatePinnedToCore(   // In vanilla FreeRTOS, use xTaskCreate()
      toggleLED_1,           // Task function
      "Toggle 1",            // Name (for debugging)
      1024,                  // Stack size in bytes (Arduino-ESP32).
                             //   Vanilla FreeRTOS uses words (so 1024 = 4096 bytes on 32-bit MCU).
      NULL,                  // Parameter to pass
      1,                     // Priority (0–24, higher = higher priority)
      NULL,                  // Task handle (optional)
      app_cpu                // Core affinity (ESP32 only)
  );

  // Create task 2 with rate_2 toggling
  xTaskCreatePinnedToCore(   // In vanilla FreeRTOS, use xTaskCreate()
      toggleLED_2,           // Task function
      "Toggle 2",            // Name (for debugging)
      1024,                  // Stack size in bytes (Arduino-ESP32).
                             //   Vanilla FreeRTOS uses words (so 1024 = 4096 bytes on 32-bit MCU).
      NULL,                  // Parameter to pass
      1,                     // Priority (0–24, higher = higher priority)
      NULL,                  // Task handle (optional)
      app_cpu                // Core affinity (ESP32 only)
  );

  // Create task 3 with rate_3 toggling
  xTaskCreatePinnedToCore(   // In vanilla FreeRTOS, use xTaskCreate()
      toggleLED_3,           // Task function
      "Toggle 3",            // Name (for debugging)
      1024,                  // Stack size in bytes (Arduino-ESP32).
                             //   Vanilla FreeRTOS uses words (so 1024 = 4096 bytes on 32-bit MCU).
      NULL,                  // Parameter to pass
      1,                     // Priority (0–24, higher = higher priority)
      NULL,                  // Task handle (optional)
      app_cpu                // Core affinity (ESP32 only)
  );
  // The LED toggles at three different rates, creating odd patterns,
  // because each task calls vTaskDelay(), which blocks it and allows
  // the scheduler to switch (preempt) to another task.


  // Notes:
  // - In Arduino-ESP32, setup() and loop() already run as FreeRTOS tasks,
  //   so no need to call vTaskStartScheduler().
  // - In vanilla FreeRTOS, tasks are created in main() and you must call vTaskStartScheduler().
}

void loop() {
  // Empty: not used in this demo
}
