//https://devgirlunderscore.medium.com/how-to-use-stellarium-http-api-f0942d2c8820
//https://stellarium.org/doc/23.0/remoteControlApi.html

#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

//Tasks
void TaskClique(void *pvParameters);
void TaskAstro(void *pvParameters);
void TaskMotorAl(void *pvParameters);
void TaskMotorAz(void *pvParameters);

//Motor Az
const int PINDirAz = 17;
const int PINStepAz = 16;
const int PINEnableAz = 15;
const int PINMS1Az =2;
const int PINMS2Az = 0;
const int PINMS3Az = 4;
const int steps_pv_Az = 35200;
double az = 0;
int MeioPeriodoAz=1;
long passosAz = 0;

//Motor Al
const int PINDirAl = 33;
const int PINStepAl = 25;
const int PINEnableAl = 12;
const int PINMS1Al =14;
const int PINMS2Al = 27;
const int PINMS3Al = 26;
const int steps_pv_Al = 24000;
double al = 0;
int MeioPeriodoAl=5;
long passosAl = 0;

//Wifi
const char* ssid = "wifizinho"; //Nome do seu Wifi
const char* password = "senha123"; //Senha do Wifi
String astro = "";
String server_ip = "192.168.11.154"; //Infelizmente, voce ainda tem que colocar o IP do computador toda vez antes de usar o telescopio
String url = "http://"+server_ip+":8090/api/objects/info?name="+astro+"&format=json";
String url_clique = "http://"+server_ip+":8090/api/main/status?actionId=0&propId=0&format=json";
bool connectWifi();
bool wifiConnected = false;

//Http
HTTPClient http_clique, http;
String payload;

void setup() {
  // Motor Az
  pinMode(PINDirAz,OUTPUT);
  pinMode(PINStepAz,OUTPUT);
  pinMode(PINMS1Az,OUTPUT);
  pinMode(PINMS2Az,OUTPUT);
  pinMode(PINMS3Az,OUTPUT);
  pinMode(PINEnableAz,OUTPUT);
  digitalWrite(PINEnableAz,LOW);
  digitalWrite(PINMS1Az,HIGH);
  digitalWrite(PINMS2Az,HIGH);
  digitalWrite(PINMS3Az,HIGH);

  // Motor Al
  pinMode(PINDirAl,OUTPUT);
  pinMode(PINStepAl,OUTPUT);
  pinMode(PINMS1Al,OUTPUT);
  pinMode(PINMS2Al,OUTPUT);
  pinMode(PINMS3Al,OUTPUT);
  pinMode(PINEnableAl,OUTPUT);
  digitalWrite(PINEnableAl,LOW);
  digitalWrite(PINMS1Al,HIGH);
  digitalWrite(PINMS2Al,HIGH);
  digitalWrite(PINMS3Al,HIGH);
  
  Serial.begin(115200);

  wifiConnected = connectWifi();

  while (!wifiConnected) {
    Serial.println("Cannot connect to WiFi. Please check data and reset the ESP.");
    delay(2000);
    wifiConnected = connectWifi();
  }

  http_clique.begin(url_clique.c_str()); //Specify the URL and certificate
  http_clique.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

  // Task Config
  xTaskCreatePinnedToCore(
    TaskClique
    ,  "TaskClique"
    ,  10000
    ,  NULL
    ,  1
    ,  NULL
    ,  1); //0 ou 1
    
  xTaskCreatePinnedToCore(
    TaskAstro
    ,  "TaskAstro"
    ,  5000
    ,  NULL
    ,  1
    ,  NULL
    ,  1); //0 ou 1
    
  xTaskCreatePinnedToCore(
    TaskMotorAl
    ,  "TaskMotorAl"
    ,  2048
    ,  NULL
    ,  1
    ,  NULL
    ,  1); //0 ou 1
  
  xTaskCreatePinnedToCore(
    TaskMotorAz
    ,  "TaskMotorAz"
    ,  2048
    ,  NULL
    ,  1
    ,  NULL
    ,  1); //0 ou 1
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(10000));
}

