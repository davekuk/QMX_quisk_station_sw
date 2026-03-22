import serial
import time
import sys
import socket
import configparser
import os

BROADCAST_PORT = 6000
MESSAGE1 = b"L0"
MESSAGE2 = b"L1"

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
# REQUIRED: enable broadcast permission on this socket
sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
# Optional: allow rapid restart without "address in use" error
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

def connect_serial():
    # Read config file
    config = configparser.ConfigParser()
    config_path = 'port.cfg'
    
    if not os.path.exists(config_path):
        print(f"Error: {config_path} not found.")
        return None

    config.read(config_path)
    
    # Get parameters
    port = config.get('SERIAL', 'port')
    baudrate = config.getint('SERIAL', 'baudrate')
    timeout = config.getint('SERIAL', 'timeout')
    
    try:
        # Initialize serial connection {Link: pySerial 3.5 documentation https://pyserial.readthedocs.io/en/latest/shortintro.html}
        ser = serial.Serial(port, baudrate, timeout=timeout)

        print(f"Connected to {port} at {baudrate} baud.")
        return ser
    except serial.SerialException as e:
        print(f"Error opening serial port {port}: {e}")
        return None    

if __name__ == "__main__":
    ser = connect_serial()
    if ser:
      last_cts_state = ser.cts  # Initial state

try:
    while True:
        current_cts_state = ser.cts
        # Check if the state has changed
        if current_cts_state != last_cts_state:
            if current_cts_state:
                print(f"[{time.strftime('%H:%M:%S')}] CTS turned ON (High)")
                sock.sendto(MESSAGE1, ('255.255.255.255', BROADCAST_PORT))
            else:
                print(f"[{time.strftime('%H:%M:%S')}] CTS turned OFF (Low)")
                sock.sendto(MESSAGE2, ('255.255.255.255', BROADCAST_PORT))
            last_cts_state = current_cts_state
        time.sleep(0.01) # Short delay to prevent 100% CPU usage

except KeyboardInterrupt:
    print("Monitoring stopped.")
    ser.close()
sock.close()
