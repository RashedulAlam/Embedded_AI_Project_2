/*
  IMU Classifier
  This example uses the on-board IMU to start reading acceleration and gyroscope
  data from on-board IMU, once enough samples are read, it then uses a
  TensorFlow Lite (Micro) model to try to classify the movement as a known gesture.
  Note: The direct use of C/C++ pointers, namespaces, and dynamic memory is generally
        discouraged in Arduino examples, and in the future the TensorFlowLite library
        might change to make the sketch simpler.
  The circuit:
  - Arduino Nano 33 BLE or Arduino Nano 33 BLE Sense board.
  Created by Don Coleman, Sandeep Mistry
  Modified by Dominic Pajak, Sandeep Mistry
  This example code is in the public domain.
*/

#include "TensorFlowLite.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
// #include <tensorflow/lite/micro/micro_error_reporter.h>
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
// #include <tensorflow/lite/version.h>

#include "model.h"
#include "0.h"

const int numSamples = 28 * 28;

// pull in all the TFLM ops, you can remove this line and
// only pull in the TFLM ops you need, if would like to reduce
// the compiled size of the sketch.
tflite::AllOpsResolver tflOpsResolver;

const tflite::Model* tflModel = nullptr;
tflite::MicroInterpreter* tflInterpreter = nullptr;
TfLiteTensor* tflInputTensor = nullptr;
TfLiteTensor* tflOutputTensor = nullptr;

// Create a static memory buffer for TFLM, the size may need to
// be adjusted based on the model you are using
constexpr int tensorArenaSize = 120 * 1024;
byte tensorArena[tensorArenaSize] __attribute__((aligned(16)));

// // array to map gesture index to a name
// const char* GESTURES[] = {
//   "punch",
//   "flex"
// };

// #define NUM_GESTURES (sizeof(GESTURES) / sizeof(GESTURES[0]))

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;

  // get the TFL representation of the model byte array
  tflModel = tflite::GetModel(model);
  if (tflModel->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println("Model schema mismatch!");
    while (1)
      ;
  }

  // Create an interpreter to run the model
  tflInterpreter = new tflite::MicroInterpreter(tflModel, tflOpsResolver, tensorArena, tensorArenaSize);

  // Allocate memory for the model's input and output tensors
  tflInterpreter->AllocateTensors();

  // Get pointers for the model's input and output tensors
  tflInputTensor = tflInterpreter->input(0);
  tflOutputTensor = tflInterpreter->output(0);
}

void loop() {

  for (int i = 0; i < numSamples; i++) {
    // Serial.println(image_data[i]);
    tflInputTensor->data.f[i] = (float)image_data[i] / 255.0;
  }

  TfLiteStatus invokeStatus = tflInterpreter->Invoke();

  if (invokeStatus != kTfLiteOk) {
    Serial.println("Invoke failed!");
  }

  float maxPrediction = tflOutputTensor->data.f[0];
  int maxIndex = 0;

  for (int i = 0; i < 10; i++) {
    Serial.print(i);
    Serial.print("-->");
    Serial.print("Prdiction-->");
    Serial.println(tflOutputTensor->data.f[i], 6);

    if (tflOutputTensor->data.f[i] > maxPrediction) {
      maxPrediction = tflOutputTensor->data.f[i];
      maxIndex = i;
    }
  }

  // Print the maximum prediction and its index
  Serial.print("Max Prediction Value-->");
  Serial.println(maxPrediction, 6);
  Serial.print("Detected Number -->");
  Serial.println(maxIndex);

  delay(10000);
}