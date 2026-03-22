#include <EtherCard.h>
#include <IPAddress.h>

#define STATIC 1           // set to 1 to disable DHCP (adjust myip/gwip values below)
#define REQUEST_RATE 5000  // milliseconds
#define LISTEN_PORT 6000
#define OUT_PIN 7
#define UDP_TX_PACKET_MAX_SIZE 100
char* packetBuffer;//[UDP_TX_PACKET_MAX_SIZE];  //buffer to hold incoming packet,
int txmode;                                 // set to 1 when transmitting, 0 when not
int txtime;                                 // counts up while transmitting for timeout detection

#if STATIC
// Ethernet interface IP address
static byte myip[] = { 192, 168, 1, 240 };
// Gateway IP address
static byte gwip[] = { 192, 168, 1, 1 };
#endif

// Ethernet MAC address - must be unique on your network
static byte mymac[] = { 0x70, 0x69, 0x69, 0x2D, 0x30, 0x31 };

byte Ethernet::buffer[500];  // TCP/IP send and receive buffer

// Callback that prints received packets to the serial port
void udpSerialPrint(uint16_t dest_port, uint8_t src_ip[IP_LEN], uint16_t src_port, const char *data, uint16_t len) {
  IPAddress src(src_ip[0], src_ip[1], src_ip[2], src_ip[3]);
  Serial.print("dest_port: ");
  Serial.println(dest_port);
  Serial.print("src_port: ");
  Serial.println(src_port);
  Serial.print("src_ip: ");
  ether.printIp(src_ip);
  Serial.println();
  Serial.print("data: ");
  Serial.println(data);
packetBuffer = data;
    Serial.print("Buffer = ");
    Serial.println(packetBuffer);
    if ((packetBuffer[0] == 'L') && (packetBuffer[1] == '1')) {
      digitalWrite(OUT_PIN, HIGH);
      txmode = 1;
    }

    if ((packetBuffer[0] == 'L') && (packetBuffer[1] == '0')) {
      digitalWrite(OUT_PIN, LOW);
      txmode = 0;
      txtime = 0;
    }
      if (txmode) {
    if (txtime++ >= 5000) {
      digitalWrite(OUT_PIN, LOW);
      txmode = 0;
      txtime = 0;
    }
  }
  }

void setup() {
  Serial.begin(9600);
  Serial.println(F("\n[backSoon]"));
  // Change 'SS' to your Slave Select pin if you aren't using the default pin
  if (ether.begin(sizeof Ethernet::buffer, mymac, SS) == 0)
    Serial.println(F("Failed to access Ethernet controller"));
#if STATIC
  ether.staticSetup(myip, gwip);
#else
  if (!ether.dhcpSetup())
    Serial.println(F("DHCP failed"));
#endif
  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip);
  int txmode;  // set to 1 when transmitting, 0 when not
  int txtime;  // counts up while transmitting for timeout detection

  // Register udpSerialPrint() to port LISTEN_PORT
  ether.udpServerListenOnPort(&udpSerialPrint, LISTEN_PORT);
  // Register udpSerialPrint() to port 42.
  ether.udpServerListenOnPort(&udpSerialPrint, 42);
}

void loop() {
  // This must be called for ethercard functions to work.
  word packetSize = ether.packetLoop(ether.packetReceive());
  delay(2);  // limit loop to 500 interations per second
}
