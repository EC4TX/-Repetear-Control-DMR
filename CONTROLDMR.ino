// Control de estado de ventilador de TX y alimentación de emisora de TX del repetidor DMR
// Por EC4TX
// EN EDICIÓN, NO ES FUNCIONAL

#include <SPI.h>
#include <Ethernet.h>
 
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 200);
EthernetServer server(8090);
 
const int pinLed1 = 3;        // Gobierna el ventilador
const int pinLed2 = 4;        // Gobierna on/off emisora TX
const int Sensor = A0 ;      // Pin que lee la temperatura TX
const int Sensor2 = A1 ;      // Pin que lee la temperatura habitaculo
const int ten12 = A2 ;      // Pin que lee la tensión 12V
const int ten5 = A3 ;      // Pin que lee la tensión 5V

int umbral = 37 ;           // Temperatura que arranca el ventilador
int umbral2 = 65;         // Temperatura a la que se apaga la emisora
int index = 0 ;         // Posicion a escribir
const int N = 32 ;     // Numero de muestras a considerar 
float Buffer[N] ;
float Tmedia = 0 ;
 
void setup()
{
  Serial.begin(9600);
 
  Ethernet.begin(mac, ip);
  server.begin();
 
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
 
  pinMode(pinLed1, OUTPUT);
  pinMode(pinLed2, OUTPUT);
  digitalWrite(pinLed1, LOW);
  digitalWrite(pinLed2, LOW);
}
 
void loop()
{
 int lectura = analogRead(Sensor);
       float voltaje = 5.0 /1024 * lectura ;    
       float temp = voltaje * 100 -5 ;
 
 int lectura2 = analogRead(Sensor2);
       float voltaje2 = 5.0 /1024 * lectura2 ;    
       float temp2 = voltaje2 * 100 -5 ;     


 int lectura3 = analogRead(ten12);
       float voltaje3 = 15.0 /1024 * lectura3 ;    
       float volt12 = voltaje3 * 100 -5 ;

 int lectura4 = analogRead(ten5);
       float voltaje4 = 6.0 /1024 * lectura4 ;    
      float volt5 = voltaje4 * 100 -5 ;

//   Buffer[ index] = temp ;
//   index = ++index % N  ;
   

//   for (int i=0 ; i< N ; i++)
//        Tmedia = Tmedia - Buffer[i] ;
               
       Serial.println("Temperatura TX: ");
       Serial.println(temp) ;
       Serial.println("Temperatura Habitaculo: ");
       Serial.println(temp2) ;
       
    //   Serial.println("Tensión +12V: , Tensión +5V:" );
    //   Serial.println(volt12,volt5) ;
     


       if ( temp >= umbral)
           digitalWrite(pinLed1, HIGH);
       else
           digitalWrite(pinLed1, LOW);

       if ( temp >= umbral2)
           digitalWrite(pinLed2, LOW);
       else
           digitalWrite(pinLed2, HIGH);


       delay(200);
       
  EthernetClient client = server.available(); 
  if (client)
  {
    Serial.println("new client");
    bool currentLineIsBlank = true;
    String cadena = "";
    while (client.connected()) 
    {
      if (client.available()) 
      {
        char c = client.read();
        Serial.write(c);
 
        if (cadena.length()<50)
        {
          cadena.concat(c);
 
           // Buscar campo data
          int posicion = cadena.indexOf("data");
          String command = cadena.substring(posicion);
 
          if (command == "data1=0")
          {
            digitalWrite(pinLed1, HIGH);
          }
          else if (command == "data1=1")
          {
            digitalWrite(pinLed1, LOW);
          }
          else if (command == "data2=0")
          {
            digitalWrite(pinLed2, LOW);
          }
          else if (command == "data2=1")
          {
            digitalWrite(pinLed2, HIGH);
          }
        }
 
        // Al recibir linea en blanco, servir página a cliente
        if (c == '\n' && currentLineIsBlank)
        {
          client.println(F("HTTP/1.1 200 OK\nContent-Type: text/html"));
          client.println();
 
          client.println(F("<html>\n<head>\n<title>EC4TX_control_DMR</title>\n</head>\n<body>"));
          client.println(F("<div style='text-align:center;'>"));

          client.println(F("<br/><br/>"));
          client.println(F("<h2>CONTROL REPETIDOR ED4ZAF DMR EC4TX</h2>"));
          
          client.print(F("CONTROL AUTOMATICO = "));
          client.println(digitalRead(pinLed2) == LOW ? "OFF" : "ON");
          client.println(F("<br/>"));
          client.println(F("<button onClick=location.href='./?data1=0'>ON</button>"));
          client.println(F("<button onClick=location.href='./?data1=1'>OFF</button>"));
          client.println(F("<br/><br/>"));
          
          client.print(F("Estado Ventilador TX = "));
          client.println(digitalRead(pinLed1) == LOW ? "OFF" : "ON");
          client.println(F("<br/>"));
          client.println(F("<button onClick=location.href='./?data1=0'>ON</button>"));
          client.println(F("<button onClick=location.href='./?data1=1'>OFF</button>"));
          client.println(F("<br/><br/>"));
        
          client.print(F("Estado Emisora TX = "));
          client.println(digitalRead(pinLed2) == LOW ? "OFF" : "ON");
          client.println(F("<br/>"));
          client.println(F("<button onClick=location.href='./?data2=0'>ON</button>"));
          client.println(F("<button onClick=location.href='./?data2=1'>OFF</button>"));
          client.println(F("<br/>"));
          client.println(F("<br/>"));
          client.println(F("Temperatura emisora de TX = "));
          client.println(temp);
          client.println(F("<br/>"));
          client.println(F("Temperatura Habitaculo = "));
          client.println(temp2);
          client.println(F("<br/>"));
          client.println(F("<br/><br/>"));
          client.println(F("Tension +12V "));
          client.println(volt12);
          client.println(F("<br/>"));
          client.println(F("Tension +5V "));
          client.println(volt5);
          client.println(F("<br/>"));
          client.println(F("<br/><br/>"));
          client.println(F("SWR: "));
          client.println(F("1:1.2")); 
          client.println(F("<br/><br/>"));
          client.println(F("<a href='http://192.168.1.177:8090'>Refrescar</a>"));
          client.println(F("</div>\n</body></html>"));
          break;
        }
        if (c == '\n') 
        {
          currentLineIsBlank = true;
        }
        else if (c != '\r') 
        {
          currentLineIsBlank = false;
        }
      }
    }
 
    delay(1);
    client.stop();
  }
}