void TaskClique(void *pvParameters){
  for(;;) {
    int httpCode = http_clique.GET();
    if (httpCode == 200){
      payload = http_clique.getString();
      Serial.println(payload);
      JSONVar informacao = JSON.parse(payload);//mudar
      if (JSON.typeof(informacao) == "undefined"){
       Serial.println("Parsing input failed!");
      }
      else{
        Serial.println("Parsing input ok!");
        String texto = payload;//informacao["selectioninfo"];
        if (!texto.isEmpty()){
          int ini = texto.indexOf("selectioninfo\":\"<h2>");
          if (ini != -1){
            //Serial.print(ini);
            //Serial.print(" ");
            int fim = texto.indexOf("</h2>",ini);
            if (fim != -1){
              //Serial.print(fim);
              //Serial.print(" ");
              String astro_novo = texto.substring(ini+4+16,fim);
              fim = astro_novo.indexOf(" ");
              if (fim != -1){
                astro_novo = astro_novo.substring(0,fim);
              }
              fim = astro_novo.indexOf("(");
              if (fim != -1){
                astro_novo = astro_novo.substring(0,fim);
              }
              fim = astro_novo.indexOf("<");
              if (fim != -1){
                astro_novo = astro_novo.substring(0,fim);
              }
              //Serial.println(fim);
              //verificar catalogo
              if (astro != astro_novo){
                astro = astro_novo;
                Serial.println(astro);
                http.end();
                url = "http://"+server_ip+":8090/api/objects/info?name="+astro+"&format=json";
                http.begin(url.c_str()); //Specify the URL and certificate
                http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
              }
            }
          }
        }
      }
    }
    else{
      Serial.print("Httpcode invalido: ");
      Serial.println(httpCode);
    }
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void TaskAstro(void *pvParameters){
  for(;;) {
    if (astro != ""){
      int httpCode2 = http.GET();
      if (httpCode2 == 200){
        payload = http.getString();
        JSONVar info_astro = JSON.parse(payload);
        if (JSON.typeof(info_astro) == "undefined"){
          Serial.println("Parsing 2 input failed!");
        }
        else{
          az = info_astro["azimuth"];
          az = 360.0-double(az);
          Serial.print(az);
          Serial.print(" - ");
          al = info_astro["altitude"];
          Serial.println(al);
          al = double(al);
        }
      }
      else{
        Serial.print("Httpcode invalido: ");
        Serial.println(httpCode2);
      }
    }
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

void TaskMotorAl(void *pvParameters){
  for(;;) {
    long passos_desejados = long(al*(double(steps_pv_Al)/360.0));
    if (passos_desejados>passosAl){
      digitalWrite(PINDirAl,LOW);
      digitalWrite(PINStepAl,HIGH);
      delay(MeioPeriodoAl);
      digitalWrite(PINStepAl,LOW);
      passosAl++;
      vTaskDelay(pdMS_TO_TICKS(MeioPeriodoAl));
    }
    else if (passos_desejados<passosAl){
      digitalWrite(PINDirAl,HIGH);
      digitalWrite(PINStepAl,HIGH);
      delay(MeioPeriodoAl);
      digitalWrite(PINStepAl,LOW);
      passosAl--;
      vTaskDelay(pdMS_TO_TICKS(MeioPeriodoAl));
    }
  }
}

void TaskMotorAz(void *pvParameters){
  for(;;) {
    long passos_desejados = long((az)*(double(steps_pv_Az)/360.0));
    if (passos_desejados>passosAz){
      long d1 = passos_desejados-passosAz;
      long d2 = passosAz+(steps_pv_Az-passos_desejados);
      Serial.print(passos_desejados);
      Serial.print(" - ");
      Serial.print(passosAz);
      Serial.print(" - ");
      Serial.print(d1);
      Serial.print(" - ");
      Serial.println(d2);
      if (d1<d2){
          digitalWrite(PINDirAz,LOW);
          digitalWrite(PINStepAz,HIGH);
          delay(MeioPeriodoAz);
          digitalWrite(PINStepAz,LOW);
          passosAz++;
          if(passosAz>(steps_pv_Az-1)){
            passosAz = 0;
          }
          // Serial.println("A");
          vTaskDelay(pdMS_TO_TICKS(MeioPeriodoAz));
          }
        else{
        digitalWrite(PINDirAz,HIGH);
        digitalWrite(PINStepAz,HIGH);
        delay(MeioPeriodoAz);
        digitalWrite(PINStepAz,LOW);
        passosAz--;
        if(passosAz<0){
            passosAz = steps_pv_Az-1;
          }
          // Serial.println("B");
        vTaskDelay(pdMS_TO_TICKS(MeioPeriodoAz));
        }
       }
        else if(passosAz>passos_desejados){
            long d1 = passosAz-passos_desejados;
            long d2 = passos_desejados+(steps_pv_Az-passosAz);
            Serial.print(passos_desejados);
            Serial.print(" - ");
            Serial.print(passosAz);
            Serial.print(" - ");
            Serial.print(d1);
            Serial.print(" - ");
            Serial.println(d2);
            if (d2<d1){
              digitalWrite(PINDirAz,LOW);
              digitalWrite(PINStepAz,HIGH);
              delay(MeioPeriodoAz);
              digitalWrite(PINStepAz,LOW);
              passosAz++;
              if(passosAz>(steps_pv_Az-1)){
                passosAz = 0;
              }
          // Serial.println("C");
              vTaskDelay(pdMS_TO_TICKS(MeioPeriodoAz));
              }
            else {
              digitalWrite(PINDirAz,HIGH);
              digitalWrite(PINStepAz,HIGH);
              delay(MeioPeriodoAz);
              digitalWrite(PINStepAz,LOW);
              passosAz--;
              if(passosAz<0){
                passosAz = steps_pv_Az-1;
              }
          // Serial.println("D");
              vTaskDelay(pdMS_TO_TICKS(MeioPeriodoAz));
              }
             }
     
  }
}

bool connectWifi(){
  bool state = true;
  int i = 0;
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 20){
      state = false; break;
    }
    i++;
  }
  Serial.println("");
  if (state){
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("Connection failed.");
  }
  return state;
}
