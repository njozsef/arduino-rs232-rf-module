bool bit2[26]={
};              // 26 bit global to store the HE device address.
int txPin = 2;                 // 433mhz transmitter on pin2

String command; // String input from command prompt
char inByte; // Byte input from command prompt
char carray[10]; // character array for string to long // manipulation
unsigned long sender=0;
boolean group = false;
boolean state = false;
int recipient = 0;
void setup()
{

  pinMode(txPin, OUTPUT);      // transmitter pin.
  Serial.begin(9600);         // console port

  Serial.println("go");
  //sender5083278|groupno|stateoff|recipient10

}

void loop()
{
  // Input serial information:
  if (Serial.available() > 0){
    inByte = Serial.read();
    // only input if a letter, number, =,?,+ are typed!
    if ((inByte >= 65 && inByte <= 90) 
      || (inByte >=97 && inByte <=122) 
      || (inByte >= 48 && inByte <=57) 
      || inByte == 43 || inByte == 61 || inByte == 63) {
      command.concat(inByte);
    }
  }

  if(inByte == '|')
  {
    if (command.startsWith("sender"))
    {
      command = command.substring(6);
      command.toCharArray(carray, 10);
      sender = atol(carray);
      itob(sender, 26);            // convert our device code..
    }
    if (command.startsWith("recipient"))
    {
      command = command.substring(9);
      command.toCharArray(carray, 10);
      recipient = atol(carray);            // convert our device code..
    }
    if (command.startsWith("group"))
    {
      command = command.substring(5);
      group = (command =="command");
    }
    if (command.startsWith("state")){
      command = command.substring(5);
        transmit(command == "on");            // send ON
        delay(10);                 // wait (socket ignores us it appears unless we do this)
        transmit(command == "on");            // send ON again
        Serial.println("--------------");
        
        Serial.print("sender ");
        Serial.println(sender, DEC);
        
        Serial.print("group ");
        Serial.println(group, HEX);
        
        Serial.print("recipient ");
        Serial.println(recipient, DEC);
        
        Serial.print("state ");
        Serial.println(command == "on", HEX);
        
        Serial.println("--------------");
    }
    inByte=0;
    command = "";
  }
}

void transmit(int blnOn)
{
  int i;

  // Do the latch sequence..
  digitalWrite(txPin, HIGH);
  delayMicroseconds(275);     // bit of radio shouting before we start.
  digitalWrite(txPin, LOW);
  delayMicroseconds(9900);     // low for 9900 for latch 1
  digitalWrite(txPin, HIGH);   // high again
  delayMicroseconds(275);      // wait a moment 275
  digitalWrite(txPin, LOW);    // low again for 2675 - latch 2.
  delayMicroseconds(2675);
  // End on a high
  digitalWrite(txPin, HIGH);

  // Send HE Device Address..
  // e.g. 1000010101000110010  272946 in binary.
  for(i=0; i<26;i++)
  {
    sendPair(bit2[i]);
  }

  // Send 26th bit - group 1/0
  sendPair(false);

  // Send 27th bit - on/off 1/0
  sendPair(blnOn);

  // last 4 bits - recipient   -- button 1 on the HE300 set to
  // slider position I in this example:

  sendPair(recipient & 8);
  sendPair(recipient & 4);
  sendPair(recipient & 2);
  sendPair(recipient & 1);

  digitalWrite(txPin, HIGH);   // high again (shut up)
  delayMicroseconds(275);      // wait a moment
  digitalWrite(txPin, LOW);    // low again for 2675 - latch 2.

}


void sendBit(boolean b) {
  if (b) {
    digitalWrite(txPin, HIGH);
    delayMicroseconds(310);   //275 orinally, but tweaked.
    digitalWrite(txPin, LOW);
    delayMicroseconds(1340);  //1225 orinally, but tweaked.
  }
  else {
    digitalWrite(txPin, HIGH);
    delayMicroseconds(310);   //275 orinally, but tweaked.
    digitalWrite(txPin, LOW);
    delayMicroseconds(310);   //275 orinally, but tweaked.
  }
}

void sendPair(boolean b) {
  // Send the Manchester Encoded data 01 or 10, never 11 or 00
  if(b)
  {
    sendBit(true);
    sendBit(false);
  }
  else
  {
    sendBit(false);
    sendBit(true);
  }
}


void itob(unsigned long integer, int length)
{  //needs bit2[length]
  // Convert long device code into binary (stores in global bit2 array.)
  for (int i=0; i<length; i++){
    if ((integer / power2(length-1-i))==1){
      integer-=power2(length-1-i);
      bit2[i]=1;
    }
    else bit2[i]=0;
  }
}

unsigned long power2(int power){    //gives 2 to the (power)
  unsigned long integer=1;          
  for (int i=0; i<power; i++){      
    integer*=2;
  }
  return integer;
}

