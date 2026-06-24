/*
  HiBREW H10 Plus - controllo pompa pressione v0.2 ultra-fast

  Arduino Nano / ATmega328P

  D2 = MOTO originale dal microcontrollore macchina
  D5 = MOTO verso scheda pompa
  A4 = sensore pressione 0.5-4.5 V, 0-12 bar
  D3 = ZERO collegato ma NON usato per ora

  Strategia:
  - D2 viene letto con INT0 diretto.
  - D5 viene aggiornato con latenza minima.
  - Sotto circa 8.5 bar copia sempre tutto.
  - Vicino a 9 bar inizia a saltare impulsi.
  - Se MOTO originale smette, D5 torna Hi-Z.
  - Niente Serial, niente EEPROM, niente millis/micros.
*/


#define MOTO_IN_PIN      2
#define ZERO_PIN         3
#define MOTO_OUT_PIN     5
#define PRESSURE_PIN     A4


// -------------------- ABILITA / DISABILITA CONTROLLO --------------------
//
// Se vuoi verificare che il codice base copi ancora perfettamente,
// metti false. In quel caso copia sempre D2 -> D5.
const bool ENABLE_PRESSURE_CONTROL = true;


// -------------------- SENSORE PRESSIONE --------------------
//
// Sensore:
// 0.5 V = 0 bar
// 4.5 V = 12 bar
//
// ADC 5 V:
// 0 bar  circa ADC 102
// 9 bar  circa ADC 716
// 12 bar circa ADC 921

const int ADC_0_BAR  = 102;
const int ADC_12_BAR = 921;

const int SENSOR_MAX_CBAR = 1200;  // 12.00 bar

const int ADC_FAULT_LOW  = 40;
const int ADC_FAULT_HIGH = 1000;


// -------------------- TARGET --------------------

const int TARGET_CBAR = 900;             // 9.00 bar

// Sotto questa pressione copia sempre tutto.
// Serve per preinfusione, vapore, salita pressione.
const int FULL_COPY_UNTIL_CBAR = 850;    // 8.50 bar

// Sopra questa pressione taglia molto aggressivo.
const int HARD_CUT_CBAR = 1050;          // 10.50 bar


// -------------------- CONTROLLO DUTY --------------------
//
// dutyPermille:
// 1000 = passa tutti gli impulsi
// 700  = passa circa 70% degli impulsi
// 0    = blocca tutto, salvo safety burst

const int HOLD_DUTY_PERMILLE = 720;

// Quanto reagisce quando siamo sopra/sotto target.
// Più alto = più aggressivo.
// 4 significa circa 4 punti duty per ogni centibar di errore.
const int KP_PER_CBAR = 4;

const int DUTY_MIN = 0;
const int DUTY_MAX = 1000;


// -------------------- FILTRO PRESSIONE --------------------
//
// Più alto = più filtrato/lento.
// 4 = abbastanza reattivo.
// 5 = più morbido se vedi pulsazioni forti.
const uint8_t EMA_SHIFT = 4;


// -------------------- MODULAZIONE IMPULSI --------------------

// Quando decide di far pompare, dà almeno 2 impulsi consecutivi.
const uint8_t MIN_BURST_PULSES = 2;

// Evita di lasciare la pompa completamente ferma troppo a lungo.
// A 100 Hz, 150 impulsi mancati sono circa 1.5 s.
const uint16_t MAX_BLOCKED_PULSES = 150;
const uint8_t SAFETY_BURST_PULSES = 2;
const int SAFETY_BURST_MAX_CBAR = 1000;


// -------------------- VARIABILI ISR --------------------

volatile uint16_t dutyPermille = 1000;
volatile uint16_t pressureCbarShared = 0;
volatile bool sensorFaultShared = true;

volatile int16_t pdmAccumulator = 0;
volatile uint8_t burstRemaining = 0;
volatile uint16_t blockedPulseStreak = 0;


// -------------------- PIN FAST --------------------
//
// Arduino Nano:
// D2 = PD2 = INT0
// D5 = PD5

inline void d5HiZ() {
  DDRD  &= ~_BV(PD5);
  PORTD &= ~_BV(PD5);
}

inline void d5HighFast() {
  PORTD |= _BV(PD5);
  DDRD  |= _BV(PD5);
}

inline void d5LowFast() {
  PORTD &= ~_BV(PD5);
  DDRD  |= _BV(PD5);
}


// -------------------- TIMER IDLE --------------------

inline void resetIdleTimer() {
  TCNT1 = 0;
  TIFR1 |= _BV(OCF1A);
  TIMSK1 |= _BV(OCIE1A);
}


// -------------------- ISR D2 / MOTO --------------------

