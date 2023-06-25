# midou

Midi output.

Flexible sliders/pads for MIDI control.

MIDI to be sent is specified in text, consisting of a sequence of MIDI events. Channel sets an ambient channel so you can, for example, set channel 1 then send a series of notes and/or CCs, and they will go to that channel (or the channel selected via the priority algorithm).

| Item | Syntax | Notes |
|--|--|--|
| Channel | **ch**&#xa0;(1&#x2011;16\|**a**\|**p**&#xa0;\[**old**\|**lo**\|**hi**\]&#xa0;_m_&#x2011;_n_ | **a** = All, **p** = priority (MPE) channels _m_-_n_ |
| Note | **n-on**\|**n-off** _note_ [_velocity_] | `n 60 1`, `n C#3 127`, `n B-4 {v+8}`, `n D6` |
| Control | **cc** _n_ _value_ | |
| Program change | **pc** _n_ | |
| Pitch bend | **pb** _value_ | |
| SysEx | **sx** _hex-string_ | Send SysEx message |
| **Meta-events** | | |
| Priority | **pri**&#xa0;(**oldest**\|**lowest**\|**highest**) | Channel/voice allocation algorithm |
| Velocity | **v** [0-127] | Set default velocity |
