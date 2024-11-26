import os
current_directory = os.getcwd()
print("Aktuelles Arbeitsverzeichnis:", current_directory)
if (not current_directory.endswith('PC-COM-App')):
    os.chdir('PC-COM-App')
    current_directory = os.getcwd()
    print("Aktuelles Arbeitsverzeichnis:", current_directory)

import tkinter as tk
from tkinter import ttk
from serial_comm import SerialCommunicator
from mode_handlers import ModeHandler
from DateCmd import DateCmd

import serial
import serial.tools.list_ports
import time 



class SerialCommandApp:
    def __init__(self, master):
        self.master = master
        self.master.title("Serial Command Interface")
        self.master.geometry("1200x800")

        self.mode_handler = ModeHandler(self)
        self.serial_comm  = SerialCommunicator(self)     # module to handle serial communication
        self.date_time     = DateCmd(self)                  # Date cmd to read current datetime from pico

        # UI layout erstellen
        self._create_ui()

    def _create_ui(self):
        # Frame für das Hauptfenster
        main_frame = tk.Frame(self.master)
        main_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

        # Top Frame für Port und Baudrate
        top_frame = tk.Frame(main_frame)
        top_frame.pack(side=tk.TOP, fill=tk.X, pady=10)

        # Auswahlbox für den Port
        tk.Label(top_frame, text="Port:").pack(side=tk.LEFT, padx=10)
        self.port_var = tk.StringVar()
        ports = [port.device for port in serial.tools.list_ports.comports()]
        self.port_menu = ttk.Combobox(top_frame, textvariable=self.port_var, values=ports, state="readonly")
        self.port_menu.pack(side=tk.LEFT)
        
        # Auswahlbox für Baudrate
        tk.Label(top_frame, text="Baudrate:").pack(side=tk.LEFT, padx=10)
        self.baud_var = tk.StringVar(value="115200")
        self.baud_menu = ttk.Combobox(top_frame, textvariable=self.baud_var, values=["9600", "115200", "19200", "38400", "57600"], state="readonly")
        self.baud_menu.pack(side=tk.LEFT)
        
        # Connect/Disconnect Button
        self.connect_button = tk.Button(top_frame, text="Connect", command=self.toggle_connection)
        self.connect_button.pack(side=tk.LEFT, padx=10)

        # Anzeige des Verbindungsstatus
        self.connection_status_label = tk.Label(top_frame, text="Disconnected", width=20)
        self.connection_status_label.pack(side=tk.LEFT, padx=10)

        # Bereich für das Modusmanagement
        self.dynamic_area_frame = tk.Frame(main_frame)
        self.dynamic_area_frame.pack(side=tk.TOP, fill=tk.BOTH, expand=True, padx=10, pady=5)

        # Rechter Bereich mit Button-Leiste für Moduswahl
        mode_button_frame = tk.Frame(main_frame)
        mode_button_frame.pack(side=tk.RIGHT, fill=tk.Y, padx=10, pady=5)

        tk.Button(mode_button_frame, text="Read DateTime", command=self.read_date_time).pack(side=tk.TOP, pady=5)
        tk.Button(mode_button_frame, text="Update DateTime", command=self.update_date_time).pack(side=tk.TOP, pady=5)

        for i in range(10):
            tk.Button(mode_button_frame, text=f"Mode {i+1}", command=lambda mode=i: self.mode_handler.switch_mode(mode)).pack(side=tk.TOP, pady=5)

        # Bereich für das Loggen von gesendeten und empfangenen Nachrichten
        self._create_message_area(main_frame)

    def _create_message_area(self, parent):
        message_frame = tk.Frame(parent)
        message_frame.pack(side=tk.BOTTOM, fill=tk.X, pady=10)

        # Sent Messages Log
        sent_frame = tk.LabelFrame(message_frame, text="Sent Messages", width=50, height=10)
        sent_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=5, pady=5)
        self.sent_text = tk.Text(sent_frame, wrap=tk.WORD, state=tk.DISABLED, bg="lightgrey", height=5)
        self.sent_text.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        sent_scroll = tk.Scrollbar(sent_frame, command=self.sent_text.yview)
        sent_scroll.pack(side=tk.RIGHT, fill=tk.Y)
        self.sent_text.configure(yscrollcommand=sent_scroll.set)

        # Received Messages Log
        received_frame = tk.LabelFrame(message_frame, text="Received Messages", width=50, height=10)
        received_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=5, pady=5)
        self.received_text = tk.Text(received_frame, wrap=tk.WORD, state=tk.DISABLED, bg="lightgrey", height=5)
        self.received_text.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        received_scroll = tk.Scrollbar(received_frame, command=self.received_text.yview)
        received_scroll.pack(side=tk.RIGHT, fill=tk.Y)
        self.received_text.configure(yscrollcommand=received_scroll.set)

    def toggle_connection(self):
        """Connect or disconnect based on the current state."""
        if self.serial_comm.is_connected():
            self.serial_comm.disconnect()
            self.connection_status_label.config(text="Disconnected")
            self.connect_button.config(text="Connect")
        else:
            self.serial_comm.connect(self.port_var.get(), int(self.baud_var.get()))
            self.connection_status_label.config(text="Connected")
            self.connect_button.config(text="Disconnect")

    def update_ui_for_mode(self, mode_data):
        """Updates the UI when the mode is changed."""
        self.mode_handler.update_mode_ui(mode_data)

    def read_date_time(self):
        """Trigger the Date Mode to read and display the current date and time."""
        self.date_time.read_date_time()

    def update_date_time(self):
        """update date time"""
        self.date_time.update_date_time()

if __name__ == "__main__":
    root = tk.Tk()
    app = SerialCommandApp(root)
    root.mainloop()
