import serial
import serial.tools.list_ports
import threading
import time


class SerialCommunicator:
    def __init__(self, app):
        self.app = app
        self.serial_port = None

    def connect(self, port, baudrate):
        """Connect to the selected serial port with the given baudrate."""
        try:
            self.serial_port = serial.Serial(port=port, baudrate=baudrate, timeout=1)
            # Start a new thread for receiving data
            threading.Thread(target=self.receive_data, daemon=True).start()
        except Exception as e:
            print(f"Error connecting: {e}")

    def disconnect(self):
        """Disconnect the serial port."""
        if self.serial_port and self.serial_port.is_open:
            self.serial_port.close()

    def is_connected(self):
        """Check if the serial port is connected."""
        return self.serial_port and self.serial_port.is_open

    def receive_data(self):
        """Receive data from the Pico in a separate thread."""
        while self.is_connected():
            if self.serial_port.in_waiting > 0:
                data = self.serial_port.read(self.serial_port.in_waiting).decode('utf-8')
                timestamp = time.strftime("%H:%M:%S", time.localtime())
                self.app.received_text.config(state='normal')
                self.app.received_text.insert("end", f"[RECV] {timestamp}: {data}\n")
                self.app.received_text.see("end")
                self.app.received_text.config(state='disabled')

    def send(self, frame):
        """Send data to the Pico."""
        if not self.serial_port or not self.serial_port.is_open:
            self._log_sent("Error: Serial port not open.")
            return

        # Validate frame structure
        if not frame.startswith("S:") or not frame.endswith("#"):
            self._log_sent("Error: Invalid frame structure.")
            return

        try:
            # Send the command
            self.serial_port.write(frame.encode('utf-8'))
            self._log_sent(frame)

        except Exception as e:
            self._log_sent("Error: serial_port.write failed.")




    def _log_sent(self,msg):
        timestamp = time.strftime("%H:%M:%S", time.localtime())
        self.app.sent_text.config(state='normal')
        self.app.sent_text.insert("end", f"[SENT] {timestamp}: {msg}\n")
        self.app.sent_text.see("end")
        self.app.sent_text.config(state='disabled')
        
