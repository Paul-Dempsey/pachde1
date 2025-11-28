# FancyBox

**FancyBox** provides ways to modify or add interest to the Rack background.
You can think of it as a twist on the legendary [DanT:Purfenator](https://library.vcvrack.com/DanTModules/Purfenator).

FancyBox supports up to 4 layers, of each with a basic graphic effect, which can be animated.
Each additional layer appears on top of the others.
In order from bottom to top:

- **Fill** — A simple color fill of the Rack window below the modules.

  - An opaque color can be used for color-keying with video software like OBS.
  - A transparent color can be used to tone the visible rails.
  - If you'd like to tone the module panels also, check out **[Paneltone](./PanelTone.md#paneltone)**.
  - More options for the rails themselves are available in **[Skiff](./Skiff.md#skiff)**.

- **Linear gradient** — A single linear gradient overlay.

- **Radial gradient** — A single radial gradient overlay.

- **Box gradient** — A single box gradient, which can provide a nice vignetting effect.

CV inputs can control the Hue, Saturation, Lightness and Alpha (opacity) components of each color, along with an additional fade for taking the color/effect in and out.

The controls of locations like the ends of a linear gradient, or the center and radius of a radial gradient are expressed as a percentage of the width/height of the rack window.

If a static background is sufficient, you can take less space and collape the panel of input jacks by clicking the "plug" icon.
But if any cables are connected, you can't close the jack panel. You don't want to damage your cable plugs!
If you decide you want to animate the background effects, expand the panel.

---

![pachde (#d) Logo](Logo.svg)
