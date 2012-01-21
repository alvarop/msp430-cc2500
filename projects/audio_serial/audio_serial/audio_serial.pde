import ddf.minim.*;
import ddf.minim.analysis.*;
import processing.serial.*;
 
// original code from http://code.compartmental.net/tools/minim/quickstart/

Minim minim;
//AudioPlayer song;
AudioInput song;
FFT fft;
  Serial myPort;       

 
void setup()
{
  size(512, 200);
 
  // always start Minim first!
  minim = new Minim(this);
  
  // List all the available serial ports:
  println(Serial.list());
  
  // Need to change this as needed
  myPort = new Serial(this, Serial.list()[1], 115200);
 
  // specify 512 for the length of the sample buffers
  // the default buffer size is 1024
  //song = minim.loadFile("man.mp3", 512);
  //song.play();
  
  // open the line in for processing
  song = minim.getLineIn(Minim.STEREO, 512);
 
  // an FFT needs to know how 
  // long the audio buffers it will be analyzing are
  // and also needs to know 
  // the sample rate of the audio it is analyzing
  fft = new FFT(song.bufferSize(), song.sampleRate());
  
  print(fft.specSize());
}
 
void draw()
{
  background(0);
  // first perform a forward fft on one of song's buffers
  // I'm using the mix buffer
  //  but you can use any one you like
  fft.forward(song.mix);
 
  stroke(255, 0, 0, 128);
  // draw the spectrum as a series of vertical lines
  // I multiple the value of getBand by 4 
  // so that we can see the lines better
  
  int low = 0;
  int mid = 0;
  int high = 0;
  int threshold = 15;
  for(int i = 0; i < fft.specSize(); i++)
  {
    line(i, height, i, height - fft.getBand(i)*4);
    
    // Add up all values in bins 5-25, take the log just for fun
    if( (i > 2) && (i < 15) )
    {
       low = low + round(log(fft.getBand(i)*10+1));
    }
    
    if( (i > 16) && (i < 30) )
    {
       mid = mid + round(log(fft.getBand(i)*10+1));
    }
    
    // Add up all values in bins 26-50, take the log just for fun
    if( (i > 31) && (i < 45) )
    {
       high = high + round(log(fft.getBand(i)*10+1));
    }
  }
 
   low -= threshold;
   mid -= threshold;
   high -= threshold;
 
  // Saturate output, only 255 brightnesses available
  if(low > 255)
  {
    low= 255;
  }
  if(mid > 255)
  {
    mid= 255;
  }
  if(high > 255)
  {
    high= 255;
  }    
  
  // Saturate output low
  if(low < 0)
  {
    low= 0;
  }
  if(mid < 0)
  {
    mid= 0;
  }  
  if(high < 0)
  {
    high= 0;
  }
  

      
  // Write RGB values to serial port.
  myPort.write(low); // Red is for the low channel
  myPort.write(mid); // Green is for the middle
  myPort.write(high); // Blue is for the higher stuff
 
  // Print values for debugging 
  //print(low);
  //print(" ");
  //print(high);
  //print("\n");
 
  stroke(255);
  // I draw the waveform by connecting 
  // neighbor values with a line. I multiply 
  // each of the values by 50 
  // because the values in the buffers are normalized
  // this means that they have values between -1 and 1. 
  // If we don't scale them up our waveform 
  // will look more or less like a straight line.
  for(int i = 0; i < song.left.size() - 1; i++)
  {
    line(i, 50 + song.left.get(i)*50, i+1, 50 + song.left.get(i+1)*50);
    line(i, 150 + song.right.get(i)*50, i+1, 150 + song.right.get(i+1)*50);
  }
  
  
}
 
void stop()
{
  song.close();
  minim.stop();
 
  super.stop();
}
