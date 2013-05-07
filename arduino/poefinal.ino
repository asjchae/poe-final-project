/*

POE Spring 2013 Final Project
Ariana Chae, Helen Lyons, Casey Monahan, Wonho Ryu, Jong Chan Woo

Sources:

BluetoothShield Demo Code Slave.pde.
2011 Copyright (c) Seeed Technology Inc.  All right reserved.
Author: Steve Chang
For more details about the product please check http://www.seeedstudio.com/depot/
 
*/
 
 
/* Upload this sketch into Seeeduino and press reset*/
 
#include <SoftwareSerial.h>   //Software Serial Port
#define RxD 6
#define TxD 7
 
#define DEBUG_ENABLED  1
 
SoftwareSerial blueToothSerial(RxD,TxD);
String pinscreen[12];
int dataPin = 2;
int latchPin = 3;
int clockPin = 4;
int motor = 5;
//const int motorpin;
const int listlength = 15;
//const int ledPins[] = {13,12,11,10,9,8,5,4,3,2,1,0};
char num_list[] = {'0','1','2','3','4','5','6','7','8','9'};
String recvDeci = "";
int recvDeci_int[listlength];
boolean recvDeci_convert_bool = true;
boolean recvDeci_convert_to_binary_bool = true;
int i = 0;
char front_char;
char back_char;


// String pinscreen[12] has 144 value of 0,1 with character
// You can use like followings
// for (int i =0; i<12; i++){
//   for (int j = 0; j<12; j++){
//     if (pinscreen[i*12+j] == '1'){
//       Do something
//     } 
//     else if (pinscreen[i*12+j] =='0'){
//       Do something different
//     }
//   }
// }



void setup() 
{ 
  Serial.begin(9600);
  pinMode(RxD, INPUT);
  pinMode(TxD, OUTPUT);
  setupBlueToothConnection();
//  pinMode(motorpin, OUTPUT);
  pinMode(dataPin, OUTPUT);       //Configure each IO Pin
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(motor, OUTPUT);
}  
void loop() 
{ 
  char recvChar;
  while (1){
    if (recvDeci.length() == listlength*4){break;}
    if (blueToothSerial.available())
    {//check if there's any data sent from the remote bluetooth shield
      recvChar = blueToothSerial.read();
      Serial.print(recvChar);
      back_char = front_char;
      front_char = recvChar;

      if (back_char == 'x'){recvDeci += back_char;}
      if (recvDeci.length()<listlength*4)
      {
        if (check_num(back_char) && check_num(front_char))
        {
          recvDeci += back_char;
        }
        else if (check_num(back_char)&& front_char == 'x'){
          recvDeci += back_char;
        }
      }
      
      if (recvDeci.length() == listlength*4-1)
      {
        recvDeci += front_char;
      }
    }
  }
  if ((recvDeci.length() == listlength*4) && (recvDeci_convert_bool))
  {
    for (int j = 0; j<listlength; j++){
      String convert;
      for (int k=0; k<4; k++){
        convert += recvDeci[j*4+k];
      }
      recvDeci_int[j] = change_to_int(convert);
    }
    for (int m = 0 ; m<listlength; m++){

      //Serial.println("");
      //Serial.print(recvDeci_int[m]);
    }
    recvDeci_convert_bool = false;
  }
  
  if((recvDeci.length() == listlength*4) && (recvDeci_convert_to_binary_bool)){
    Serial.println("Convert to binary!");
    for (int m = 0 ; m<listlength; m++){
      pinscreen[m] = convert_to_binary(recvDeci_int[m]);
      Serial.println(pinscreen[m]);
      int partone1 = change_to_int_char(pinscreen[m][0]);
      int partone2 = change_to_int_char(pinscreen[m][1]);
      int partone3 = change_to_int_char(pinscreen[m][2]);
      int partone4 = change_to_int_char(pinscreen[m][3]);
      int partone[] = {partone1, partone2, partone3, partone4};
      
      float fourpart = convert_four_to_deci(partone);
      int fp = ceil(fourpart);
      // fp is the number for the size four array.
      
      int parttwo1 = change_to_int_char(pinscreen[m][4]);
      int parttwo2 = change_to_int_char(pinscreen[m][5]);
      int parttwo3 = change_to_int_char(pinscreen[m][6]);
      int parttwo4 = change_to_int_char(pinscreen[m][7]);
      int parttwo5 = change_to_int_char(pinscreen[m][8]);
      int parttwo6 = change_to_int_char(pinscreen[m][9]);
      int parttwo7 = change_to_int_char(pinscreen[m][10]);
      int parttwo8 = change_to_int_char(pinscreen[m][11]);
      int parttwo[] = {parttwo1, parttwo2, parttwo3, parttwo4, parttwo5, parttwo6, parttwo7, parttwo8};
      float eightpart = convert_eight_to_deci(parttwo);
      int ep = ceil(eightpart);
      // ep is the number for the size eight array.
      
      Serial.println(fp);
      Serial.println(ep);
      
      digitalWrite(latchPin, LOW);           //Pull latch LOW to start sending data
      shiftOut(dataPin, clockPin, MSBFIRST, fp); //Send the data byte 1
      shiftOut(dataPin, clockPin, MSBFIRST, ep); //Send the data byte 2
      digitalWrite(latchPin, HIGH);          //Pull latch HIGH to stop sending data
      delay(500);
      
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST, 0);
      shiftOut(dataPin, clockPin, MSBFIRST, 0);
      digitalWrite(latchPin, HIGH);
      
      digitalWrite(motor, 1); // turn motor on.
      delay(50);
      digitalWrite(motor, 0); // turn motor off. 
    }
    recvDeci_convert_to_binary_bool = false;
  }
}

 
void setupBlueToothConnection()
{
  blueToothSerial.begin(38400); //Set BluetoothBee BaudRate to default baud rate 38400
  blueToothSerial.print("\r\n+STWMOD=0\r\n"); //set the bluetooth work in slave mode
  blueToothSerial.print("\r\n+STNA=SeeedBTSlave\r\n"); //set the bluetooth name as "SeeedBTSlave"
  blueToothSerial.print("\r\n+STOAUT=1\r\n"); // Permit Paired device to connect me
  blueToothSerial.print("\r\n+STAUTO=0\r\n"); // Auto-connection should be forbidden here
  delay(2000); // This delay is required.
  blueToothSerial.print("\r\n+INQ=1\r\n"); //make the slave bluetooth inquirable 
  Serial.println("The slave bluetooth is inquirable!");
  delay(2000); // This delay is required.
  blueToothSerial.flush();
}

