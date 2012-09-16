import processing.serial.*;
 
Serial myPort;       
int i = 0;
 
void setup()
{
  size(512, 200);
   
  // List all the available serial ports:
  println(Serial.list());
  
  // Need to change this as needed
  myPort = new Serial(this, Serial.list()[1], 115200);
 
}
 
void draw()
{     
  // Write RGB values to serial port.
  myPort.write(0x7E); 
  myPort.write("test ");
  myPort.write('0' + i++);
  myPort.write("\r\n");
  myPort.write(0x7F);
  
  if(i>9) {
    i=0;
  }
  
  delay(1000);
}
 
