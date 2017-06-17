#include <LiquidCrystal.h>
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

//Cartão SD
#include <SD.h>
File myFile;

//define variaveis globais e constantes
#define SENSORPIN  8 //Pino do A3144

//Pinos dos Botões
#define BUTTONPLUS  A5
#define BUTTONMINUS A4
#define BUTTONBACK  A3
#define BUTTONNEXT  A2

//Controlo da inserção e do numero de algarismos do raio
int inputState = 1;

//controlo de tempo em segundos
double tempoI = 0;
double tempoF = 0;
double deltaT = 0; 
//PodeDarVolta
double podeDarVolta = 1; 

int voltas = 0; //controla as voltas
float raio = 0.03; //raio em metros
double vel = 0; //velocidade em KM/H
float dist = 0; //distancia em KM

int estado = 0; //controla o estado do Sensor A3144

float deltatempo(float ti, float tf){
  int dt;
  if(ti*tf > 0){
    dt = abs(tf - ti);
  }
  else{
    dt = abs(tf + ti);
  }
  return dt;
}

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("BikePlus");
  lcd.setCursor(0, 1);
  lcd.print("Versao 1.0");
  delay(10000);
  //Serial 
  Serial.begin(9600);
  //imprime a info no serial
  Serial.println("Raio\tTempo\tDeltaT\tDistancia\tVelocidade");
  
  //Define os estados dos pinos
  pinMode(SENSORPIN, INPUT);

  //vamos guardar tudo isto no SD
  pinMode(10, OUTPUT);

}

void loop() {
  
  
  while(inputState <= 6){
    //limpa o lcd
    lcd.clear();
    //Vamos ver qual dos botões foi precionado
    
    if(digitalRead(BUTTONPLUS) == HIGH){ //Botão +
      raio++;
    }
    
    if(digitalRead(BUTTONMINUS) == HIGH){ //Botão -
      raio--;
    }
    
    if(digitalRead(BUTTONBACK) == HIGH){ //Botão <
      raio /= 10;
      raio = int(raio);
      inputState--;
    }
    
    if(digitalRead(BUTTONNEXT) == HIGH && inputState < 4){ //Botão > e o numero ainda não esta todo escrito
      raio *= 10;
      inputState++;
    }
    else if(digitalRead(BUTTONNEXT) == HIGH && inputState == 4 ) {
      inputState++;
    }
    else if(digitalRead(BUTTONNEXT) == HIGH && inputState == 5){ //Botão > e o numero já esta todo escrito
      inputState++;
    }
    else if(digitalRead(BUTTONNEXT) == HIGH && inputState == 6){ //Botão > e o numero já esta todo escrito
      raio/=100;
      inputState++;
    }
    
    //Estamos no Fim do Loop vamos imprimir a info ta e qual como a temos
    if(inputState < 5){
      // Print a message to the LCD.
      lcd.setCursor(0, 0);
      lcd.print("Raio da Roda: ");
      lcd.print(inputState);
      lcd.setCursor(0, 1);
      lcd.print(raio);
      lcd.print("mm");
    }
    else if(inputState == 5){
      lcd.setCursor(0, 0);
      lcd.print("Raio: ");
      lcd.print(raio);
      lcd.print("mm");
      lcd.setCursor(0, 1);
      lcd.print("NEXT:> BACK:<");
    }

    delay(200);
  }
  
  //Obtem estado do sensor agora
  estado = digitalRead(SENSORPIN);

  //Obtem o tempo de agora
  tempoF = millis();

  //se o estado que lemos agora for High significa que começamos agora uma volta
  if(estado == LOW && podeDarVolta == 1){
    
    //prepara o deltaT
    deltaT = deltatempo(tempoI, tempoF)/1000;

    //calcula a velocidade
    vel = (2 * 3.1415 * raio)/deltaT;
    //velocidade em km/h
    vel = vel * 3.6;
    //distancia em m
    dist += (2 * 3.1415 * raio);

    voltas++;
    
    //reinicia o ciclo de tempo
    tempoI = tempoF;
    podeDarVolta == 0;
  }
  else if(deltatempo(tempoI, tempoF) >= 10000){ //Já não detetamos sinal à pelo menos 10 seg. Ou temos mesmo pouca sorte ou a bike esta parada
    //é tudo zero
    deltaT = 0;
    vel = 0;
  }
  else if(podeDarVolta == 0){ //Já não detetamos sinal à pelo menos 10 seg. Ou temos mesmo pouca sorte ou a bike esta parada
    podeDarVolta == 1;
    delay(50);
  }

  //limpa o lcd
  lcd.clear();
  
  // Print a message to the LCD.
  lcd.setCursor(0, 0);
  lcd.print("Dist: ");
  lcd.print(dist);
  lcd.print(" m");
  lcd.setCursor(0, 1);
  lcd.print("Vel: ");
  lcd.print(vel);
  lcd.print(" km/h");

  //Imprime no Serial
  Serial.print(raio);
  Serial.print("\t");
  Serial.print(tempoF);
  Serial.print("\t");
  Serial.print(deltaT);
  Serial.print("\t");
  Serial.print(dist);
  Serial.print("\t\t");
  Serial.println(vel);
  
  
  /*if (!SD.begin(4)) {
     Serial.println("initialization failed!");
     return;
  }
 
  Serial.println("initialization done.");
  
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("data.txt", FILE_WRITE);
  
  // Se a abertura correr bem, escreve:
  if(myFile) {
    Serial.print("Escrever no test.txt...");
    myFile.println("testing 1, 2, 3.");
    myFile.print("Volta: ");
    myFile.print(voltas);
    myFile.print("\tDeltaT: ");
    myFile.print(deltaT);
    myFile.print("seg");
    myFile.print("\tVelocidade: ");
    myFile.print(vel);
    myFile.println( "m/s");
    // fecha o ficheiro:
    myFile.close();
    Serial.println("done.")
  } else {;
    // Se não abrir da um erro no LCD:
    Serial.println("error opening data.txt");
  }*/

  delay(50);
}
