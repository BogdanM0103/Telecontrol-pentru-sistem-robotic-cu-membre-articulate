#!/usr/bin/env python3
import os
import signal
import socket
import subprocess
import logging

# ── Configuration ─────────────────────────────────────────────────────────────
UDP_IP      = "0.0.0.0"   # Listen on all interfaces
UDP_PORT    = 6006          # Must match server.py UDP_PORT
REMOTE_BIN  = "/home/bogdan/Desktop/Hexapod/cmake-build-debug/cmake-build-debug/cmake-build-debug/HexapodRobot"
# Stop removed from SIMPLE_CMDS to intercept it separately
SIMPLE_CMDS = {"forward", "rotate_left", "rotate_right", "move_backward"}  # stop removed so it's handled separately
# ───────────────────────────────────────────────────────────────────────────────

# Set up logging
tlogging = logging.getLogger('udp_server')
logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s [udp_server] %(message)s",
    datefmt="%Y-%m-%d %H:%M:%S"
)

crab_proc = None  # Global handle for running crab process


def kill_crab_proc():
    """Terminate any running crab subprocess group."""
    global crab_proc
    if crab_proc and crab_proc.poll() is None:
        try:
            pgid = os.getpgid(crab_proc.pid)
            logging.info(f"Killing crab process group (pgid={pgid})")
            os.killpg(pgid, signal.SIGTERM)
        except Exception as e:
            logging.error(f"Error killing crab process group: {e}")
        crab_proc = None


def main():
    # Cleanup any leftover HexapodRobot processes from previous runs
    try:
        logging.info("Cleaning up leftover processes...")
        subprocess.call(["pkill", "-f", REMOTE_BIN])
    except Exception as cleanup_err:
        logging.error(f"Error during cleanup: {cleanup_err}")

    global crab_proc
    global crab_proc
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

            # 1) STOP: kill crab loop & invoke servo stop
            if cmd == "stop":
                kill_crab_proc()
                ##subprocess.Popen([REMOTE_BIN, "stop"])
                logging.info("Executed stop command")
                continue

            # 2) CRAB: start a new long-running crab in its own process group
            if cmd == "crab" and len(parts) >= 2:
                angle = parts[1]
                kill_crab_proc()
                crab_proc = subprocess.Popen(
                    [REMOTE_BIN, "crab", angle],
                    preexec_fn=os.setsid
                )
                pgid = os.getpgid(crab_proc.pid)
                logging.info(f"Started crab (pid={crab_proc.pid}, pgid={pgid}) at angle {angle}")
                continue

            # 3) Simple one-shot commands
            if cmd in SIMPLE_CMDS:
                subprocess.Popen([REMOTE_BIN, cmd])
                logging.info(f"Executed command: {cmd}")
                continue

            logging.warning(f"Ignored unknown command: '{line}'")

        except Exception as e:
            logging.error(f"Error in receive loop: {e}")

if __name__ == "__main__":
    main()