#!/usr/bin/python
# -*- coding: utf-8 -*-
import keras_ocr
import matplotlib.pyplot as plt
pipeline = keras_ocr.pipeline.Pipeline()

# Read images from the folder path of ESP32 captured images gallery

images = [keras_ocr.tools.read(img) for img in ['/content/Image1.png',
          '/content/Image2.png']]

# generate text predictions from the images

prediction_groups = pipeline.recognize(images)

# plot the text predictions

(fig, axs) = plt.subplots(nrows=len(images), figsize=(10, 20))
for (ax, image, predictions) in zip(axs, images, prediction_groups):
    keras_ocr.tools.drawAnnotations(image=image,
                                    predictions=predictions, ax=ax)
