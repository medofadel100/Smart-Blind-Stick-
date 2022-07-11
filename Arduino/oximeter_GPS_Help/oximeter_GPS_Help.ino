/*
 * an oximeter diy. v.0.92 (minor fixes)
 * by hacking a ky-039 heartbeat sensor or using an infrared led
 * a red led and a photodiode.
 * https://hackaday.io/project/170752-oximeter-do-it-yourself
 */
#define help 4
#include <Wire.h> 
//#include <LiquidCrystal_I2C.h>

#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// Choose two Arduino pins to use for software serial
int RXPin = 2;
int TXPin = 3;

int GPSBaud = 9600;

// Create a TinyGPS++ object
TinyGPSPlus gps;

// Create a software serial port called "gpsSerial"
SoftwareSerial gpsSerial(RXPin, TXPin);

#define maxperiod_siz 80 // max number of samples in a period
#define measures 10      // number of periods stored
#define samp_siz 4       // number of samples for average
#define rise_threshold 3//edited was 3 // number of rising measures to determine a peak
 
// a liquid crystal displays BPM 
//LiquidCrystal_I2C //lcd(0x3F, 16, 2);

int T = 20;              // slot milliseconds to read a value from the sensor
int sensorPin = A0; 
int REDLed = 8;
int IRLed = 9;

byte sym[3][8] = {
 {
  B00000,
  B01010,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
  B00000
},{
  B00000,
  B00000,
  B00000,
  B11000,
  B00100,
  B01000,
  B10000,
  B11100
},{
  B00000,
  B00100,
  B01010,
  B00010,
  B00100,
  B00100,
  B00000,
  B00100
}
};

void setup() {
   Serial.begin(9600);
   Serial.flush();
   pinMode(sensorPin,INPUT);
   pinMode(REDLed,OUTPUT);
   pinMode(IRLed,OUTPUT);

   pinMode(help,INPUT);
   // initialize the //lcd
   //lcd.init();
   //lcd.backlight();

   // turn off leds
   digitalWrite(REDLed,LOW);
   digitalWrite(IRLed,LOW);

  // for(int i=0;i<8;i++) ////lcd.createChar(i, sym[i]);


  // Start the software serial port at the GPS's default baud
  gpsSerial.begin(GPSBaud);

}

void loop ()
{
  if (help== HIGH){
    Serial.println("Help");
  }
 
GPS();
 }


void GPS(){
  // This sketch displays information every time a new sentence is correctly encoded.
  while (gpsSerial.available() > 0)
    if (gps.encode(gpsSerial.read()))
      displayInfo();

  // If 5000 milliseconds pass and there are no characters coming in
  // over the software serial port, show a "No GPS detected" error
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println("No GPS detected");
    while(true);
  }
}
void displayInfo()
{
  if (gps.location.isValid())
  {
    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6);
    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);
    Serial.print("Altitude: ");
    Serial.println(gps.altitude.meters());
  }
  else
  {
    Serial.println("Location: Not Available");
  }
  
  Serial.print("Date: ");
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print("/");
    Serial.print(gps.date.day());
    Serial.print("/");
    Serial.println(gps.date.year());
  }
  else
  {
    Serial.println("Not Available");
  }

  Serial.print("Time: ");
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(":");
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(":");
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(".");
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.println(gps.time.centisecond());
  }
  else
  {
    Serial.println("Not Available");
  }

  Serial.println();
  Serial.println();
  
  delay(1000);
  spo();
  
}

