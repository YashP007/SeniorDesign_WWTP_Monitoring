import serial
import time

# ——— CONFIGURATION ———
PORT = '/dev/cu.usbmodem1101'   # ← your Arduino’s port
BAUDRATE = 115200
DURATION = 60.0*10             # seconds to record
OUTPUT_FILE = 'adc_log_8bitADC.txt'
# ————————————————————


def main():
    # open the port one time, up front
    ser = serial.Serial(PORT, BAUDRATE, timeout=1)
    time.sleep(2)           # allow Arduino reset

    start = time.time()
    end_time = start + DURATION

    with open(OUTPUT_FILE, 'w') as f:
        f.write('time_s, raw16_hex\n')
        # loop until 60 s have elapsed, but never re-open or re-close in here
        while time.time() < end_time:
            line = ser.readline().decode('ascii', errors='ignore').strip()
            if not line:
                continue

            # normalize to 0x... format
            raw = line.upper()
            if not raw.startswith('0X'):
                raw = '0x' + raw

            elapsed = time.time() - start
            # timestamp in seconds with microsecond resolution
            ts = f"{elapsed:.6f}"

            f.write(f"{ts}, {raw}\n")

    # now that the capture is done, close once
    ser.close()
    print(f"Finished. Data saved to {OUTPUT_FILE}")


if __name__ == '__main__':
    main()
