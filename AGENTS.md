# discord-connector

SA:MP / open.mp plugin bridging Discord to PAWN scripts. C++14, built with **CMake + Conan**, targeting **32-bit x86 only**.

## Build

```bash
git clone --recursive https://github.com/maddinat0r/samp-discord-connector.git
mkdir build && cd build
cmake .. -A Win32 -Thost=x86 -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

- Windows: `-A Win32 -Thost=x86` required. Needs Boost + OpenSSL pre-installed with `USING_BOOST_NO_CONAN=TRUE USING_OPENSSL_NO_CONAN=TRUE` (or Conan handles them).
- Linux: requires `gcc/g++-multilib` (i686). Use `-DCMAKE_CXX_FLAGS=-m32`.
- Output lands in `build/artifact/plugins/` (`.dll`/`.so`) and `build/artifact/pawno/include/` (`.inc`).
- Version defined in root `CMakeLists.txt:3`. Override via `-DDCC_VERSION=...` at configure time.

## Architecture

Single CMake target (`discord-connector`, defined in `src/CMakeLists.txt`). Two entrypoints coexist in `src/main.cpp`:

1. **Classic SA:MP plugin** — exports `Supports`, `Load`, `Unload`, `AmxLoad`, `AmxUnload`, `ProcessTick` via `.def` file (Windows).
2. **open.mp component** — `DiscordComponent` class registered via `COMPONENT_ENTRY_POINT()`.

Core layers: `Network` → `WebSocket` + `Http` → `Bot` → `Guild/Channel/User/Message/Role/Embed/Emoji/Command` managers. `PawnDispatcher` queues PAWN callbacks from Discord event threads.

## Bot token config (priority order)

1. Env var `DCC_BOT_TOKEN`
2. Env var `SAMP_DISCORD_BOT_TOKEN` (deprecated)
3. open.mp: `config.json` key `discord.bot_token`
4. SA:MP: `server.cfg` var `discord_bot_token`

Intents (bitmask) via `DCC_BOT_INTENTS` env var or `discord.intents` / `discord_bot_intents` config. Default: `131071` (all intents).

## Key files

| File | Role |
|------|------|
| `src/main.cpp` | SA:MP plugin + open.mp component entrypoints |
| `src/natives.cpp` | PAWN native function implementations |
| `src/Network.hpp` | HTTP + WebSocket layer |
| `discord-connector.inc.in` | PAWN include template (version substituted by CMake) |
| `pawn.json` | open.mp package manager manifest |

## No tests

No test framework, no test directory. CI only builds.

## CI

`.github/workflows/build.yml` — Builds on `windows-latest` (MSVC) and `ubuntu-20.04` (gcc-9). Fires on pushes/PRs to `master` touching `*.hpp`, `*.cpp`, `CMakeLists.txt`, or workflow file. Build artifacts zipped and uploaded.
