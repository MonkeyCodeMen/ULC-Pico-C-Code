# -*- coding: utf-8 -*-
"""
Created on Fri Nov 29 21:56:58 2024

@author: MonekyCodeMen
"""

import os
import tkinter as tk
from tkinter import filedialog, messagebox, ttk
from serial_comm import SerialCommunicator
import base64
from ProgressWindow import ProgressWindow


class FileManager:
    def __init__(self, app):
        self.app = app
        self.serial_comm = app.serial_comm

    def _open_file_manager(self,frame):
        """setup work frame for file manager."""
        self.frame = frame
        self._clear_frame()
        self.frame.configure(text="File manager")

        
        
        # SD Card files section
        file_frame = tk.Frame(self.frame)
        file_frame.pack(side=tk.TOP, expand=True, fill=tk.BOTH, padx=5, pady=5)

        self.sd_files_tree = ttk.Treeview(file_frame, columns=("Filename", "Size"), show="headings")
        self.sd_files_tree.heading("Filename", text="Filename")
        self.sd_files_tree.heading("Size", text="Size (bytes)")
        self.sd_files_tree.column("Filename", width=200)
        self.sd_files_tree.column("Size", width=100)
        self.sd_files_tree.pack(side=tk.LEFT,expand=True, fill=tk.X)

        self.local_files_tree = ttk.Treeview(file_frame, columns=("Filename", "Size"), show="headings")
        self.local_files_tree.heading("Filename", text="Filename")
        self.local_files_tree.heading("Size", text="Size (bytes)")
        self.local_files_tree.column("Filename", width=200)
        self.local_files_tree.column("Size", width=100)
        self.local_files_tree.pack(side=tk.RIGHT,expand=True, fill=tk.X)
        
        # button interface for file manager
        self.file_button_frame = tk.Frame(self.frame)
        self.file_button_frame.pack(side=tk.BOTTOM, fill=tk.X, pady=10)
        
        # Dropdown to select the target directory on Pico
        self.target_directory_var = tk.StringVar(value="/")
        self.target_directory_select = ttk.Combobox(self.file_button_frame, textvariable=self.target_directory_var, state="readonly",width=30)
        self.target_directory_select.pack(side=tk.LEFT, padx=5)

        tk.Button(self.file_button_frame, text="copy to SD",         command=self._copy_to_pico                 ).pack(side=tk.RIGHT, padx=5)
        tk.Button(self.file_button_frame, text="copy from SD",       command=self._start_copy_from_pico         ).pack(side=tk.RIGHT, padx=5)
        tk.Button(self.file_button_frame, text="change PC dir",      command=self._change_local_directory       ).pack(side=tk.RIGHT, padx=5)
        tk.Button(self.file_button_frame, text="update SD",          command=self._update_sd_card_files         ).pack(side=tk.RIGHT, padx=5)
        tk.Button(self.file_button_frame, text="update SD dirs",     command=self._update_sd_card_directories   ).pack(side=tk.RIGHT, padx=5)


        # Initialize local files and target directory
        self.local_directory = os.getcwd()
        self._update_local_files()
        self._update_sd_card_directories()
        self._update_sd_card_files()

        


    def _clear_frame(self):
        """Clears the content of the frame before adding new content."""
        for widget in self.frame.winfo_children():
            widget.destroy()

    
    def _update_sd_card_files(self):
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
                    # Split the entry into file_name and size (if size is provided)
                    if "*" in file_entry:
                        # File with size
                        file_name, size = file_entry.rsplit("*", 1)
                        size = size.strip()
                    else:
                        # Directory or file without size
                        file_name = file_entry.strip()
                        size = "Directory" if file_name.endswith("/") else "Unknown"

                    # Insert into the tree view
                    self.sd_files_tree.insert("", "end", values=(file_name, size))
            else:
                messagebox.showerror("Error", "Failed to retrieve SD card contents or unexpected response format. \n " + response)
        except Exception as e:
            messagebox.showerror("Error", f"Failed to update SD card files: {e}")

    def _update_sd_card_directories(self):
        """Updates the list of directories on the Pico's SD card for the target directory dropdown."""
        if not self.serial_comm or not self.serial_comm.is_connected():
            messagebox.showerror("Error", "Serial port is not open.")
            return

        try:
            response = self.app.serial_comm.send('S:C0,FILELIST#')

            # Check if the response contains the expected structure
            if response and response.startswith("A:C0,FILELIST#OK-directory of SD card:"):
                # Extract the directory content
                directory_content = response.split("directory of SD card:")[1].strip()
                
                # Split into individual entries and filter directories
                files = directory_content.split("\n")
                directories = ["/"]  # Always include the root directory
                directories.extend([file.strip() for file in files if file.strip().endswith("/")])
                
                # Update the dropdown menu
                self.target_directory_select["values"] = directories
                self.target_directory_select.set(directories[0] if directories else "/")
            else:
                messagebox.showerror("Error", "Failed to retrieve SD card directories or unexpected response format.")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to update SD card directories: {e}")


    def _change_local_directory(self):
        """Changes the directory for the local file list."""
        new_directory = filedialog.askdirectory(initialdir=self.local_directory, title="Select Directory")
        if new_directory:
            self.local_directory = new_directory
            self._update_local_files()

    def _update_local_files(self):
        """Updates the list of local files in the current directory."""
        self.local_files_tree.delete(*self.local_files_tree.get_children())
        try:
            for file in os.listdir(self.local_directory):
                file_path = os.path.join(self.local_directory, file)
                size = os.path.getsize(file_path) if os.path.isfile(file_path) else "-"
                self.local_files_tree.insert("", "end", values=(file, size))
        except Exception as e:
            messagebox.showerror("Error", f"Failed to list files in directory: {e}")


    def _copy_to_pico(self):
        """Copy a file from the local file system to the Pico's SD card."""
        selected_item = self.local_files_tree.focus()
        if not selected_item:
            messagebox.showerror("Error", "No file selected to copy to Pico.")
            return

        self.file_name = self.local_files_tree.item(selected_item, "values")[0]
        local_file_path = os.path.join(self.local_directory, self.file_name)

        if not os.path.isfile(local_file_path):
            messagebox.showerror("Error", f"Invalid file selected: {self.file_name}")
            return

        try:
            with open(local_file_path, "rb") as f:
                data = f.read()

            self.file_size = len(data)
            chunk_size = 128
            self.chunks = (self.file_size + chunk_size - 1) // chunk_size  # Calculate the number of chunks
            target_directory = self.target_directory_var.get()
            self.target_path = os.path.join(target_directory, self.file_name)

            # Create the progress window
            self.progress_window = ProgressWindow(self.app.master, self.chunks)

            # Send INIT frame
            init_frame = f'S:C0,FILEWRITE,0,0,{self.chunks},{self.file_size},"{self.target_path}"#'
            response = self.app.serial_comm.send(init_frame)
            if not response or "#OK-" not in response:
                raise Exception("Failed to initialize file write.")

            # Prepare data chunks
            self.data_chunks = [data[i * chunk_size:(i + 1) * chunk_size] for i in range(self.chunks)]

            # Start sending the first chunk
            self.app.master.after(100, self._send_chunks,self.progress_window)
            
            self.app._check_log_buffer()
            self._update_sd_card_files()

        except Exception as e:
            messagebox.showerror("Error", f"Failed to copy file to Pico: {e}")
            self.app._log_sent(f"Failed to copy file to Pico: {e}")
            if hasattr(self, 'progress_window') and self.progress_window.is_open:
                self.progress_window.close_window()
            self.app._check_log_buffer()


    def _send_chunks(self,progress_window):
        # Function to handle sending data chunks
        for i in range(self.chunks):
            progress_window.update_progress(i + 1, text=f"Copying chunk {i + 1} of {self.chunks}")
            chunk_data = self.data_chunks[i]
            encoded_data = base64.b64encode(chunk_data).decode("utf-8")
            chunk_frame = f'S:C0,FILEWRITE,0x0D,{i+1},{self.chunks},{self.file_size},"{encoded_data}"#'
            response = self.app.serial_comm.send_buffered_log(chunk_frame)
        
            if not response or "#OK-" not in response:
                messagebox.showerror("Error", f"Failed to write chunk {i+1}.")
                self.app._log_sent(f"Failed to write chunk {i+1}.")
                progress_window.close_window()
                self.app._check_log_buffer()
                return
 
        progress_window.close_window()
        self.app._check_log_buffer()




    def _start_copy_from_pico(self):
        """Copy a file from the Pico's SD card to the local file system."""
        selected_item = self.sd_files_tree.focus()
        if not selected_item:
            messagebox.showerror("Error", "No file selected to copy from Pico.")
            return
    
        file_name = self.sd_files_tree.item(selected_item, "values")[0]
        if file_name.endswith("/"):
            messagebox.showerror("Error", f"Selected item '{file_name}' is a directory, not a file.")
            return
    
        local_file_path = os.path.join(self.local_directory, os.path.basename(file_name))
    
        try:
            # Request file info
            init_frame = f'S:C0,FILEREAD,0,0,0,0,"{file_name}"#'
            response = self.app.serial_comm.send(init_frame)
    
            if not response or "OK-" not in response:
                raise Exception("Failed to initialize file read.")
    
            # Parse file size and chunk count from response
            try:
                if response.split(',')[1] != 'FILEREAD' or int(response.split(',')[2], 16) != 0 or int(response.split(',')[3], 16) != 0:
                    raise Exception("response not an answer to FILEREAD init command")
    
                self.chunks = int(response.split(',')[4], 16)
                self.file_size = int(response.split(',')[5], 16)
    
                if self.file_size > self.chunks * 128 or self.file_size < (self.chunks - 1) * 128:
                    raise Exception(f"File read failed due to invalid chunk or file size. chunks:{self.chunks} file size:{self.file_size}")
    
                # Create progress window
                self.progress_window = ProgressWindow(self.app.master, self.chunks)
                
                # Use `after` to prevent blocking and update progress asynchronously
                self.app.master.after(100, self._copy_from_pico, self.progress_window, file_name, local_file_path)
                
                self._update_local_files()
    
            except ValueError:
                raise Exception(f"Invalid file information received.--{response}-- chunks:{self.chunks} file size:{self.file_size}")
    
        except Exception as e:
            messagebox.showerror("Error", f"Failed to copy file from Pico: {e}")
            self.app._log_sent(f"Failed to copy file from Pico: {e}")
            if hasattr(self, 'progress_window') and self.progress_window.is_open:
                self.progress_window.close_window()
    
    def _copy_from_pico(self, progress_window, file_name, local_file_path):
        # Retrieve file data chunk by chunk
        try:
            data = bytearray()
            for i in range(self.chunks):
                progress_window.update_progress(i, text=f'File manager: copying from PICO chunk: {i} of {self.chunks}')
                chunk_frame = f'S:C0,FILEREAD,0x0D,{i+1},{self.chunks},{self.file_size},"{file_name}"#'
                response = self.app.serial_comm.send_buffered_log(chunk_frame)
    
                if (not response 
                    or response.split(',')[0] != 'A:C0'
                    or response.split(',')[1] != 'FILEREAD'
                    or int(response.split(',')[2], 16) != 0x0D
                    or int(response.split(',')[3], 16) != i+1
                    or int(response.split(',')[4], 16) != self.chunks
                    or int(response.split(',')[5], 16) != self.file_size
                    or '"#OK-' not in response):
                    raise Exception(f"Failed to read chunk {i}.")
                
                encoded_data = response.split('"')[1]
                data.extend(base64.b64decode(encoded_data))
    
            # Write to local file
            with open(local_file_path, "wb") as f:
                f.write(data)
    
            # Successfully copied, close the progress window
            progress_window.close_window()
            self.app._check_log_buffer()
    
        except Exception as e:
            messagebox.showerror("Error", f"Failed to copy file from Pico: {e}")
            self.app._log_sent(f"Failed to copy file from Pico: {e}")
            if hasattr(self, 'progress_window') and self.progress_window.is_open:
                self.progress_window.close_window()
            self.app._check_log_buffer()


