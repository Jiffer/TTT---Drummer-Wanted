#include <StopWatch.h>

// leaky integrator variables 
 float alpha = .15;
 float a0_in = 0.0;
 float a0_last = 0.0;
 float a_out = 0.0;
 float a_out_last = 0.0;
 // thresholding variables
 float a_diff = 0.0;
 float W = 0.0;
 float THRESHOLD = 2.5;

// timer -- http://www.arduino.cc/playground/Code/StopWatchClass
StopWatch timer;
const int numBeats = 8;
int beatPtr = 0;
float beatTimes[numBeats];

int motorPin = 11;  // analogWrite(motorPin, value), value range [0, 255]


void setup() {
  // initialize the serial communication:
  Serial.begin(9600);
  
  // start the stopwatch
  timer.start();
}

void loop() {
  
  // leaky integrator
  a0_in = analogRead(A0);
  a_out = a0_in * alpha + (1 - alpha) * a_out_last;
  a_out_last = a_out;
  
  // Detect onsets
  a_diff = abs(a0_in - a_out_last);
  W = a_diff / a_out;
  
  // registered a beat
  if(W > THRESHOLD)
  {
    //Serial.println(512);
    
    // don't register the time if it is too large (5 seconds)
    if (timer.elapsed() < 5000) 
    {
      beatTimes[beatPtr] = timer.elapsed();
      beatPtr++;
      beatPtr %= numBeats;
    
    // average beatTimes
    float avgTime = 0;
    for (int i = 0; i < numBeats; i++){
      avgTime += beatTimes[i];
    }
    avgTime /= numBeats;
    //Serial.print('avg: ');
    
    
    if (avgTime < 100)
    {
      avgTime = 100;
    }
    
    Serial.println(avgTime);
    avgTime -= 100;  
    avgTime /= 14900;  // should be between 0 and 1 now
    avgTime = 1.0 - avgTime;  // smaller numbers mean faster beats so inverting the order
    
    float rate = 255 * avgTime;
    
    Serial.println(rate);
    analogWrite(motorPin, rate);
    
    }
    else // its been 5 seconds without a beat, make sure we're off
    { 
      
      analogWrite(motorPin, 0);
    }
    
    // reset the timer
    timer.reset();
    timer.start();
    
  }
  else // no new beat
  {
    if (timer.elapsed() > 5000) 
    {
      analogWrite(motorPin, 0);
    }
    //Serial.println(0);
  }
  
}


