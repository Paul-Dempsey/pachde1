# PanelTone

To set a mood, tint the panels in your skiff with **PanelTone**.

You can choose a theme for PanelTone from the Right click menu.
This only changes the look of PanelTone itself.

| Light | Dark| High |
| :-: | :-: | :-: |
|![The pachde One PanelTone module (Light)](./images/PanelTone-light.png) | ![The pachde One PanelTone module (Dark)](./images/PanelTone-dark.png)| ![The pachde One PanelTone module (High Contrast)](./images/PanelTone-hc.png)|

Here is an example of PanelTone with it's _Forest_ factory preset and some VCV modules with _Prefer dark panels_ enabled.
Toning works best on Dark modules.

![Example of toned VCV dark panels](./images/toned.png)

Use the color picker (the button shaped like an artists's palette) to pick a color and its transparency.
Click the _Go_ button or press **F2** with the mouse over PanelTone to fade in toned panels. Click or F2 to fade out.

The hamburger menu at the top lets you choose which modules will be affected when toning is turned on.
For choosing the effected modules, using _Selected modules_ is a little bit tricky.
Skiff saves the list of selected modules and uses that list until you change the scope using the menu.
Once you've selected modules, choose _Selected modules_, then un-select modules.
From here, you can click _Go_ to fade in and out, and the same set of modules are toned.
To work with a different selection, select those modules, and choose _Selected modules_ from the menu again.

In the hamburger menu, you can also decide if the panel toning will affect this instance of PanelTone.
The _Tone this PanelTone_ option only affects toning from _this_ PanelTone.
You can have another PanelTone in the patch that affects this instance.

You can select and animate the color using **Copper** or **Copper-Mini** as an extender.
The Copper must be set to enable _Share color as an Extender_ in its right click menu,
and choose _Use Copper color_ in the PanelTone menu.

> Note — PanelTone operates on assumptions about how a module is constructed, which is generally the case, but may not always be valid. So, results can be mixed, especially for modules with an atypcial construction or custom-draw their module and widgets.

PanelTone works by inserting an overlay widget the size of a module's panel, filled with the specified color.
A transparent color tints the panel.
An opaque color covers it.
The strength of the effect depends on the opacity (alpha) of the color, which is the left-hand slider in the color picker.

The switch below the palette button selects the position of the toning overlay.
When set to _Panel_ (the default), only the panel is toned, leaving the module's widgets unaffected.
When the position is set to _Widget_, the overlay covers everything in the affected module except things that are bright when the room brightness is low.

Below the knob is the _GO_ button (**F2**), which toggles the toning effect on and off.
When toggled on, it fades in over the duration set by the _Fade timne_ knob.
when toggled off, it fades out over the same duration.

The CV input can either trigger the fade (the default) or be continuous modulation of the transparency of the overlay.
Choose between _Trigger_ and _Continuous_ using the switch underneath the input jack.
When using continuous input, you must still engage and disengage toning with _Go_.

---

![pachde (#d) Logo](Logo.svg)