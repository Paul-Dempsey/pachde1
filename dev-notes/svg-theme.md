# Notes for lightweight (nano)SVG themeing with a json style sheet

> **NOTE**
>
> Since almost everything in pachde-One is drawn procedurally,
> I haven't actually needed SVG theming, so I haven't implemented it (yet).

I did a little research into the parsed representation of SVGs in nano,
and I can see a path to a simple, lightweight, in-process way to implement SVG themeing in a declarative way.

The key is that nano preserves element ids, providing an easy way to target theme-able elements.
Inkscape preserves existing ids across edits of the SVG, so the mechanism is robust to edits of the SVG.

The svg element id is used to target an element for modification.
The element id to be targeted has a suffix beginning with two dashes, followed by a tag.
For example, the following element is tagged with `theme-bezel`:

```xml
<circle id="outside_ring--theme-bezel" cx="7.5" cy="7.5" r="6.2" fill="#999999" />
```

In this example, all the tags begin with `theme-`, but this prefix isn't required.
The only requirement is that each item has a unique name after the last double-dash in the id.

Nano restricts ids to 64 characters.

The JSON is an array of theme objects.
A theme objects has a `"name"` string and a `"theme"` object containing tag-named members that each provide an object contians a definition of what atributes will be modified when the theme is applied.

A limited set of attributes can be modified by theming:

- Color of stroke and fill.

- Stroke width

- Color and offset of a gradient stop.
To target a gradient, the element in the master SVG must also define a gradient.
This is necessary because the scheme does not have a complete definition of a gradient.

  A possibility is to use gradient defs from an SVG (e.g. your panel SVG).
This may be unwieldy to author in regards to start/end coordinates, requiring multiple dimension-dependent gradients.
It may be that both stop-targeting and gradient refs are needed.

Unlike SVG, the theme colors in the json are full Rack-style RGB + Alpha colors,
so we don't need to add complexity of separately targeting opacity (although that is possible).

## Example theme

Not everything you see here may be implemented (e.g. gradient defs), but the scheme is easily extended this way.

```json
[
    {
        "name": "Light",
        "theme": {
            "gradient-defs": "<relative-path>.svg",
            "theme_background": {
                "fill": {
                    "color": "#RRGGBBaa",
                    "gradient": [            
                        {
                            "index":0,
                            "color":"#RRGGBBaa",
                            "offset":0.0
                        }
                    ]
                },
                "stroke": { "color":"#RRGGBBaa", "width":1.5 }
            },
            "theme_bezel": {
                "fill": { "color": "#RRGGBBaa", "gradient-ref": "name" },
                "stroke": { "color": "#RRGGBBaa", "width": 2.0, "gradient-ref": "name" },
            },
            "theme_ouzel": {
                "fill": {
                    "gradient" [
                        {
                            "index":0,
                            "color":"#RRGGBBaa",
                            "offset":0.0
                        },
                        {
                            "index":1,
                            "color":"#RRGGBBaa",
                            "offset":0.0
                        }
                    ]
                }
            }
        }
    },
    {
        "name": "Dark",
        "theme": {
        //...
        }
    }
]
```

## Implementation

The theme description (style sheet) is a simple hash of "style" objects.
To apply the theme, we travers the list of elements in the svg, parsing any element id present to exract a tag.
If the id contains a tag, it's looked up in the style sheet.
If the tag is present in the stylesheet, the style is then applied to modify the appropriate colors of the svg element.

This is a straightforward single pass through the nano parsed representation, so shouldnot have performance issues.

TBD what would be required to manage any caching that rack widgets may be doing, but this is likely straightforward.

## API

API is provided to:

- Load the themes json, return an object providing most of the apis.
- Apply a theme to an svg.
- Add a Theme submenu, containing an option list for the themes defined in the json, and a callback for applying hte selected theme.

More advanced scenarios are conceivable so that less code is needed to take advantage of theme-able SVGs.

- Apply a theme to widget hierarchy for all widgets that implement a specific interface needed to apply it to svgs contained in the widget.
- Query a theme for a tag, which is an open definition so that modules can look up colors for theme-based procedural UI in additiion to SVG-based UI.
- Possibly templates for common widget structures containing SVGs (members, backgrounds, frames) that make it easy to apply a theme.