"""
    def _copy_from_pico(self):
        #Copy a file from the Pico's SD card to the local file system.
        selected_item = self.sd_files_tree.focus()
        if not selected_item:
            messagebox.showerror("Error", "No file selected to copy from Pico.")
            return

        file_name = self.sd_files_tree.item(selected_item, "values")[0]
        if file_name.endswith("/"):
            messagebox.showerror("Error", f"Selected item '{file_name}' is a directory, not a file.")
            return

        local_file_path = os.path.join(self.local_directory, os.path.basename(file_name))

        try:
            # Request file info
            init_frame = f'S:C0,FILEREAD,0,0,0,0,"{file_name}"#'
            response = self.app.serial_comm.send(init_frame)
            
            # response frame look like :  A:C0,FILEWRITE,0,0,8,1024,"example.txt"#
            if not response or "OK-" not in response:
                raise Exception("Failed to initialize file read.")

            # Parse file size and chunk count from response
            try:
                if response.split(',')[1] != 'FILEREAD' or int(response.split(',')[2],16) != 0 or int(response.split(',')[3],16) != 0 :
                    raise Exception("response not an answer to FILEREAD init command")
                
                chunks    = int(response.split(',')[4],16)
                file_size = int(response.split(',')[5],16)
                
                if file_size > chunks*128 or file_size < (chunks-1)*128:
                    raise Exception(f"File read failed because of inavlid chunk or filesize. chunks:{chunks} file size:{file_size}")
                    
                progress_window = ProgressWindow(self.frame, chunks)

                # Retrieve file data chunk by chunk
                data = bytearray()
                for i in range(chunks):
                    progress_window.update_progress(i, text= f'File manager: copy from PICO chunk: {i} of {chunks}')
                    chunk_frame = f'S:C0,FILEREAD,0x0D,{i+1},{chunks},{file_size},"{file_name}"#'
                    response = self.app.serial_comm.send_buffered_log(chunk_frame)
                    
                    if (not response 
                        or response.split(',')[0] != 'A:C0'
                        or response.split(',')[1] != 'FILEREAD'
                        or int(response.split(',')[2],16) != 0x0D
                        or int(response.split(',')[3],16) != i+1
                        or int(response.split(',')[4],16) != chunks
                        or int(response.split(',')[5],16) != file_size
                        or '"#OK-' not in response 
                               ):
                        raise Exception(f"Failed to read chunk {i}.")
                    encoded_data = response.split('"')[1]
                    data.extend(base64.b64decode(encoded_data))
            
                    # Write to local file
                    with open(local_file_path, "wb") as f:
                        f.write(data)
                        
                    progress_window.close_window()
                    
                    self.app._log_recv("File {filename} received successfully in {chunks} chunks")

                
            except ValueError:
                raise Exception(f"Invalid file information received.--{response}-- chunks:{chunks} file size:{file_size}")
            
        except Exception as e:
            messagebox.showerror("Error", f"Failed to copy file from Pico: {e}")
            self.app._log_sent(f"Failed to copy file from Pico: {e}")
            progress_window.close_window()
        

    def _copy_to_pico(self):
        #Copy a file from the local file system to the Pico's SD card.
        selected_item = self.local_files_tree.focus()
        if not selected_item:
            messagebox.showerror("Error", "No file selected to copy to Pico.")
            return

        file_name = self.local_files_tree.item(selected_item, "values")[0]
        local_file_path = os.path.join(self.local_directory, file_name)

        if not os.path.isfile(local_file_path):
            messagebox.showerror("Error", f"Invalid file selected: {file_name}")
            return

        try:
            with open(local_file_path, "rb") as f:
                data = f.read()

            file_size = len(data)
            chunk_size = 128
            chunks = (file_size + chunk_size - 1) // chunk_size
            target_directory = self.target_directory_var.get()
            target_path = os.path.join(target_directory, file_name)

            # Send INIT frame
            init_frame = f'S:C0,FILEWRITE,0,0,0,0,"INIT~&~{target_path}~&~{file_size}~&~{chunks}"#'
            response = self.app.serial_comm.send(init_frame)
            if not response or "OK" not in response:
                raise Exception("Failed to initialize file write.")

            # Send chunks
            for i in range(chunks):
                chunk_data = data[i * chunk_size:(i + 1) * chunk_size]
                encoded_data = base64.b64encode(chunk_data).decode("utf-8")
                chunk_frame = f'S:C0,FILEWRITE,0,0,0,0,"DATA~&~{target_path}~&~{i}~&~{encoded_data}"#'
                response = self.app.serial_comm.send(chunk_frame)
                if not response or "OK" not in response:
                    raise Exception(f"Failed to write chunk {i}.")

            messagebox.showinfo("Copy to Pico", f"File '{file_name}' successfully copied to Pico.")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to copy file to Pico: {e}")

            
"""