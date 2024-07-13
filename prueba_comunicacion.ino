//using data = uint8_t;
typedef uint8_t data;
///***ASIGNACIONES DE PINES***///

//Motores
const uint8_t n = 2; //numero de motores/puertas activas (maximo 225)
const data PinesEnable[n] = {10, 11}; //motorA: 10 | motorB: 11
const data PinesDir1[n] = {2, 4}; //Dir1A: 2 | Dir1B: 4
const data PinesDir2[n] = {3, 5}; //Dir2A: 3 | Dir2B: 5
const data PinesPuertas[n] = {6, 7}; //Sesores de puertas

///***DECLARACIÓN DE VARIABLES***///
data flagComandoDes = 0b00000000; //Flag Comando enviado por la aplicación (1:Asegurar pueta | 0:Liberar puerta)
data flagComandoAct = 0b00000011; //Flag Comando actual del mecanismo (1:Asegurar pueta | 0:Liberar puerta)
data flagCambioComando = 0b00000000; //Flag que muestra si ha habido un cambio en algún comando
data confirmacionComando = 0b00000000;//Flag qeu confirma la posición 

data estPuerta = 0b00000000; //Flag Estado de puerta (1:puerta abierta | 0:puerta cerrada)
data tramaDir1 = 0b00000000; //Trama de dirección 1 para todos los motores
data tramaDir2 = 0b00000000; //Trama de dirección 2 para todo los motores

const uint8_t velTrabajo = 75; //Velocidad de trabajo de los motores
data lecComando = 0b00000000; //Variable que almacena la información enviada por la aplicación (se debe procesar)
data tempa = 0b00000000; //Variable auxiliar
data tempb = 0b00000000; //Variable auxiliar
String tempc;

//Funciones usadas
bool cambioComando(){
  if (flagComandoDes == flagComandoAct){
    return false;
  }else{
    return true;
  }
}

void imprimirSerial(String mensaje,bool ln = false){
   if(Serial.available() != 0){
      if(ln == false){
        Serial.print(mensaje);
      }else{
        Serial.println(mensaje);
      }
   }
}

void string2comando(String palabra){
  char tempchar;
  for (int i=0; i<tempc.length(); i++){
      tempchar = tempc[i];
      if (tempchar == '1'){
        tempa = 0b00000001;
      }else{
        tempa = 0b00000000;
      }
      tempa = tempa << i;
      tempb = tempb | tempa;
    }
    
  	flagComandoDes = tempb;
    tempa = 0b00000000;
    tempb = 0b00000000;
}


uint8_t readBit(uint8_t num, uint8_t nbit){
  /*Funcion que lee un determinado bit en un byte
  	- Imputs:
      |> byte: nombre de byte a leer
      |> nbit: # de bit (0-leng(data)-1)
    - Output:
      |> uint8_t donde el LSB posee el valor del bit leido
  */
  uint8_t resp = 0;
  resp = num >> nbit;
  resp = resp & 0b00000001;
  return (resp);
}

uint8_t writeBit(uint8_t num, uint8_t nbit, bool val){
  /*Funcion que lee un determinado bit en un byte
  	- Imputs:
      |> byte: nombre de byte a leer
      |> nbit: # de bit
      |> val: valor a escribir (0, 1)
      
    - Output:
      |> variable de tipo data con el bit escrito
  */
  uint8_t tempA = 0;
  uint8_t resp = 0;
  if (val == 1) {
    tempA = val << nbit;
    resp = num | tempA;
    
  }else{
    if (readBit(num, nbit) == 1){
      tempA = 1;
      tempA = tempA << nbit;
      resp = num - tempA;
    }else{
      resp = num;
    }
  }
  return(resp);
}

///***DESIGNACIÓN DE PINES***///
void setup() {
  for(int i = 0; i<=n-1; i++){
    pinMode(PinesDir1[i], OUTPUT); //Pines de dirección1 como salida
    pinMode(PinesDir2[i], OUTPUT); //Pines de dirección2 como salida
    pinMode(PinesEnable[i], OUTPUT); //pines de enable como salida
    
    pinMode(PinesPuertas[i], INPUT); //Pines asociados a los sensores de puertas como salida
  }

  Serial.begin(9600);
}

