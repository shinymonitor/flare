<div align="center">
    <img src="assets/LOGO.png", width="200">
    <h1>FLARE Prediction Engine</h1>
</div>
FLARE (Field Learnable ARbitrary Event) Prediction Engine is a self-supervised closed-loop event prediction algorithm that has automatic online adaptation. It requires no pre-training, labeling or prior knowledge of the usage. It uses a neural network that learns the signatures of any detectable event directly in the field. 

## How It Works

FLARE maintains a rolling buffer of the inputs called the context. At any moment, the networks input is a slice of that buffer called the window.

Every frame the engine makes an inference based on the most recent window. When its output crosses a confidence threshold, it fires the prediction (ahead of the event by however many samples the rollback was set to). 

Then it waits to see if its prediction was correct or not. If it was incorrect (false positive or false negative), it trains itself on the precursor window.

## What makes this different

In conventional embedded ML, a model is trained offline on a known dataset and deployed as a static artifact. It cannot adapt to a new environment, a new signal source, or a new definition of "event" without going back to a development machine.

FLARE has automatic online adaptation and self-supervised closed-loop learning.

## Target applications

- **Power electronics**: predict voltage collapse or brownout from supply trajectory
- **Mechanical systems**: predict impact, stall, or resonance from vibration buildup  
- **Process control**: predict threshold crossings in temperature, pressure, or flow
- **Any system** where an event is consistently preceded by a signal pattern and where that pattern is unknown in advance

## Demo

### RPi Pico 2

Compile with pico-sdk.
It reads 8-bit input from GPIO pins 0-7, detects events on GPIO pin 8, and outputs predictions on GPIO pin 9. 
- GPIO 0-7 : 8-bit input
- GPIO 8   : Event detect
- GPIO 9   : Event predict

### STM32F413H-DISCO

Compile with Eclipse (Embedded C++).
It reads 8-bit input from GPIO pins D0-D7, detects events on GPIO pin A0, and outputs predictions on GPIO pin A1. 
- GPIO D0-D7 : 8-bit input
- GPIO A0    : Event detect
- GPIO A1    : Event predict