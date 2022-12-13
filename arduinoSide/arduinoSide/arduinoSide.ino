//PIN VARIABLES - CHANGE THIS
int red_light_pin= 9;
int green_light_pin = 10;
int blue_light_pin = 11;

//Variables
typedef struct
  {
      int short r;
      int short g;
      int short b;
  }  RGBtemplate;

int strobeDuration = 200; //In milliseconds
//Colors
RGBtemplate colors[9] = {
  {255,255,255}, //White
  {255, 0, 0}, //Red
  {255, 132, 0}, //Orange
  {255, 247, 0}, //Yellow
  {81, 255, 0}, //Green
  {0, 234, 255},//Teal
  {0, 8, 255}, //Blue
  {255, 0, 217}, //Pink
  {0,0,0}        //Black/Off
};

void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
 {
  analogWrite(red_light_pin, red_light_value);
  analogWrite(green_light_pin, green_light_value);
  analogWrite(blue_light_pin, blue_light_value);
}

void setup() {
  Serial.begin(9600);
  pinMode(red_light_pin, OUTPUT);
  pinMode(green_light_pin, OUTPUT);
  pinMode(blue_light_pin, OUTPUT);
}

void loop() {
  if(Serial.available()>0){   //Cita serial input (podatke sa node.js servera)
    String receivedData = Serial.readString();

    String lightMode = receivedData.substring(0,1);
    int firstColorIndex = receivedData.substring(1,2).toInt();
    int lastColorIndex = receivedData.substring(2,3).toInt();

    if (lightMode=="0"){                            //mode 0 - static color (indexed with firstColorIndex from colors array)
      RGB_color(colors[firstColorIndex].r, colors[firstColorIndex].g, colors[firstColorIndex].b); 
    }else if(lightMode=="1"){                       //mode 1 - gradient between colors indexed by firstcolorindex and lastcolorindex from colors array
        int maxI = max(firstColorIndex,lastColorIndex);
        int minI = min(firstColorIndex,lastColorIndex);
        
        int r1 = colors[minI].r;
        int g1 = colors[minI].g;
        int b1 = colors[minI].b;
        int r2 = colors[maxI].r;
        int g2 = colors[maxI].g;
        int b2 = colors[maxI].b;

        int rc = r1;
        int gc = g1;
        int bc = b1;

        bool rclimb = true;
        bool gclimb = true;
        bool bclimb = true;

        while (Serial.available()==0){
          delay(25);
          
          if(rc==r1){rclimb=true;}else if(rc==r2){rclimb=false;}
          if(gc==g1){gclimb=true;}else if(gc==g2){gclimb=false;}
          if(bc==b1){bclimb=true;}else if(bc==b2){bclimb=false;}

          if (r1!=r2){
            if(rclimb==true){rc++;}else{rc--;}
          }
           if (g1!=g2){
            if(gclimb==true){gc++;}else{gc--;}
          }
           if (b1!=b2){
            if(bclimb==true){bc++;}else{bc--;}
          }
          
          RGB_color(rc, gc, bc);
      }
      
      
      }else if(lightMode=="2"){ 
        while (Serial.available()==0){//mode 2 - Strobe lights
          RGB_color(colors[firstColorIndex].r, colors[firstColorIndex].g, colors[firstColorIndex].b);
          delay(strobeDuration);
          RGB_color(colors[lastColorIndex].r, colors[lastColorIndex].g, colors[lastColorIndex].b);
          delay(strobeDuration);
        }
      }
  }
}
