// Receive sensor data from transmitter and map them to motor and servo
// Library to make a Software UART
#include <SoftwareSerial.h>
#include <Servo.h>

#define RX 3
#define TX 2

// Motor Driver and Servo variables
#define enable 6
#define phase 5
#define FORWARD HIGH
#define BACKWARD LOW
#define servo_pin 9
  int throttle_pos;
  int steering_pos;
int servo_pos=90;
int throttle_min = 0;
int throttle_neutral = 337;
int throttle_max = 700;
int throttle_forward = 367;
int throttle_reverse = 307;
int motor;


#define BAUDRATE 9600
 
SoftwareSerial BTSerial(RX, TX); // (RX, TX)

// Struct to hold the data we want to receive
struct Packet {
  int throttle_pos;
  int steering_pos;
} pkt; // Instantiate a Packet struct

Servo servo; 
void setup() {
  // Start Serial Monitor for feedback
  Serial.begin(BAUDRATE);

  // HM-10 virtual UART
  BTSerial.begin(BAUDRATE);
    // Set phase (digital) pin as an output
  pinMode(phase, OUTPUT);

  // Set initial condition for motor driver
  digitalWrite(phase, FORWARD);
  analogWrite(enable, 0);

  // Attach servo pin to Servo object  
  servo.attach(servo_pin);
}
 
void loop() {
  // Receive data from the bluetooth
  bluetooth_receive();  

  // Necessary forced delay, if we receive too fast (no delay)
  //  the error rate increases
  delay(1);
}

// Function responsible for receiving data over bluetooth
void bluetooth_receive() {
  // Check the software serial buffer for data to read
  if(BTSerial.available() >= sizeof(Packet)) {
    // Read in the appropriate number of bytes to fit our Packet
    BTSerial.readBytes((byte *) & pkt,sizeof(Packet));

  //Map sensor data to motor and servo
    if (throttle_pos >= throttle_forward) {
    digitalWrite(phase, FORWARD);
    motor = map(throttle_pos, throttle_forward, throttle_max, 0, 255);
    analogWrite(enable, motor);
  }

  else if (throttle_pos <= throttle_reverse) {
    digitalWrite(phase, BACKWARD);
    motor = map(throttle_pos, throttle_reverse, throttle_min, 0, 255);
    analogWrite(enable, motor);
  }

else {
    motor = 0;
    analogWrite(enable,motor);
  }
  
 servo_pos = steering_pos/3.75;
 servo.write(servo_pos); 
    
    // Print packet (debug)
    print_packet();
  }   
}

// Function to print packet data and motor/servo (debug)
void print_packet() {  
  Serial.print("RX: (throttle_pos,steering_pos,motor,servo)=(");
  Serial.print(pkt.throttle_pos); Serial.print(",");
  Serial.print(pkt.steering_pos); Serial.print(",");
  Serial.print(motor); Serial.print(",");
  Serial.print(servo_pos); Serial.print(",");
  Serial.println(")");
  Serial.println(")");
}