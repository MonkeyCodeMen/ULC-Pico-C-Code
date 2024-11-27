import os
import tkinter as tk
from tkinter import filedialog, messagebox, ttk
from serial_comm import SerialCommunicator
import base64


class FileManager:
    def __init__(self, app):
        self.app = app
        self.serial_comm = app.serial_comm

    def _open_file_manager(self,frame):
        """setup work frame for file manager."""
        self.frame = frame
        self._clear_work_frame()
        
        # SD Card files section
        pico_files = tk.LabelFrame(self.frame, text="Pico SD Card Files")
        pico_files.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=5, pady=5)
        self.sd_files_tree = ttk.Treeview(pico_files, columns=("Filename", "Size"), show="headings")
        self.sd_files_tree.heading("Filename", text="Filename")
        self.sd_files_tree.heading("Size", text="Size (bytes)")
        self.sd_files_tree.column("Filename", width=200)
        self.sd_files_tree.column("Size", width=100)
        self.sd_files_tree.pack(side=tk.RIGHT, fill=tk.Y)

        # Local files section
        PC_files = tk.Label(self.frame, text="Local Files")
        PC_files.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=5, pady=5)
        self.local_files_tree = ttk.Treeview(PC_files, columns=("Filename", "Size"), show="headings")
        self.local_files_tree.heading("Filename", text="Filename")
        self.local_files_tree.heading("Size", text="Size (bytes)")
        self.local_files_tree.column("Filename", width=200)
        self.local_files_tree.column("Size", width=100)
        self.local_files_tree.pack(side=tk.RIGHT, fill=tk.Y)
        
        # button interface for file manager
        self.file_button_frame = tk.Frame(self.frame)
        self.file_button_frame.pack(side=tk.RIGHT, fill=tk.X, pady=10)
        
        tk.Button(self.file_button_frame, text="change PC dir",      command=self.change_local_directory         ).pack(side=tk.RIGHT, pady=5)
        tk.Button(self.file_button_frame, text="copy to SD",         command=self.copy_to_pico                   ).pack(side=tk.RIGHT, pady=5)
        tk.Button(self.file_button_frame, text="copy from SD",       command=self.copy_from_pico                 ).pack(side=tk.RIGHT, pady=5)
        

        # Dropdown to select the target directory on Pico
        self.target_dir_frame = tk.Label(self.file_button_frame, text="Target Directory on Pico")
        self.target_dir_frame.pack(side=tk.TOP, pady=5)
        self.target_directory_var = tk.StringVar(value="/")
        self.target_directory_menu = ttk.Combobox(self.target_dir_frame, textvariable=self.target_directory_var, state="readonly").pack(side=tk.LEFT, pady=5)

        # Initialize local files and target directory
        self.local_directory = os.getcwd()
        self.update_local_files()
        #self.update_sd_card_directories()

        


    def _clear_work_frame(self):
        """Clears the content of the work_frame before adding new content."""
        for widget in self.frame.winfo_children():
            widget.destroy()

    def _frame_syntax_check(self, file_path):
        """Syntax check for the selected file."""
        if not os.path.isfile(file_path):
            messagebox.showerror("Error", f"The selected file {file_path} is not valid.")
            return False
        return True
    
    def update_sd_card_files(self):
        """Updates the list of files on the Pico's SD card."""
        if not self.app.serial_comm.is_connected():
            messagebox.showerror("Error", "not connected.")
            return

        try:
            response = self.app.serial_comm.send('S:C0,FILELIST#')
            
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
