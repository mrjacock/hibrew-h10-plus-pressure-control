/*
  HiBREW H10 Plus - pressure pump control v0.2 ultra-fast

  Arduino Nano / ATmega328P

  D2 = original MOTO signal from the machine microcontroller
  D5 = modified MOTO signal to the pump power board
  A4 = 0.5-4.5 V pressure sensor, 0-12 bar
  D3 = ZERO signal connected, currently not used

  Strategy:
  - D2 is read through the INT0 interrupt with direct port access.
  - D5 is updated with minimal latency.
  - Below about 8.5 bar, the original MOTO signal is copied unchanged.
  - Near 9 bar, the controller starts skipping pump pulses.
  - If the original MOTO signal stops, D5 returns to high impedance.
  - No Serial, no EEPROM, no millis(), no micros().
*/


#define MOTO_IN_PIN      2
#define ZERO_PIN         3
#define MOTO_OUT_PIN     5
#define PRESSURE_PIN     A4


// -------------------- ENABLE / DISABLE CONTROL --------------------
//
// Set to false to verify that the firmware still performs a clean
// D2 -> D5 pass-through without pressure control.
const bool ENABLE_PRESSURE_CONTROL = true;


// -------------------- PRESSURE SENSOR --------------------
//
// Sensor:
// 0.5 V = 0 bar
// 4.5 V = 12 bar
//
// ADC with 5 V reference:
// 0 bar  is about ADC 102
// 9 bar  is about ADC 716
// 12 bar is about ADC 921

const int ADC_0_BAR  = 102;
const int ADC_12_BAR = 921;

const int SENSOR_MAX_CBAR = 1200;  // 12.00 bar

const int ADC_FAULT_LOW  = 40;
const int ADC_FAULT_HIGH = 1000;


// -------------------- TARGET --------------------

const int TARGET_CBAR = 900;             // 9.00 bar

// Below this pressure, the original signal is copied unchanged.
// This keeps preinfusion, steam mode, and pressure ramp-up simple.
const int FULL_COPY_UNTIL_CBAR = 850;    // 8.50 bar

// Above this pressure, the controller cuts the pump aggressively.
const int HARD_CUT_CBAR = 1050;          // 10.50 bar


// -------------------- DUTY CONTROL --------------------
//
// dutyPermille:
// 1000 = pass all pulses
// 700  = pass about 70% of pulses
// 0    = block everything, except safety bursts

const int HOLD_DUTY_PERMILLE = 720;

// Controller gain around the target pressure.
// Higher value = more aggressive response.
// 4 means about 4 duty points for each centibar of error.
const int KP_PER_CBAR = 4;

const int DUTY_MIN = 0;
const int DUTY_MAX = 1000;


// -------------------- PRESSURE FILTER --------------------
//
// Higher value = smoother but slower response.
// 4 = reasonably responsive.
// 5 = smoother if pump pulsation is still too visible.
const uint8_t EMA_SHIFT = 4;


// -------------------- PULSE MODULATION --------------------

// When the controller decides to pump, it passes at least 2 consecutive pulses.
const uint8_t MIN_BURST_PULSES = 2;

// Prevents the pump from being left completely stopped for too long.
// At 100 Hz, 150 skipped pulses are about 1.5 s.
const uint16_t MAX_BLOCKED_PULSES = 150;
const uint8_t SAFETY_BURST_PULSES = 2;
const int SAFETY_BURST_MAX_CBAR = 1000;


// -------------------- ISR SHARED STATE --------------------

volatile uint16_t dutyPermille = 1000;
volatile uint16_t pressureCbarShared = 0;
volatile bool sensorFaultShared = true;

volatile int16_t pdmAccumulator = 0;
volatile uint8_t burstRemaining = 0;
volatile uint16_t blockedPulseStreak = 0;


// -------------------- FAST PIN CONTROL --------------------
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


// -------------------- IDLE TIMER --------------------

inline void resetIdleTimer() {
  TCNT1 = 0;
  TIFR1 |= _BV(OCF1A);
  TIMSK1 |= _BV(OCIE1A);
}


// -------------------- D2 / MOTO ISR --------------------

ISR(INT0_vect) {
  bool motoState = PIND & _BV(PD2);

  // Most urgent case: when original MOTO goes low, D5 must go low immediately.
  if (!motoState) {
    d5LowFast();
    resetIdleTimer();
    return;
  }

  // Original MOTO rising edge.
  // Decide whether to pass or skip this pump pulse.

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


// If no MOTO edges arrive for about 250 ms, release D5.
ISR(TIMER1_COMPA_vect) {
  d5HiZ();

  pdmAccumulator = 0;
  burstRemaining = 0;
  blockedPulseStreak = 0;

  TIMSK1 &= ~_BV(OCIE1A);
}


// -------------------- PRESSURE --------------------

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
    // Fail-open: if the sensor reading is not believable, copy everything.
    newDuty = 1000;
  }

  else if (pressureCbar < FULL_COPY_UNTIL_CBAR) {
    // Preinfusion, steam mode, and pressure ramp-up.
    newDuty = 1000;
  }

  else if (pressureCbar >= HARD_CUT_CBAR) {
    newDuty = 0;
  }

  else {
    int errorCbar = TARGET_CBAR - pressureCbar;

    // Around 9 bar, duty stays near HOLD_DUTY_PERMILLE.
    // Below 9 bar it increases; above 9 bar it decreases.
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

  // D2 input, no pull-up.
  DDRD &= ~_BV(PD2);
  PORTD &= ~_BV(PD2);

  // D3 ZERO input, currently not used.
  DDRD &= ~_BV(PD3);
  PORTD &= ~_BV(PD3);

  // D5 starts in high impedance.
  d5HiZ();

  // Disable Timer0: no millis(), delay(), or Serial timing.
  // This is one of the changes that made the ultra-fast sketch stable.
  TIMSK0 = 0;

  // INT0 on D2, any logical change.
  EICRA &= ~_BV(ISC01);
  EICRA |=  _BV(ISC00);
  EIFR  |=  _BV(INTF0);
  EIMSK |=  _BV(INT0);

  // Timer1 for idle timeout.
  // Prescaler 256: 16 MHz / 256 = 62500 Hz = 16 us/tick
  // 250 ms = 15625 ticks
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
  // analogRead() must not block external interrupts:
  // during the conversion, INT0 continues to process MOTO edges.
  updatePressureAndDuty();
}