///****BUCLE PRINCIPAL****///
void loop() {
  //Serial.println("==================================");
  
  //Lectura de los estado de puerta:
  /*
  for(int i = 0; i<n;i++){
    tempa = 0b00000000;
    Serial.println(digitalRead(PinesPuertas[i]));
    tempa = digitalRead(PinesPuertas[i]); //Leo el pin correspondiente a la puerta i    
    tempa = tempa & 0b00000001;
    tempa = tempa << i; //Desplazo la lectura a la posición de byte correspondiente
    tempb = tempb|tempa; //Acumulo las lecturas un variable temporal
  }*/
  
  //leo puerta 2
  tempa = digitalRead(PinesPuertas[1]);
  tempa = tempa << 1;
  //Leo puerta 1
  tempb = digitalRead(PinesPuertas[0]);
  
  //Sumo lecturas
  estPuerta = tempb | tempa; //guardo las lecturas en el flag correspondiente
  
  tempb = 0b00000000; //reseteo variables temporales
  tempa = 0b00000000;
  
    
  
  //IMPRESIÓN DE "RESULTADOS"
  //Serial.print("Estado de puertas: ");
  //Serial.println(estPuerta);

  
  //Lectura de comandos de sensor
  if(Serial.available() != 0){
    tempc = Serial.readStringUntil('\n'); //Leo el puerto serial
    //Serial.println("==================================");
    //Serial.println("Orden Recibida");
    Serial.print("Comando recibido: ");
    Serial.println(tempc);
    
    string2comando(tempc); //proceso la lectura para actualizar flagComandoDes
    
    //Serial.print("Comando procesado: ");
    //Serial.println(flagComandoDes);    
  }
  
  
  //Serial.print("ComandoDes: ");
  //Serial.println(flagComandoDes);
  
  
  // Verificar estado de puerta
  
  if (readBit(flagComandoAct,0) == 0 & readBit(flagComandoDes,0) == 1 & readBit (estPuerta,0) == 1) {
  	Serial.println("No se puede bloquear puerta 1.Cierrala.");
    flagComandoDes = writeBit(flagComandoDes,0,0);
    Serial.print("Nuevo Comando Deseado: ");
    Serial.println(flagComandoDes);  
  }
  
  if (readBit(flagComandoAct,1) == 0 & readBit(flagComandoDes,1) == 1 & readBit (estPuerta,1) == 1) {
  	Serial.println("No se puede bloquear puerta 2.Cierrala.");
    flagComandoDes = writeBit(flagComandoDes,1,0);
    Serial.print("Nuevo Comando Deseado: ");
    Serial.println(flagComandoDes);  
  }
  
  // Cambio de direcciones de acuerdo a flags
    
  flagCambioComando = flagComandoDes^flagComandoAct;
  //Serial.print("Cambio comando: ");
  //Serial.println(flagCambioComando);
  
  tramaDir1=flagComandoDes;
  tramaDir2=~flagComandoDes;
  
  for(int i = 0; i < n; i++){
    //Asignación cambio a pings
    tempa = readBit(tramaDir1, i);
    tempb = readBit(tramaDir2, i);
    digitalWrite(PinesDir1[i], tempa);
  	digitalWrite(PinesDir2[i], tempb);
    
    //habilitar movimeinto de motores de acuerdo al flag de cambios
    if(readBit(flagCambioComando,i)){
      analogWrite(PinesEnable[i], velTrabajo);
      writeBit(confirmacionComando, i, readBit(flagComandoDes,i));
    }
  }
  
  //Serial.print("ComandoActual: ");
  //Serial.println(flagComandoAct);
  
  //Apagado de motores
  delay(2000);
  for(int i = 0; i < n; i++){
    //habilitar movimeinto de motores
    if(readBit(flagCambioComando, i)){
      analogWrite(PinesEnable[i], 0);
    }
  }
  
  flagComandoAct = flagComandoDes;
}