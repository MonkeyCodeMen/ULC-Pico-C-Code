import tkinter as tk
from tkinter import ttk, messagebox, filedialog
import serial
import serial.tools.list_ports
import os
import base64
import threading
import time



CHUNK_SIZE = 128  # Maximum chunk size for file operations



class SerialCommandApp:
    def __init__(self, master):
        self.master = master
        self.master.title("Serial Command Interface")
        self.master.geometry("1200x600")

        # Serial Port Configuration
        self.serial_port = None

        # UI Layout
        self._create_ui()

    def _create_ui(self):
        # Top Frame: Buttons for commands
        command_frame = tk.Frame(self.master)
        command_frame.pack(side=tk.TOP, fill=tk.X, padx=10, pady=5)

        # Buttons for each command
                # Button to open serial configuration
        tk.Button(command_frame, text="Serial Port",    command=self.open_serial_config, width=12).pack(side=tk.LEFT, padx=5)
        tk.Button(command_frame, text="Clear Logs",     command=self.clear_logs).pack(side=tk.LEFT, padx=5)
        tk.Button(command_frame, text="Dump",           command=self.dump_command, width=10).pack(side=tk.LEFT, padx=5)
        tk.Button(command_frame, text="Read Clock",     command=self.read_clock_command, width=12).pack(side=tk.LEFT, padx=5)
        tk.Button(command_frame, text="Sync Clock",     command=self.sync_clock_command, width=12).pack(side=tk.LEFT, padx=5)
        tk.Button(command_frame, text="File Manager",   command=self.open_file_manager, width=12).pack(side=tk.LEFT, padx=5)
        tk.Button(command_frame, text="LED Config",     command=self.open_led_config_window, width=12).pack(side=tk.LEFT, padx=5)

        # Middle Frame: Logs for sent and received messages
        log_frame = tk.Frame(self.master)
        log_frame.pack(side=tk.TOP, fill=tk.BOTH, expand=True, padx=10, pady=5)

        # Sent Messages Log
        sent_frame = tk.LabelFrame(log_frame, text="Sent Messages", width=50, height=10)
        sent_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=5, pady=5)
        self.sent_text = tk.Text(sent_frame, wrap=tk.WORD, state=tk.DISABLED, bg="lightgrey", height=15)
        self.sent_text.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        sent_scroll = tk.Scrollbar(sent_frame, command=self.sent_text.yview)
        sent_scroll.pack(side=tk.RIGHT, fill=tk.Y)
        self.sent_text.configure(yscrollcommand=sent_scroll.set)

        # Received Messages Log
        received_frame = tk.LabelFrame(log_frame, text="Received Messages", width=50, height=10)
        received_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=5, pady=5)
        self.received_text = tk.Text(received_frame, wrap=tk.WORD, state=tk.DISABLED, bg="lightgrey", height=15)
        self.received_text.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        received_scroll = tk.Scrollbar(received_frame, command=self.received_text.yview)
        received_scroll.pack(side=tk.RIGHT, fill=tk.Y)
        self.received_text.configure(yscrollcommand=received_scroll.set)

    def _log_message(self, text_widget, message, is_sent=True):
        """
        Log a message with a timestamp to the specified text widget.
        Args:
            text_widget: The text widget to update.
            message: The message to log.
            is_sent: True if the message is sent, False if received.
        """
        timestamp = time.strftime("%H:%M:%S", time.localtime())
        prefix = "[SENT] " if is_sent else "[RECV] "
        text_widget.configure(state=tk.NORMAL)
        text_widget.insert(tk.END, f"{prefix}{timestamp}: {message}\n")
        text_widget.see(tk.END)
        text_widget.configure(state=tk.DISABLED)

    def open_serial_config(self):
        """Open the serial port configuration dialog."""
        config_window = tk.Toplevel(self.master)
        config_window.title("Serial Port Configuration")
        config_window.geometry("400x300")

        # Port selection
        tk.Label(config_window, text="Select Port:").grid(row=0, column=0, padx=10, pady=5)
        port_var = tk.StringVar()
        ports = [port.device for port in serial.tools.list_ports.comports()]
        port_menu = ttk.Combobox(config_window, textvariable=port_var, values=ports, state="readonly")
        port_menu.grid(row=0, column=1, padx=10, pady=5)

        # Baud rate selection
        tk.Label(config_window, text="Baud Rate:").grid(row=1, column=0, padx=10, pady=5)
        baud_var = tk.StringVar(value="9600")
        baud_menu = ttk.Combobox(config_window, textvariable=baud_var, values=["9600", "19200", "38400", "57600", "115200"], state="readonly")
        baud_menu.grid(row=1, column=1, padx=10, pady=5)

        # Data bits selection
        tk.Label(config_window, text="Data Bits:").grid(row=2, column=0, padx=10, pady=5)
        data_bits_var = tk.StringVar(value="8")
        data_bits_menu = ttk.Combobox(config_window, textvariable=data_bits_var, values=["5", "6", "7", "8"], state="readonly")
        data_bits_menu.grid(row=2, column=1, padx=10, pady=5)

        # Stop bits selection
        tk.Label(config_window, text="Stop Bits:").grid(row=3, column=0, padx=10, pady=5)
        stop_bits_var = tk.StringVar(value="1")
        stop_bits_menu = ttk.Combobox(config_window, textvariable=stop_bits_var, values=["1", "1.5", "2"], state="readonly")
        stop_bits_menu.grid(row=3, column=1, padx=10, pady=5)

        # Parity selection
        tk.Label(config_window, text="Parity:").grid(row=4, column=0, padx=10, pady=5)
        parity_var = tk.StringVar(value="None")
        parity_menu = ttk.Combobox(config_window, textvariable=parity_var, values=["None", "Even", "Odd", "Mark", "Space"], state="readonly")
        parity_menu.grid(row=4, column=1, padx=10, pady=5)

        # Connect button
        def connect_serial():
            """Connect to the selected serial port with the specified settings."""
            if not port_var.get():
                messagebox.showerror("Error", "Please select a port.")
                return

            try:
                parity_map = {
                    "None": serial.PARITY_NONE,
                    "Even": serial.PARITY_EVEN,
                    "Odd": serial.PARITY_ODD,
                    "Mark": serial.PARITY_MARK,
                    "Space": serial.PARITY_SPACE,
                }
                stop_bits_map = {
                    "1": serial.STOPBITS_ONE,
                    "1.5": serial.STOPBITS_ONE_POINT_FIVE,
                    "2": serial.STOPBITS_TWO,
                }
                data_bits_map = {
                    "5": serial.FIVEBITS,
                    "6": serial.SIXBITS,
                    "7": serial.SEVENBITS,
                    "8": serial.EIGHTBITS,
                }

                self.serial_port = serial.Serial(
                    port=port_var.get(),
                    baudrate=int(baud_var.get()),
                    bytesize=data_bits_map[data_bits_var.get()],
                    stopbits=stop_bits_map[stop_bits_var.get()],
                    parity=parity_map[parity_var.get()],
                    timeout=1,
                )
                messagebox.showinfo("Success", f"Connected to {port_var.get()}.")
                config_window.destroy()
            except Exception as e:
                messagebox.showerror("Error", f"Failed to open serial port: {e}")

        connect_button = tk.Button(config_window, text="Connect", command=connect_serial)
        connect_button.grid(row=5, column=0, columnspan=2, pady=10)

    def send_command(self, command_frame):
        """
        Send a command and wait for a valid response.
        Logs the sent and received messages.
        Args:
            command_frame: The command frame string to send.
        """
        if not self.serial_port or not self.serial_port.is_open:
            self._log_message(self.sent_text, "Error: Serial port not open.", is_sent=True)
            return

        # Validate frame structure
        if not command_frame.startswith("S:") or not command_frame.endswith("#"):
            self._log_message(self.sent_text, "Error: Invalid frame structure.", is_sent=True)
            return

        try:
            # Send the command
            self.serial_port.write(command_frame.encode('utf-8'))
            self._log_message(self.sent_text, command_frame, is_sent=True)

            # Wait for a valid response
            start_time = time.time()
            response_buffer = ""
            while True:
                # Check for timeout
                if time.time() - start_time > 10:
                    self._log_message(self.received_text, "Timeout: No response within 10 seconds.", is_sent=False)
                    break

                # Check if data is available
                if self.serial_port.in_waiting > 0:
                    data = self.serial_port.read(self.serial_port.in_waiting).decode('utf-8')
                    response_buffer += data

                    # Look for a complete frame (ends with '#')
                    if response_buffer.endswith("#"):
                        self._log_message(self.received_text, response_buffer.strip(), is_sent=False)
                        return response_buffer
            
        except Exception as e:
            self._log_message(self.received_text, f"Error: {str(e)}", is_sent=False)


    # Define the routine to clear the logs
    def clear_logs(self):
        """Clears the content of the send and receive message windows."""
        self.sent_text.delete(1.0, tk.END)  # Clear the send message window
        self.received_text.delete(1.0, tk.END)  # Clear the receive message window

    def dump_command(self):
        command_frame = "S:L0,DUMP#"
        self.send_command(command_frame)

    def read_clock_command(self):
        command_frame = "S:C0,CLOCK#"
        self.send_command(command_frame)

    def sync_clock_command(self):
        current_time = time.strftime("%Y-%m-%dT%H:%M:%S", time.localtime())
        command_frame = f'S:C0,CLOCKSET,"{current_time}"#'
        self.send_command(command_frame)

    def open_file_manager(self):
        """Opens the file manager dialog."""
        file_manager = tk.Toplevel(self.master)
        file_manager.title("File Manager")
        file_manager.geometry("900x600")

        # SD Card files section
        tk.Label(file_manager, text="Pico SD Card Files").grid(row=0, column=0, padx=10, pady=10)
        self.sd_files_tree = ttk.Treeview(file_manager, columns=("Filename", "Size"), show="headings")
        self.sd_files_tree.heading("Filename", text="Filename")
        self.sd_files_tree.heading("Size", text="Size (bytes)")
        self.sd_files_tree.column("Filename", width=200)
        self.sd_files_tree.column("Size", width=100)
        self.sd_files_tree.grid(row=1, column=0, padx=10, pady=10)

        tk.Button(file_manager, text="Update SD Card", command=self.update_sd_card_files).grid(row=2, column=0, padx=10, pady=10)

        # Dropdown to select the target directory on Pico
        tk.Label(file_manager, text="Target Directory on Pico").grid(row=3, column=0, padx=10, pady=5)
        self.target_directory_var = tk.StringVar(value="/")
        self.target_directory_menu = ttk.Combobox(file_manager, textvariable=self.target_directory_var, state="readonly")
        self.target_directory_menu.grid(row=4, column=0, padx=10, pady=5)

        # Local files section
        tk.Label(file_manager, text="Local Files").grid(row=0, column=1, padx=10, pady=10)
        self.local_files_tree = ttk.Treeview(file_manager, columns=("Filename", "Size"), show="headings")
        self.local_files_tree.heading("Filename", text="Filename")
        self.local_files_tree.heading("Size", text="Size (bytes)")
        self.local_files_tree.column("Filename", width=200)
        self.local_files_tree.column("Size", width=100)
        self.local_files_tree.grid(row=1, column=1, padx=10, pady=10)

        tk.Button(file_manager, text="Change Directory", command=self.change_local_directory).grid(row=2, column=1, padx=10, pady=10)

        # Buttons for file operations
        tk.Button(file_manager, text="Copy to Pico", command=self.copy_to_pico).grid(row=5, column=0, padx=10, pady=10)
        tk.Button(file_manager, text="Copy from Pico", command=self.copy_from_pico).grid(row=5, column=1, padx=10, pady=10)

        # Initialize local files and target directory
        self.local_directory = os.getcwd()
        self.update_local_files()
        self.update_sd_card_directories()

    def update_sd_card_files(self):
        """Updates the list of files on the Pico's SD card."""
        if not self.serial_port or not self.serial_port.is_open:
            messagebox.showerror("Error", "Serial port is not open.")
            return

        try:
            response = self.send_command('S:C0,FILELIST#')
            
            # Check if the response is valid and contains the expected structure
            if response and response.startswith("A:C0,FILELIST#OK-directory of SD card:"):
                # Extract the directory content
                directory_content = response.split("directory of SD card:")[1].strip()
                
                # Split into individual file entries
                files = directory_content.split("\n")
                
                # Clear the tree view
                self.sd_files_tree.delete(*self.sd_files_tree.get_children())
                
                for file_entry in files:
                    # Split the entry into filename and size (if size is provided)
                    if "*" in file_entry:
                        # File with size
                        filename, size = file_entry.rsplit("*", 1)
                        size = size.strip()
                    else:
                        # Directory or file without size
                        filename = file_entry.strip()
                        size = "Directory" if filename.endswith("/") else "Unknown"

                    # Insert into the tree view
                    self.sd_files_tree.insert("", "end", values=(filename, size))
            else:
                messagebox.showerror("Error", "Failed to retrieve SD card contents or unexpected response format. \n " + response)
        except Exception as e:
            messagebox.showerror("Error", f"Failed to update SD card files: {e}")

    def update_sd_card_directories(self):
        """Updates the list of directories on the Pico's SD card for the target directory dropdown."""
        if not self.serial_port or not self.serial_port.is_open:
            messagebox.showerror("Error", "Serial port is not open.")
            return

        try:
            response = self.send_command('S:C0,FILELIST#')

            # Check if the response contains the expected structure
            if response and response.startswith("A:C0,FILELIST#OK-directory of SD card:"):
                # Extract the directory content
                directory_content = response.split("directory of SD card:")[1].strip()
                
                # Split into individual entries and filter directories
                files = directory_content.split("\n")
                directories = ["/"]  # Always include the root directory
                directories.extend([file.strip() for file in files if file.strip().endswith("/")])
                
                # Update the dropdown menu
                self.target_directory_menu["values"] = directories
                self.target_directory_var.set(directories[0] if directories else "/")
            else:
                messagebox.showerror("Error", "Failed to retrieve SD card directories or unexpected response format.")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to update SD card directories: {e}")


    def change_local_directory(self):
        """Changes the directory for the local file list."""
        new_directory = filedialog.askdirectory(initialdir=self.local_directory, title="Select Directory")
        if new_directory:
            self.local_directory = new_directory
            self.update_local_files()

    def update_local_files(self):
        """Updates the list of local files in the current directory."""
        self.local_files_tree.delete(*self.local_files_tree.get_children())
        try:
            for file in os.listdir(self.local_directory):
                file_path = os.path.join(self.local_directory, file)
                size = os.path.getsize(file_path) if os.path.isfile(file_path) else "-"
                self.local_files_tree.insert("", "end", values=(file, size))
        except Exception as e:
            messagebox.showerror("Error", f"Failed to list files in directory: {e}")


    def copy_to_pico(self):
        """Copy a file from the local file system to the Pico's SD card."""
        selected_item = self.local_files_tree.focus()
        if not selected_item:
            messagebox.showerror("Error", "No file selected to copy to Pico.")
            return

        filename = self.local_files_tree.item(selected_item, "values")[0]
        local_file_path = os.path.join(self.local_directory, filename)

        if not os.path.isfile(local_file_path):
            messagebox.showerror("Error", f"Invalid file selected: {filename}")
            return

        try:
            with open(local_file_path, "rb") as f:
                data = f.read()

            file_size = len(data)
            chunk_size = 128
            chunks = (file_size + chunk_size - 1) // chunk_size
            target_directory = self.target_directory_var.get()
            target_path = os.path.join(target_directory, filename)

            # Send INIT frame
            init_frame = f'S:C0,FILEWRITE,0,0,0,0,"INIT~&~{target_path}~&~{file_size}~&~{chunks}"#'
            response = self.send_command(init_frame)
            if not response or "OK" not in response:
                raise Exception("Failed to initialize file write.")

            # Send chunks
            for i in range(chunks):
                chunk_data = data[i * chunk_size:(i + 1) * chunk_size]
                encoded_data = base64.b64encode(chunk_data).decode("utf-8")
                chunk_frame = f'S:C0,FILEWRITE,0,0,0,0,"DATA~&~{target_path}~&~{i}~&~{encoded_data}"#'
                response = self.send_command(chunk_frame)
                if not response or "OK" not in response:
                    raise Exception(f"Failed to write chunk {i}.")

            messagebox.showinfo("Copy to Pico", f"File '{filename}' successfully copied to Pico.")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to copy file to Pico: {e}")

    def copy_from_pico(self):
        """Copy a file from the Pico's SD card to the local file system."""
        selected_item = self.sd_files_tree.focus()
        if not selected_item:
            messagebox.showerror("Error", "No file selected to copy from Pico.")
            return

        filename = self.sd_files_tree.item(selected_item, "values")[0]
        if filename.endswith("/"):
            messagebox.showerror("Error", f"Selected item '{filename}' is a directory, not a file.")
            return

        local_file_path = os.path.join(self.local_directory, os.path.basename(filename))

        try:
            # Request file info
            init_frame = f'S:C0,FILEREAD,0,0,0,0,"INIT~&~{filename}"#'
            response = self.send_command(init_frame)
            
            # response frame look like :  A:C0,FILEREAD,0,0,0,0,"INIT~&~/HOTKEY.CFG"#OK-INIT~&~/HOTKEY.CFG~&~11101~&~87#
            if not response or "OK-" not in response:
                raise Exception("Failed to initialize file read.")

            # Parse file size and chunk count from response

            try:
                response = response.split("OK-")[1]  # we are only intrested in everything after OK-
                # response should contain now     INIT~&~/HOTKEY.CFG~&~11101~&~87
                if response.startswith("INIT") is False:
                    raise Exception("response not an answer to FILE init command")
                
                #file_size = int(response.split("~&~")[2])
                chunks = int(response.split("~&~")[3].strip("#"))
            except ValueError:
                raise Exception("Invalid file information received."+response+"chunks :"+response.split("~&~")[3])

            # Retrieve file data chunk by chunk
            data = bytearray()
            for i in range(chunks):
                chunk_frame = f'S:C0,FILEREAD,0,0,0,0,"DATA~&~{filename}~&~{i}"#'
                response = self.send_command(chunk_frame)
                if not response or "OK" not in response:
                    raise Exception(f"Failed to read chunk {i}.")
                encoded_data = response.split("~&~")[2]
                data.extend(base64.b64decode(encoded_data))

            # Write to local file
            with open(local_file_path, "wb") as f:
                f.write(data)

            messagebox.showinfo("Copy from Pico", f"File '{filename}' successfully copied to {local_file_path}.")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to copy file from Pico: {e}")



    def open_led_config_window(self):
        """Opens a new window for configuring LED module commands."""
        config_window = tk.Toplevel(self.master)
        config_window.title("LED Module Configuration")
        config_window.geometry("900x500")

        # Module and Index Selection
        rowStart = 0
        tk.Label(config_window, text="Module").grid(row=rowStart+0, column=0, padx=10, pady=5)
        module_var = tk.StringVar(value="L")
        module_menu = ttk.Combobox(config_window, textvariable=module_var, values=["L", "R", "S", "M"], state="readonly")
        module_menu.grid(row=rowStart+0, column=1, padx=10, pady=5)

        tk.Label(config_window, text="Index").grid(row=rowStart+1, column=0, padx=10, pady=5)
        index_var = tk.IntVar(value=0)
        index_menu = ttk.Combobox(config_window, textvariable=index_var, values=[0, 1, 2, 3, 4, 5, 6, 7, 8, 9], state="readonly")
        index_menu.grid(row=rowStart+1, column=1, padx=10, pady=5)

        # Command Entry
        cmds = ["off","on","breath","blink","multi flash","rainbow",
                "Off","Static","Blink","Breath","Color Wipe","Color Wipe Inverse","Color Wipe Reverse","Color Wipe Reverse Inverse","Color Wipe Random","Random Color","Single Dynamic",
                "Multi Dynamic","Rainbow","Rainbow Cycle","Scan","Dual Scan","Fade","Theater Chase","Theater Chase Rainbow","Running Lights","Twinkle","Twinkle Random","Twinkle Fade",
                "Twinkle Fade Random","Sparkle","Flash Sparkle","Hyper Sparkle","Strobe","Strobe Rainbow","Multi Strobe","Blink Rainbow","Chase White","Chase Color","Chase Random",
                "Chase Rainbow","Chase Flash","Chase Flash Random","Chase Rainbow White","Chase Blackout","Chase Blackout Rainbow","Color Sweep Random","Running Color","Running Red Blue",
                "Running Random","Larson Scanner","Comet","Fireworks","Fireworks Random","Merry Christmas","Fire Flicker","Fire Flicker (soft)","Fire Flicker (intense)","Circus Combustus",
                "Halloween","Bicolor Chase","Tricolor Chase","TwinkleFOX","Rain",
                "Custom 0","Custom 1","Custom 2","Custom 3","Custom 4","Custom 5","Custom 6","Custom 7"]
        
        tk.Label(config_window, text="Command").grid(row=rowStart+2, column=0, padx=10, pady=5)
        command_var = tk.StringVar(value="breath")
        command_entry = ttk.Combobox(config_window, textvariable=command_var, values=cmds, state="readonly")
        command_entry.grid(row=rowStart+2, column=1, columnspan=3, padx=10, pady=5)

        # DimCtrl Configuration
        rowStart = 3
        tk.Label(config_window, text="DimCfg").grid(row=rowStart+0, column=0, padx=10, pady=10)
        wrDim = tk.IntVar(value=0)
        tk.Checkbutton(config_window, text="WR", variable=wrDim).grid(row=rowStart+0, column=1)

        dim_value_var = tk.IntVar(value=0)
        tk.Label(config_window, text="value (0-255)").grid(row=rowStart+1, column=0, padx=10, pady=5)
        tk.Entry(config_window, textvariable=dim_value_var).grid(row=rowStart+1, column=1, padx=10, pady=5)

        dim_inc_var = tk.IntVar(value=0)
        tk.Label(config_window, text="Inc [-127;128]").grid(row=rowStart+2, column=0, padx=10, pady=5)
        tk.Entry(config_window, textvariable=dim_inc_var).grid(row=rowStart+2, column=1, padx=10, pady=5)

        dim_speed_var = tk.IntVar(value=0)
        tk.Label(config_window, text="Speed [0;4095]").grid(row=rowStart+3, column=0, padx=10, pady=5)
        tk.Entry(config_window, textvariable=dim_speed_var).grid(row=rowStart+3, column=1, padx=10, pady=5)

        
        # ColorCtrl Configuration
        rowStart = 7
        tk.Label(config_window, text="ColorCfg").grid(row=rowStart+0, column=0, padx=10, pady=10)
        wrColor = tk.IntVar(value=0)
        tk.Checkbutton(config_window, text="WR", variable=wrColor).grid(row=rowStart+0, column=1)

        color_start_var = tk.IntVar(value=0)
        tk.Label(config_window, text="start (0-255)").grid(row=rowStart+1, column=0, padx=10, pady=5)
        tk.Entry(config_window, textvariable=color_start_var).grid(row=rowStart+1, column=1, padx=10, pady=5)

        color_inc_var = tk.IntVar(value=0)
        tk.Label(config_window, text="Inc [-127;128]").grid(row=rowStart+2, column=0, padx=10, pady=5)
        tk.Entry(config_window, textvariable=color_inc_var).grid(row=rowStart+2, column=1, padx=10, pady=5)

        color_time_var = tk.IntVar(value=0)
        tk.Label(config_window, text="time [0;255]").grid(row=rowStart+3, column=0, padx=10, pady=5)
        tk.Entry(config_window, textvariable=color_time_var).grid(row=rowStart+3, column=1, padx=10, pady=5)
        
        color_div_var = tk.IntVar(value=0)
        tk.Label(config_window, text="event divider [0;255]").grid(row=rowStart+4, column=0, padx=10, pady=5)
        tk.Entry(config_window, textvariable=color_div_var).grid(row=rowStart+4, column=1, padx=10, pady=5)




        # cfg.str Configuration
        rowStart = 12
        tk.Label(config_window, text="cfg.str").grid(row=rowStart, column=0, padx=10, pady=5)
        str_var = tk.StringVar()
        str_entry = tk.Entry(config_window, textvariable=str_var, width=50)
        str_entry.grid(row=rowStart+1, column=0, columnspan=2, padx=10, pady=5)


        # Response Display
        rowStart = 14
        tk.Label(config_window, text="Response").grid(row=rowStart, column=0, padx=10, pady=5)
        response_text = tk.Text(config_window, height=10, width=40)
        response_text.grid(row=rowStart+1, column=0, columnspan=2, padx=10, pady=5)

        def send_command():
            """Sends the configured command to the Pico."""
            module = module_var.get()
            index = index_var.get()
            command = command_var.get()
            cfg_str = str_var.get()

            # Build configuration fields
            dim_ctrl = f"{dim_value_var.get():02X}{dim_inc_var.get():02X}"
            #color_ctrl = f"{color_start_var.get():02X}{color_inc_var.get():02X}"
            #flash_ctrl = f"{flashes_per_group_var.get():02X}{flash_time_var.get():02X}"
            #breath_ctrl = f"{breath_delta_var.get():02X}{breath_time_up_var.get():02X}"

            # Build frame
            frame = f"S:{module}{index},{command},{dim_ctrl},{0},{0},{0},{cfg_str}#"

            # Send frame and await response
            def send_and_receive():
                try:
                    self.serial_port.write(frame.encode())
                    start_time = time.time()
                    response = ""
                    while time.time() - start_time < 10:
                        if self.serial_port.in_waiting > 0:
                            response += self.serial_port.read(self.serial_port.in_waiting).decode()
                            if response.endswith("#"):
                                break
                    else:
                        response = "Timeout: No response received within 10 seconds."
                    response_text.insert("1.0", f"Response:\n{response}\n")
                except Exception as e:
                    messagebox.showerror("Error", f"Failed to send command: {e}")

            threading.Thread(target=send_and_receive, daemon=True).start()

        # Send Button
        tk.Button(config_window, text="Send", command=send_command).grid(row=15, column=0, padx=10, pady=10)
        dim_param_label = tk.Label(config_window, text="dimCfg : 0x____ ____")
        dim_param_label.grid(row=16, column=0,columnspan=2)


if __name__ == "__main__":
    root = tk.Tk()
    app = SerialCommandApp(root)
    root.mainloop()
