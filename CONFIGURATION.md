# Configuration
Environment variables:
- `NEON_NO_TELEMETRY=1` — Disable metrics/trace at runtime.
- `NO_COLOR=1` — Request no-color output in supported terminals.
Build options (CMake):
- See `CMakeLists.txt` and `CMakePresets.json` for documented flags (ASAN/TSAN, unity, PCH, LLD, etc.).


## File-based configuration
You can provide settings via an INI file. Precedence:
1. `$NEON_CONFIG_FILE`
2. `$XDG_CONFIG_HOME/neon/neon.conf`
3. `$HOME/.config/neon/neon.conf`
4. `/etc/neon/neon.conf` (POSIX)

Supported keys (sectionless):
- `telemetry = on|off` — if **off**, CLI helpers avoid emitting trace data.

Example:
```ini
# ~/.config/neon/neon.conf
telemetry = off
```