boolean check_num (char character){
  for (int i = 0; i<10; i++){
    if (character==num_list[i]){
      return true;
    }
  }
  return false;
}

int change_to_int (String str)
{
  int toint = 0;
  for (int k = 0; k < str.length(); k++)
  {
    if (check_num(str[k])){
      toint = toint*10 + change_to_int_char(str[k]);
    }
  }
  return toint;
}

int change_to_int_char (char character)
{
  if (character == '0'){return 0;}
  else if (character == '1'){return 1;}
  else if (character == '2'){return 2;}
  else if (character == '3'){return 3;}
  else if (character == '4'){return 4;}
  else if (character == '5'){return 5;}
  else if (character == '6'){return 6;}
  else if (character == '7'){return 7;}
  else if (character == '8'){return 8;}
  else if (character == '9'){return 9;}
}

String convert_to_binary (int deci) {
  String retro_binary="";
  String binary="";
  for (int i =0; i <12; i ++ ){
    retro_binary += int_to_char(deci%2);
    deci = (deci-(deci%2))/2;
  }
  for (int j =0; j<12 ; j ++){
    binary += retro_binary[11-j];
  }
  return binary;
}

float convert_four_to_deci (int binary[]) {
  float fourdeci = 0;
  int power[] = {3, 2, 1, 0};
  for (int q=4; q>0; q--) {
    if(binary[q-1] == 0) {
      continue;
    } else if (binary[q-1] == 1) {
      float add = pow(2, power[q-1]);
      fourdeci = fourdeci + add;
    }
  }
  return fourdeci;
}

float convert_eight_to_deci (int binary[]) {
  float eightdeci = 0;
  int power[] = {7, 6, 5, 4, 3, 2, 1, 0};
  for (int q=8; q>0; q--) {
    if(binary[q-1] == 0) {
      continue;
    } else if (binary[q-1] == 1) {
      float add = pow(2, power[q-1]);
      eightdeci = eightdeci + add;
    }
  }
  return eightdeci;
}

char int_to_char (int one){
  if (one == 0){return '0';}
  else if (one == 1){return '1';}
  else if (one == 2){return '2';}
  else if (one == 3){return '3';}
  else if (one == 4){return '4';}
  else if (one == 5){return '5';}
  else if (one == 6){return '6';}
  else if (one == 7){return '7';}
  else if (one == 8){return '8';}
  else if (one == 9){return '9';}
}

