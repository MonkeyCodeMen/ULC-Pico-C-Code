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
            # threading.Thread(target=self.receive_data, daemon=True).start()
        except Exception as e:
            print(f"Error connecting: {e}")

    def disconnect(self):
        """Disconnect the serial port."""
        if self.serial_port and self.serial_port.is_open:
            self.serial_port.close()

    def is_connected(self):
        """Check if the serial port is connected."""
        return self.serial_port and self.serial_port.is_open

    """
    def receive_data(self):
        
        while self.is_connected():
            if self.serial_port.in_waiting > 0:
                data = self.serial_port.read(self.serial_port.in_waiting).decode('utf-8')
                self.app._log_recv(data)

    def send(self, frame):
        if not self.serial_port or not self.serial_port.is_open:
            self.app._log_sent("Error: Serial port not open.")
            return

        # Validate frame structure
        if not frame.startswith("S:") or not frame.endswith("#"):
            self.app._log_sent("Error: Invalid frame structure.")
            return

        try:
            # Send the command
            self.serial_port.write(frame.encode('utf-8'))
            self.app._log_sent(frame)

        except Exception as e:
            self.app._log_sent("Error: serial_port.write failed.")
            """



    def send(self, frame):
        """
        Send a command and wait for a valid response.
        Logs the sent and received messages.
        Args:
            frame: The command frame string to send.
        """
        if not self.serial_port or not self.serial_port.is_open:
            self.app._log_sent("Error: Serial port not open.")
            return

        # Validate frame structure
        if not frame.startswith("S:") or not frame.endswith("#"):
            self.app._log_sent("Error: Invalid frame structure.")
            return

        try:
            # Send the command
            self.serial_port.write(frame.encode('utf-8'))
            self.app._log_sent(frame)

            # Wait for a valid response
            start_time = time.time()
            response_buffer = ""
            while True:
                # Check for timeout
                if time.time() - start_time > 10:
                    self.app._log_recv("Timeout: No response within 10 seconds.")
                    break

                # Check if data is available
                if self.serial_port.in_waiting > 0:
                    data = self.serial_port.read(self.serial_port.in_waiting).decode('utf-8')
                    response_buffer += data

                    # Look for a complete frame (ends with '#')
                    if response_buffer.endswith("#"):
                        self.app._log_recv(response_buffer.strip())
                        return response_buffer
            
        except Exception as e:
            self.app._log_recv( f"Error: {str(e)}")