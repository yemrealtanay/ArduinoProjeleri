#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

String agAdi= "TP-Link_91AC";
String agSifresi= "Yunusemrealtanay";

int rxPin = 11;
int txPin = 10;

String ip= "184.106.153.149";

#define DEBUG true

SoftwareSerial esp(rxPin, txPin);
LiquidCrystal_I2C lcd(0x27, 16, 2);


void setup() {
  // put your setup code here, to run once:
  lcd.begin(0x27, 16, 2);
  Serial.begin(9600);
  Serial.println("Başlatılıyor");

  esp.begin(115200);

  //esp.begin(9600);

  esp.println("AT");

  Serial.println("AT gönderildi");

  while(!esp.find("OK")){
    esp.println("AT");
    Serial.println("ESP ile iletişim kurulamadı");
  }

  esp.println("AT+UART_DEF=9600,8,1,0,0"); // ilk seferdre bir kere çalıştırılacak...

  Serial.println("OK komutu alındı");

  esp.println("AT+CWMODE=1");

  while(!esp.find("OK")){
    esp.println("AT+CWMODE=1");
    Serial.println("Modül ayarı yapılıyor");
  }

  Serial.println("Modül client olarak ayarlandı...");

  Serial.println("Ağa bağlanıyor");

  esp.println("AT+CWJAP=\""+agAdi+"\",\""+agSifresi+"\"");

  while(!esp.find("OK"));

  Serial.println("Ağa bağlanıldı");

  delay(1000);

  lcd.clear();
  lcd.home();

}

void loop() {
  // put your main code here, to run repeatedly:

  esp.println("AT+CIPSTART=\"TCP\",\""+ip+"\",80");
  if(esp.find("Error")){
    Serial.println("AT+CIPSTART Error");
  }

  lcd.home();
  lcd.print("ALTIN CANLI GR");
  lcd.setCursor(4, 1);
  lcd.print(altin());
  Serial.println("Bağlantı Kapatıldı");
  esp.println("AT+CIPCLOSE");
  delay(1000);
  

}

String altin(){
  String rest ="AT+CIPSEND=90";

  rest += "\r\n";

  sendData(rest, 2000, 0);

  String hostt = "GET /apps/thinghttp/send_request?api_key=6SRHG10ZBFUFZRFC";

  hostt += "\r\n";

  hostt += "Host:api.thingspeak.com";

  hostt += "\r\n\r\n\r\n\r\n\r\n";

  String Altin = sendData(hostt, 2000, 1);

  int baslangic = Altin.indexOf(':');
  Altin = Altin.substring(baslangic+1,baslangic+9);

  return (Altin);
    
  }

String sendData(String komut, const int zamangecen, boolean debug){

  String response = "";

  esp.print(komut);

  long int Zaman = millis();

  while((Zaman + zamangecen) > millis()) {
    while(esp.available()) {
      char c = esp.read();

      response += c;
    }
  }

  if(debug){
    Serial.print(response);
  }

  return response;
}