ISR(INT0_vect) {
  bool motoState = PIND & _BV(PD2);

  // Caso più urgente: quando MOTO originale scende, D5 deve scendere subito.
  if (!motoState) {
    d5LowFast();
    resetIdleTimer();
    return;
  }

  // Fronte salita MOTO originale.
  // Decidiamo se passarlo o saltarlo.

  bool passPulse = true;

  if (ENABLE_PRESSURE_CONTROL) {
    uint16_t duty = dutyPermille;
    uint16_t pressureCbar = pressureCbarShared;
    bool sensorFault = sensorFaultShared;

    if (sensorFault || duty >= 1000) {
      passPulse = true;
    }

    else if (burstRemaining > 0) {
      passPulse = true;
      burstRemaining--;
    }

    else if (
      blockedPulseStreak >= MAX_BLOCKED_PULSES &&
      pressureCbar < SAFETY_BURST_MAX_CBAR
    ) {
      passPulse = true;
      burstRemaining = SAFETY_BURST_PULSES > 0 ? SAFETY_BURST_PULSES - 1 : 0;
    }

    else if (duty <= 0) {
      passPulse = false;
    }

    else {
      pdmAccumulator += duty;

      if (pdmAccumulator >= 1000) {
        pdmAccumulator -= 1000;
        passPulse = true;

        if (MIN_BURST_PULSES > 1) {
          burstRemaining = MIN_BURST_PULSES - 1;
        }
      } else {
        passPulse = false;
      }
    }
  }

  if (passPulse) {
    d5HighFast();
    blockedPulseStreak = 0;
  } else {
    d5LowFast();

    if (blockedPulseStreak < 65000) {
      blockedPulseStreak++;
    }
  }

  resetIdleTimer();
}


// Se non arrivano fronti MOTO per circa 250 ms, sgancia D5.
ISR(TIMER1_COMPA_vect) {
  d5HiZ();

  pdmAccumulator = 0;
  burstRemaining = 0;
  blockedPulseStreak = 0;

  TIMSK1 &= ~_BV(OCIE1A);
}


// -------------------- PRESSIONE --------------------

int adcToCbar(int adc) {
  long cbar = ((long)(adc - ADC_0_BAR) * SENSOR_MAX_CBAR) / (ADC_12_BAR - ADC_0_BAR);

  if (cbar < 0) cbar = 0;
  if (cbar > SENSOR_MAX_CBAR) cbar = SENSOR_MAX_CBAR;

  return (int)cbar;
}


int clampInt(int v, int lo, int hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}


void updatePressureAndDuty() {
  static bool filterInitialized = false;
  static int32_t filteredAdcX16 = 0;

  int adc = analogRead(PRESSURE_PIN);

  bool sensorFault = (adc < ADC_FAULT_LOW || adc > ADC_FAULT_HIGH);

  if (!filterInitialized) {
    filteredAdcX16 = ((int32_t)adc) * 16;
    filterInitialized = true;
  } else {
    int32_t target = ((int32_t)adc) * 16;
    filteredAdcX16 += (target - filteredAdcX16) >> EMA_SHIFT;
  }

  int filteredAdc = filteredAdcX16 / 16;
  int pressureCbar = adcToCbar(filteredAdc);

  int newDuty = 1000;

  if (!ENABLE_PRESSURE_CONTROL) {
    newDuty = 1000;
  }

  else if (sensorFault) {
    // Fail-open: se il sensore non torna, copia tutto.
    newDuty = 1000;
  }

  else if (pressureCbar < FULL_COPY_UNTIL_CBAR) {
    // Preinfusione, vapore, salita pressione.
    newDuty = 1000;
  }

  else if (pressureCbar >= HARD_CUT_CBAR) {
    newDuty = 0;
  }

  else {
    int errorCbar = TARGET_CBAR - pressureCbar;

    // A 9 bar circa sta attorno a HOLD_DUTY.
    // Sotto 9 aumenta duty, sopra 9 lo diminuisce.
    newDuty = HOLD_DUTY_PERMILLE + (errorCbar * KP_PER_CBAR);

    newDuty = clampInt(newDuty, DUTY_MIN, DUTY_MAX);
  }

  noInterrupts();
  pressureCbarShared = pressureCbar;
  sensorFaultShared = sensorFault;
  dutyPermille = (uint16_t)newDuty;
  interrupts();
}


// -------------------- SETUP / LOOP --------------------

void setup() {
  cli();

  // D2 input, no pullup.
  DDRD &= ~_BV(PD2);
  PORTD &= ~_BV(PD2);

  // D3 ZERO input, per ora non usato.
  DDRD &= ~_BV(PD3);
  PORTD &= ~_BV(PD3);

  // D5 inizialmente alta impedenza.
  d5HiZ();

  // Disabilita Timer0: niente millis/delay/Serial timing.
  // Questo è uno dei punti che ha reso stabile lo sketch ultra-fast.
  TIMSK0 = 0;

  // INT0 su D2, qualsiasi cambio logico.
  EICRA &= ~_BV(ISC01);
  EICRA |=  _BV(ISC00);
  EIFR  |=  _BV(INTF0);
  EIMSK |=  _BV(INT0);

  // Timer1 per timeout idle.
  // Prescaler 256: 16 MHz / 256 = 62500 Hz = 16 us/tick
  // 250 ms = 15625 tick
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  OCR1A  = 15625;

  TCCR1B |= _BV(WGM12);
  TCCR1B |= _BV(CS12);

  TIMSK1 &= ~_BV(OCIE1A);

  // ADC default: Vref = AVcc.
  analogReference(DEFAULT);

  sei();
}


void loop() {
  // analogRead non deve bloccare gli interrupt esterni:
  // durante la conversione, INT0 continua a copiare MOTO.
  updatePressureAndDuty();
}