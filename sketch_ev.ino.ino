#include "arc_model.h"

#define N 1000
#define FS 50000.0f
#define RELAY_PIN 4
#define LED_PIN 2

Eloquent::ML::Port::DecisionTree clf;
float x[N];
float sinTab[N];

float frand(float a, float b) {
  return a + (b - a) * (random(0, 10000) / 10000.0f);
}

// Fast approximate Gaussian noise (sum of 4 uniforms)
float gauss(float s) {
  float t = frand(0, 1) + frand(0, 1) + frand(0, 1) + frand(0, 1);
  return (t - 2.0f) * 1.732f * s;
}

void genNormal() {
  float base = frand(5, 30);
  for (int i = 0; i < N; i++)
    x[i] = base + 0.2f * sinTab[i] + gauss(0.05f);
}

void genDisturbance() {
  genNormal();
  int st = random(100, 900);
  float step = frand(-3, 3);
  for (int i = st; i < N; i++) x[i] += step;
  int n = random(0, 3);
  for (int k = 0; k < n; k++) x[random(0, N)] += frand(1, 4);
}

void genArc() {
  genNormal();
  int bursts = random(2, 6);
  for (int b = 0; b < bursts; b++) {
    int s = random(0, N - 120), l = random(40, 120);
    float amp = frand(0.6, 2.0), dip = frand(0.5, 3);
    for (int i = s; i < s + l; i++) x[i] += gauss(amp) - dip;
  }
}

void features(float *f) {
  float sum = 0, sq = 0;
  for (int i = 0; i < N; i++) {
    sum += x[i];
    sq += x[i] * x[i];
  }
  float mean = sum / N;
  float var = sq / N - mean * mean;
  float maxd = 0, de = 0;
  for (int i = 1; i < N; i++) {
    float d = x[i] - x[i - 1];
    if (fabsf(d) > maxd) maxd = fabsf(d);
    de += d * d;
  }
  f[0] = mean;
  f[1] = sqrtf(var > 0 ? var : 0);
  f[2] = maxd;
  f[3] = de / (N - 1);
}

int classify(unsigned long &us) {
  float f[4];
  unsigned long t0 = micros();
  features(f);
  int p = clf.predict(f);
  us = micros() - t0;
  return p;
}

void generate(int type) {
  if (type == 0) genNormal();
  else if (type == 1) genDisturbance();
  else genArc();
}

const char* NAMES[] = {"normal", "disturbance", "ARC"};

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting batch test...");

  for (int i = 0; i < N; i++) sinTab[i] = sinf(2 * PI * 20000.0f * i / FS);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  randomSeed(42);

  int tn = 0, fp = 0, fn = 0, tp = 0;
  unsigned long total = 0, us = 0, worst = 0;
  int runs = 0;

  for (int type = 0; type < 3; type++) {
    for (int k = 0; k < 50; k++) {
      generate(type);
      int p = classify(us);
      total += us;
      runs++;
      if (us > worst) worst = us;
      if (type == 2) { if (p) tp++; else fn++; }
      else           { if (p) fp++; else tn++; }
    }
  }

  Serial.printf("TN=%d FP=%d FN=%d TP=%d\n", tn, fp, fn, tp);
  Serial.printf("Accuracy=%.1f%%\n", 100.0f * (tn + tp) / (tn + fp + fn + tp));
  Serial.printf("Avg decision time=%lu us, worst=%lu us (simulated)\n", total / runs, worst);
  Serial.println("Batch test done. Starting live demo...");
}

void loop() {
  for (int type = 0; type < 3; type++) {
    generate(type);
    unsigned long us;
    int p = classify(us);
    digitalWrite(RELAY_PIN, p ? HIGH : LOW);
    Serial.printf("Input: %-11s -> %s (%lu us)\n", NAMES[type], p ? "TRIP" : "OK", us);
    digitalWrite(LED_PIN, HIGH);
    delay(1500);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(RELAY_PIN, LOW);
  }
}