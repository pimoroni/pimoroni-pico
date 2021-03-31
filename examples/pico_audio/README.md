# Pico Audio Pack Demo

This demo requires the `pico-extras` repository to compile - https://github.com/raspberrypi/pico-extras.

You should clone it alongside `pico-sdk` and `pimoroni-pico`:

```
git clone https://github.com/raspberrypi/pico-extras
```

And adjust your `cmake` configure command to include the path:

```
cmake .. -DPICO_SDK_POST_LIST_DIRS=/path/to/pico-extras
```

If you're using Visual Studio Code you can add this to `settings.json`:

```json
{
    "cmake.configureSettings": {"PICO_SDK_POST_LIST_DIRS": "/path/to/pico-extras"}
}
```

