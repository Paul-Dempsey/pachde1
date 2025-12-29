# Pachde One Change Log

## Version 2.4

- FancyBox gets an image background, with a configuration dialog (and grows by 1hp to make room).
- Option for whether to retain effects after removing FancyBox from the path.
- Various bug fixes and improvements from user's feedback.

## Version 2.3

- ***New Modules*** — [FancyBox](docs/FancyBox#fancybox), [Skiff](docs/Skiff.md#skiff), [Paneltone](docs/Paneltone.md#paneltone), [Rui](docs/Rui.md#rui), [Guide](docs/Guide.md#guide)

- [Info](docs//Info.md#info):
  - On-panel quick text editing is a click away!
  - Friendly dialog for all text options (click the `i` symbol, or press F2).
  - Vertical (Up and Down) and inverted text
  - Bug fixes (sizing on opening presets, and loading from **Strip**)

- For Info, Null, Copper, and Copper-mini, more control over what a Copper affects.
  Copper-as-extender can be turned off in the right click menu, so that dragging it around a patch doesn't leave a wake of altered modules.

- New theming system and improved options for tracking Rack theme settings.

- Broadcast theme to all pachde One modules in the patch.

- New color pickers with continuous control.

- Use Svgs to improve the #d brand on lower res screens.

Note: expanding and improving Imagine remains in the possible future.

## Version 2.2

- ***New*** [Copper-mini](docs/Copper-mini.md#copper-mini) module.

- **Imagine**
  - ***new*** **minT** knob and input for setting the minimum time before the next gate/trigger transition can happen.
  - ***new*** *Seaweed-frame* and *Corset* factory preset images.

- **Null**
  - ***new*** Brand logo option.

- **Copper** Fix flawed Sat CV.

## Version 2.1

- Add **Follow Rack - Use dark panels** option to all modules. This is enabled by default, using the Dark theme. You may want to consider switching to the High Contrast theme for an even darker look.

- **Imagine**
  - Add CV input for Speed.
  - Two new traversals: **Talabar** and **Ralabat**.
  - Fix the Mica factory preset to show the correct image.

- **Null**
  - Add **Show branding** option so you can turn off the remaining adornment. Useful for blanking and green-screen applications.
  - New factory presets for green-screen, blue-screen, and magenta-screen.
  - Option to ignore Copper
  - Support Initialize and Randomize

- **Info**
  - Option to ignore Copper
  - Support Initialize and Randomize

- **Copper**
  - Support Randomize

- Internal:
  - Revamp theme infrastructure, cleaning up many issues coding-wise and UI glitches when changing ui options.
  - Self-positioning screws and resize handles

## Version 2.0

Original release of **pachde One**.
