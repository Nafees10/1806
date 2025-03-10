# 1806

Extremely lightweight
[Interception Tools](https://gitlab.com/interception/linux/tools) plugin for
remapping keys.

## Building

Pre-requisites:

- `git`
- `gcc`
- `make`
- Interception Tools

Run the following to build and install:

```bash
git clone https://github.com/Nafees10/1806.git
cd 1806
make
strip 1806
sudo cp 1806 /usr/local/bin/
```

## Setting It Up

Below is an example udevmon config (`/etc/interception/udevmon.yaml`):

```yaml
- JOB: "intercept -g $DEVNODE | 1806 | uinput -d $DEVNODE"
  DEVICE:
    EVENTS:
      EV_KEY: [KEY_W, KEY_A, KEY_S, KEY_D]
    NAME: ".*((k|K)(eyboard|EYBOARD)|).*"
```

After writing this file, execute the following as root:
```bash
sleep 2; systemctl restart udevmon.service
```

---

## Key Mappings

The key mappings are hardcoded. Currently, they are as follows:

- `KEY_CAPSLOCK` -> `KEY_ESCAPE`
- `KEY_ESCAPE` -> `KEY_GRAVE`
- `KEY_RIGHTALT` -> activates `alt_layer`. `KEY_RIGHTALT` itself is never sent

### `alt_layer`

- `HJKL` -> arrow keys
- `KEY_O` -> `KEY_HOME`
- `KEY_P` -> `KEY_END`
- `[` -> `KEY_PAGEUP`
- `]` -> `KEY_PAGEDOWN`
- `KEY_M` -> `KEY_MUTE`
- `KEY_COMMA` -> `KEY_VOLUMEDOWN`
- `KEY_DOT` -> `KEY_VOLUMEUP`
- `KEY_1` to `KEY_0` -> `KEY_F1` to `KEY_F10`
- `KEY_MINUS` -> `KEY_F11`
- `KEY_EQUAL` -> `KEY_F12`
- `QWE` `ASD` `ZXCV` -> `789` `456` `1230`

> [!NOTE]
> The above mapping list may go out of date as the program is updated. The
> actual implementation is just a switch statement in the source code, you
> should see that for the actual mappings.
