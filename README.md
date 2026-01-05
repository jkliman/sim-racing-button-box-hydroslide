# sim-racing-button-box-hydroslide
Sim Racing Button Box

BOM:

Electronics
1x Button console
https://tinyurl.com/buttonconsole
4x Toggle Switches
https://tinyurl.com/5fzb23pj
1x Momentary Switch
https://tinyurl.com/y464e2yy
1x piezio electric haptic motor
https://tinyurl.com/24f7a8be
1x ardunio pro micro
https://tinyurl.com/32rhu5z4

Hardware
4x M3x8 threaded inserts
https://tinyurl.com/pwz7wb7x
4x M3x6  button head hex screws
https://tinyurl.com/mrmutwa2
(Mounting Hardware of Choice)

# Ignition Panel - Wiring Diagram

## Arduino Pro Micro Pinout

| Pin | Function |
|-----|----------|
| 2 | Ignition Switch |
| 3 | Start Button |
| 4 | Accessory 1 Switch |
| 5 | Accessory 2 Switch |
| 6 | Accessory 3 Switch |
| 8 | Bypass Switch |
| 9 | Vibration Motor |
| 10, 16 | Acc3 Mode Switch |
| 14, 15 | Acc2 Mode Switch |
| 18, 19 | Acc1 Mode Switch |
| 20, 21 | Ignition Mode Switch |

## Vibration Motor Wiring (Simple Direct Connection)

```
ARDUINO PRO MICRO                 VIBRATION MOTOR
┌─────────────────┐               ┌───────────┐
│                 │               │           │
│           Pin 9 ├───────────────┤ + (Red)   │
│                 │               │           │
│             GND ├───────────────┤ - (Black) │
│                 │               │           │
└─────────────────┘               └───────────┘
```

## Switch Wiring (All Switches)

All switches use INPUT_PULLUP, so wire between the pin and GND:

```
ARDUINO PIN ────────┐
                    │
                   ┌┴┐
                   │ │  SWITCH
                   └┬┘
                    │
GND ────────────────┘
```

## Joystick Button Mapping

| Button | Primary | Secondary (Mode C) |
|--------|---------|-------------------|
| Ignition | 0 | 5 |
| Start | 1 | - |
| Acc1 | 2 | 6 |
| Acc2 | 3 | 7 |
| Acc3 | 4 | 8 |

Upload inginition.ino to the arduino pro micro and configure the usb gaming controller

piezio motor simulates engine start-up vibration when the start button is pushed.


