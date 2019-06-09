//Libraries
#include <SPI.h>
#include <Ethernet.h>
#include <DHT.h>

//Constants
#define DHTPIN 7     //Pino utilizado para coletar dados do sensor
#define DHTTYPE DHT22   // Sensor DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //Inicializador do sensor pino e tipo.

//MAC Address a ser utilizado
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF
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
int chk;
float umid;  //Dados da umidade
float temp; //Dados da temperatura

void verificacoesIniciais(){
  //Verificação de existência do hardware
   if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Desculpe, não é possível rodar sem o hardware Ethernet shield.");
    while (true) {
      delay(1);
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Verifique o cabeamento.");
  }
}
void configurarIpMacAddress(){
  // Configura e inicializa
  Ethernet.begin(mac, ip);
}
void inicializar(){
  server.begin();
  dht.begin();
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
    umid = dht.readHumidity();
    temp= dht.readTemperature();
    
    //Serial.print("Humidity: ");
    //Serial.print(umid);
    //Serial.print(" %, Temp: ");
    //Serial.print(temp);
    //Serial.println(" Celsius");
}
void realizarLeituraSensorYl69(){
    //Serial.println("Realizando leitura do sensor yl69: ");
    yl69dataSensor = analogRead(A0);
    //Serial.println(yl69dataSensor);
  }
  void adicionarRespostaRequisicao(EthernetClient client, float percentualUmidade){
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close"); // A conexão fechará após completar a resposta
    //client.println("Refresh: 2"); // Refresh da página
    client.println();
    client.println("<!DOCTYPE HTML>");

    client.print("<html>");
    client.print("");
    client.print("  <head>");
    client.print("    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">");
    client.print("    <link href=\"http://solotech/resources/css/style.css\" rel=\"stylesheet\" type=\"text/css\">");
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
    client.print("          <span><b>Orientadores:</b> <label style=\"color: blue\"> ODAIR CRISTIANO ABREU DA SILVA; RONEI NUNES RIBEIRO;</label></span>");
    client.print("          <h4>Equipe:</h4> ");
    client.print("<textarea id=\"equipe\" row=\"10\" col=\"10\" value=\"a\" class=\"textarea\" disabled></textarea></li>");
    client.print("        </div>");
    client.print("        <input id=\"yl69data\" type=\"text\" value=\"");
    client.print(yl69dataSensor);
    client.print("\">");
    client.print("<script src=\"http://solotech/resources/js/script.js\"></script>");
    client.print("</body></html>");
  }
void loop() {
  //Aguardando conexões
  EthernetClient client = server.available();
  if (client) {
    Serial.println("Cliente conectado.");
    // A requisição HTTP termina com uma linha em branco
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        
        char c = client.read();
        
        //Verifica se leitura chegou ao fim por meio da verificação da quebra de linha
        if (c == '\n' && currentLineIsBlank) {
          realizarLeituraSensorYl69();
          percentualUmidade = 100.0 - (yl69dataSensor / 1024.0) * 100;
          realizarLeituraSensorDHT22();
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
    //Serial.println("client disconnected");
  }
}
