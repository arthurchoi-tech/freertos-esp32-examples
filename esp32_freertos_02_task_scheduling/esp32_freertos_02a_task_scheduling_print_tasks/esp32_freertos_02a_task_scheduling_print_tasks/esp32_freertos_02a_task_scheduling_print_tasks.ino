// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Message that Task 1 will print character by character
const char msg[] = "Das Leben ist ein ewiges Wünschen und Streben";

// Task handles (used to control/suspend/resume tasks after creation)
static TaskHandle_t task_1 = NULL; 
static TaskHandle_t task_2 = NULL;

// Task 1: prints a message to the Serial Monitor (lower priority)
void startTask1(void *parameter) {

	// Cache the length of the message string
	int msg_len = strlen (msg);
	
	// Print string to Terminal
	while (1) {
    // Print one character at a time to better observe scheduling,
    // instead of sending the whole string to the Serial buffer at once
		Serial.println();
		for (int i = 0; i < msg_len; i++) {
			Serial.print (msg[i]);  
		}
		Serial.println();

    // Delay 1 second between printing the full string 
		vTaskDelay(1000 / portTICK_PERIOD_MS);                                             
	}
  
}

// Task 2: prints '*' every 100 ms (higher priority)
void startTask2 (void *parameter) {
	while (1) {
		Serial.print ('*');
    // Delay 100 ms between each '*' character
		vTaskDelay(100 / portTICK_PERIOD_MS);  
	}
}

void setup() {

  // Configure Serial (slow baud rate so task preemption effects are easier to see)
  Serial.begin (300);

  // Wait a moment to start (so we don't miss Serial output)
  vTaskDelay (1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println ("_--FreeRTOS Task Demo---") ;

  // Print info about the main Arduino task
  Serial.print ("Setup and loop task running on core ") ;
  Serial.print (xPortGetCoreID());
  Serial.print (" with priority ");
  Serial.println (uxTaskPriorityGet(NULL));

  // Create Task 1 (lower priority, runs continuously)
  xTaskCreatePinnedToCore(startTask1,
                          "Task 1", 
                          1024, 
                          NULL,
                          1,
                          &task_1,
                          app_cpu);
                            
  // Create Task 2 (higher priority, runs continuously)
  xTaskCreatePinnedToCore(startTask2,
                          "Task 2", 
                          1024, 
                          NULL,
                          2,
                          &task_2,
                          app_cpu);

}

void loop() {

  // Temporarily suspend Task 2 (higher priority) at 2s intervals
  for (int i = 0; i < 3; i++) {
    vTaskSuspend (task_2) ;
    vTaskDelay (2000 / portTICK_PERIOD_MS) ;  // 2s
    vTaskResume (task_2) ;
    vTaskDelay (2000 / portTICK_PERIOD_MS) ;  // 2s
  }

  // Delete Task 1 safely. vTaskDelete(NULL) would crash, so check first.
  if (task_1 != NULL) {
    vTaskDelete(task_1) ;
    task_1 = NULL;
  }

}
