byte pins[] = {23, 25, 27, 29, 31, 33, 35,37};
int byteNumber = 0;
boolean isOdd = false;
int LowerAddressByte = 0;
int UpperAddressByte = 0;
boolean startReceived = false;
boolean processorBusGrantAck = false;
double cycles;
byte num;
void setup() {
  setupOutputPins();
  Serial.begin(115200);
}

void loop() {
    if(cycles < 50000){
      cycles++;
    }
    else{
       pinMode(40, INPUT);
       digitalWrite(40, HIGH); // set memory clk output pin to semi tristate
       digitalWrite(32, HIGH); // set buffers array to tristate.
       digitalWrite(36, HIGH); // set Bus Master to be released. active high.
       digitalWrite(38, LOW); // set Bus Master to be released. active high.
    }
}

void serialEvent() {
  cycles = 0;
  int val;
  byte num;
  
  if(Serial.available()){ // Do nothing until serial input is received
    num = Serial.read(); // Get num from somewhere

      if(num == 115){
          startReceived = true;
           
          digitalWrite(36, LOW); //Bus Acknowledge input.
          while(digitalRead(34)){
          }
          //digitalWrite(38, LOW); //Bus Acknowledge input.
          Serial.print('z');
      }
      else if(startReceived){
        val = getIntFromASCII(num);
        pinMode(40, OUTPUT);
        digitalWrite(32, LOW); //set buffers array to output.

        if(isOdd){
          writeToPins(val, 0); //write least significant nibble of a byte.
          latchByte("DATA", isEven(byteNumber));
          incrementAddress(!isEven(byteNumber));   
          byteNumber++;
        } 
        else {
          writeToFullPins(LowerAddressByte);
          latchByte("ADDRESS", true);
          writeToFullPins(UpperAddressByte);
          latchByte( "ADDRESS", false); 
          writeToPins(val, 4); //write most significant nibble of a byte.
        }
        isOdd = !isOdd;
    }
  }
}

void incrementAddress(boolean isEven){
    if(isEven){ 
      latchDataWord();
      if(LowerAddressByte >= 255){
        LowerAddressByte = 0;
        UpperAddressByte++;
      }
      else{
        LowerAddressByte++;
      }      
    }
}

void latchByte(String busType, boolean byteOrder){
    int pin;
    if(busType == "DATA"){
      pin = byteOrder == false ? 22 : 24;
    }
    else if(busType == "ADDRESS"){
      pin = byteOrder ? 28 : 30;
    }
    digitalWrite(pin, LOW);
    digitalWrite(pin, HIGH);
}

void latchDataWord(){
    digitalWrite(40, LOW);
    digitalWrite(40, HIGH);
}

boolean isEven(int x){ 
  return (x%2)==0;
}

void setupOutputPins(){
  //set bus pins 23, 25, 27, 29, 31, 33, 35, 37 to OUTPUT LOW
  for(int pin = 23; pin<38; pin=pin+2){
      pinMode(pin, OUTPUT);
      digitalWrite(pin, LOW);
  }
  //set signal pins 22, 24, 26, 28, 30, 32 to OUTPUT HIGH
  for(int pin = 22; pin<34; pin=pin+2){
      pinMode(pin, OUTPUT);
      digitalWrite(pin, HIGH);
  }
  pinMode(34, INPUT);
  digitalWrite(34, HIGH); //Bus Acknowledge input active low.
  pinMode(36, OUTPUT);
  digitalWrite(36, HIGH); //Bus Master Request active low.
  pinMode(38, OUTPUT);
  digitalWrite(38, HIGH); //Bus Master Request active low.
}

void writeToPins(int val, int offset){
      for (byte i=(0+offset); i<(4+offset); i++) {
      byte state = bitRead(byte(val), i-offset);
      digitalWrite(pins[i], state);
    }
}

void writeToFullPins(int val){
      for (byte i=0; i<8; i++) {
      byte state = bitRead(byte(val), i);
      digitalWrite(pins[i], state);
    }
}

int getIntFromASCII(byte num){
    if(num >= 48 && num <= 57){
      return num - 48;
    }
    if(num >= 65 && num <= 70){
      return num - 55;
    }
    if(num >= 97 && num <= 102){
      return num - 87;
    }
    
    if(num = 115){
      return num;
    }
    return 0;
}