void spo(){
  bool finger_status = true;
  
  float readsIR[samp_siz], sumIR,lastIR, reader, start;
  float readsRED[samp_siz], sumRED,lastRED;


  int period, samples;
  period=0; samples=0;
  int samplesCounter = 0;
  float readsIRMM[maxperiod_siz],readsREDMM[maxperiod_siz];
  int ptrMM =0;
  for (int i = 0; i < maxperiod_siz; i++) { readsIRMM[i] = 0;readsREDMM[i]=0;}
  float IRmax=0;
  float IRmin=0;
  float REDmax=0;
  float REDmin=0;
  double R=0;

  float measuresR[measures];
  int measuresPeriods[measures];
  int m = 0;
  for (int i = 0; i < measures; i++) { measuresPeriods[i]=0; measuresR[i]=0; }
   
  int ptr;

  float beforeIR;

  bool rising;
  int rise_count;
  int n;
  long int last_beat;
  for (int i = 0; i < samp_siz; i++) { readsIR[i] = 0; readsRED[i]=0; }
  sumIR = 0; sumRED=0; 
  ptr = 0; 
    
 

  while(1)
  {
    
    //
    // turn on IR LED
    digitalWrite(REDLed,LOW);
    digitalWrite(IRLed,HIGH);
    
    // calculate an average of the sensor
    // during a 20 ms (T) period (this will eliminate
    // the 50 Hz noise caused by electric light
    n = 0;
    start = millis();
    reader = 0.;
    do
    {
      reader += analogRead (sensorPin);
      n++;
    }
    while (millis() < start + T);  
    reader /= n;  // we got an average
    // Add the newest measurement to an array
    // and subtract the oldest measurement from the array
    // to maintain a sum of last measurements
    sumIR -= readsIR[ptr];
    sumIR += reader;
    readsIR[ptr] = reader;
    lastIR = sumIR / samp_siz;
    
    
    

    //
    // TURN ON RED LED and do the same
    digitalWrite(IRLed,LOW);
    digitalWrite(REDLed,HIGH);

    n = 0;
    start = millis();
    reader = 0.;
    do
    {
      reader += analogRead (sensorPin);
      n++;
    }
    while (millis() < start + T);  
    reader /= n;  // we got an average
    // Add the newest measurement to an array
    // and subtract the oldest measurement from the array
    // to maintain a sum of last measurements
    sumRED -= readsRED[ptr];
    sumRED += reader;
    readsRED[ptr] = reader;
    lastRED = sumRED / samp_siz;







    //                                  
    // R CALCULATION
    // save all the samples of a period both for IR and for RED
    readsIRMM[ptrMM]=lastIR;
    readsREDMM[ptrMM]=lastRED;
    ptrMM++;
    ptrMM %= maxperiod_siz;
    samplesCounter++;
    //
    // if I've saved all the samples of a period, look to find
    // max and min values and calculate R parameter
    if(samplesCounter>=samples){
      samplesCounter =0;
      IRmax = 0; IRmin=1023; REDmax = 0; REDmin=1023;
      for(int i=0;i<maxperiod_siz;i++) {
        if( readsIRMM[i]> IRmax) IRmax = readsIRMM[i];
        if( readsIRMM[i]>0 && readsIRMM[i]< IRmin ) IRmin = readsIRMM[i];
        readsIRMM[i] =0;
        if( readsREDMM[i]> REDmax) REDmax = readsREDMM[i];
        if( readsREDMM[i]>0 && readsREDMM[i]< REDmin ) REDmin = readsREDMM[i];
        readsREDMM[i] =0;
      }
      R =  ( (REDmax-REDmin) / REDmin) / ( (IRmax-IRmin) / IRmin ) ;
    }

    

    




    // check that the finger is placed inside
    // the sensor. If the finger is missing 
    // RED curve is under the IR.
    //
    if (lastIR < lastRED) {
      if(finger_status==true) {
        finger_status = false;
        //lcd.clear();
        //lcd.setCursor(0,0);
        Serial.print("No finger?");  
        
        
          
      }
    } else {
      if(finger_status==false) {
        //lcd.clear();
        finger_status = true;

            //lcd.setCursor(10,0);
            //lcd.print("c=");

            //lcd.setCursor(0,0);
            //lcd.print("bpm");
            //lcd.setCursor(0,1);
            //lcd.print("SpO"); //lcd.write(1);  //2            
            //lcd.setCursor(10,1);
            //lcd.print("R=");

        
      }
    }


    float avR = 0;
    int avBPM=0;

   

    if (finger_status==true){

      
     
       // lastIR holds the average of the values in the array
       // check for a rising curve (= a heart beat)
       if (lastIR > beforeIR)
       {
 
         rise_count++;  // count the number of samples that are rising
         if (!rising && rise_count > rise_threshold)
         {
           //lcd.setCursor(3,0); 
           //lcd.write( 0 );       // <3
            // Ok, we have detected a rising curve, which implies a heartbeat.
            // Record the time since last beat, keep track of the 10 previous
            // peaks to get an average value.
            // The rising flag prevents us from detecting the same rise 
            // more than once. 
            rising = true;

            measuresR[m] = R;
            measuresPeriods[m] = millis() - last_beat;
            last_beat = millis();
            int period = 0;
            for(int i =0; i<measures; i++) period += measuresPeriods[i];

            // calculate average period and number of samples
            // to store to find min and max values
            period = period / measures;
            samples = period / (2*T);
              
             int avPeriod = 0;

            int c = 0;
            // c stores the number of good measures (not floating more than 10%),
            // in the last 10 peaks
            for(int i =1; i<measures; i++) {
              if ( (measuresPeriods[i] <  measuresPeriods[i-1] * 1.1)  &&  
                    (measuresPeriods[i] >  measuresPeriods[i-1] / 1.1)  ) {

                  c++;
                  avPeriod += measuresPeriods[i];
                  avR += measuresR[i];

              }
            }
              
            m++;
            m %= measures;
                        
            //lcd.setCursor(12,0);
            //lcd.print(String(c)+"  ");
             // Serial.println(c);

            // bpm and R shown are calculated as the
            // average of at least 5 good peaks
            avBPM = 60000 / ( avPeriod / c) ;
            avR = avR / c ;
              //Serial.print("bpm="+String(avBPM)+" "); 

            // if there are at last 5 measures 
            //lcd.setCursor(12,1);            
            // if there are at least 5 good measures...
           
            if(c >=1) {

              //
              // SATURTION IS A FUNCTION OF R (calibration)
              // Y = k*x + m
              // k and m are calculated with another oximeter
              int SpO2 =-19 * R + 110;
               // Serial.print("bpm="+String(avBPM)+" "); 
                //Serial.print( " " + String(SpO2) +"% ");
              //lcd.setCursor(4,0);
              if(avBPM > 40 && avBPM <200){
                Serial.print("bpm="+String(avBPM)+" "); 
              }
              else {
                 // Serial.print("---");
              }

              //lcd.setCursor(4,1); 
              if(SpO2 > 70 && SpO2 <150) {
                Serial.print( "   SpO2=" + String(SpO2) +"% ");
                
                }
                                

            } else {
             

              
            }
   
           
         }
       }
       else
       {
         // Ok, the curve is falling
         rising = false;
         rise_count = 0;
         
       }
  
       // to compare it with the new value and find peaks
       beforeIR = lastIR;
  

   } 




   // handle the arrays      
   ptr++;
   ptr %= samp_siz;
    GPS();
   // z=0;  
  } // END loop while 1

 
}
