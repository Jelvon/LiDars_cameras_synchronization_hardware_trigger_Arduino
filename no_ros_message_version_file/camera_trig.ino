#define READY_PIN 17
#define TRIG_GAP 200
#define TRIG_WIDTH 10
#define MAX_TRIG_PER_SEC 5

#define CAM_TRIG_0 7
#define CAM_TRIG_1 6
#define CAM_TRIG_2 5
#define CAM_TRIG_3 4

#define CAM_flash_0 8
bool pps_last;
bool pps_now;
unsigned long timeLast = 0;
unsigned long timeNow = 0;
unsigned long trigTimer = 0;
unsigned int trigCounter = 0;
bool trigIsOn = false;
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(CAM_TRIG_0, OUTPUT);
  pinMode(CAM_TRIG_1, OUTPUT);
  pinMode(CAM_TRIG_2, OUTPUT);
  pinMode(CAM_TRIG_3, OUTPUT);
  pinMode(READY_PIN, INPUT_PULLUP);

  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(CAM_TRIG_0, LOW);
  digitalWrite(CAM_TRIG_1, LOW);
  digitalWrite(CAM_TRIG_2, LOW);
  digitalWrite(CAM_TRIG_3, LOW);

  trigIsOn = LOW;
}

// the loop function runs over and over again forever
void loop() {

  pps_now = digitalRead(READY_PIN);
  if ((pps_now == HIGH) && (pps_last == LOW))
  {
    timeNow = millis();
    trigTimer = 0;
    timeLast = timeNow;
    trigIsOn = HIGH;
    trigCounter = 1;
  }

  timeNow = millis();
  if (timeNow - timeLast >TRIG_GAP)
  {
    timeLast = timeNow;
    trigTimer = 0;
    trigCounter++;
    if (trigCounter <= MAX_TRIG_PER_SEC)
      trigIsOn = HIGH;
  }

  if (trigIsOn == HIGH)
  {
    trigTimer++;
    if (trigTimer >= TRIG_WIDTH)
        trigIsOn = LOW;
  }

  digitalWrite(LED_BUILTIN, trigIsOn);
  digitalWrite(CAM_TRIG_0, trigIsOn);
  digitalWrite(CAM_TRIG_1, trigIsOn);
  digitalWrite(CAM_TRIG_2, trigIsOn);
  digitalWrite(CAM_TRIG_3, trigIsOn);
//  if (pps_now == HIGH)
//  {
//    digitalWrite(LED_BUILTIN, HIGH);
//  }
//  else
//  {
//    digitalWrite(LED_BUILTIN, LOW);
//  }

  pps_last = pps_now;

}
