# Info

A Minimalist resizable patch notes panel.

**See also:** [All pachde-One modules](index.md)

Here are some samples with Light, Dark and High Contrast themes, and selected options.

![Info panel with themes and menu](./images/Info.png)

- Resizable - Drag the left and right edges when the overlay appears under the cursor.

## Options

| Option | Description |
| -- | -- |
| Screws | Add or remove the screw caps. |
| Theme | Choose Light, Dark, or High Contrast theme. |
| Panel color | Enter a standard Rack #_hex_ code for a custom color to override the theme. Even the screw caps adopt your color!. You can copy/paste the hex code from Copper. |
| Bright text in a dark room. | The text and text background stay bright in a darkened room. |
| Edit Info > | Edit the info displayed on the panel. Use the standard Rack text input to add text. |
| Copy info | Copy the info to the clipboard. |
| Paste info | Paste the info from the clipboard. |
| Font... | Open a Truetype font (.ttf) to use for the text. The name of the font file current used appears above this item. |
| Font size | Pick a size for the text using the standard Rack slider. |
| Text orientation | Choose from **Normal**, **Down (right)**, **Left (up)**, or **Inverted** |
| Text alignment | Choose **Left**, **Center**, or **Right**-aligned layout. |
| Text color  | A standard Rack #_hex_ code for the color of the text. |
| Copper | Choose which colors the Copper expander module sets. Choose from **None**, **Panel** or **Interior**. When set to **None**, the module is not affected by Copper. When set to **Interior**, it gets Background color from a Copper on the left, and Text color from a Copper on the right. |

## Fonts

Any Truetype font can be used. There are thousands of free fonts available for download on the web.
[Google Fonts](https://fonts.google.com/) has some nice ones.

On Windows, it can be difficult to browse to a font in the Windows font folder because the file browser doesn't show the actual .ttf files.
But there's a trick:
You can get to the actual file path you need from a regular Windows command prompt.

Start a command prompt and type:

```console
dir /S /B c:\windows\fonts\*.ttf
```

Copy the full path of file, then paste it into the Font chooser dialog.

## Expander

[Copper](Copper.md) expands **Info** to set it's panel and text colors.
By default Copper sets the panel color.

---

![pachde (#d) Logo](Logo.svg)
