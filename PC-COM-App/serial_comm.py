import serial
import serial.tools.list_ports
import time
import base64

class SerialCommunicator:
    def __init__(self):
        self.serial_port = None

    def auto_connect(self, baudrate=115200):
        """Automatically detect available serial ports and connect to the first one with the specified baudrate."""
        ports = [port.device for port in serial.tools.list_ports.comports()]
        if not ports:
            raise Exception("No serial ports found.")

        # Try connecting to the first available port with the specified baudrate
        try:
            self.serial_port = serial.Serial(
                port=ports[0],
                baudrate=baudrate,
                timeout=1
            )
            return True, f"Connected to {ports[0]} at {baudrate} baud."
        except Exception as e:
            return False, str(e)

    def send_command(self, command_frame):
        """Send a command to the serial port and wait for a response."""
        if not self.serial_port or not self.serial_port.is_open:
            raise Exception("Serial port is not open.")
        
        self.serial_port.write(command_frame.encode('utf-8'))

        response_buffer = ""
        start_time = time.time()
        while time.time() - start_time < 10:
            if self.serial_port.in_waiting > 0:
                data = self.serial_port.read(self.serial_port.in_waiting).decode('utf-8')
                response_buffer += data
                if response_buffer.endswith("#"):
                    return response_buffer.strip()
        return None  # Timeout if no response

    def close(self):
        """Close the serial port."""
        if self.serial_port and self.serial_port.is_open:
            self.serial_port.close()
