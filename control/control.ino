void setup() {
  Serial.begin(9600); // Initialize serial communication at 9600 baud
  pinMode(13, OUTPUT);// Set pin 13 as an output,change to any pin
  pinMode(12, OUTPUT);// Set pin 12 as an output
}

void loop() {
  if (Serial.available() > 0) { // Check if there is any data available on the serial port
    char command = Serial.read(); // Read the incoming command
    if (command == '1') { // If the command is '1', turn the lights on
      digitalWrite(13, HIGH);
    }if (command=='3'){
      digitalWrite(12,1);}
    else if (command == '0') { // If the command is '0', turn the lights off
      digitalWrite(13, LOW);
     
    }else if(command=='4'){digitalWrite(12,0);
    digitalWrite(13, LOW);
      }
     
}}
