#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CONFIG_PATH="$SCRIPT_DIR/postgres.local.sh"

if [[ ! -f "$CONFIG_PATH" ]]; then
  echo "Missing postgres.local.sh. Copy postgres.local.sh.example to postgres.local.sh and set your password." >&2
  exit 1
fi

# shellcheck source=/dev/null
source "$CONFIG_PATH"

if [[ -z "${BLOCKCHAIN_POSTGRES_CONNINFO:-}" ]]; then
  echo "BLOCKCHAIN_POSTGRES_CONNINFO is not set." >&2
  exit 1
fi

if [[ -d /mingw64/bin && ":$PATH:" != *":/mingw64/bin:"* ]]; then
  export PATH="/mingw64/bin:$PATH"
fi

if [[ ! -f "$SCRIPT_DIR/zkp_server.exe" ]]; then
  echo "Missing zkp_server.exe in $SCRIPT_DIR. Build the server first." >&2
  exit 1
fi

exec "$SCRIPT_DIR/zkp_server.exe" "$@"
