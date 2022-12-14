#include <Adafruit_Fingerprint.h>

const uint32_t password = 0x0; 

Adafruit_Fingerprint fingerSensor = Adafruit_Fingerprint(&Serial2, password);

#define RELE_PIN       18
#define ACCESS_DELAY    5000 // Keep lock unlocked for 5 seconds

int contador[120];

void setup()
{
    Serial.begin(115200);                                   // Inicializa o Serial
    fingerSensor.begin(57600);                              // Inicializa o Sensor
    if(!fingerSensor.verifyPassword()){                     // Verifica se não é a senha do sensor
        Serial.println("Não foi possível conectar ao sensor. Verifique a senha ou a conexão");
        while(true){};                                      // Loop Infinito
    }
  
  pinMode(RELE_PIN, OUTPUT);
  digitalWrite(RELE_PIN, HIGH);   //Switch off relay initially. Relay is LOW level triggered relay so we need to write HIGH.

}

void loop(){

  if (getFingerPrint() != -1 && contador[getFingerPrint()] != 1)
  {
    digitalWrite(RELE_PIN, LOW);
    delay(ACCESS_DELAY);
    digitalWrite(RELE_PIN, HIGH);
    contador[getFingerPrint()] = {1};
  }

  delay(50);            //Add some delay before next scan.
}

// returns -1 if failed, otherwise returns ID #
int getFingerPrint() 
{
  int p = fingerSensor.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = fingerSensor.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = fingerSensor.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!

  return fingerSensor.fingerID;
}
