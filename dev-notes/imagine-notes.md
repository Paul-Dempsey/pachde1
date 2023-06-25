# Dev notes for Imagine

**Imagine** is an image sampler.

Load an image and generate voltages, gates, and triggers as the image is scanned or touched.

Consider: multi-frame formats (gif, animated png, video).

How to position initial read head? Click may be too coarse. Right click head for precise numeric x/y coordinates?

Allow scrub always (drag on read head).

While it isn't really a design goal to be a very sophisticated interpreter of images, consider providing _some_ image processing to enhance the musical usefulness of the output.

- "smart blur" to smooth + enhance edges

   This could be the most direct way to improve the musical usefulness of the module

- "Posterize" the image might be another way to do the combined smoothing + edges.

- downsample large images

  Apparent speed of the traversal is dependent on image size, becuase speed is in terms of pixels/time, and the image is scaled to fit the constrained size of a module.

Parameters:

| Parameter | Description                                     |
|:---------:|-------------------------------------------------|
| x         | x position of read head                         |
| y         | y position of read head                         |
| size      | radius of reading head, in pixels               |
| hs        | head shape: circle, square, bar                 |
| step      | (speed) Distance of a step                      |
| vc        | Voltage criteria: Luminance, Saturation, Hue    |
| gc        | Gate criteria: Luminance, Saturation, Hue       |
| gt        | Gate threshold                                  |
| gd        | Gate direction: up, down                        |
| tc        | Trigger criteria: Luminance, Saturation, or Hue |
| tt        | Trigger threshold                               |
| td        | Trigger direction: up, down, both               |
| path      | head motion algorithm                           |
| slew      | amount of smoothing between samples (perhaps also # of samples to include) |

For smoothing and edge detection, keep a queue of _n_ samples for path. Consider second thread to precompute data, if per-step is problematic.

x,y = % width/height.

Outputs:

| Output | Signal  | Basis                                             |
|:------:|:-------:|---------------------------------------------------|
| v      | voltage | Luminance, Saturation, Hue                        |
| g      | gate    | Transitions when value crosses the gate threshold |
| t      | trigger | Trigger when value crosses the trigger threshold  |

Path:

| Motion         | Description                    |
|:--------------:| ------------------------------ |
| lrtb           | Left to right, top to bottom   |
| rltb           | Right to left, top to bottom   |
| lrbt           | Left to right, bottom to top   |
| rlbt           | Right to left, bottom to top   |
| spiral         | spiral in                      |
| circle/ellipse | orbit                          |
| bounce         | Pong-style                     |
| browndrunk     | Brownian motion                |
| scrub          | Manual x/y position from click |
| trajectory     | continous path on a toroid     |

+ reverse path
