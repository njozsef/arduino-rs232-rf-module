int rxPin = 12;
 
 
void setup()
{       pinMode(rxPin, INPUT);
        Serial.begin(9600);
}
 
 
void loop()
{
        int i = 0;
        unsigned long t = 0;
 
        byte prevBit = 0;
        byte bit = 0;
 
        unsigned long sender = 0;
        bool group = false;
        bool on = false;
        unsigned int recipient = 0;
 
        // latch 1
        while((t < 7480 || t > 12350))
        {       t = pulseIn(rxPin, LOW, 1000000);
        }
 
        // latch 2
        while(t < 2250 || t > 3000)
        {       t = pulseIn(rxPin, LOW, 1000000);
        }
 
        // data
        while(i < 64)
        {
                t = pulseIn(rxPin, LOW, 1000000);
 
                if(t > 170 && t < 415)
                {       bit = 0;
                }
                else if(t > 900 && t < 1460)
                {       bit = 1;
                }
                else
                {       i = 0;
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
                                sender <<= 1;
                                sender |= prevBit;
                        }      
                        else if(i == 53)
                        {       // 26th data bit
                                group = prevBit;
                        }
                        else if(i == 55)
                        {       // 27th data bit
                                on = prevBit;
                        }
                        else
                        {       // last 4 data bits
                                recipient <<= 1;
                                recipient |= prevBit;
                        }
                }
 
                prevBit = bit;
                ++i;
        }
 
        // interpret message
        if(i > 0)
        {       printResult(sender, group, on, recipient);
        }
}
 
 
void printResult(unsigned long sender, bool group, bool on, unsigned int recipient)
{
        Serial.print("<sender");
        Serial.print(sender);
        Serial.print("|");
 
        if(group)
        {       Serial.print("groupcommand");
        }
        else
        {       Serial.print("groupno");
        }
        Serial.print("|");
        
        Serial.print("recipient");
        Serial.print(recipient);
        
        Serial.print("|");
        if(on)
        {       Serial.print("stateon");
        }
        else
        {       Serial.print("stateoff");
        }
        Serial.print("|");
        
        Serial.print(">"); 
}




