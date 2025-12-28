/*
 * ESP32 LED Strip Controller con Rotary Encoder
 * Hardware:
 * - MOSFET (Gate) -> GPIO 26
 * - Encoder CLK   -> GPIO 27
 * - Encoder DT    -> GPIO 14
 * - Encoder SW    -> GPIO 25 (Pin corretto!)
 */

// --- DEFINIZIONE PIN ---
const int PIN_MOSFET = 25;
const int PIN_CLK    = 27;
const int PIN_DT     = 14;
const int PIN_SW     = 26;

// --- PARAMETRI PWM (LEDC) ---
const int PWM_FREQ = 200;      // 5 KHz è buono per i LED
const int PWM_CHANNEL = 0;      // Canale PWM interno (0-15)
const int PWM_RESOLUTION = 8;   // 8 bit = valori da 0 a 255

// --- VARIABILI DI STATO ---
volatile int brightness = 128;  // Luminosità iniziale (metà)
bool isLedOn = false;           // Stato iniziale (spento)
volatile unsigned long lastEncoderTime = 0; // Per debounce rotazione

// Variabili per il pulsante
int lastButtonState = HIGH;     // Pullup interno -> HIGH quando non premuto
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50; // 50ms di ritardo per il pulsante

// --- INTERRUPT ROTAZIONE ---
// Questa funzione viene chiamata automaticamente quando ruoti l'encoder
void IRAM_ATTR handleEncoder() {
  unsigned long currentTime = millis();
  // Semplice debounce: ignora segnali troppo veloci (<5ms)
  if (currentTime - lastEncoderTime > 5) { 
    int dtValue = digitalRead(PIN_DT);
    if (dtValue == HIGH) {
      // Rotazione Oraria (CW) -> Aumenta
      if (brightness < 255) brightness += 5; // Passo di 5 per fare prima
    } else {
      // Rotazione Antioraria (CCW) -> Diminuisci
      if (brightness > 0) brightness -= 5;
    }
    // Correzione limiti per sicurezza
    if (brightness > 255) brightness = 255;
    if (brightness < 0) brightness = 0;
    
    lastEncoderTime = currentTime;
  }
}

void setup() {
  Serial.begin(115200);

  // Configurazione Pin
  pinMode(PIN_CLK, INPUT); // L'encoder ha resistenze fisiche o moduli, usiamo input
  pinMode(PIN_DT, INPUT);
  pinMode(PIN_SW, INPUT_PULLUP); // Fondamentale: abilita resistenza interna per il tasto
  
  // Configurazione PWM per il MOSFET
  // Nota: Su ESP32 core 3.0+ la sintassi cambia leggermente, 
  // questa è quella classica (compatibile con la maggior parte dei core attuali)
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(PIN_MOSFET, PWM_CHANNEL);
  
  // Stato iniziale: Spento
  ledcWrite(PWM_CHANNEL, 0);

  // Attacchiamo l'interrupt al pin CLK
  // Rileva quando il segnale scende (FALLING)
  attachInterrupt(digitalPinToInterrupt(PIN_CLK), handleEncoder, FALLING);
  
  Serial.println("Sistema Avviato. Premi il tasto per accendere.");
}

void loop() {
  // --- GESTIONE PULSANTE (SW) ---
  int reading = digitalRead(PIN_SW);

  // Se lo stato cambia (rumore o pressione)
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  // Se lo stato è stabile da più di 50ms
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Se il tasto è stato effettivamente premuto (LOW) e prima non lo era
    // Nota: usiamo una variabile statica o globale per tracciare lo stato "logico" precedente
    static int buttonState = HIGH; 
    
    if (reading != buttonState) {
      buttonState = reading;
      
      // Agiamo solo quando il tasto viene PREMUTO (LOW)
      if (buttonState == LOW) {
        isLedOn = !isLedOn; // Inverti stato (ON <-> OFF)
        Serial.print("Stato LED: ");
        Serial.println(isLedOn ? "ACCESO" : "SPENTO");
      }
    }
  }
  lastButtonState = reading;

  // --- AGGIORNAMENTO USCITA ---
  // Applichiamo la luminosità solo se il sistema è "Acceso"
  if (isLedOn) {
    ledcWrite(PWM_CHANNEL, brightness);
    Serial.println(brightness);
  } else {
    ledcWrite(PWM_CHANNEL, 0); // Spegni forzatamente (ma mantieni 'brightness' in memoria)
  }
  
  // Piccolo delay per non intasare la CPU
  delay(10);
}