# pachde-One

The first VCV Rack plugin for pachde (#d).

All pachde-One modules have Light, Dark, and High Contrast themes. Right click to choose a theme.
They also come with caps on the screws for an elegant look.
All screws are removable if you're more relaxed about your Rack and not worried about the panels falling out.

- [Documentation](docs/index.md)
- [Patches on patchstorage](https://patchstorage.com/platform/vcv-rack/?search_query=pachde-One)

## Null

A truly blank, blank panel with some fun features (for a blank panel).
[Null user guide](docs/Null.md)

## Info

A Minimalist resizable patch notes panel.
[Info user guide](docs/Info.md)

## Imagine

A unique and eccentric source of voltage, gates, and triggers using an image.
You can think of it as a sequencer or image sampler.
[Imagine user guide](docs/Imagine.md)

## Copper

A color picker with modulation and color component outputs. Extends Null and Info. [Copper user guide](docs/Copper.md)

## Building

There's nothing special about building this module. There are no external dependencies, and no submodules.

If you already have a Rack build environment set up:

1. clone the repo
2. cd to the plugin folder
3. run `make`

   Or `make plugins` from the Rack root if you have a full rack build.

## Credits

- cosinekitty (Don Cross) for showing me how to get Github Actions working in my repo to build all the versions (including Mac ARM aka M1).

![pachde logo](docs/Logo.svg)
