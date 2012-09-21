/*
This is tranceiver sketch
 for transmitt command to HomeEasy reseiver send command into terminal
 genericoff|sender5083278|groupno|recipient10|stateoff|
 genericon|sender482624|groupno|recipient0|stateoff|
 
 where:
 genericoff -switch transmitter to homeeasy mode (genericoff;genericon)
 5083278 - device ID 
 groupno - whithout group cpmmand (groupcommand;groupno)
 recipient10 - (10) button ID
 stateoff - state (stateon;stateoff)
 */

int rxPin = 12;
bool bit2[26]={
};              // 26 bit global to store the HE device address.

int txPin = 2;                 // 433mhz transmitter on pin2

String command; // String input from command prompt
char inByte; // Byte input from command prompt
char carray[10]; // character array for string to long // manipulation
unsigned long senderS=0;
boolean groupS = false;
boolean stateS = false;
int recipientS = 0;
boolean GenericProtocol = false;
void setup()
{
  pinMode(txPin, OUTPUT);
  pinMode(rxPin, INPUT);
  Serial.begin(9600);

  Serial.println("go");
}

void loop()
{
  Receiver();
}

void Transmitter()
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
    if (command.startsWith("generic"))
    {
      command = command.substring(7);
      GenericProtocol = (command == "on");
    }

    if (command.startsWith("sender"))
    {
      command = command.substring(6);
      command.toCharArray(carray, 10);
      senderS = atol(carray);
      itob(senderS, 26);            // convert our device code..
    }
    if (command.startsWith("recipient"))
    {
      command = command.substring(9);
      command.toCharArray(carray, 10);
      recipientS = atol(carray);            // convert our device code..
    }
    if (command.startsWith("group"))
    {
      command = command.substring(5);
      groupS = (command =="command");
    }
    if (command.startsWith("state")){
      command = command.substring(5);
      transmit(command == "on");            // send ON
      delay(10);                 // wait (socket ignores us it appears unless we do this)
      transmit(command == "on");            // send ON again
      delay(10);                 // wait (socket ignores us it appears unless we do this)
      transmit(command == "on");            // send ON again
      delay(10);                 // wait (socket ignores us it appears unless we do this)
      transmit(command == "on");            // send ON again
      delay(10);                 // wait (socket ignores us it appears unless we do this)
      transmit(command == "on");            // send ON again
//      Serial.println("--------------");
//
//      Serial.print("generic ");
//      Serial.println(GenericProtocol, HEX);
//
//      Serial.print("sender ");
//      Serial.println(senderS, DEC);
//
//      Serial.print("group ");
//      Serial.println(groupS, HEX);
//
//      Serial.print("recipient ");
//      Serial.println(recipientS, DEC);
//
//      Serial.print("state ");
//      Serial.println(command == "on", HEX);
//
//      Serial.println("--------------");
    }
    inByte=0;
    command = "";
  }
}
void transmit(boolean blnOn)
{
  if (GenericProtocol)
    transmitGenericData(blnOn);
  else
    transmitHEdata(blnOn);
}
void transmitGenericData(boolean blnOn)
{
  //digitalWrite(txPin, LOW);
  //delayMicroseconds(8800);     // low for 9900 for latch 1
  // Send generic command
  for(int i=6; i<26;i++)
  {
    sendGenericBit(bit2[i]);
  }

  sendGenericBit(blnOn);
  sendGenericBit(blnOn);
  sendGenericBit(!blnOn);
  sendGenericBit(!blnOn);
  
  sendGenericBit(recipientS & 1);
}

void transmitHEdata(boolean blnOn)
{
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
  for(int i=0; i<26;i++)
  {
    sendPair(bit2[i]);
  }

  // Send 26th bit - group 1/0
  sendPair(groupS);

  // Send 27th bit - on/off 1/0
  sendPair(blnOn);

  // last 4 bits - recipient   -- button 1 on the HE300 set to
  // slider position I in this example:

  sendPair(recipientS & 8);
  sendPair(recipientS & 4);
  sendPair(recipientS & 2);
  sendPair(recipientS & 1);

  digitalWrite(txPin, HIGH);   // high again (shut up)
  delayMicroseconds(275);      // wait a moment
  digitalWrite(txPin, LOW);    // low again for 2675 - latch 2.

}

void sendGenericBit(boolean b) {
  digitalWrite(txPin, HIGH);
  delayMicroseconds(250);
  if(b)
    digitalWrite(txPin, HIGH);
  else
    digitalWrite(txPin, LOW);
  delayMicroseconds(650);    
  digitalWrite(txPin, LOW);
  delayMicroseconds(250);
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


void Receiver()
{
  int i = 0;
  unsigned long t = 0;

  byte prevBit = 0;
  byte bit = 0;

  unsigned long senderR = 0;
  bool groupR = false;
  bool onR = false;
  unsigned int recipientR = 0;

  // latch 1
  while((t < 7480 || t > 12350))
  {       
    t = pulseIn(rxPin, LOW, 1000000);
    if (Serial.available() > 0)
      Transmitter();
  }

  // latch 2
  while(t < 2250 || t > 3000)
  {       
    t = pulseIn(rxPin, LOW, 1000000);
  }

  // data
  while(i < 64)
  {
    t = pulseIn(rxPin, LOW, 1000000);

    if(t > 170 && t < 415)
    {       
      bit = 0;
    }
    else if(t > 900 && t < 1460)
    {       
      bit = 1;
    }
    else
    {       
      i = 0;
      break;
    }

    if(i % 2 == 1)
    {
      if((prevBit ^ bit) == 0)
      {       // must be either 01 or 10, cannot be 00 or 11
        i = 0;
        break;
      }

      if(i < 53)
      {       // first 26 data bits
        senderR <<= 1;
        senderR |= prevBit;
      }      
      else if(i == 53)
      {       // 26th data bit
        groupR = prevBit;
      }
      else if(i == 55)
      {       // 27th data bit
        onR = prevBit;
      }
      else
      {       // last 4 data bits
        recipientR <<= 1;
        recipientR |= prevBit;
      }
    }

    prevBit = bit;
    ++i;
  }

  // interpret message
  if(i > 0)
  {       
    printResult(senderR, groupR, onR, recipientR);
  }
}


void printResult(unsigned long senderR, bool groupR, bool onR, unsigned int recipientR)
{
  Serial.print("<sender");
  Serial.print(senderR);
  Serial.print("|");

  if(groupR)
  {       
    Serial.print("groupcommand");
  }
  else
  {       
    Serial.print("groupno");
  }
  Serial.print("|");

  Serial.print("recipient");
  Serial.print(recipientR);

  Serial.print("|");
  if(onR)
  {       
    Serial.print("stateon");
  }
  else
  {       
    Serial.print("stateoff");
  }
  Serial.print("|");

  Serial.print(">"); 
}










