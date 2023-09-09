# Dev notes for pachde-One (et al.) plugin(s)

- Contemplate designing a standard for color interchange using CV, if there isn't one already.
Might be a nicer way to connect Copper than as an expander.
I thought I had seen one on the forums, but I can't find it now.

  Packing a full RGBA color into a float doesn't work,
  because some colors could create NaNs which are blocked by the engine in 2.x.
  One could do multi-sample signalling: send a negative 1 as a start marker, followed by 4 floats of RGBA.
  This protocol could be extended to send other data, much like many other protocols.

  I can imagine data cables as a first-class thing in Rack, where you use a USB port visual.

  I see that it is possible for modules to use MIDI (which, with some effort can send arbitrary data), but these connections aren't visible - just configuration of the Midi ports.

- Contemplate a design for inter-module communication that isn't a cable (CV) or adjacency (expander).
