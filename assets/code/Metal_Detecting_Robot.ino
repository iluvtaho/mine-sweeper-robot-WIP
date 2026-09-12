const int motorR1 = 12;
const int motorR2 = 14;
const int motorL2 = 27;
const int motorL1 = 26;
const int motorLS = 25;
const int motorRS = 33;
const int metaldetect = 5;
const int irobstacleR = 4;
const int irobstacleL = 2;
const int buzzer = 13;

unsigned long lastd = 0;    // last direction change
const long interval = 600;  // # of milliseconds between switches
bool direction = false;     // false = turn left, true = turn right
unsigned long lastm = 0;    // last metal detection
const long metalt = 1500;   // # of milliseconds before metal can be detected again
bool metal = false;         // debounce

void MovementLoop() {
  digitalWrite(motorL1, HIGH);
  digitalWrite(motorL2, LOW);
  digitalWrite(motorR1, LOW);
  digitalWrite(motorR2, HIGH);

  unsigned long currentd = millis();  
  
  if (currentd - lastd >= interval) {
    lastd = currentd;        
    direction = !direction;  
  }
  
  if (direction == true) {
    analogWrite(motorLS, 120);  //turn right
    analogWrite(motorRS, 0);
  } else {
    analogWrite(motorLS, 0);
    analogWrite(motorRS, 150);  //turn right
  }
}

void AvoidObstacleR() {
  digitalWrite(motorL1, LOW);
  digitalWrite(motorL2, HIGH);
  digitalWrite(motorR1, HIGH);
  digitalWrite(motorR2, LOW);
  analogWrite(motorRS, 120);
  analogWrite(motorLS, 150);
  delay(600);

  digitalWrite(motorL1, LOW);
  digitalWrite(motorL2, HIGH);
  digitalWrite(motorR1, LOW);
  digitalWrite(motorR2, HIGH);
  analogWrite(motorRS, 120);
  analogWrite(motorLS, 150);
  delay(300);
  direction = false; //turn left
  lastd = millis(); 
}

void AvoidObstacleL() {
  digitalWrite(motorL1, LOW);
  digitalWrite(motorL2, HIGH);
  digitalWrite(motorR1, HIGH);
  digitalWrite(motorR2, LOW);
  analogWrite(motorRS, 120);
  analogWrite(motorLS, 150);
  delay(600);

  digitalWrite(motorL1, HIGH);
  digitalWrite(motorL2, LOW);
  digitalWrite(motorR1, HIGH);
  digitalWrite(motorR2, LOW);
  analogWrite(motorRS, 120);
  analogWrite(motorLS, 150);
  delay(300);
  direction = true; //turn right
  lastd = millis();
}

void MetalDetected() {
  if (metal == false) {
    metal = true;  
    
    analogWrite(motorLS, 0);
    analogWrite(motorRS, 0);

    analogWrite(buzzer, 10);
    delay(2000);
    analogWrite(buzzer, 0);
    delay(2500);

    digitalWrite(motorL1, HIGH);
    digitalWrite(motorL2, LOW);
    digitalWrite(motorR1, LOW);
    digitalWrite(motorR2, HIGH);  
    
    for (int i = 0; i < 3; i++) {
      analogWrite(motorLS, 120);
      analogWrite(motorRS, 0);
      delay(500); 
      
      analogWrite(motorLS, 0);
      analogWrite(motorRS, 150);
      delay(500);
    }
    
    analogWrite(motorLS, 0);
    analogWrite(motorRS, 0);
    
    lastm = millis();
  }
}

void MetalCountdown() {
  if (metal == true) {
    unsigned long currentm = millis();
    if ((currentm - lastm >= metalt) && (digitalRead(metaldetect) == HIGH)) { 
      metal = false; 
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(motorL1, OUTPUT);
  pinMode(motorL2, OUTPUT);
  pinMode(motorR1, OUTPUT);
  pinMode(motorR2, OUTPUT);
  pinMode(motorRS, OUTPUT);
  pinMode(motorLS, OUTPUT);
  pinMode(buzzer, OUTPUT);

  pinMode(metaldetect, INPUT_PULLUP);
  pinMode(irobstacleR, INPUT_PULLUP); 
  pinMode(irobstacleL, INPUT_PULLUP); 
  Serial.println("Ready1");
}

void loop() {
  MetalCountdown(); 

  int right = digitalRead(irobstacleR);
  int left = digitalRead(irobstacleL);
  int metald = digitalRead(metaldetect);

  if (metald == LOW && metal == false) {
    MetalDetected();
  } else if (left == LOW) {
    AvoidObstacleL();
  } else if (right == LOW) {
    AvoidObstacleR();
  } else {
    MovementLoop();
  }
}
