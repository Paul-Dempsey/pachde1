# Rui

**Rui** (short for Rack User Interface) provides nice knobs with control voltage inputs for some of the Rack visuals. I like turning a knob better than dragging a slider in a menu.

**Rui** comes in three themes: Light, Dark, and High Contrast.
Choose a theme from the Right click menu.

| Light | Dark| High Contrast |
| :-: | :-: | :-: |
|![The pachde One Rui module](./images/Rui.png) | ![The pachde One Rui module (dark)](./images/Rui-dark.png)| ![The pachde One Rui module (high contrast)](./images/Rui-hc.png)|

**Rui** controls from top to bottom are:

| Rack&nbsp;UI&nbsp;Setting | Background&nbsp;Color | Note |
| -- | :--: | -- |
| Cable&nbsp;opacity | cyan | Labelled "O". Has a button undo-able and keybinding (F62) for toggling Zero opacity. |
| Cable&nbsp;tension | green | Labelled "T". |
| Room&nbsp;brightness | magenta | Labelled "B". |
| Light&nbsp;bloom | yellow-green | Labelled "L". |

Add some gentle LFO modulation for a pulsing rack.
By default, modulation amount is zero.
Turn up the trim pot to fade in the amount of CV modulation.

Each parameter has a knob, a CV input for modulation, and a trim pot to adjust the amount of modulation. Bipolar inputs work best for animation.

For _Cable opacity_, the little button between the knob and modulation trim pot sets the cable opacity immediately to zero. You can also press **F6** with the mouse over Rui to set immediate zero cable opacity. Undo restores the previous opacity.

The Pause/Play button (**F2**) (just above the logo at the botttom of Rui) lets you pause CV-driven Rack changes without bypassing or unplugging the CV.

When Rui is active, you the Rack menu controls for the same parameters are ineffective.

## The One and only

Rui makes changes that are global to Rack, so it makes sense to only have one instance in your patch.
If you add a second instance, it will be a nonfunctional one that looks something like this (varies by theme):
If you see it like this, then you have a live Rui somewhere else in your patch.

![A disabled pachde One Rui module](./images/no-Rui.png)

---

![pachde (#d) Logo](Logo.svg)