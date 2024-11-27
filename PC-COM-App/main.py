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

import time
import serial
import serial.tools.list_ports
from file_manager import FileManager


class SerialCommandApp:
    def __init__(self, master):
        self.master = master
        self.master.title("Serial Command Interface")
        self.master.geometry("1200x800")

        self.mode_handler = ModeHandler(self)
        self.serial_comm  = SerialCommunicator(self)     # module to handle serial communication
        self.date_time    = DateCmd(self)                # Date cmd to read current datetime from pico
        self.file_manager = FileManager(self) 

        # main window .. the root of all
        self.main_frame = tk.Frame(self.master)
        self.main_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

        # Top Frame für Port und Baudrate
        self.connection_frame =  tk.LabelFrame(self.main_frame, text="connection")
        self.connection_frame.pack(side=tk.TOP, expand = True, fill=tk.X, pady=10)
        self._setup_connection_area(self.connection_frame)

        # create on button side frame for message log
        self.message_frame = tk.LabelFrame(self.main_frame, text="frames log")
        self.message_frame.pack(side=tk.BOTTOM,  expand=True, fill=tk.X, pady=10)
        self._setup_message_area(self.message_frame)

        # create work area 
        self.work_frame = tk.LabelFrame(self.main_frame, text="app")
        self.work_frame.pack(side=tk.TOP, fill=tk.BOTH, expand=True, padx=10, pady=5)


        # create on right side a command button 
        self.button_frame = tk.LabelFrame(self.main_frame, text="commands")
        self.button_frame.pack(side=tk.TOP,  expand=True, fill=tk.X, padx=10, pady=5)
        self._setup_command_area(self.button_frame)

    def __del__(self):
        self.serial_comm.disconnect()



    def _setup_command_area(self,frame):
        tk.Button(frame, text="clear log",          command=self._clear_log                     ).pack(side=tk.LEFT, pady=5)
        tk.Button(frame, text="Read DateTime",      command=self.date_time.read_date_time       ).pack(side=tk.LEFT, pady=5)
        tk.Button(frame, text="Update DateTime",    command=self.date_time.update_date_time     ).pack(side=tk.LEFT, pady=5)
        tk.Button(frame, text="File Manager",       command=self._start_file_manager            ).pack(side=tk.LEFT, pady=5)
        
    def _start_file_manager(self):
        self.file_manager._open_file_manager(self.work_frame)

    def _setup_connection_area(self,frame):
        # slection of port
        tk.Label(frame, text="Port:").pack(side=tk.LEFT, padx=10)
        self.port_var = tk.StringVar()
        ports = [port.device for port in serial.tools.list_ports.comports()]
        self.port_menu = ttk.Combobox(frame, textvariable=self.port_var, values=ports, state="readonly")
        self.port_menu.pack(side=tk.LEFT)
        
        # selection of baudrate
        tk.Label(frame, text="Baudrate:").pack(side=tk.LEFT, padx=10)
        self.baud_var = tk.StringVar(value="115200")
        self.baud_menu = ttk.Combobox(frame, textvariable=self.baud_var, values=["9600", "115200", "19200", "38400", "57600"], state="readonly")
        self.baud_menu.pack(side=tk.LEFT)
        
        # Connect/Disconnect Button
        self.connect_button = tk.Button(frame, text="Connect", command=self.toggle_connection)
        self.connect_button.pack(side=tk.LEFT, padx=10)

        # status of connection
        self.connection_status_label = tk.Label(frame, text="Disconnected", width=20)
        self.connection_status_label.pack(side=tk.LEFT, padx=10)

    def _setup_message_area(self, frame):
        # Sent Messages Log
        sent_frame = tk.LabelFrame(frame, text="Sent frames")
        sent_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=5, pady=5)
        self.sent_text = tk.Text(sent_frame, wrap=tk.WORD, state=tk.DISABLED, bg="lightgrey", height=5)
        self.sent_text.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        sent_scroll = tk.Scrollbar(sent_frame, command=self.sent_text.yview)
        sent_scroll.pack(side=tk.RIGHT, fill=tk.Y)
        self.sent_text.configure(yscrollcommand=sent_scroll.set)

        # Received Messages Log
        received_frame = tk.LabelFrame(frame, text="Received frames")
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


    def _log_recv(self,data):
        timestamp = time.strftime("%H:%M:%S", time.localtime())
        self.received_text.config(state='normal')
        self.received_text.insert(tk.END, f"[RECV] {timestamp}: {data}\n")
        self.received_text.see(tk.END)
        self.received_text.config(state='disabled')


    def _log_sent(self,data):
        timestamp = time.strftime("%H:%M:%S", time.localtime())
        self.sent_text.config(state='normal')
        self.sent_text.insert(tk.END, f"[SENT] {timestamp}: {data}\n")
        self.sent_text.see(tk.END)
        self.sent_text.config(state='disabled')
        
    def _clear_log(self):
        self.sent_text.config(state='normal')
        self.sent_text.delete("1.0", tk.END)  # Clear the send message window
        self.sent_text.config(state='disabled')

        self.received_text.config(state='normal')
        self.received_text.delete("1.0", tk.END)  # Clear the receive message window
        self.received_text.config(state='disabled')
        

if __name__ == "__main__":
    root = tk.Tk()
    app = SerialCommandApp(root)
    root.mainloop()
