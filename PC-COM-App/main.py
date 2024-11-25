import tkinter as tk
from tkinter import ttk, messagebox, filedialog
import serial
import serial.tools.list_ports
import os
import base64
import threading
import time



CHUNK_SIZE = 128  # Maximum chunk size for file operations


class App:
    def __init__(self, root):
        self.root = root
        self.root.title("Pico Serial Communication")

        # Serial configuration
        self.serial_port = None
        self.baud_rate = 9600
        self.config_window = None

        # Menu
        menu = tk.Menu(root)
        root.config(menu=menu)

        file_menu = tk.Menu(menu, tearoff=0)
        menu.add_cascade(label="Port", menu=file_menu)
        file_menu.add_command(label="Open Serial Port", command=self.open_serial_config)

        actions_menu = tk.Menu(menu, tearoff=0)
        menu.add_cascade(label="Commands", menu=actions_menu)
        actions_menu.add_command(label="Dump", command=self.dump_command)
        actions_menu.add_command(label="Read Clock", command=self.read_clock)
        actions_menu.add_command(label="Sync Clock", command=self.sync_clock)
        actions_menu.add_command(label="File Manager", command=self.open_file_manager)
        actions_menu.add_command(label="LED-config", command=self.open_led_config_window)


        # SD Card and Local Files
        self.sd_files_list = None
        self.local_files_list = None

    def open_serial_config(self):
        """Opens the serial configuration dialog."""
        if self.config_window:
            return  # Prevent multiple instances

        self.config_window = tk.Toplevel(self.root)
        self.config_window.title("Serial Port Configuration")
        self.config_window.geometry("400x200")
        self.config_window.protocol("WM_DELETE_WINDOW", self.close_serial_config)

        tk.Label(self.config_window, text="Select Serial Port:").pack(pady=5)
        self.port_combobox = ttk.Combobox(self.config_window, values=self.get_serial_ports(), state="readonly")
        self.port_combobox.pack(pady=5)

        tk.Label(self.config_window, text="Select Baud Rate:").pack(pady=5)
        self.baud_rate_combobox = ttk.Combobox(self.config_window, values=[9600, 19200, 38400, 57600, 115200], state="readonly")
        self.baud_rate_combobox.set(self.baud_rate)
        self.baud_rate_combobox.pack(pady=5)

        tk.Button(self.config_window, text="Connect", command=self.connect_serial).pack(pady=10)

    def close_serial_config(self):
        """Closes the serial configuration dialog."""
        self.config_window.destroy()
        self.config_window = None

    def get_serial_ports(self):
        """Lists available serial ports."""
        ports = serial.tools.list_ports.comports()
        return [port.device for port in ports]

    def connect_serial(self):
        """Connects to the selected serial port."""
        port = self.port_combobox.get()
        baud_rate = self.baud_rate_combobox.get()

        if not port or not baud_rate:
            messagebox.showerror("Error", "Please select a valid serial port and baud rate.")
            return

        try:
            self.serial_port = serial.Serial(port, int(baud_rate), timeout=1)
            self.baud_rate = int(baud_rate)
            messagebox.showinfo("Success", f"Connected to {port} at {baud_rate} baud.")
            self.close_serial_config()
        except Exception as e:
            messagebox.showerror("Error", f"Failed to open serial port: {e}")

    def send_command(self, command):
        """Sends a command via the serial port and returns the response."""
        if not self.serial_port or not self.serial_port.is_open:
            messagebox.showerror("Error", "Serial port is not open.")
            return None

        try:
            self.serial_port.write(f"{command}\n".encode())
            response = self.serial_port.readline().decode().strip()
            return response
        except Exception as e:
            messagebox.showerror("Error", f"Failed to send command: {e}")
            return None

    def dump_command(self):
        """Sends a dump command."""
        response = self.send_command("S:C0,DUMP#")
        if response:
            messagebox.showinfo("Dump Response", response)

    def read_clock(self):
        """Sends a clock read command."""
        response = self.send_command("S:C0,CLOCK#")
        if response:
            messagebox.showinfo("Clock Response", response)

    def sync_clock(self):
        """Synchronizes the clock with the PC's clock."""
        import time
        now = time.strftime("%Y-%m-%dT%H:%M:%S", time.gmtime())
        response = self.send_command(f'S:C0,CLOCKSET,"{now}"#')
        if response:
            messagebox.showinfo("Clock Sync Response", response)


    def open_file_manager(self):
        """Opens the file manager dialog."""
        file_manager = tk.Toplevel(self.root)
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
            response = self.send_command('S:C0,DIR#')
            if response and "directory of SD card:" in response:
                files = response.split("directory of SD card:")[1].strip().split("\n")
                self.sd_files_tree.delete(*self.sd_files_tree.get_children())
                for file in files:
                    parts = file.split("    ")  # Filename and size are separated by spaces
                    if len(parts) == 2:
                        self.sd_files_tree.insert("", "end", values=(parts[0], parts[1]))
            else:
                messagebox.showerror("Error", "Failed to retrieve SD card contents.")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to update SD card files: {e}")

    def update_sd_card_directories(self):
        """Updates the list of directories on the Pico's SD card for the target directory dropdown."""
        if not self.serial_port or not self.serial_port.is_open:
            messagebox.showerror("Error", "Serial port is not open.")
            return

        try:
            response = self.send_command('S:C0,DIR#')
            if response and "directory of SD card:" in response:
                files = response.split("directory of SD card:")[1].strip().split("\n")
                directories = [file.split("/")[0] for file in files if file.endswith("/")]
                self.target_directory_menu["values"] = directories
                self.target_directory_var.set(directories[0] if directories else "/")
            else:
                messagebox.showerror("Error", "Failed to retrieve SD card directories.")
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
        selected = self.local_files_list.get(self.local_files_list.curselection())
        if selected:
            try:
                with open(selected, "rb") as f:
                    data = f.read()
                file_size = len(data)
                chunks = (file_size + CHUNK_SIZE - 1) // CHUNK_SIZE

                # Send INIT frame
                init_frame = f'S:C0,FILEWRITEINIT,"{selected}","{file_size}","{chunks}"#'
                response = self.send_command(init_frame)
                if not response or "OK" not in response:
                    raise Exception("Failed to initialize file write.")

                # Send chunks
                for i in range(chunks):
                    chunk_data = data[i * CHUNK_SIZE:(i + 1) * CHUNK_SIZE]
                    encoded_data = base64.b64encode(chunk_data).decode("utf-8")
                    chunk_frame = f'S:C0,FILEWRITE,"{selected}","{i}","{encoded_data}"#'
                    response = self.send_command(chunk_frame)
                    if not response or "OK" not in response:
                        raise Exception(f"Failed to write chunk {i}.")
                messagebox.showinfo("Copy to Pico", "File successfully copied to Pico.")
            except Exception as e:
                messagebox.showerror("Error", f"Failed to copy file: {e}")

    def copy_from_pico(self):
        """Copy a file from the Pico's SD card to the local file system."""
        selected = self.sd_files_list.get(self.sd_files_list.curselection())
        if selected:
            try:
                # Request file info
                init_frame = f'S:C0,FILEREADINIT,"{selected}"#'
                response = self.send_command(init_frame)
                if not response or "OK" not in response:
                    raise Exception("Failed to initialize file read.")

                # Parse file size and chunk count from response
                _, file_size, chunks = response.split(",")[1:]
                file_size = int(file_size)
                chunks = int(chunks)

                data = bytearray()
                for i in range(chunks):
                    chunk_frame = f'S:C0,FILEREAD,"{selected}","{i}"#'
                    response = self.send_command(chunk_frame)
                    if not response or "OK" not in response:
                        raise Exception(f"Failed to read chunk {i}.")
                    encoded_data = response.split(",")[2]
                    data.extend(base64.b64decode(encoded_data))

                with open(selected, "wb") as f:
                    f.write(data)
                messagebox.showinfo("Copy from Pico", f"File {selected} successfully copied.")
            except Exception as e:
                messagebox.showerror("Error", f"Failed to copy file: {e}")

    def open_led_config_window(self):
        """Opens a new window for configuring LED module commands."""
        config_window = tk.Toplevel(self.root)
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
    app = App(root)
   
