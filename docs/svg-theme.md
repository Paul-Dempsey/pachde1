# Sketch of lightweight SVG themeing with a json style sheet

The svg element id is used to target an element for modification.
The element id to be targeted has a suffix beginning with two dashes, followed by a tag.
For example, the following element is tagged with `theme-bezel`:

```xml
<circle id="outside_ring--theme-bezel" cx="7.5" cy="7.5" r="6.2" fill="#999999" />
```

In this example, all the tags begin with `theme-`, but this prefix isn't required.
The only requirement is that each item has a unique name.

A theme in the JSON is an object containing tag-named children that provide a definition of what atributes will be modified.

A limited set of attributes can be modified by theming:

- Color of stroke and fill.

- Stroke width

- Color and offset of a gradient stop.
To target a gradient, the element in the master SVG must also define a gradient.
This is necessary because the scheme does not have a complete definition of a gradient.

  A possibility is to use gradient defs from an SVG (e.g. your panel SVG).
This may be unwieldy to author in regards to start/end coordinates, requiring multiple dimension-dependent gradients. It may be that both stop-targeting and gradient refs are needed.

Unlike SVG, the theme color attributes in the theme json are full Rack-style RGB + Alpha colors, so we don;t need to add complexity of separately targeting opacity (although that is possible).

## Example theme

```jsonc
[
    {
        "name": "Light",
        "theme": {
            "gradient-defs": "<relative-path>.svg", // svg containg defs tag definining id'd gradients
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

The theme description (style sheet) is a simple hash of styles.
To apply the theme, we travers the list of elements in the svg, parsing any element id present to exract a theme tag. If the id contains a them tag, it's looked up in the style sheet. The style is then applied to modify the appropriate members of the svg element.

Uncertain what invalidation would be required to invalidate any caching that rack widgets may be doing.

## API

The following interface is required to be implemented for theme-able widgets:

```cpp
struct IApplyTheme {
    void applyTheme(SvgTheme* theme) = 0;
}
```

The implementation will apply the theme to any Svgs it contains.

API is provided to:

- Load set of themes.
- Add a Theme submenu, containing an option list for the themes defined in the set.
- Apply a theme to an svg.
- Apply a theme to widget hierarchy for all widgets that implement IApplyTheme.
- Query theme for tag, which is an open definition so that modules can use theme-based explicit drawing, in additiion to SVG-based ui.

Possibly templates for various structures containing SVGs (members, backgrounds, frames).
