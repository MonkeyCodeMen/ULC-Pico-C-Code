# -*- coding: utf-8 -*-
"""
Created on Fri Nov 29 21:56:58 2024

@author: MonekyCodeMen
"""

import os
current_directory = os.getcwd()
print("Aktuelles Arbeitsverzeichnis:", current_directory)
if (not current_directory.endswith('PC-COM-App')):
    os.chdir('PC-COM-App')
    current_directory = os.getcwd()
    print("Aktuelles Arbeitsverzeichnis:", current_directory)

import os
import tkinter as tk
from tkinter import ttk
from serial_comm import SerialCommunicator
from mode_handlers import ModeHandler
from DateCmd import DateCmd
import time
import serial
import serial.tools.list_ports
from file_manager import FileManager


class SerialCommandApp:
    def __init__(self, master):
        self.master = master
        self.master.title("Serial Command Interface")
        self.master.geometry("1400x800")

        self.mode_handler = ModeHandler(self)
        self.serial_comm  = SerialCommunicator(self)     # module to handle serial communication
        self.date_time    = DateCmd(self)                # Date cmd to read current datetime from pico
        self.file_manager = FileManager(self) 

        # main window .. the root of all
        self.main_frame = tk.Frame(self.master)
        self.main_frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)

        # tkframe für Port und Baudrate
        self.connection_frame =  tk.LabelFrame(self.main_frame, text="Connection")
        self.connection_frame.pack(side=tk.TOP, fill=tk.X, pady=5)
        self._setup_connection_area(self.connection_frame)

        # area with work frame and buttons
        self.mid_frame = tk.Frame(self.main_frame)
        self.mid_frame.pack(side=tk.TOP, fill=tk.BOTH, expand=True, padx=5, pady=5)
        self._fill_mid_frame(self.mid_frame)

        # tkframe für frame log
        self.message_frame = tk.LabelFrame(self.main_frame, text="Frames Log")
        self.message_frame.pack(side=tk.BOTTOM, fill=tk.BOTH, expand=True, pady=5)  # Füllt den unteren Bereich aus
        self._setup_message_area(self.message_frame)

    def __del__(self):
        self.serial_comm.disconnect()

    def _fill_mid_frame(self, frame):
        # work_frame
        self.work_frame = tk.LabelFrame(frame, text="No function selected")
        self.work_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=5, pady=5)

        # tkframe for button
        self.button_frame = tk.LabelFrame(frame, text="Commands")
        self.button_frame.pack(side=tk.RIGHT, fill=tk.Y, padx=10, pady=5)
        tk.Button(self.button_frame, text="Read DateTime", command=self.date_time.read_date_time).pack(side=tk.TOP, pady=5)
        tk.Button(self.button_frame, text="Update DateTime", command=self.date_time.update_date_time).pack(side=tk.TOP, pady=5)
        tk.Button(self.button_frame, text="File Manager", command=self._start_file_manager).pack(side=tk.TOP, pady=5)

    def _start_file_manager(self):
        self.file_manager._open_file_manager(self.work_frame)

    def _setup_connection_area(self, frame):
        # Selection of port
        tk.Label(frame, text="Port:").pack(side=tk.LEFT, padx=5)
        self.port_var = tk.StringVar()
        ports = [port.device for port in serial.tools.list_ports.comports()]
        self.port_menu = ttk.Combobox(frame, textvariable=self.port_var, values=ports, state="readonly")
        self.port_menu.pack(side=tk.LEFT)

        # Selection of baudrate
        tk.Label(frame, text="Baudrate:").pack(side=tk.LEFT, padx=5)
        self.baud_var = tk.StringVar(value="115200")
        self.baud_menu = ttk.Combobox(frame, textvariable=self.baud_var, values=["9600", "115200", "19200", "38400", "57600"], state="readonly")
        self.baud_menu.pack(side=tk.LEFT)

        # Connect/Disconnect Button
        self.connect_button = tk.Button(frame, text="Connect", command=self.toggle_connection)
        self.connect_button.pack(side=tk.LEFT, padx=5, pady=5)

        # Status of connection
        self.connection_status_label = tk.Label(frame, text="Disconnected", width=20)
        self.connection_status_label.pack(side=tk.LEFT, padx=5)

    def _setup_message_area(self, frame):
        # Messages Log
        #self.frame_log = tk.Text(frame, wrap=tk.WORD, state=tk.DISABLED, bg="lightgrey", height=15)  # Mehr Zeilen
        self.frame_log = tk.Text(frame, state=tk.DISABLED, bg="lightgrey", height=15)  # Mehr Zeilen
        self.frame_log.pack(side=tk.TOP, fill=tk.BOTH, expand=True, padx=5, pady=5)  # Füllt den Bereich besser aus
        scroll = tk.Scrollbar(self.frame_log, command=self.frame_log.yview)
        scroll.pack(side=tk.RIGHT, fill=tk.Y)
        self.frame_log.configure(yscrollcommand=scroll.set)
        tk.Button(frame, text="Clear Log", command=self._clear_log).pack(side=tk.LEFT, pady=2, padx=10)
        tk.Button(frame, text="save Log", command=self._save_log_to_file).pack(side=tk.LEFT, pady=2, padx = 10)
        self.log_buffer=""

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

    def _log_recv(self, data):
        self._check_log_buffer()
        timestamp = time.strftime("%H:%M:%S", time.localtime())
        self.frame_log.config(state='normal')
        self.frame_log.insert(tk.END, f"[RECV] {timestamp}: {data}\n")
        self.frame_log.see(tk.END)
        self.frame_log.config(state='disabled')

    def _log_sent(self, data):
        self._check_log_buffer()
        timestamp = time.strftime("%H:%M:%S", time.localtime())
        self.frame_log.config(state='normal')
        self.frame_log.insert(tk.END, f"[SENT] {timestamp}: {data}\n")
        self.frame_log.see(tk.END)
        self.frame_log.config(state='disabled')
    
    def _check_log_buffer(self):
        if self.log_buffer != "":
            self.frame_log.config(state='normal')
            self.frame_log.insert(tk.END, self.log_buffer)
            self.frame_log.see(tk.END)
            self.frame_log.config(state='disabled')
            self.log_buffer = ""
        

    def _log_recv_buffered(self, data):
        timestamp = time.strftime("%H:%M:%S", time.localtime())
        self.log_buffer+=f"[RECV] {timestamp}: {data}\n"

    def _log_sent_buffered(self, data):
        timestamp = time.strftime("%H:%M:%S", time.localtime())
        self.log_buffer+=f"[SENT] {timestamp}: {data}\n"


    def _clear_log(self):
        self.log_buffer = ""
        self.frame_log.config(state='normal')
        self.frame_log.delete("1.0", tk.END)  # Clear the send message window
        self.frame_log.config(state='disabled')
        
    def _save_log_to_file(self):
        """write current log to file """
        log_content = self.frame_log.get("1.0", tk.END)  # Holt den gesamten Inhalt des Text-Widgets
        
        # Öffnen der Datei im Anhängemodus
        with open("frame_log.txt", "a") as log_file:
            log_file.write(log_content)  # Schreibe den Inhalt in die Datei


if __name__ == "__main__":
    root = tk.Tk()
    app = SerialCommandApp(root)
    root.mainloop()
