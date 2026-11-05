/*
 * CHRC T07 — ESP32 firmware skeleton
 *
 * Goal: produce one set of simulated raw joint angles at a steady 100 Hz,
 * then print it over USB serial so we can verify the actual loop rate.
 *
 * Hardware required: ESP32 dev board + USB cable (NO sensors yet).
 * Serial Monitor baud: 921600.
 *
 * CSV format (TEMPORARY / T07 ONLY):
 *   seq,t_us,dt_us,joint_0,...,joint_20
 * A statistics line beginning with # prints about once per second.
 * T12 will replace this human-readable text with a documented binary frame.
 */

#include <Arduino.h>

constexpr uint8_t JOINT_COUNT = 21;  // Placeholder: update after joint-count decision.
constexpr uint32_t SAMPLE_RATE_HZ = 100;
constexpr uint32_t SAMPLE_PERIOD_US = 1000000UL / SAMPLE_RATE_HZ; // 100hz = T = 1 /100 = 1s/100 = 1000000miscro s/100 = 10 000 microcros
constexpr uint32_t SERIAL_BAUD = 921600;

uint32_t nextSampleUs = 0;
uint32_t sampleTimeUs = 0;
uint32_t previousSampleUs = 0;
uint32_t sequenceNumber = 0;
uint32_t skippedTicks = 0;

uint32_t rateWindowStartUs = 0;
uint32_t rateWindowSamples = 0;
uint32_t maxPeriodErrorUs = 0;

// HARDWARE ABSTRACTION LAYER: the rest of the program does not care
// whether joint readings come from this simulator or real Hall sensors.
// For now, return a slowly changing, joint-specific 12-bit value (0..4095).
uint16_t readJoint(uint8_t id) {
  const uint32_t CYCLE_US = 4000000UL;   // 4-second triangular motion.
  const uint32_t HALF_CYCLE_US = CYCLE_US / 2;

  // Shift each joint in time so all printed columns are not identical.
  const uint32_t phaseUs =
      (sampleTimeUs + static_cast<uint32_t>(id) * 137000UL) % CYCLE_US;
  const uint32_t rampUs = (phaseUs <= HALF_CYCLE_US)
                              ? phaseUs
                              : (CYCLE_US - phaseUs);
  return static_cast<uint16_t>(
      (static_cast<uint64_t>(rampUs) * 4095ULL) / HALF_CYCLE_US);
}

void printCsvHeader() {
  Serial.print("seq,t_us,dt_us");
  for (uint8_t id = 0; id < JOINT_COUNT; ++id) {
    Serial.print(",joint_");
    Serial.print(static_cast<unsigned int>(id));
  }
  Serial.println();
}

void printRateStats(uint32_t elapsedUs) {
  if (elapsedUs == 0) return;

  const float measuredHz =
      (static_cast<float>(rateWindowSamples) * 1000000.0f) / elapsedUs;
  Serial.print("# STATS measured_hz=");
  Serial.print(measuredHz, 2);
  Serial.print(" max_period_error_us=");
  Serial.print(maxPeriodErrorUs);
  Serial.print(" skipped_ticks_total=");
  Serial.println(skippedTicks);
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(300);  // Gives the serial connection time to start up.

  const uint32_t startUs = micros();
  nextSampleUs = startUs + SAMPLE_PERIOD_US;
  rateWindowStartUs = startUs;

  Serial.println("# CHRC T07: fake angle data, not real sensor readings");
  Serial.print("# target_hz=");
  Serial.print(SAMPLE_RATE_HZ);
  Serial.print(" joints=");
  Serial.println(static_cast<unsigned int>(JOINT_COUNT));
  printCsvHeader();
}

void loop() {
  const uint32_t nowUs = micros();

  // Signed difference handles micros() wraparound for short time intervals.
  if (static_cast<int32_t>(nowUs - nextSampleUs) < 0) {
    if (static_cast<uint32_t>(nextSampleUs - nowUs) > 1500UL) {
      delay(1); // Avoid a busy loop while the next sample is far away.
    }
    return;
  }

  // Timestamp once, then read ALL configured joints at this sample tick.
  sampleTimeUs = nowUs;
  uint32_t periodUs = 0;
  if (sequenceNumber > 0) {
    periodUs = sampleTimeUs - previousSampleUs;
    const uint32_t errorUs = (periodUs > SAMPLE_PERIOD_US)
                                 ? (periodUs - SAMPLE_PERIOD_US)
                                 : (SAMPLE_PERIOD_US - periodUs);
    if (errorUs > maxPeriodErrorUs) maxPeriodErrorUs = errorUs;
  }
  previousSampleUs = sampleTimeUs;

  // Temporary human-readable data stream for T07's first demonstration.
  Serial.print(sequenceNumber);
  Serial.print(',');
  Serial.print(sampleTimeUs);
  Serial.print(',');
  Serial.print(periodUs);
  for (uint8_t id = 0; id < JOINT_COUNT; ++id) {
    Serial.print(',');
    Serial.print(readJoint(id));
  }
  Serial.println();

  ++sequenceNumber;
  ++rateWindowSamples;

  // Advance using scheduled times, not "delay(10)", to prevent drift.
  nextSampleUs += SAMPLE_PERIOD_US;
  while (static_cast<int32_t>(micros() - nextSampleUs) >= 0) {
    nextSampleUs += SAMPLE_PERIOD_US;
    ++skippedTicks;
  }

  // Report measured loop rate and worst period error about once a second.
  const uint32_t windowElapsedUs = sampleTimeUs - rateWindowStartUs;
  if (windowElapsedUs >= 1000000UL) {
    printRateStats(windowElapsedUs);
    rateWindowStartUs = sampleTimeUs;
    rateWindowSamples = 0;
    maxPeriodErrorUs = 0;
  }
}
