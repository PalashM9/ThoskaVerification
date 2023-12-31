# Overview

The approach involved testing text recognition using Tesseract API on an Arduino
and found that it worked well with large and clear words. The approach stored the
image in flash memory using SPIFFS and built a frame using LEGO blocks to capture
stable images for better detection accuracy. The wide aperture made text detection
difficult, but it was found that capturing images from 35cm away produced good
quality images of Thoska. By fixing the position of the ESP32 cam, it was able to
capture better and stable images. The pre-processing of the captured images involved
cropping them to eliminate unnecessary details and used Gaussian blur, sharpening,
opacity, and RGB correction to improve image quality.

## Application Inferences

![approach2-1](https://user-images.githubusercontent.com/100582448/228065751-5358237d-2131-42c8-a805-ed51974fc336.png)
