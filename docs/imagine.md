# imagine

**imagine** is an image sampler.

Load an image and generate voltages, gates, and triggers as the image is scanned or touched.

Parameters:

| Parameter | Description                                     |
|:---------:|-------------------------------------------------|
| size      | radius of reading head, in pixels               |
| x         | x position of read head                         |
| y         | y position of read head                         |
| step      | (speed) Distance of a step                      |
| radius    | radius of circular head motion                  |
| vc        | Voltage criteria: Luminance, Saturation, or Hue |
| gc        | Gate criteria: Luminance, Saturation, or Hue    |
| gt        | Gate threshold                                  |
| gd        | Gate direction: up, down                        |
| tc        | Trigger criteria: Luminance, Saturation, or Hue |
| tt        | Trigger threshold                               |
| td        | Trigger direction: up, down, both               |
| path      | head motion algorithm                           |

x,y = % width/height.

Outputs:

| Output | Signal  | Basis                                             |
|:------:|:-------:|---------------------------------------------------|
| v      | voltage | Luminance, Saturation, Hue                        |
| g      | gate    | Transitions when value crosses the gate threshold |
| t      | trigger | Trigger when value crosses the trigger threshold  |

Sample head motion:

| Motion         | Description                   |
|:--------------:|-------------------------------|
| lrtb           | Left to right, top to bottom  |
| rltb           | Right to left, top to bottom  |
| lrbt           | Left to right, bottom to top  |
| rlbt           | Right to left, bottom to top  |
| out            | spiral out                    |
| in             | spiral in                     |
| circle/ellipse | orbit                         |
| bounce         | Pong-style                    |
| brown          | Brownian motion               |
| scrub          | x/y position from click or CV |
| trajectory     | continous path on a toroid    |
