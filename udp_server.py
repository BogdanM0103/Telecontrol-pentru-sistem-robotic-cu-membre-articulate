#!/usr/bin/env python3
import socket
import subprocess
import logging

# ── Configuration ─────────────────────────────────────────────────────────────
UDP_IP      = "0.0.0.0"   # Listen on all interfaces
UDP_PORT    = 6006        # Must match the client’s UDP_PORT
REMOTE_BIN  = "/home/bogdan/Desktop/Hexapod/cmake-build-debug/cmake-build-debug/cmake-build-debug/HexapodRobot"
SIMPLE_CMDS = {"forward", "rotate_left", "rotate_right", "move_backward"}
# ───────────────────────────────────────────────────────────────────────────────

# Set up logging
logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s [udp_server] %(message)s",
    datefmt="%Y-%m-%d %H:%M:%S"
)

def main():
    # Create & bind UDP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind((UDP_IP, UDP_PORT))
    logging.info(f"Listening for UDP on {UDP_IP}:{UDP_PORT}")

    while True:
        try:
            data, addr = sock.recvfrom(1024)
            line = data.decode().strip()
            logging.info(f"Received '{line}' from {addr}")

            parts = line.split()
            if not parts:
                continue

            cmd = parts[0]

            # ─── Simple commands ───────────────────────────────────────
            if cmd in SIMPLE_CMDS:
                launch = f"nohup {REMOTE_BIN} {cmd} > /dev/null 2>&1 &"
                subprocess.Popen(launch, shell=True)
                logging.info(f"Executed: {launch}")
                continue

            # ─── Crab walk: "crab <angle> <power>" ────────────────────
            if cmd == "crab" and len(parts) >= 2:
                angle = parts[1]
                launch = f"nohup {REMOTE_BIN} crab {angle} > /dev/null 2>&1 &"
                subprocess.Popen(launch, shell=True)
                logging.info(f"Executed: {launch}")
                continue

            # ─── Unknown / malformed ───────────────────────────────────
            logging.warning(f"Ignored unknown command: '{line}'")

        except Exception as e:
            logging.error(f"Error in receive loop: {e}")

if __name__ == "__main__":
    main()
