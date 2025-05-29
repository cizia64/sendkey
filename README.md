# sendkey

A lightweight C utility to simulate key presses, button input, and D-Pad/trigger movements by writing directly to a Linux `/dev/input/eventX` device.

Ideal for handhelds like the Trimui Smart Pro.

It allows to send combo keys from command line.

## Features

- Sends `EV_KEY` events (standard buttons)
- Sends `EV_ABS` events (D-Pad and triggers like L2/R2)
- Supports press (`1`), release (`0`), and quick tap (`2`)

## Supported Keys

| Name    | Linux Code         | Type     |
|---------|--------------------|----------|
| B       | `BTN_SOUTH`        | `EV_KEY` |
| A       | `BTN_EAST`         | `EV_KEY` |
| X       | `BTN_WEST`         | `EV_KEY` |
| Y       | `BTN_NORTH`        | `EV_KEY` |
| MENU    | `BTN_MODE`         | `EV_KEY` |
| SELECT  | `BTN_SELECT`       | `EV_KEY` |
| START   | `BTN_START`        | `EV_KEY` |
| L1      | `BTN_TL`           | `EV_KEY` |
| R1      | `BTN_TR`           | `EV_KEY` |
| L2      | `ABS_Z`            | `EV_ABS` |
| R2      | `ABS_RZ`           | `EV_ABS` |
| UP      | `ABS_HAT0Y=-1`     | `EV_ABS` |
| DOWN    | `ABS_HAT0Y=+1`     | `EV_ABS` |
| LEFT    | `ABS_HAT0X=-1`     | `EV_ABS` |
| RIGHT   | `ABS_HAT0X=+1`     | `EV_ABS` |

## Build

```sh
gcc -o sendkey sendkey.c
```

## Usage

```sh
./sendkey /dev/input/eventX [KEY] [VALUE] ...
```

### Value options:

- `1` → press
- `0` → release
- `2` → quick tap (press + release with 20ms delay)

### Examples

Press then release `A` and `B`:
```sh
./sendkey /dev/input/event0 A 1 A 0 B 1  B 0
```

Quick tap `START`:
```sh
./sendkey /dev/input/event0 START 2
```

Press D-Pad left:
```sh
./sendkey /dev/input/event0 LEFT 1 LEFT 0
```

Press and release L2 (analog trigger):
```sh
./sendkey /dev/input/event0 L2 1 L2 0
```

## Notes

- Requires write permission to `/dev/input/eventX`
- On some devices (e.g. Trimui Smart Pro), the D-Pad is reported as `ABS_HAT0X/Y` with values `-1`, `0`, `1`
- Triggers like L2/R2 use `ABS_Z` / `ABS_RZ` and expect values between 0 and 255 (this tool uses `255` as press, `0` as release)

## License

MIT




If you enjoy my work and want to support the countless hours/days invested, here are my sponsors:

- [![Patreon](_assets/readme/patreon.png)](https://patreon.com/Cizia)
- [![Buy Me a Coffee](_assets/readme/bmc.png)](https://www.buymeacoffee.com/cizia)
- [![ko-fi](_assets/readme/ko-fi.png)](https://ko-fi.com/H2H7YPH3H)


