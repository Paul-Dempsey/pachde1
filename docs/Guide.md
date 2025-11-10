# Guide

This one is for plugin developers.
Place **Guide** to the right of a module you're working on.
It works very much like an extender for the panel on the left.
When immediately to the right of a module, **Guide** adds an overlay to the module to screen the panel and display guides.
You can adjust the transparency fo the overlay.

When you move **Guide** so that it is no longer adjacent to a module, the overlay disappears.

Guides aren't shown on the module added to the list.

At the top is a preview swatch and the color picker for the overlay on the left.
To its right is a switch to select the position of the overlay.
You can choose from covering only the panel, or also covering the knobs, plugs, and other widgets on the panel.
WHen widgets are covered, you can no longer interact with the module.

The next row is a selection of pre-made guides you can add.
From the hamburger menu at the end of the row of templates, you can Save a guide setup to a file or Open a previously made one.
There is also selection of Factory templates.

Templates are additive, so you can build up a combined set of guides.


The edit box lets you provide a descriptive name for a guide added to the list.
If you leave it empty, **Guide** names it for you when you click **+** to add it the list.

In the next row is a preview swatch and picker for the color of guide.

The knobs control the parameters of a guide.

|Knob|Description|
| :-: | - |
| X| X position of the guide. A negative value is the offset from the right edge. Used for right margin. |
| Y| Y position of the guide. A negative value is the offset from the bottom. Used for bottom margin. |
| W| Width of the guide. |
| R| Repeat interval. The guide is repeated every _R_ pixels across the panel. |
| A| Angle. At the time of this writing, only 0° and 90° are supported. |

| Button | Decription |
|:-:| |
|**+** | AAdds the current configuration as a guide. If you currently have a guide sleected in the list, that guide is updated. |
| **-** | Removes the current guide from the list. |
| **x** | Clears the entire list. |