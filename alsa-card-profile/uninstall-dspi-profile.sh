#!/usr/bin/env bash
set -euo pipefail

PROFILE_NAME="dspi-iec958.conf"
PATH_NAME="dspi-iec958-output.conf"
RULE_NAME="91-dspi-pipewire-alsa.rules"

NO_RESTART=0
DRY_RUN=0

usage() {
    cat <<EOF
Usage: $(basename "$0") [--no-restart] [--dry-run]

Removes the local DSPi ACP profile, mixer path, and udev rule.

Options:
  --no-restart   Do not restart user audio services after uninstall.
  --dry-run      Print what would be done without changing the system.
  -h, --help     Show this help.
EOF
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --no-restart) NO_RESTART=1 ;;
        --dry-run) DRY_RUN=1 ;;
        -h|--help) usage; exit 0 ;;
        *) echo "unknown option: $1" >&2; usage >&2; exit 2 ;;
    esac
    shift
done

run() {
    printf '+'
    printf ' %q' "$@"
    printf '\n'
    if [[ "${DRY_RUN}" -eq 0 ]]; then
        "$@"
    fi
}

audio_service_state() {
    if command -v systemctl >/dev/null 2>&1; then
        if systemctl --user --quiet is-active wireplumber 2>/dev/null ||
           systemctl --user --quiet is-active pipewire 2>/dev/null; then
            echo "pipewire"
            return
        fi
    fi
    if command -v pactl >/dev/null 2>&1 && pactl info 2>/dev/null | grep -qi '^Server Name:.*PulseAudio'; then
        echo "pulseaudio"
        return
    fi
    echo "unknown"
}

retrigger_dspi_cards() {
    local found=0
    local card real

    [[ -d /sys/class/sound ]] || return 0
    for card in /sys/class/sound/card*; do
        [[ -e "${card}/id" ]] || continue
        [[ "$(cat "${card}/id")" == "DSPi" ]] || continue
        real="$(readlink -f "${card}")"
        found=1
        run sudo udevadm trigger --action=change "${real}"
    done

    if [[ "${found}" -eq 0 ]]; then
        echo "DSPi sound card is not currently present."
    fi
}

restart_audio() {
    local server
    server="$(audio_service_state)"

    if [[ "${NO_RESTART}" -eq 1 ]]; then
        echo "Skipping audio service restart (--no-restart). Replug DSPi or restart audio manually."
        return 0
    fi

    case "${server}" in
        pipewire)
            echo "Restarting user PipeWire services..."
            run systemctl --user restart wireplumber pipewire pipewire-pulse
            ;;
        pulseaudio)
            echo "Restarting PulseAudio..."
            if command -v systemctl >/dev/null 2>&1 &&
               systemctl --user list-unit-files pulseaudio.service >/dev/null 2>&1; then
                run systemctl --user restart pulseaudio.service
            else
                run pulseaudio -k
            fi
            ;;
        *)
            echo "Could not detect PipeWire or PulseAudio. Replug DSPi or restart your audio session manually."
            ;;
    esac
}

if ! command -v sudo >/dev/null 2>&1; then
    echo "missing required command: sudo" >&2
    exit 1
fi
if ! command -v udevadm >/dev/null 2>&1; then
    echo "missing required command: udevadm" >&2
    exit 1
fi

TARGETS=(
    "/usr/share/alsa-card-profile/mixer/profile-sets/${PROFILE_NAME}"
    "/usr/share/alsa-card-profile/mixer/paths/${PATH_NAME}"
    "/usr/share/pulseaudio/alsa-mixer/profile-sets/${PROFILE_NAME}"
    "/usr/share/pulseaudio/alsa-mixer/paths/${PATH_NAME}"
    "/etc/udev/rules.d/${RULE_NAME}"
)

echo "Removing DSPi ACP files..."
for target in "${TARGETS[@]}"; do
    if [[ -e "${target}" || "${DRY_RUN}" -eq 1 ]]; then
        run sudo rm -f "${target}"
    fi
done

echo "Reloading udev rules..."
run sudo udevadm control --reload-rules

echo "Retriggering DSPi sound card if present..."
retrigger_dspi_cards

restart_audio

echo "Removed DSPi IEC958 profile."
