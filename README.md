# pachde-One

The first VCV Rack plugin for pachde (#d).

All pachde-One modules have Light, Dark, and High Contrast themes, and allow you to choose whether to follow the Rack **Use dark panels** option.
Right click to choose a theme.
They also come with caps on the screws for an elegant look.
All screws are removable if you're more relaxed about your Rack and not worried about the panels falling out.

- [Documentation](docs/index.md)

- [Demo Patches on patchstorage](https://patchstorage.com/platform/vcv-rack/?search_query=pachde-One)

- [Buy me a coffee](https://venmo.com/u/pcdempsey). *Thank you!*

  Donations go to helping fund continued development and hardware for testing and development. (I have a wish list :-). Venmo takes the smallest cut out of the payment systems I've found.\
  ![Buy pachde a coffee with VenMo](docs/VenMo.png)

## Change log

See [CHANGELOG](CHANGELOG.md)

## Modules
### Null

A truly blank, blank panel with some fun features (for a blank panel).\
[Null user guide](docs/Null.md)

### Info

A Minimalist resizable patch notes panel.\
[Info user guide](docs/Info.md)

### Imagine

A unique and eccentric source of voltage, gates, and triggers using an image.
You can think of it as a sequencer or image sampler.\
[Imagine user guide](docs/Imagine.md)

### Copper

A color picker with modulation and color component outputs. Extends Null and Info.\
[Copper user guide](docs/Copper.md)

## Building

There's nothing special about building this plugin. There are no external dependencies, and no submodules. It's all basic stock VCV Rack plugin build per the Rack docs.

If you already have a Rack build environment set up:

1. Clone the repo if you just want to build.

   `git clone https://github.com/Paul-Dempsey/pachde1.git`

    Or, fork in GitHub if you want to make PRs.

1. cd to the plugin folder

1. run `make`

   Or `make plugins` from the Rack root if you have a full rack build.

No need to `make dep` or `git submodule update --init --recursive` for this module, because there are no dependencies other than Rack itself.

## Contributing

Contributions are generally welcome.
I'd appreciate if you're planning to add something, please open a Discussion so we can discuss it.

If you've found a bug, please open an issue.
If you have a fix for the bug, then that's fantastic. Bug fix PRs are always welcome.

At this time, pull requests are accepted only from forks. GitHub makes it super easy to create a fork.

## Credits

- Andrew Belt for creating VCV Rack
- cosinekitty (Don Cross) for showing me how to get Github Actions working in my repo to build all the versions (including Mac ARM aka M1).
- baconpaul for sharing improvements in the Github Actions, and other Rack inspiration.

## What else?

I have a couple of other repos for VCV Rack-related projects you may be interested in if you are, or want to become a plugin developer:

- **[Generic Blank](https://github.com/Paul-Dempsey/GenericBlank)**
  This is a great place to get started making a VCV Rack plugin. It's a good alternative to the VCV Rack tutorial. While it's a generic blanking plate out of the box, the code is well-commented with what you need to do to build out a functional module.

- **[svg_theme](https://github.com/Paul-Dempsey/svg_theme)** For plugin developers: Lean in-memory SVG theming for VCV Rack plugins, using a json configuration. This allows you to maintain a single SVG for your rack, and theme it as needed. Includes a simple demo module to show you how it works.

![pachde logo](docs/Logo.svg)
