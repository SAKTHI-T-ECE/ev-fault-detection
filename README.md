# ev-fault-detection
# Two-Stage AI Fault Detection for EV Charging Cables and Batteries

Simulation-based capstone project (Dept. of ECE, AURCM).

## Problem
Micro-arcs in worn connectors and early internal battery faults go unnoticed until
breakers or thermal sensors react, often too late.

## Approach
- **Stage 1 (MCU):** decision-tree model on an ESP32 separates arc faults from normal
  noise and trips a relay in milliseconds.
- **Stage 2 (background):** anomaly detection for connector wear / battery degradation,
  with an LLM-generated plain-English report.

## Repository layout
- `ml/` Colab notebook (data generation + training)
- `firmware/` ESP32 sketch + exported model (`arc_model.h`)
- `data/` synthetic_arc_dataset.csv
- `docs/` confusion matrix, waveforms, screenshots

## Simulation
Wokwi project:https://wokwi.com/projects/478449297900790785

## Results
- Accuracy:100.0%
- False-trip rate:0%
- Simulated decision time:1557 us

Note: all data is synthetic; real-hardware validation is future work.
