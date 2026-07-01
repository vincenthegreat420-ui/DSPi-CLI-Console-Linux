#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"

PROFILE_NAME="dspi-iec958.conf"
PATH_NAME="dspi-iec958-output.conf"
RULE_NAME="91-dspi-pipewire-alsa.rules"

PROFILE_SRC="${SCRIPT_DIR}/${PROFILE_NAME}"
PATH_SRC="${SCRIPT_DIR}/${PATH_NAME}"
RULE_SRC="${SCRIPT_DIR}/${RULE_NAME}"

NO_RESTART=0
DRY_RUN=0

usage() {
    cat <<EOF
Usage: $(basename "$0") [--no-restart] [--dry-run]

Installs the local DSPi ACP profile, mixer path, and udev rule.

Options:
  --no-restart   Do not restart user audio services after install.
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

need_cmd() {
    if ! command -v "$1" >/dev/null 2>&1; then
        echo "missing required command: $1" >&2
        exit 1
    fi
}

run() {
    printf '+'
    printf ' %q' "$@"
    printf '\n'
    if [[ "${DRY_RUN}" -eq 0 ]]; then
        "$@"
    fi
}

first_existing_dir() {
    local dir
    for dir in "$@"; do
        if [[ -d "${dir}" ]]; then
            printf '%s\n' "${dir}"
            return 0
        fi
    done
    return 1
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

print_dspi_summary() {
    if ! command -v pactl >/dev/null 2>&1; then
        echo "pactl not found; skipping PulseAudio/PipeWire summary."
        return
    fi
    if ! pactl info >/dev/null 2>&1; then
        echo "pactl cannot connect to the current audio server; skipping summary."
        return
    fi

    echo
    echo "DSPi cards:"
    pactl list cards short | grep -F 'WeebLabs_Weeb_Labs_DSPi' || echo "  (none visible)"
    echo
    echo "DSPi sinks:"
    pactl list sinks short | grep -F 'WeebLabs_Weeb_Labs_DSPi' || echo "  (none visible)"
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
        echo "DSPi sound card is not currently present; install will apply after replug/reboot."
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
            if ! command -v systemctl >/dev/null 2>&1; then
                echo "systemctl not found; restart PipeWire manually or re-login."
                return 0
            fi
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

for src in "${PROFILE_SRC}" "${PATH_SRC}" "${RULE_SRC}"; do
    if [[ ! -f "${src}" ]]; then
        echo "missing required file: ${src}" >&2
        exit 1
    fi
done

need_cmd sudo
need_cmd install
need_cmd udevadm

PROFILE_DIR="$(first_existing_dir \
    /usr/share/alsa-card-profile/mixer/profile-sets \
    /usr/share/pulseaudio/alsa-mixer/profile-sets)" || {
    echo "could not find an ACP profile-set directory." >&2
    echo "Expected one of:" >&2
    echo "  /usr/share/alsa-card-profile/mixer/profile-sets" >&2
    echo "  /usr/share/pulseaudio/alsa-mixer/profile-sets" >&2
    exit 1
}

PATH_DIR="$(first_existing_dir \
    /usr/share/alsa-card-profile/mixer/paths \
    /usr/share/pulseaudio/alsa-mixer/paths)" || {
    echo "could not find an ACP mixer paths directory." >&2
    echo "Expected one of:" >&2
    echo "  /usr/share/alsa-card-profile/mixer/paths" >&2
    echo "  /usr/share/pulseaudio/alsa-mixer/paths" >&2
    exit 1
}

PROFILE_DST="${PROFILE_DIR}/${PROFILE_NAME}"
PATH_DST="${PATH_DIR}/${PATH_NAME}"
RULE_DST="/etc/udev/rules.d/${RULE_NAME}"

echo "Detected profile directory: ${PROFILE_DIR}"
echo "Detected mixer path directory: ${PATH_DIR}"
print_dspi_summary

echo
echo "Installing DSPi ACP files..."
run sudo install -m 0644 "${PROFILE_SRC}" "${PROFILE_DST}"
run sudo install -m 0644 "${PATH_SRC}" "${PATH_DST}"
run sudo install -m 0644 "${RULE_SRC}" "${RULE_DST}"

echo "Reloading udev rules..."
run sudo udevadm control --reload-rules

echo "Retriggering DSPi sound card if present..."
retrigger_dspi_cards

restart_audio

echo
echo "Installed DSPi IEC958 profile."
print_dspi_summary
echo
echo "Expected result: one DSPi profile named Digital Stereo (IEC958), with HW_VOLUME_CTRL on the DSPi sink."
