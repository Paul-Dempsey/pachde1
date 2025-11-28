# pachde1 TODO

- nicer-looking tiny button?

## Info

Review Copper interaction - docs describe an "Interior" setting which does panel on the left and text on the right. Today, I'd look at using remote connections rather than extenders. See Copper notes on "sticky" extender for the idea, like CHEM has "kiss" to connect.

Better to do Left-Copper and Right-Copper settings, more flexible than "interior".

## Skiff

- Undo for PackModules?

## Guide: Panel Design Guides

- Widget mover: move widget
  - Record position data as text that can be copied
  - Position data in format that can be used at runtime (provide library for runtime positioning)
  - Get param/light/in/out id, or other: enter name, or [type name](https://en.cppreference.com/w/cpp/types/type_info/name)

- Panel blank module to use with designer.
  - Basic panel templates
  - User SVG
  - Widget library
  - Save to working folder
  - Generate ModuleWidget code

## FancyBox

- randomize
- randomize trigger

## Copper

Idea: "Remote extender"/"Sticky extender"/"Linked extender" - once extended, allowing moving the extender elsewhere in the patch, retaining the connection.
Message passing doesn't work in this scenario.

## Alternate module browser

- ala CHEM: Preset, with selectable width.
- Search/filter defined fields.
- Select columns.
- Custom tags (with export/import for community sharing).
- Pop out to full screen.
- Multi-select for adding.
- Companion detail extender to show full details of selected module.