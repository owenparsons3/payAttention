#define LED_BUILTIN 13

volatile unsigned int counter = 0; // Use volatile for variables accessed within the ISR
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
volatile unsigned long beatTimes[21]; // Array to store the timestamps of the last 21 beats
volatile int beatIndex = 0; // Index to keep track of the current position in the beatTimes array
volatile bool readyToCalculate = false; // Flag to indicate when we have enough data to calculate the heart rate

const int GSR=A0;
int sensorValue=0;
int gsr_average=0;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(2), blinkLED, RISING); // Use digitalPinToInterrupt to ensure compatibility
    Serial.begin(9600);
}

void loop() {
  // GSR block
  // Connecting device: black->GND, red->5V, yellow->A0
  long sum=0;
  for(int i=0;i<10;i++)           //Average the 10 measurements to remove the glitch
      {
      sensorValue=analogRead(GSR);
      sum += sensorValue;
      delay(5);
      }
   gsr_average = sum/10;
   Serial.println(gsr_average);

    if (readyToCalculate) {
        noInterrupts(); // Temporarily disable interrupts to safely access shared variables

        // Calculate the total interval by summing the differences between successive timestamps
        unsigned long totalInterval = 0;
        for(int i = 0; i < 20; i++) { // There are 20 intervals between 21 timestamps
            int currentIndex = (beatIndex + i) % 21;
            int nextIndex = (beatIndex + i + 1) % 21;
            if (beatTimes[nextIndex] > beatTimes[currentIndex]) { // Ensure no overflow issue
                totalInterval += (beatTimes[nextIndex] - beatTimes[currentIndex]);
            }
        }

        interrupts(); // Re-enable interrupts

        if (totalInterval > 0) { // Check to avoid division by zero
            unsigned long averageInterval = totalInterval / 20; // Average interval between beats
            unsigned int heartRate = (60000 / averageInterval); // Calculate BPM
            Serial.print("Heart Rate: ");
            Serial.print(heartRate);
            Serial.println(" BPM");
        }

        readyToCalculate = false; // Reset the flag until the next calculation
    }
}


// Interrupt Service Routine (ISR)
void blinkLED() {
    unsigned long currentMillis = millis();
    // Check if enough time has passed since the last interrupt
    if ((millis() - lastDebounceTime) > debounceDelay) {
        counter++; // Increment the counter
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // Toggle the state of the built-in LED
        lastDebounceTime = millis(); // Reset the debounce timer
        // Store the current timestamp in the beatTimes array and update the index
        beatTimes[beatIndex] = currentMillis;
        beatIndex = (beatIndex + 1) % 21; // Ensure the index wraps around

        // Set the flag to calculate heart rate after the first 21 beats
        if (!readyToCalculate && beatTimes[20] > 0) {
          readyToCalculate = true;
        }

    }
}
