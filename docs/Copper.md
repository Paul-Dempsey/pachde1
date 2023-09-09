# Copper

Color picker with modulation and outputs.

**See also:** [All pachde-One modules](index.md)

Here's a sample of the module in Light, Dark, and High Contrast themes.

![Copper in Light, Dark, and High Contrast themes](Copper.png)

Click on the Hue selector on the left or twist the H knob to select the color's hue.

Click in the Lightness/Saturation gradient to set the lightness (vertical) and saturation (horizontal) of the color. Or, twist the S and L buttons to select the corresponding color component.

In addition to dragging to twist a knob, all the knobs are clicky. Click any knob to advance the value by increments. Ctrl+Click (Cmd+Click on Mac) to decrement.

Below the colored pickers, the selected color is displayed opaquely on the left half, as if alpha is 1. On the right side it is laid over a white to black gradient with 50% gray texture, so you can see how it overlays with transparency. If you don't see a gradient or texture, the color has little to no transparency -- it has an alpha close to 1.

If the color is modulated by CV inputs, the color bar is split, with the unmodulated color on top, and the current modulated color below.

The selected, unmodulated hex color text is shown in the outputs section.
Right click to find a menu option to copy that value for use in another module.

## Copper vs Copper-mini

Copper and Copper-mini share the same underlying color module. They present a slightly different UI and selection of output ports, but otherwise behave the same, including as expanders for Null and Info.

## As expander

Both Copper and Copper-mini work as expanders to [Null](Null.md) for choosing its panel color -- even live modulating it for some excitement.
See [Null](Null.md) for more details on the entertainment value of this configuration.

Copper is also an expander for [Info](Info.md) for choosing its colors, configured in the module menu.

Null and Info both allow you to turn off using Copper as an expander.

## Inputs

H (hue), S (saturation, AKA chroma), L (lightness), and A (alpha, AKA transparency) inputs modulate the corresponding component of the chosen color.

## Outputs

All output ports represent the modulated color when any input ports are connected.

| Jack | Jack Color | Color component |
| -- | -- | -- |
| R | Red | Red component |
| G | Green | Green component |
| B | Blue | Blue component |
| h | Yellow | Hue component |
| s | Orange | Saturation (chroma) |
| l | Gray | Lightness |
| a | Pink | Alpha |
| poly | Magenta | All of the above on separate channels of one polyphonic output in the order H, S, L, A, R, G, B |

## Options

| Option | Description |
| -- | -- |
| Screws | Add or remove the screw caps. |
| Theme | Choose Light, Dark, or High Contrast theme, and options for following the Rack dark panels setting, and the theme to use when doing so. |
| Palette color | Choose the color from a palette of named colors. |
| Copy hex color | Copies the hex code of the selected color to the clipboard. You can use this to paste into most color inputs of other Rack modules (including all of pachde-One). |
| Paste hex color | If the clipboard contains a hex color, changes the chosen color. |

![pachde (#d) Logo](Logo.svg)
