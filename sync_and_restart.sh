#!/bin/bash
SRC_DIR="$(pwd)"
PI_USER="tanza"
PI_HOST="192.168.1.211"
PI_DIR="/home/tanza/pv-c"
PROGRAM="./bin/pv-c"
RSYNC_EXCLUDE="--exclude .git --exclude build"
SCREEN_SESSION="pv-c"

sync_and_restart() {
    echo "Syncing and building on Pi..."
    rsync -az $RSYNC_EXCLUDE "$SRC_DIR/" "$PI_USER@$PI_HOST:$PI_DIR/"

ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null $PI_USER@$PI_HOST bash <<EOF
cd "$PI_DIR"
# kill old program safely
pkill -f "$PROGRAM" || true
# build
./build.sh
# ensure screen exists
echo "Finding screen"
screen -ls | grep -q "$SCREEN_SESSION" || screen -S "$SCREEN_SESSION" -D -m bash
# start new program in screen
echo "Starting in screen"
screen -S "$SCREEN_SESSION" -X stuff "cd $PI_DIR && $PROGRAM^M"
EOF

}

# --- run once immediately ---
sync_and_restart

# --- loop watching for changes ---
while true; do
    inotifywait -r -e modify,create,delete,move "$SRC_DIR" || true
    sync_and_restart
done
