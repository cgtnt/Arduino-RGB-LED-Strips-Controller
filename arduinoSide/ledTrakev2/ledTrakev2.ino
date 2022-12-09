//Variables

int red_light_pin= 9;
int green_light_pin = 10;
int blue_light_pin = 11;
#define sensorPin A1
float sensorValue = 0, filteredSignal = 0,
    filteredSignalValues[] = {2.9, 2.7, 2.4, 2.1, 1.7, 1.3, 0.9, 0.2, 0.15};
bool soundReact = false;
typedef struct
  {
      int short r;
      int short g;
      int short b;
  }  RGBtemplate;


//Boje
RGBtemplate colors[9] = { //Duzina liste //Boje stavljati po redu RGB spektruma zbog gradient funkcije // bijela - sve ostale (crvena do roza) - crna 
  {255,255,255}, //bijela
  {255, 0, 0}, //crvena
  {255, 132, 0}, //narandz
  {255, 247, 0}, //zuta
  {81, 255, 0}, //zel
  {0, 234, 255},//svijetlo plava
  {0, 8, 255}, //tamno plava
  {255, 0, 217}, //roza
  {0,0,0}        //crna
};

void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
 {
  analogWrite(red_light_pin, red_light_value);
  analogWrite(green_light_pin, green_light_value);
  analogWrite(blue_light_pin, blue_light_value);
}

//Pocetak
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

    if (lightMode=="0"){  //Na osnovu dobijenih podataka bira jedan od nacina funkcionisanja trake
      //mode 0 - staticna boja tj boja u tabeli s indexom firstcolorIndex
      RGB_color(colors[firstColorIndex].r, colors[firstColorIndex].g, colors[firstColorIndex].b); 
    }else if(lightMode=="1"){   //mode 1 - gradient boja u tabeli od indexa firstcolorindex do lastcolorindex
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
          Serial.print("exec");
          //ovdje output code sa rc, gc, bc vrijednostima
          
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
      
      
      }else if(lightMode=="2"){ //mode 2 - traka reaguje na zvuk
        if (soundReact==false){
          soundReact = true;
          while (Serial.available()==0){
            delay(30);
          reactToSoundF();
        }
        soundReact = false;
      }
    }
  }
}

//Sensor za zvuk kod
void reactToSoundF() {

  sensorValue = (float) analogRead(sensorPin) * (5.0 / 1024.0);

  FilterSignal(sensorValue);

  Serial.print(sensorValue);
  Serial.print(" ");
  Serial.println(filteredSignal);

  CompareSignalFiltered(filteredSignal);
}

void FilterSignal(float sensorSignal) {

  filteredSignal = (0.945 * filteredSignal) + (0.0549 * sensorSignal);
}

void CompareSignalFiltered(float filteredSignal) {
  if (filteredSignal <= filteredSignalValues[6] && filteredSignal > filteredSignalValues[8]) {
    RGB_color(255, 127, 0);
    //Serial.println("Orange");
  } else if (filteredSignal <= filteredSignalValues[7]){
    RGB_color(255, 247, 0);
  } else {
    RGB_color(255, 0, 0);
    //Red
  }
}
