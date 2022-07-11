

#define echoPin3 4 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin3 5 //attach pin D3 Arduino to pin Trig of HC-SR04

#define echoPin1 12 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin1 13 //attach pin D3 Arduino to pin Trig of HC-SR04

#define echoPin2 8 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin2 9 //attach pin D3 Arduino to pin Trig of HC-SR04

#define buzzer 10 //bazzer
#define motor 11 //motor

// defines variables
long duration1; // variable for the duration1 of sound wave travel
int distance1; // variable for the distance1 measurement

long duration2; // variable for the duration1 of sound wave travel
int distance2; // variable for the distance1 measurement

long duration3; // variable for the duration1 of sound wave travel
int distance3; // variable for the distance1 measurement

void setup() {
  pinMode(trigPin1, OUTPUT); // Sets the trigPin1 as an OUTPUT
  pinMode(echoPin1, INPUT); // Sets the echoPin1 as an INPUT
    pinMode(trigPin2, OUTPUT); // Sets the trigPin1 as an OUTPUT
  pinMode(echoPin2, INPUT); // Sets the echoPin1 as an INPUT
    pinMode(trigPin3, OUTPUT); // Sets the trigPin1 as an OUTPUT
  pinMode(echoPin3, INPUT); // Sets the echoPin1 as an INPUT
  Serial.begin(9600); // // Serial Communication is starting with 9600 of baudrate speed
  Serial.println("ultra1sonic Sensor HC-SR04 Test"); // print some text in Serial Monitor
  pinMode(buzzer, OUTPUT); 
  pinMode(motor, OUTPUT); // Sets the trigPin1 as an OUTPUT

}
void loop() {
  
  ultra1 ();
  ultra2 ();
  ultra3 ();
  
  Serial.print("distance1: ");
  Serial.print(distance1);
  Serial.println(" cm");
  
  Serial.print("distance2: ");
  Serial.print(distance2);
  Serial.println(" cm");

  Serial.print("distance3: ");
  Serial.print(distance3);
  Serial.println(" cm");

  if (distance1 <= 80 && distance1 >= 1   ){
    vib1();
    digitalWrite(motor, HIGH);
    delay(500);
    
  }

    else if ( distance2 <= 60 && distance2 >= 1   ){
    vib2();
    digitalWrite(motor, HIGH);
    delay(500);
    
  }


   else if ( distance3 <= 40 && distance3 >= 1  ){
    vib3();
    digitalWrite(motor, HIGH);
    delay(500);
    
  }
  else{
    
    noTone(buzzer);     // Stop sound...
    digitalWrite(motor, LOW);
    delay(500);
    
  }
}
void ultra1 (){
  
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  // Sets the trigPin1 HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  // Reads the echoPin1, returns the sound wave travel time in microseconds
  duration1 = pulseIn(echoPin1, HIGH);
  // Calculating the distance1
  distance1 = duration1 * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)

}

void ultra2 (){
  
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  // Sets the trigPin1 HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  // Reads the echoPin1, returns the sound wave travel time in microseconds
  duration2 = pulseIn(echoPin2, HIGH);
  // Calculating the distance1
  distance2 = duration2 * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)

}
void ultra3 (){
  
  digitalWrite(trigPin3, LOW);
  delayMicroseconds(2);
  // Sets the trigPin1 HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin3, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin3, LOW);
  // Reads the echoPin1, returns the sound wave travel time in microseconds
  duration3 = pulseIn(echoPin3, HIGH);
  // Calculating the distance1
  distance3 = duration3 * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)

}

void vib1(){

  tone(buzzer, 1000); 
  delay(1000);        // ...for 1 sec
  noTone(buzzer);     // Stop sound...
  delay(1000);        // ...for 1sec
  
}


void vib2 (){

  tone(buzzer, 800); 
  delay(1000);        // ...for 1 sec
  noTone(buzzer);     // Stop sound...
  delay(1000);        // ...for 1sec
  
}


void vib3 (){

  tone(buzzer, 500); 
  delay(1000);        // ...for 1 sec
  noTone(buzzer);     // Stop sound...
  delay(1000);        // ...for 1sec
}


void vib4 (){

  tone(buzzer, 240); 
  delay(1000);        // ...for 1 sec
  noTone(buzzer);     // Stop sound...
  delay(1000);        // ...for 1sec
  
}
