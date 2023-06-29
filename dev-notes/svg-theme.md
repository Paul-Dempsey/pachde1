# Notes for lightweight (nano)SVG themeing with a json style sheet

> **NOTE**
>
> Since almost everything in pachde-One is drawn procedurally,
> I haven't actually needed SVG theming, so I haven't implemented it (yet).

I did a little research into the parsed representation of SVGs in nano,
and I can see a path to a simple, lightweight, in-process way to implement SVG themeing in a declarative way.

The key is that nano preserves element ids, providing an easy way to target theme-able elements.
Inkscape preserves ids across edits of the SVG, so the mechanism is robust to editing.

The svg element id is used to target an element for modification.
The element id to be targeted has a suffix beginning with two dashes, followed by a tag.
For example, the following element is tagged with `theme-bezel`:

```xml
<circle id="outside_ring--theme-bezel" cx="7.5" cy="7.5" r="6.2" fill="#999999" />
```

In this example, all the tags begin with `theme-`, but this prefix isn't required.
It's just a handy way to notate that the element is a theme-able element.

If a double dash isn't present, then the entire id is considered the tag.
This means that you can target specific elements by id, or multiple elements with a double-dash tag.

You can edit an object's id in Inkscape: Right click and choose **Object Properties...**. The id is separate from Inkscape's object label which is kept in a separate `inkscape:label` attribute. Inkscape uses it's labels for what it shows you in **Layers and Objects**.

Nano restricts ids to 63 characters.

The JSON is an array of theme objects.
A theme objects has a `"name"` string and a `"theme"` object containing tag-named members that each provide an object contians a definition of what atributes will be modified when the theme is applied.

A limited set of attributes can be modified by theming:

- Opacity of the element.

- Stroke and fill color. Colors include opacity.
Targeting only the opacity of a stroke or fill color is currently not supported -- only the full rgba.

- Stroke width

- Color and offset of an existing gradient stop.
To target a gradient, the element in the master SVG must also define a gradient with that stop.
This is necessary because the scheme does not have a complete definition of a gradient,
and we don't have the ability to synthesize gradients.

- Gradients can't be removed. A color fill or stroke can be changed to 'none', but setting a gradient fill or stroke to 'none' or a color would cause a memory leak.

- Stroke dashes not supported, but could be.

Unlike SVG, the theme colors in the json are full RGB + Alpha colors,
so we don't need to add complexity of separately targeting opacity (although that is possible).

## Example theme

```json
[
    {
        "name": "Light",
        "theme": {
            "theme_background": {
                "fill": {
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
            "theme_no-fill": {
                "fill":"none",
            },
            "theme_no-stroke": {
                "stroke":"none",
            },
            "theme_bezel": {
                "fill": "#RRGGBBaa",
                "stroke": "#RRGGBBaa",
            },
            "theme_ouzel": {
                "Opacity": 0.8,
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

## Creating a theme

- Start with a design that will be one of your themes.

- Decide which elements will change depending on the theme.

- For each theme-able element, define a style to address the things that will be changed, with the attributes your base SVG.
This is necessary so that user can get back to the default theme after changing it.

- Copy the theme, rename it, and change the colors to suit.

- To prevent leftovers from one theme appearing when another is chosen, each theme must provide all the same style names, setting the same alements.
At this time, the theme engine does not provide a way to verify this, so care must be taken.

## Implementation

The theme description (style sheet) is a simple hash of "style" objects.
To apply the theme, traverse the list of elements in the svg, parsing any element id present to exract a tag.
If the id contains a tag, it's looked up in the style sheet.
If the tag is present in the stylesheet, the style is then applied to modify the appropriate colors of the svg element.

It would be easy to support targeting entire tag ids.

This is a straightforward single pass through the nanosvg parsed representation, so should not have performance issues.

TBD what would be required to manage caching that rack widgets may be doing, but this is likely straightforward.
This likely requires nothing more than sending a Dirty or Change event. For any widget that is modified when applying a theme.

## API

API is provided to:

- Load the themes json, return an object providing most of the apis.
- Apply a theme to an svg.
- Add a Theme submenu, containing an option list for the themes defined in the json, and a callback for applying the selected theme.

More advanced scenarios are conceivable so that less code is needed to take advantage of theme-able SVGs.

- Apply a theme to widget hierarchy for all widgets that implement a specific interface needed to apply it to svgs contained in the widget.
- Query a theme for a tag, which is an open definition so that modules can look up colors for theme-based procedural UI in additiion to SVG-based UI.
- Possibly templates for common widget structures containing SVGs (members, backgrounds, frames) that make it easy to apply a theme.
