# Copper-mini

Color picker with modulation and outputs.

**See also:** [All pachde-One modules](index.md)

Here's a sample of the module in Light, Dark, and High Contrast themes.

|Light|Dark|High Contrast|
|:-:|:-:|:-:|
||||

Twist the H knob to select the color's hue.
Twist the S and L buttons to select the corresponding color component (Saturation and Lightness).

In addition to dragging to twist a knob, all the knobs are clicky.
Click any knob to advance the value by increments. Ctrl+Click (Cmd+Click on Mac) to decrement.
Add Shift for a bigger increment.

Below the outputs, the selected color is displayed opaquely on the left half,
and on the right side it shows the modulated color.

The modulated hex color text appears above the color sample.
Right click to find a menu option to copy that value for use in another module.

## Copper vs Copper-mini

Copper and Copper-mini share the same underlying color module.
They present a slightly different user itnerface and selection of output ports, but otherwise behave the same, including as expander for **Null**, **Info**, and **PanelTone**.

## As expander

**New in #d One 2.3** — Using Copper-mini as an expander can now be turned on and off in the module menu, or by pressing **F2** while the mouse is over the module.
Turning it off means that you can freely move the Copper-mini around the patch without leaving a trail of changed colors in it's wake.
So, you can now be more intentional about the effects of a Copper-mini in your patch.

When a Copper(mini) is available as an expander, the blue light on the Copper(mini) panel light up.

Copper and Copper-mini work as expanders to other modules in **#d One**:

| Module | Usage |
| -- | -- |
| [Null](./Null.md#info) | Set the panel color -- even live modulating it for some excitement. See [Null](Null.md) for more details on the entertainment value of this configuration. |
| [Info](./Info.md#info) | Set the panel or text color, or both from a Copper(-mini) on either side, configured in the Info Text options dialog. |
| [PanelTone](./PanelTone.md#paneltone) | Sets and if modulated, animates the color used to tone the panels in your Rack. This can bring your entire patch alive, visually. |

All of these modules have an option whether to use an adjacent Copper(mini) for color.
Providing/Using the extender color must be enabled in both modules.

## Inputs

| Jack Color | Color component |
| -- | -- |
| Blue | Hue component |
| Orange | Saturation (chroma) |
| Yellow | Lightness component |
| Pink | Alpha (opacity) |

All inputs are relative modulation of the knob setting, except when connected to the outputs of another Copper or Copper-mini.
When the outputs of one are connected to the inputs of another, the source module is the absolute source of values for the connected one.
They are completely synchronized.

This can be useful when animating the colors of other modules. You can extend with a less intrusive Copper-mini driven by a full Copper elsewhere in the patch.

## Outputs

All output ports represent the modulated color when any input ports are connected.

| Jack Color | Color component |
| -- | -- |
| Blue | Hue component |
| Orange | Saturation (chroma) |
| Yellow | Lightness component |
| Pink | Alpha |

## Options

| Option | Description |
| -- | -- |
| Theme | Choose Light, Dark, or High Contrast theme, and options for following the Rack dark panels setting, and the theme to use when doing so. |
| Palette color | Choose the color from a palette of named colors. |
| Copy hex color | Copies the hex code of the selected color to the clipboard. You can use this to paste into most color inputs of other Rack modules (including all of pachde-One). |
| Paste hex color | If the clipboard contains a hex color, changes the chosen color. |

![pachde (#d) Logo](Logo.svg)
