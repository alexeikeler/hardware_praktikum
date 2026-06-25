"""
Exercise 04: Real-Time IMU Visualization

Complete all TODO sections.
"""

import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from collections import deque
import numpy as np
plt.style.use('ggplot')
# =====================
# TODO (1): Set correct serial port
# =====================
# HINT: Find your board's COM port (Windows) or /dev/ttyUSB* (Linux/Mac)
# Common Windows ports: COM3, COM4, COM5, COM6
PORT = 'COM5'   # TODO: Change to your board's port
BAUD = 115200

WINDOW_SIZE = 100

# =====================
# TODO (2): Initialize serial connection
# =====================
# HINT: Use serial.Serial(PORT, BAUD) to create connection
#ser = serial.Serial(PORT, BAUD)


# =====================
# TODO (3): Create buffers for ax, ay, az, gyrX, gyrY, gyrZ
# =====================
# HINT: Use deque(maxlen=WINDOW_SIZE) for each sensor axis
ax_data = deque(maxlen=WINDOW_SIZE)
ay_data = deque(maxlen=WINDOW_SIZE)
az_data = deque(maxlen=WINDOW_SIZE)
gyrX_data = deque(maxlen=WINDOW_SIZE)
gyrY_data = deque(maxlen=WINDOW_SIZE)
gyrZ_data = deque(maxlen=WINDOW_SIZE)


# =====================
# TODO (4): Initialize text variables
# =====================
# HINT: Store orientation and both gesture types
orientation_text = ""
fsm_gesture_text = ""
gyro_gesture_text = ""


# =====================
# TODO (5): Setup plot with dual subplots
# =====================
# HINT: Create 2 vertical subplots, 3 lines per subplot, text displays, and configure axes
# HINT: Accelerometer subplot: y-range [-15, 15], title "Accelerometer Data (m/s²)"
# HINT: Gyroscope subplot: y-range [-500, 500], title "Gyroscope Data (°/s)"
fig, (ax1, ax2) = plt.subplots(2)
ax1.set_title("Accelerometer Data (m/s²)")
ax1.set_ylim([-15, 15])
ax1.set_xlim([0, 100])
ax1.set_ylabel("Acceleration")
ax1.text(1, 10, f'Orientation:{orientation_text}\nFSM Gesture: {fsm_gesture_text}', fontsize=12)
ax1.legend(loc="upper right")

ax2.set_title("Gyroscope Data (°/s)")
ax2.set_ylim([-500, 500])
ax2.set_xlim([0, 100])
ax2.set_xlabel("Sample")
ax2.set_ylabel("Angular velocity")
ax2.text(1, 400, f'Gyro Gesture: {gyro_gesture_text}', fontsize=12)
ax2.legend(loc="upper right")



line_ax = None
line_ay = None
line_az = None

line_gyrX = None
line_gyrY = None
line_gyrZ = None

text_display1 = None
text_display2 = None


# =====================
# TODO (6): Parse serial line
# =====================
# HINT: New serial format from Arduino:
# "ax: X | ay: Y | az: Z | gyrX: X | gyrY: Y | gyrZ: Z | Orientation: ... | FSM: ... | Gyro: ..."
#
def parse_line(line: str) -> tuple | None:
    global orientation_text, fsm_gesture_text, gyro_gesture_text

    try:
        values = []
        # HINT: Serial format: "ax: X | ay: Y | az: Z | gyrX: X | gyrY: Y | gyrZ: Z | Orientation: ... | FSM: ... | Gyro: ..."
        # TODO: Split by '|' and extract 6 sensor values (ax, ay, az, gyrX, gyrY, gyrZ)
        # TODO: Extract 3 text strings (orientation_text, fsm_gesture_text, gyro_gesture_text)
        # TODO: Return tuple (ax_val, ay_val, az_val, gyrX_val, gyrY_val, gyrZ_val)
        line = line.replace(" ", "")
        for substring in line.split("|"):
            name, value = substring.split(":")
            if (value.isnumeric()) or ("-" in value) or ("." in value):
                values.append(float(value))
            else:
                if name == "Orientation":
                    orientation_text = value
                elif name == "FSM":
                    fsm_gesture_text = value
                elif name == "Gyro":
                    gyro_gesture_text = value
                else:
                    raise ValueError("UNKNOWN NAME in parse_line(...)")

        return tuple(values)

    except:
        return None


# =====================
# TODO (7): Update function
# =====================
def update(frame):
    global orientation_text, fsm_gesture_text, gyro_gesture_text

    # TODO: Read all available serial data, parse, and append to buffers
    # TODO: Update all 6 line objects (3 accel + 3 gyro)
    # TODO: Update both text displays
    # TODO: Return all plot objects to redraw
    
    return []


# =====================
# TODO (8): Create animation
# =====================
# TODO: Create animation with FuncAnimation(fig, update, interval=50)
# TODO: Call plt.tight_layout() for proper spacing


def main():
    global orientation_text, fsm_gesture_text, gyro_gesture_text

    string = "ax: 15 | ay: 0 | az: -1 | gyrX: 2 | gyrY: 2.3 | gyrZ: -1.54 | Orientation: ori | FSM: fsm | Gyro: gyro"
    result = parse_line(string)
    print(result)
    print()
    print(orientation_text, fsm_gesture_text, gyro_gesture_text)
    x = np.linspace(0, 2*np.pi, 400)
    y = np.sin(x**2)

    ax1.plot(x, y)
    ax2.plot(x, y)

    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    main()