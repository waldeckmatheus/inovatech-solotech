//Libraries
#include <SPI.h>
#include <Ethernet.h>
#include <DHT.h>

//Constants
#define P_DHT 7     //Pino utilizado para coletar dados do sensor
#define DHTTYPE DHT22   // Sensor DHT 22  (AM2302)

//dados do RELÉ (Relay module - 1 channel)
#define P_RM_1 6

//Inicializador DHT
DHT dht(P_DHT, DHTTYPE); //sensor pino e tipo.

//MAC Address a ser utilizado
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEC
};
//IP a ser utilizado
IPAddress ip(192, 168, 0, 121);
//---------------------------------------------------------------
//Inicializa a biblioteca Ethernet server na porta 80
EthernetServer server(80);

//Dados do sensor YL-69
int yl69dataSensor;
float percentualUmidade;
 
//Dados do sensor DHT22
float umid;  //Dados da umidade
float temp; //Dados da temperatura

void inicializar(){
  //Serial.print("Inicializando server...\n");
  server.begin();
  //Serial.print("Server iniciado.\n");
  //Serial.print("Inicializando sensor DHT22.\n");
  dht.begin();
  //Serial.print("Sensor DHT22 inicializado.\n");
  
  pinMode(P_RM_1, OUTPUT); 
}

void verificacoesIniciais(){
  //Verificação de existência do hardware
  //Serial.print("Verificando hardware ethernet...\n");
   if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    //Serial.print("Desculpe, não é possível rodar sem o hardware Ethernet shield.\n");
    while (true) {
      delay(1);
    }
    
  }
  else {
    //Serial.print("Hardware ethernet OK...\n");
  }
  //Serial.print("Verificando LINK...\n");
  if (Ethernet.linkStatus() == LinkOFF) {
    //Serial.print("Verifique o cabeamento.\n");
  }
  else {
    //Serial.print("Link OK.");
  }
}
void configurarIpMacAddress(){
  // Configura e inicializa
  //Serial.print("Inicializando module Ethernet...\n");
  Ethernet.begin(mac, ip);
  //Serial.print("Module Ethernet inicializado.\n");
}

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  configurarIpMacAddress();
  verificacoesIniciais();
  inicializar();
}
void realizarLeituraSensorDHT22(){
    //Realização de leitura do DHT22 e atribuição nas variáveis "umid" e "temp"
    //Serial.print("Realizando leitura do sensor DHT22...\n");
    umid = dht.readHumidity();
    temp= dht.readTemperature();
    
    //Serial.print("Humidity: ");
    //Serial.print(umid);
    //Serial.print(" %, Temp: ");
    //Serial.print(temp);
    //Serial.print(" Celsius\n");
}
void realizarLeituraSensorYl69(){
    //Serial.print("Realizando leitura do sensor yl69: \n");
    //Serial.print("Realizando leitura do sensor YL-69...");
    yl69dataSensor = analogRead(A0);
    //Serial.print(yl69dataSensor);
  }
  void adicionarRespostaRequisicao(EthernetClient client, float percentualUmidade){
    //Serial.print("Adicionando resposta da requisição...\n");
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close"); // A conexão fechará após completar a resposta
    client.println("Refresh: 5"); // Refresh da página
    client.println();
    client.println("<!DOCTYPE HTML>");

    client.print("<html>");
    client.print("");
    client.print("  <head>");
    client.print("    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">");
    client.print("    <link href=\"http://192.168.0.102/solotech/resources/css/style.css\" rel=\"stylesheet\" type=\"text/css\">");
    client.print("  </head>");
    client.print("  <body>");
    client.print("    <div class=\"container-1\">");
    client.print("       <div class=\"header\">");
    client.print("          <h3>INOVATECH - SoloTech - Controle da umidade do solo.</h>");
    client.print("       </div>");
    client.print("       <div class=\"container-2\">");
    client.print("          Temperatura e umidade no ambiente: ");
    client.print(temp);
    client.print("(ºC) e ");
    client.print(umid);
    client.print("%<br/>");
    client.print("          Umidade no solo: ");
    client.print(percentualUmidade);
    client.print("%");
    client.print("          <br/><span id=\"mensagem\"></span>");
    client.print("        </div>");
    client.print("        <br />");
    client.print("        <div class=\"footer\">");
    client.print("          <span><b>Orientadores:</b> <label style=\"color: blue\" id=\"orientadores\"></label></span>");
    client.print("          <h4>Equipe:</h4> ");
    client.print("<textarea id=\"equipe\" row=\"10\" col=\"10\" value=\"a\" class=\"textarea\" disabled></textarea></li>");
    client.print("        </div>");
    client.print("        <input id=\"yl69data\" type=\"text\" value=\"");
    client.print(yl69dataSensor);
    client.print("\">");
    client.print("<script src=\"http://192.168.0.102/solotech/resources/js/script.js\"></script>");
    client.print("</body></html>");
  }
void loop() {
  //Aguardando conexões
  EthernetClient client = server.available();
    if (client) {
     realizarLeituraSensorYl69();
     realizarLeituraSensorDHT22();
     if (yl69dataSensor<750){
          digitalWrite(P_RM_1,LOW);
        }
        else{
          digitalWrite(P_RM_1,HIGH);
      }
    //Serial.print("Cliente conectado.");
    // A requisição HTTP termina com uma linha em branco
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        
        char c = client.read();
        
        //Verifica se leitura chegou ao fim por meio da verificação da quebra de linha
        if (c == '\n' && currentLineIsBlank) {

          //100% <-> 1024
          //x    <-> 750
          //x=750*100/1024
          //x = 73.2421875‬ % (seco)
          //100.0%-73.2421875 = 26.7578125‬% (úmido)

          //Serial.print("Calculando percentual de umidade do solo...\n");
          percentualUmidade = 100.0 - (yl69dataSensor / 1024.0) * 100;
          String percentualUmidadeSoloTxt = "Percentual de umidade do solo: "+String(percentualUmidade)+"\n";
          //Serial.print(percentualUmidadeSoloTxt);

          adicionarRespostaRequisicao(client,percentualUmidade);
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    //Delay para realizar próxima verificação
    delay(1);
    // close the connection:
    client.stop();
    //Serial.print("client disconnected");
  }
}
