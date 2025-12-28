import subprocess
import sys

PORT = ""          # CHANGE THIS
BAUD = "460800"
BIN_FILE = r".pio\build\esp32dev\firmware.bin"

cmd = [
    "esptool.py",
    "--chip", "esp32",
    "--port", PORT,
    "--baud", BAUD,
    "write_flash", "-z",
    "0x10000", BIN_FILE
]



print("⚡ Flashing ESP32...")
result = subprocess.run(cmd)

if result.returncode == 0:
    print("✅ Flash OK!")
else:
    print("❌ Flash failed!")
