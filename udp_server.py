#!/usr/bin/env python3
import socket
import subprocess
import logging

# Configuration
UDP_IP = "0.0.0.0"      # Listen on all interfaces
UDP_PORT = 6006           # Must match client port
# Full path to your built Hexapod binary on the Pi
REMOTE_BIN = "/home/bogdan/Desktop/Hexapod/cmake-build-debug/cmake-build-debug/cmake-build-debug/HexapodRobot"
ALLOWED_CMDS = {"forward", "rotate_left", "rotate_right", "move_backward"}

# Set up logging
title = "udp_server"
logging.basicConfig(level=logging.INFO, format=f"%(asctime)s [{title}] %(message)s")

# Create and bind the UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))
logging.info(f"Listening for UDP on {UDP_IP}:{UDP_PORT}")

while True:
    try:
        data, addr = sock.recvfrom(1024)
        cmd = data.decode().strip()
        logging.info(f"Received '{cmd}' from {addr}")

        if cmd not in ALLOWED_CMDS:
            logging.warning(f"Invalid command: {cmd}")
            continue

        # Build and launch the process in background
        launch_cmd = f"nohup {REMOTE_BIN} {cmd} > /dev/null 2>&1 &"
        subprocess.Popen(launch_cmd, shell=True)
        logging.info(f"Executed: {launch_cmd}")

    except Exception as e:
        logging.error(f"Error: {e}")