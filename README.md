# pachde-One

The first VCV Rack plugin for pachde (#d).

[![Build Status](https://github.com/Paul-Dempsey/pachde1/actions/workflows/build-plugin.yml/badge.svg)](https://github.com/Paul-Dempsey/pachde1/actions/workflows/build-plugin.yml)

All pachde-One modules have Light, Dark, and High Contrast themes, and allow you to choose whether to follow the Rack UI theme or the _Use dark panels_ option.
Right click to choose a theme and tracking option.
If you want all the #d one modules to use a consistent theme, you can send the theme to all the peer pachde1 modules in the patch.

#d One modules also come with caps on the screws for an elegant look.
All screws are removable if you're more relaxed about your Rack and not worried about the panels falling out.

- [Documentation](docs/index.md#pachde-one-modules-for-vcv-rack)

- [Buy me a coffee](https://venmo.com/u/pcdempsey). *Thank you!*

  Donations go to helping fund continued development and hardware for testing and development. (I have a wish list :-). Venmo takes the smallest cut out of the payment systems I've found.\
  ![Buy pachde a coffee with VenMo](docs/images/VenMo.png)

## Innovative ? user interface

The 2.3 update of **#d One** brings a number of possible innovations for a Rack module UIs.
Most of these certainly depart from a strict hardware metaphor, which some may not prefer.
Rack and it's modules themselves already depart from a strict hardware metaphor when convenient.
For example, no Eurorack hardware has a right click menu for the module, or it's knobs or switches.

Here are some notable thigns to see in the **#d One** implementation.

**[Rack dev notes (by pachde #d)](https://github.com/Paul-Dempsey/rack-dev-notes/blob/main/doc/index.md#dev-notes-for-vcv-rack)** will get udpates with documentation, examples, and free-to-use reusable code for all of these (?) innovations:

- Hamburger menus —
  These provide context-sensitive menus appropriate for specific parts of a module,
  reducing the size and complexity of the module menu, which can already get large and difficult to use.
  In the end, not much of a difference from the right click menus for panels, knobs and switches.

- Expanable/collapsible module (**FancyBox**) —
  Instead of hunting for the right extender in the module browser, just expand the panel to show an extension with more jacks or options.
  Recently I noticed another brand that provides buttons to insert selected extenders, which I think is a great idea.

- Dialogs (**Info**, **FancyBox**) — Another tactic for making complex configurable easier than piling things into the module menu.
  Turns out that making a dialog is really no more complicated or substantially different than creating a menu or the UI for a module.
  **#d One** constructs dialogs much like a module widget, with an SVG base.
  All the amenities mentioned below for iterating on a (themed) panel apply just the same

- Runtime SVG theming —
  Instead of creating (and maintaining) multiple versions of a panel or widget SVG, one for each theme, **#d One** applies a style sheet at runtime.
  You can find examples of my new (_not-JSON_) SVG theming stylesheets in the `res/themes` folder.

- SVG Hot-swap, and dynamic widget positiong — In dev builds, the new modules (and some of the old ones) support hot-swap panel Svgs, with runtime positioning based on placeholders in the SVG.
  These together make a big difference for iterating on panel designs.
  Oh, and when hot-swapping the SVGs, not only do you get a new panel graphic, and the widgets move to their new positions, but the themes are reloaded as well, so that one keystroke does it all for iterating on the look and feel of a panel design.

## Development builds

To download the latest development build, see the [Nightly Release](https://github.com/Paul-Dempsey/pachde1/releases/tag/Nightly) page.

## Change log

See [CHANGELOG](CHANGELOG.md)

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
- plus Gotno, and Bloodbat for the essential components that make all those cross-builds possible.
- baconpaul for sharing improvements in the Github Actions, and other Rack inspiration.
- Venom (Dave Benham) for great feedback, requests, bug reporting, and all-round good Rack citizen.
- everyone who's beta-tested, reported issues, and given feedback - those make it all possible.

## What else?

I have a couple of other repos for VCV Rack-related projects you may be interested in if you are, or want to become a plugin developer:

- **[Generic Blank](https://github.com/Paul-Dempsey/GenericBlank)**
  This is a great place to get started making a VCV Rack plugin. It's a good alternative to the VCV Rack tutorial. While it's a generic blanking plate out of the box, the code is well-commented with what you need to do to build out a functional module.

- **[Rack dev notes (by pachde #d)](https://github.com/Paul-Dempsey/rack-dev-notes/blob/main/doc/index.md#dev-notes-for-vcv-rack)**.
  A growing set of tech notes for creating and maintaining VCV Rack plugins.

- **[#d CHEM](https://github.com/Paul-Dempsey/pachde-CHEM?tab=readme-ov-file#pachde-chem)**
  Controller for the Haken EaganMatrix. 16 modules for working with EaganMatrix devices, including Haken Continuum, ContinuuMini, EM Micro, and the EaganMatrix Eurorack Module.
  Also, the Osmose from Expressive E.

---
![pachde logo](docs/Logo.svg)
