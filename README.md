# DSPi CLI for Linux

This is a Linux CLI tool for configuring a [WeebLabs DSPi](https://github.com/WeebLabs/DSPi/). It does not expose a GUI like the official Mac and Windows versions.

## Dependencies

Debian/Ubuntu:

```sh
sudo apt install build-essential pkg-config libusb-1.0-0-dev
```

Fedora:

```sh
sudo dnf install gcc make pkgconf-pkg-config libusb1-devel
```

Arch:

```sh
sudo pacman -S base-devel pkgconf libusb
```

## Build

```sh
make
```

The resulting executable is `dspi-cli`.

## Install

Install the CLI to `/usr/local/bin`:

```sh
sudo make install
```

To install somewhere else, override `PREFIX`:

```sh
sudo make install PREFIX=/usr
```

Uninstall the CLI and udev rule:

```sh
sudo make uninstall
```

## Device Permissions

Install the udev rule so the active desktop user can access the DSPi device
without `sudo`:

```sh
sudo make install-udev
sudo udevadm control --reload-rules
sudo udevadm trigger
```

Unplug and reconnect the DSPi after reloading rules. The rule uses systemd's
`uaccess` tag, which is supported by Fedora and most desktop Linux
distributions.

## PipeWire Card Profile

The DSPi is a USB Audio Class device with a physical S/PDIF output. Without a
DSPi-specific card profile, PipeWire can expose it as two outputs (analog and
digital) and the digital output volume control may not work correctly.

The files in `pipewire-card-profile/` install a card profile, mixer path, and udev
rule that make PipeWire use a single `Digital Stereo (IEC958)` profile for the
DSPi and expose the USB Audio hardware volume control on the digital output.

> **PipeWire only.** This relies on PipeWire's ACP mechanism (the
> `ACP_PROFILE_SET` udev property and the `~/.config/alsa-card-profile` search
> path). It does not take effect under the classic PulseAudio daemon, which uses
> a different udev property (`PULSE_PROFILE_SET`) and config directory. PipeWire
> is the default audio server on most current mainstream desktops, including
> Fedora, Ubuntu, Pop!_OS, Linux Mint, Debian and openSUSE Tumbleweed, and is
> the standard choice on Arch.

Install the profile:

```sh
cd pipewire-card-profile
./install-dspi-profile.sh
```

The installer:

- copies the profile and mixer path into your user config directory
  (`$XDG_CONFIG_HOME/alsa-card-profile/mixer`, no root required). PipeWire's ACP
  loader reads this in addition to `/usr/share`, so it also works on immutable
  ostree systems such as Fedora Silverblue/Kinoite where `/usr/share` is
  read-only;
- installs `/etc/udev/rules.d/91-dspi-pipewire-alsa.rules` (needs root), reloads
  udev, and retriggers the DSPi sound card if it is connected;
- restarts the user audio services when possible.

If you run the installer inside a Toolbx container, the udev steps are relayed
to the host with `flatpak-spawn --host` (you authenticate once via polkit),
since udev lives on the host rather than in the container.

Useful installer options:

```sh
./install-dspi-profile.sh --dry-run
./install-dspi-profile.sh --no-restart
```

After installation, unplug and reconnect the DSPi if it does not immediately
appear as a single digital output.

Remove the profile:

```sh
cd pipewire-card-profile
./uninstall-dspi-profile.sh
```

## Commands

```sh
./dspi-cli list
./dspi-cli version
./dspi-cli platform
./dspi-cli serial
./dspi-cli status
./dspi-cli get-eq 0 0
./dspi-cli set-eq 0 0 peaking 1000 0.707 -3.0
./dspi-cli set-eq 0 1 lowshelf 120 0.7 2.5 off
./dspi-cli get-preamp
./dspi-cli set-preamp -6
./dspi-cli get-bypass
./dspi-cli set-bypass on
./dspi-cli get-master-volume
./dspi-cli set-master-volume -12
./dspi-cli get-user-volume
./dspi-cli set-user-volume -20
./dspi-cli get-loudness
./dspi-cli set-loudness on
./dspi-cli get-output-gain 0
./dspi-cli set-output-gain 0 -3
./dspi-cli get-matrix 0 0
./dspi-cli set-matrix 0 0 on 0 off
./dspi-cli get-input-source
./dspi-cli get-channel-name 0
./dspi-cli preset-list
./dspi-cli preset-load 0
./dspi-cli preset-save 0
./dspi-cli get-all-params backup.bin
./dspi-cli save
./dspi-cli factory-reset
./dspi-cli raw-in 0x7f 0 2 4
```

Run `./dspi-cli help` for the full command list.

EQ channels and bands are the raw firmware indexes. Common channels are `0`
for USB/input left and `1` for USB/input right. The packet sent by `set-eq` is
`[ch, band, type, bypass, freq_le32, q_le32, gain_le32]`.

## Uploading REW/Equalizer APO Files

Use `upload-apo` to parse a REW/Equalizer APO text file and upload each enabled
filter:

```sh
./dspi-cli upload-apo my-apo-preset.txt 0,1
```

By default it uploads up to 10 filters starting at band 0 and clears unused
bands to flat. Useful options:

```sh
./dspi-cli upload-apo profile.txt 0 --dry-run
./dspi-cli upload-apo profile.txt 0,1
./dspi-cli upload-apo profile.txt 0 2 8 --no-clear
./dspi-cli upload-apo profile.txt 0 --apply-preamp
```

Uploads only change the running device state. Run `./dspi-cli save`
separately when you want to commit the result to flash.

## Feature Status

Implemented:

- Device discovery, version reporting, platform, serial, status, sample rate,
  clip clearing, and raw USB request access.
- EQ band control and REW/Equalizer APO upload, including preamp import.
- Bypass, preamp, loudness, crossfeed, leveller, master/user volume, channel
  gain/mute/name, output gain/mute/delay/enable, and matrix routing.
- Preset load/save/delete/name/startup commands, full parameter backup/restore,
  save, factory reset, and bootloader entry.
- Output pin/type, I2S BCK, MCK, input source, S/PDIF RX, LG Sound Sync, DAC
  hardware mute, and buffer statistics commands.

Missing or limited:

- No notification endpoint listener for DSPi notification protocol v2.
- No named commands yet for USB error diagnostics or vendor user mute controls.
- Some diagnostics are still raw or lightly parsed, including S/PDIF RX status,
  S/PDIF channel status, and buffer/status counters.
- Firmware update support only enters the UF2 bootloader; it does not copy UF2
  firmware files.

## License

This project is licensed under the GNU General Public License v3.0. See
`LICENSE`.
