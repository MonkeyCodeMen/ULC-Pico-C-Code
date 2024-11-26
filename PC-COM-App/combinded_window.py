import tkinter as tk
from tkinter import ttk, messagebox
import serial
import serial.tools.list_ports
import os
import threading
import time

class SerialCommandApp:
    def __init__(self, master):
        self.master = master
        self.master.title("Serial Command Interface")
        self.master.geometry("800x600")

        self.serial_port = None
        self.local_directory = os.getcwd()
        self.target_directory_var = tk.StringVar(value="/")

        # Create the UI layout
        self._create_ui()

    def _create_ui(self):
        # Main frame for the app
        main_frame = tk.Frame(self.master)
        main_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

        # Split the window into 3 parts:
        # Part 1: Sent and Received Messages
        self._create_message_area(main_frame)
        
        # Part 2: File Manager or LED Config (Dynamic area)
        self.dynamic_area_frame = tk.Frame(main_frame)
        self.dynamic_area_frame.pack(side=tk.TOP, fill=tk.BOTH, expand=True, padx=10, pady=5)

        # Part 3: Commands and Port Settings
        self._create_command_and_port_area(main_frame)

    def _create_message_area(self, parent):
        message_frame = tk.Frame(parent)
        message_frame.pack(side=tk.TOP, fill=tk.X, pady=10)

        # Sent Messages Log
        sent_frame = tk.LabelFrame(message_frame, text="Sent Messages", width=50, height=10)
        sent_frame.pack(side=tk.TOP, fill=tk.X, padx=5, pady=5)
        self.sent_text = tk.Text(sent_frame, wrap=tk.WORD, state=tk.DISABLED, bg="lightgrey", height=5)
        self.sent_text.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        sent_scroll = tk.Scrollbar(sent_frame, command=self.sent_text.yview)
        sent_scroll.pack(side=tk.RIGHT, fill=tk.Y)
        self.sent_text.configure(yscrollcommand=sent_scroll.set)

        # Received Messages Log
        received_frame = tk.LabelFrame(message_frame, text="Received Messages", width=50, height=10)
        received_frame.pack(side=tk.TOP, fill=tk.X, padx=5, pady=5)
        self.received_text = tk.Text(received_frame, wrap=tk.WORD, state=tk.DISABLED, bg="lightgrey", height=5)
        self.received_text.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        received_scroll = tk.Scrollbar(received_frame, command=self.received_text.yview)
        received_scroll.pack(side=tk.RIGHT, fill=tk.Y)
        self.received_text.configure(yscrollcommand=received_scroll.set)

    def _create_command_and_port_area(self, parent):
        # Frame for commands and port settings
        command_frame = tk.Frame(parent)
        command_frame.pack(side=tk.TOP, fill=tk.X, pady=5)

        # Group 1: Command Buttons
        command_button_frame = tk.Frame(command_frame)
        command_button_frame.pack(side=tk.LEFT, fill=tk.Y, padx=5, pady=5)

        tk.Button(command_button_frame, text="Clock Read", command=self.read_clock_command).pack(padx=5, pady=5)
        tk.Button(command_button_frame, text="Clock Update", command=self.sync_clock_command).pack(padx=5, pady=5)

        # Group 2: Port Settings
        port_settings_frame = tk.Frame(command_frame)
        port_settings_frame.pack(side=tk.LEFT, fill=tk.Y, padx=5, pady=5)

        # Port auto-detection button
        tk.Button(port_settings_frame, text="Auto Connect", command=self.auto_connect).pack(padx=5, pady=5)

        self.connection_status_label = tk.Label(port_settings_frame, text="Disconnected", width=20)
        self.connection_status_label.pack(padx=5, pady=5)

        self.connect_button = tk.Button(port_settings_frame, text="Connect", command=self.toggle_connection)
        self.connect_button.pack(padx=5, pady=5)

        # File Manager or LED Config area
        self._create_file_manager_or_led_config()

    def _create_file_manager_or_led_config(self):
        """Creates either the file manager or LED configuration based on the context."""
        # Clear the previous dynamic area
        for widget in self.dynamic_area_frame.winfo_children():
            widget.destroy()

        # Create the file manager section
        file_manager_frame = tk.Frame(self.dynamic_area_frame)
        file_manager_frame.pack(side=tk.TOP, fill=tk.BOTH, expand=True, padx=10, pady=10)

        tk.Label(file_manager_frame, text="Pico SD Card Files").pack(side=tk.TOP, padx=5, pady=5)
        self.sd_files_listbox = tk.Listbox(file_manager_frame, selectmode=tk.SINGLE)
        self.sd_files_listbox.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

        # Local files
        tk.Label(file_manager_frame, text="Local Files").pack(side=tk.TOP, padx=5, pady=5)
        self.local_files_listbox = tk.Listbox(file_manager_frame, selectmode=tk.SINGLE)
        self.local_files_listbox.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

        # Buttons for file operations
        button_frame = tk.Frame(file_manager_frame)
        button_frame.pack(side=tk.BOTTOM, fill=tk.X, padx=5, pady=5)

        tk.Button(button_frame, text="Update SD File List", command=self.update_sd_card_files).pack(side=tk.LEFT, padx=5, pady=5)
        tk.Button(button_frame, text="Change Target Dir", command=self.change_target_directory).pack(side=tk.LEFT, padx=5, pady=5)
        tk.Button(button_frame, text="Copy to Pico", command=self.copy_to_pico).pack(side=tk.LEFT, padx=5, pady=5)
        tk.Button(button_frame, text="Copy from Pico", command=self.copy_from_pico).pack(side=tk.LEFT, padx=5, pady=5)

        # Target directory selection for Pico SD card
        tk.Label(file_manager_frame, text="Select Target Dir on Pico").pack(side=tk.TOP, padx=5, pady=5)
        target_dir_menu = ttk.Combobox(file_manager_frame, textvariable=self.target_directory_var)
        target_dir_menu.pack(side=tk.TOP, padx=5, pady=5)

    def auto_connect(self):
        """Automatically connect to the first available serial port with 115200 baud."""
        ports = [port.device for port in serial.tools.list_ports.comports()]
        if ports:
            try:
                self.serial_port = serial.Serial(port=ports[0], baudrate=115200, timeout=1)
                self.connection_status_label.config(text=f"Connected to {ports[0]}")
                self.connect_button.config(text="Disconnect")
            except Exception as e:
                messagebox.showerror("Error", f"Failed to open serial port: {e}")
        else:
            messagebox.showerror("Error", "No serial ports found.")

    def toggle_connection(self):
        """Toggles between Connect and Disconnect for the serial port."""
        if self.serial_port and self.serial_port.is_open:
            self.serial_port.close()
            self.connection_status_label.config(text="Disconnected")
            self.connect_button.config(text="Connect")
        else:
            self.auto_connect()

    def read_clock_command(self):
        """Send a read clock command."""
        command = "S:C0,CLOCK#"
        self.send_command(command)

    def sync_clock_command(self):
        """Send a sync clock command."""
        current_time = time.strftime("%Y-%m-%dT%H:%M:%S", time.localtime())
        command = f"S:C0,CLOCKSET,{current_time}#"
        self.send_command(command)

    def send_command(self, command):
        """Send command and log response."""
        if not self.serial_port or not self.serial_port.is_open:
            messagebox.showerror("Error", "Serial port not open.")
            return

        try:
            self.serial_port.write(command.encode('utf-8'))
            self._log_message(self.sent_text, command, is_sent=True)

            response = self.serial_port.readline().decode('utf-8').strip()
            self._log_message(self.received_text, response, is_sent=False)
        except Exception as e:
            messagebox.showerror("Error", f"Failed to send command: {e}")

    def _log_message(self, text_widget, message, is_sent=True):
        """Logs a message with timestamp."""
        timestamp = time.strftime("%H:%M:%S", time.localtime())
        prefix = "[SENT] " if is_sent else "[RECV] "
        text_widget.configure(state=tk.NORMAL)
        text_widget.insert(tk.END, f"{prefix}{timestamp}: {message}\n")
        text_widget.see(tk.END)
        text_widget.configure(state=tk.DISABLED)

    def update_sd_card_files(self):
        """Update the list of files on the Pico's SD card."""
        # This should implement the logic to fetch and display SD card files
        pass

    def change_target_directory(self):
        """Change the target directory on the Pico."""
        pass

    def copy_to_pico(self):
        """Copy a file to the Pico."""
        pass

    def copy_from_pico(self):
        """Copy a file from the Pico."""
        pass


if __name__ == "__main__":
    root = tk.Tk()
    app = SerialCommandApp(root)
    root.mainloop()
