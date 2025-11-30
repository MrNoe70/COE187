# Orange vs Banana Classification on MAX78000 


## Overview

This project performs real-time fruit classification between:

* Orange
* Banana

using:

* MAX78000 MCU (Cortex-M4 + Hardware CNN Accelerator)
* On-board CMOS VGA Image Sensor (RGB888 stream, 4-byte)
* DMA-based camera streaming
* Hardware-accelerated CNN inference

The system captures a 128×128 RGB image, preprocesses it, feeds it into the CNN, and outputs classification probabilities.

---

## CNN Model and Generation

The deployed model was generated with the following command:

```
ai8xize.py --device MAX78000 --softmax --fifo \
  --checkpoint-file qat_best-quantized.pth.tar \
  --config-file networks/orange-banana-hwc.yaml
```

The tool generated:

* cnn.c, cnn.h – hardware CNN layers
* weights.c, weights.h
* sampledata.h, sampleoutput.h
* Entry code block integrated into main.c

The CNN runs entirely inside the MAX78000 CNN accelerator.

---

## Camera Capture Pipeline

The onboard VGA camera streams RGB888 in 4-byte format (0x00bbggrr).

Your final code:

* Uses PIXFORMAT_RGB888 with FIFO_THREE_BYTE mode
* Converts pixel format into the CNN input format
* Adds timeouts to prevent hanging
* Adds debug logs for every stage

### Data Flow

1. `camera_init()` configures the camera clock and SCCB.
2. `camera_setup()` configures resolution and pixel format.
3. DMA streams camera data per line.
4. Code extracts R, G, B from 0x00bbggrr.
5. Converts to signed format via XOR with 0x808080.
6. Stores into `input_0[]` for CNN input.

---

## Sample Classification Results

### Orange Image

```
[  63987] -> Orange: 99.2%
[ -46561] -> Banana: 0.8%
```

Prediction: Orange

---

### Banana Image

```
[ -74057] -> Orange: 0.2%
[  77887] -> Banana: 99.8%
```

Prediction: Banana

---

## How to Run the Program

### 1. Flash the firmware

```
make build
make flash
```

### 2. Open Serial Monitor

* Baud rate: 115200
* 8-N-1 configuration

### 3. Reset the board

You should see:

```
*** CNN Inference Test orange-banana_gen ***
********** Press PB1(SW1) to capture an image **********
```

### 4. Press PB1 to capture

The system will:

* Capture the image
* Preprocess camera data
* Run CNN inference
* Print classification results
* Show ASCII art visualization if enabled

---

## Project File Summary

```
main.c                → Full program (camera + CNN + debug + ASCII)
cnn.c / cnn.h         → CNN hardware logic
weights.c / weights.h → Quantized CNN weights
camera.c / camera.h   → Camera driver
sampledata.h          → Optional reference input
orange_and_banana_results.pdf → Test results
README.md             → Project documentation
```

---

## Conclusion

This final version of the project demonstrates:

* Real-time embedded image classification
* Robust camera streaming
* On-chip CNN acceleration using MAX78000
* Fully integrated debug output, timeout checks, and visualization
---

