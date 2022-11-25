//Biblioteca -----------------------------------------------------------------------------------------
#include <Adafruit_Fingerprint.h>                           // Responsável pelo Sensor Biométrico

//Declarações de Variáveis----------------------------------------------------------------------------
const uint32_t password = 0x0;                              // Senha padrão do sensor biométrico

//Objetos --------------------------------------------------------------------------------------------
Adafruit_Fingerprint fingerSensor = Adafruit_Fingerprint(&Serial2, password);

//Funções Auxiliares ---------------------------------------------------------------------------------
void printMenu();                                           // Responsável pelo Menu
String pegaComando();                                       // Responsável por Pegar Comando
void cadastraDigital();                                     // Responsável por Cadastrar Digital
void verificaDigital();                                     // Responsável por Verificar Digital
void contDigital();                                         // Responsável por Contar Digitais
void deleteDigital();                                       // Responsável por Deletar Digital
void limpaDatabase();                                       // Responsável por Limpar Banco de Dados

//Setup ----------------------------------------------------------------------------------------------
void setup(){
    Serial.begin(115200);                                   // Inicializa o Serial
    fingerSensor.begin(57600);                              // Inicializa o Sensor
    if(!fingerSensor.verifyPassword()){                     // Verifica se não é a senha do sensor
        Serial.println("Não foi possível conectar ao sensor. Verifique a senha ou a conexão");
        while(true){};                                      // Loop Infinito
    }
}

//Loop -----------------------------------------------------------------------------------------------
void loop(){
    printMenu();                                            // Chama função printMenu()
    String command = pegaComando();                         // Faz a leitura do comando digitado
    int i = command.toInt();                                // Transforma em inteiro
    switch (i){                                             // Verifica qual o número digitado
    case 1:                                                 // Se i = 1
        cadastraDigital();                                  // Chama Função cadastraDigital()
        break;
    case 2:                                                 // Se i = 2
        verificaDigital();                                  // Chama Função verificaDigital()
        break;
    case 3:                                                 // Se i = 3
        contDigital();                                      // Chama Função contDigital()
        break;
    case 4:                                                 // Se i = 4
        deleteDigital();                                    // Chama Função deletaDigital()
        break;
    case 5:                                                 // Se i = 5
        limpaDatabase();                                    // Chama Função limpaDatabase()
        break;
    default:
        Serial.println("Opção inválida");                   // Imprime na Serial
        break;
    }
    delay(1000);                                            // Espera 1s
}

//Imprime o Menu na Serial ---------------------------------------------------------------------------
void printMenu(){
    Serial.println();
    Serial.println("Digite um dos números do menu abaixo");
    Serial.println("1 - Cadastrar digital");
    Serial.println("2 - Verificar digital");
    Serial.println("3 - Mostrar quantidade de digitais cadastradas");
    Serial.println("4 - Apagar digital em uma posição");
    Serial.println("5 - Apagar banco de digitais");
}

//Pega Comando ---------------------------------------------------------------------------------------
String pegaComando(){
    while(!Serial.available()) delay(100);
    return Serial.readStringUntil('\n');                    // Retorna o que foi digitado na Serial
}

