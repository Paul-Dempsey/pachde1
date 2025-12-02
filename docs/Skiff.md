# Skiff

**Skiff** provides some neat ways to customize the look of your rack.
It comes in three themes: Light, Dark, and High-Contrast.
Change themes and turn off the shouting button labels in the mdoule menu.

| Light | Dark| High | NOT SHOUTING (Dark) |
|:-: | :-: | :-: | :-: |
|![The pachde One Skiff module](./images/Skiff.png) | ![The pachde One Skiff module (dark)](./images/Skiff-dark.png)| ![The pachde One Skiff module (high contrast)](./images/Skiff-hc.png)|![The pachde One Skiff module (dark) with non-shouting labels](./images/Skiff-whisper.png) |

1. **Alternate rails** — The hamburger menu at the top contains a selection of alternate rails for your rack.
   Or you choose a custom rail SVG of your own.
   SVGs seem to work best when they are a multiple of HP units (15 pixels) wide and at least 380 pixels high.
   The provided alternate rail SVGs have the identical size of Rack's rail SVGs, but that isn't required for a custom one.

1. **DERAIL** — Toggles between Rack's rails visible and invisible (black).

1. **NOPANELS** — Toggle between hidden and visible module panels. When panels are hidden they are see-through.

1. **CALM**  — Toggle between standard Rack black knobs and i/o ports and calm ones that are less glossy and ornate.
  The effect is a bit subtle.

1. **UNCREW** — Unscrew all modules in the Rack.
  If modules start falling out, you can click it again to restore the screws.
  Only standard Rack screws are affected.
  I don't have the right screwdriver for the custom screws that some modules use.
  **#d One** modules that have screws come with their own screwdriver in the module menu.

1. **NOJACK** — Hide and show unused jacks. When unused jacks are hidden, you can't plug in new cables.
   The action is not continuous, so if you unplug a port, it remains visible.

1. **DARKNESS**  — Quell das blinken lights (toggle between hidden and visible lights).

1. **PACK'EM (F7)** — Pack your modules together in neat, left-aligned rows.
  Works on just the selection if you have one.
  Any vacant rows are filled.
  Modules in a given row are kept together in the same row to avoid breaking extenders.

1. **Restore Rack** — QUickly restore everything to default Rack apeparance.

Hover the **?** icon to show keyboard tips. There, you'll see that you can:

1. **Zoom to selection (F6)** Press **F6** with the mouse over **Skiff** to zoom the selected modules.
   It's a puzzle that Rack doesn't do this itself.

1. **Pack selected modules (F7)** Press **F7** with the mouse over **Skiff** to pack the selected modules.

## The One and only

Skiff makes changes that are global to rack, so it makes sense to only have one instance in your patch.
If you add a second instance, it will be a nonfunctional one that looks something like this (varies by theme):

![A disabled pachde One Skiff module](./images/no-Skiff.png)

---

![pachde (#d) Logo](Logo.svg)