//Cadastra Digital -----------------------------------------------------------------------------------
void cadastraDigital(){
    Serial.println("Qual a posição para guardar a digital? (1 a 149)"); // Imprime na Serial
    String strLocation = pegaComando();                     // Faz a leitura do comando digitado
    int location = strLocation.toInt();                     // Transforma em inteiro

    if(location < 1 || location > 169){                     // Verifica se a posição é invalida
        Serial.println("Posição inválida");                 // Imprime na Serial
        return;
    }

    Serial.println("Encoste o dedo no sensor");             // Imprime na Serial
    while (fingerSensor.getImage() != FINGERPRINT_OK){};    // Espera até pegar uma imagem válida
    
    if (fingerSensor.image2Tz(1) != FINGERPRINT_OK){        // Converte a imagem para primeiro padrão
        Serial.println("Erro image2Tz 1");                  // Erro de Imagem
        return;
    }
    
    Serial.println("Tire o dedo do sensor");                // Imprime na Serial
    delay(2000);                                            // Espera 2s
    while (fingerSensor.getImage() != FINGERPRINT_NOFINGER){}; // Espera até tirar o dedo
    Serial.println("Encoste o mesmo dedo no sensor");       // Imprime na Serial
    while (fingerSensor.getImage() != FINGERPRINT_OK){};    // Espera até pegar uma imagem válida

    if(fingerSensor.image2Tz(2) != FINGERPRINT_OK){         // Converte a imagem para o segundo padrão
        Serial.println("Erro image2Tz 2");                  // Erro de Imagem
        return;
    }

    if(fingerSensor.createModel() != FINGERPRINT_OK){       // Cria um modelo com os dois padrões
        Serial.println("Erro createModel");                 // Erro para criar o Modelo
        return;
    }

    if(fingerSensor.storeModel(location) != FINGERPRINT_OK){// Guarda o modelo da digital no sensor
        Serial.println("Erro já existe uma biométrica cadastrada"); // Já existe 
        return;
    }

    Serial.println("Sucesso!!!");                           // Guardada com sucesso
}

//Verifica Digital -----------------------------------------------------------------------------------
void verificaDigital(){
    Serial.println("Encoste o dedo no sensor");             // Imprime na Serial
    while (fingerSensor.getImage() != FINGERPRINT_OK){};    // Espera até pegar uma imagem válida

    if (fingerSensor.image2Tz() != FINGERPRINT_OK){         // Converte a imagem para utilizar
        Serial.println("Erro image2Tz");                    // Imprime na Serial
        return;
    }

    if (fingerSensor.fingerFastSearch() != FINGERPRINT_OK){ // Procura pela digital no banco de dados
        Serial.println("Digital não encontrada");           // Imprime na Serial
        return;
    }

    Serial.print("Digital encontrada com confiança de ");   // Se chegou aqui a digital foi encontrada
    Serial.print(fingerSensor.confidence);                  // Imprime a confiança quanto maior melhor
    Serial.print(" na posição ");                           // Imprime a posição da digital
    Serial.println(fingerSensor.fingerID);                  // Imprime a posição da digital
}

//Conta Qtde de Digitais -----------------------------------------------------------------------------
void contDigital(){
    fingerSensor.getTemplateCount();                        // Atribui a "templateCount" a qtde
    Serial.print("Digitais cadastradas: ");                 // Imprime na Serial
    Serial.println(fingerSensor.templateCount);             // Imprime na Serial a quantidade salva
}

//Deleta uma Digital ---------------------------------------------------------------------------------
void deleteDigital(){
    Serial.println("Qual a posição para apagar a digital? (1 a 149)");  // Imprime na Serial
    String strLocation = pegaComando();                     // Faz a leitura do comando digitado
    int location = strLocation.toInt();                     // Transforma em inteiro

    if(location < 1 || location > 149){                     // Verifica se a posição é invalida
        Serial.println("Posição inválida");                 // Imprime na Serial
        return;
    }

    if(fingerSensor.deleteModel(location) != FINGERPRINT_OK){ // Apaga a digital na posição location
        Serial.println("Erro ao apagar digital");           // Imprime na Serial
    }
    else{
        Serial.println("Digital apagada com sucesso!!!");   // Imprime na Serial
    }
}

//Limpa Banco de Dados -------------------------------------------------------------------------------
void limpaDatabase(){
    Serial.println("Tem certeza? (S/N)");                   // Imprime na Serial
    String command = pegaComando();                         // Faz a leitura do comando digitado
    command.toUpperCase();                                  // Coloca tudo em maiúsculo

    if(command == "S" || command == "SIM"){                 // Verifica se foi digitado "S" ou "SIM"
        Serial.println("Apagando banco de digitais...");    // Imprime na Serial

        if(fingerSensor.emptyDatabase() != FINGERPRINT_OK){ // Apaga todas as digitais
            Serial.println("Erro ao apagar banco de digitais"); // Imprime na Serial
        }
        else{
            Serial.println("Banco de digitais apagado com sucesso!!!"); // Imprime na Serial
        }
    }
    else{
        Serial.println("Cancelado");                        // Imprime na Serial
    }
}
