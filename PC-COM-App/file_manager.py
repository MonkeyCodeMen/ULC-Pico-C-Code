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

    def open_file_manager(self,frame):
        """setup work frame for file manager."""
        self.frame = frame
        self.frame.configure(text="File manager")

        # at top of file manager area a bar to select/change directories
        self.file_select_frame = tk.Frame(self.frame)
        self.file_select_frame.pack(side=tk.TOP, fill=tk.X, padx=5, pady=5)
        ## Dropdown to select the target directory on Pico
        self.sd_current_dir = "/"
        self.target_directory_select = ttk.Combobox(self.file_select_frame, textvariable=self.sd_current_dir, state="readonly",width=30)
        self.target_directory_select.pack(side=tk.LEFT, padx=5)
        self.target_directory_select.bind("<<ComboboxSelected>>", self._new_sd_dir_selected)
        tk.Button(self.file_select_frame, text="change PC dir",      command=self._change_local_directory       ).pack(side=tk.RIGHT, padx=5)
        tk.Button(self.file_select_frame, text="update SD",          command=self._complete_update_sd_tree_view ).pack(side=tk.RIGHT, padx=5)
        
        # file tree windows under selection bar
        file_view_frame = tk.Frame(self.frame)
        file_view_frame.pack(side=tk.TOP, expand=True, fill=tk.BOTH, padx=5, pady=5)
        
        ## SD card tree view
        self.sd_files_tree = ttk.Treeview(file_view_frame, columns=("Filename", "Size"), show="headings")
        self.sd_files_tree.heading("Filename", text="Filename")
        self.sd_files_tree.heading("Size", text="Size (bytes)")
        self.sd_files_tree.column("Filename", width=200)
        self.sd_files_tree.column("Size", width=20)
        self.sd_files_tree.pack(side=tk.LEFT,expand=True, fill=tk.X)
        # Add a scrollbar to the SD files tree view
        sd_files_scroll = tk.Scrollbar(self.sd_files_tree_frame, command=self.sd_files_tree.yview)
        sd_files_scroll.pack(side=tk.RIGHT, fill=tk.Y)
        self.sd_files_tree.configure(yscrollcommand=sd_files_scroll.set)

        ## local tree vire
        self.local_files_tree = ttk.Treeview(file_view_frame, columns=("Filename", "Size"), show="headings")
        self.local_files_tree.heading("Filename", text="Filename")
        self.local_files_tree.heading("Size", text="Size (bytes)")
        self.local_files_tree.column("Filename", width=200)
        self.local_files_tree.column("Size", width=20)
        self.local_files_tree.pack(side=tk.RIGHT,expand=True, fill=tk.X)
        # Add a scrollbar to the local files tree view
        local_files_scroll = tk.Scrollbar(self.local_files_tree_frame, command=self.local_files_tree.yview)
        local_files_scroll.pack(side=tk.RIGHT, fill=tk.Y)
        self.local_files_tree.configure(yscrollcommand=local_files_scroll.set)
    
        # file manager commands are placed under the file windows
        self.file_command_frame = tk.Frame(self.frame)
        self.file_command_frame.pack(side=tk.BOTTOM, fill=tk.X, padx=5, pady=5)
        tk.Button(self.file_command_frame, text="copy to SD",       command=self._copy_to_pico                 ).pack(side=tk.RIGHT, padx=5)
        tk.Button(self.file_command_frame, text="copy from SD",     command=self._start_copy_from_pico         ).pack(side=tk.RIGHT, padx=5)
        tk.Button(self.file_command_frame, text="delete file",      command=self._delete_file_from_pico        ).pack(side=tk.RIGHT, padx=5)
        tk.Button(self.file_command_frame, text="delete dir",       command=self._delete_dir_from_pico         ).pack(side=tk.RIGHT, padx=5)
        tk.Button(self.file_command_frame, text="create dir",       command=self._create_new_directory         ).pack(side=tk.RIGHT, padx=5)

        # Initialize local files and target directory
        self.local_directory       = os.getcwd()
        self._update_local_files()
        self._complete_update_sd_tree_view()

    def stop(self):
        """Stop the background thread by setting the stop flag"""
        self.stop_flag = True  # Set the stop_flag to True to exit the loop in the background task   

 


    def _get_sd_card_complete_file_list(self):
        """read from pico the complete recursive file list"""
        list = []
        if not self.serial_comm or not self.serial_comm.is_connected():
            messagebox.showerror("Error", "Serial port is not open.")
            return

        try:
            response = self.app.serial_comm.send('S:C0,FILE list#')

            # Check if the response contains the expected structure
            if response and response.startswith("A:C0,FILE list#OK-directory of SD card:"):
                # Extract the directory content
                directory_content = response.split("directory of SD card:")[1].strip()
                
                # Split into individual entries and filter directories
                files = directory_content.split("\n")
                list = ["/"]  # Always include the root directory
                list.extend([file.strip() for file in files ])
            else:
                messagebox.showerror("Error", "Failed to retrieve SD card file list or unexpected response format.")

            return list
        except Exception as e:
            messagebox.showerror("Error", f"Failed to update SD card file list: {e}")
            return list

    def _extract_directories(self,list):
        """ extract and return from list all entries that represent a directory"""
        dir_list = []
        dir_list.extend([entry for entry in list if entry.endswith('/') ])
        return dir_list
    
    def _filter_current_dir_list(self,current_dir,list):
        """ extract from complete list a view of the current directory"""
        # Filter and separate directories and files
        directories = []
        regular_files = []
        for entry in list:
            entry = entry.strip()

            # Skip if the entry is empty
            if not entry:
                continue

            # Check if the file entry starts with the selected directory
            if not entry.startswith(current_dir):
                continue

            # show files of this directory and subdirectory of this directory
            rest = entry.removeprefix(current_dir)
            if rest.count('/') > 1 or (rest.count('/') == 1 and not rest.endswith('/')) :
                continue

            # If the entry ends with a slash, it's a directory
            if entry.endswith("/"):
                directories.append(entry)
            else:
                regular_files.append(entry)

        regular_files.sort()
        directories.sort()

        res = directories + regular_files
        return res
        


    def _update_sd_tree_view(self):
        """Updates the list of files on the Pico's SD card based on the selected directory."""
        # Clear the tree view
        self.sd_files_tree.delete(*self.sd_files_tree.get_children())

        # insert elements
        for entry in self.sd_current_dir_list:
                if entry.endswith('/'):
                    self.sd_files_tree.insert("", "end", values=(entry, "dir"))
                else:
                    if "*" in entry:
                        # File with size
                        file_name, size = entry.rsplit("*", 1)
                        size = size.strip()
                    else:
                        # File without size
                        file_name = entry.strip()
                        size = "?"

                    # Insert into the tree view
                    self.sd_files_tree.insert("", "end", values=(file_name, size))

    def _new_sd_dir_selected(self,event):
        """ get selected sd base dir and update file tree view base on this selection """
        if not self.sd_directories: 
            self.sd_current_dir = "/"
        else:
            self.sd_current_dir = self.target_directory_select.get()
            if self.sd_current_dir not in self.sd_directories:
                self.sd_current_dir = self.sd_directories[0]
            
        self.sd_current_dir_list = self._filter_current_dir_list(self.sd_current_dir,self.sd_complete_file_list)
        self._update_sd_tree_view()


    def _complete_update_sd_tree_view(self):
        """ read file list of sd and upadte tree view """
        self.sd_complete_file_list = self._get_sd_card_complete_file_list()
        self.sd_directories        = self._extract_directories(self.sd_complete_file_list)
        self.target_directory_select.config(values = self.sd_directories)
        self._new_sd_dir_selected("<<ComboboxSelected>>")

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
            target_directory = self.sd_current_dir
            self.target_path = os.path.join(target_directory, self.file_name)

            # Create the progress window
            self.progress_window = ProgressWindow(self.app.master, self.chunks)

            # Send INIT frame
            init_frame = f'S:C0,FILE write,0,0,{self.chunks},{self.file_size},"{self.target_path}"#'
            response = self.app.serial_comm.send(init_frame)
            if not response or "#OK-" not in response:
                raise Exception("Failed to initialize file write.")

            # Prepare data chunks
            self.data_chunks = [data[i * chunk_size:(i + 1) * chunk_size] for i in range(self.chunks)]

            # Start sending the first chunk
            self.app.master.after(100, self._send_chunks,self.progress_window)
            
            self.app._check_log_buffer()

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
            chunk_frame = f'S:C0,FILE write,0x0D,{i+1},{self.chunks},{self.file_size},"{encoded_data}"#'
            response = self.app.serial_comm.send_buffered_log(chunk_frame)
        
            if not response or "#OK-" not in response:
                messagebox.showerror("Error", f"Failed to write chunk {i+1}.")
                self.app._log_sent(f"Failed to write chunk {i+1}.")
                progress_window.close_window()
                self.app._check_log_buffer()
                return
 
        progress_window.close_window()
        self.app._check_log_buffer()
        self._complete_update_sd_tree_view()




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
            init_frame = f'S:C0,FILE read,0,0,0,0,"{file_name}"#'
            response = self.app.serial_comm.send(init_frame)
    
            if not response or "OK-" not in response:
                raise Exception("Failed to initialize file read.")
    
            # Parse file size and chunk count from response
            try:
                if response.split(',')[1] != 'FILE read' or int(response.split(',')[2], 16) != 0 or int(response.split(',')[3], 16) != 0:
                    raise Exception("response not an answer to 'FILE read' init command")
    
                self.chunks = int(response.split(',')[4], 16)
                self.file_size = int(response.split(',')[5], 16)
    
                if self.file_size > self.chunks * 128 or self.file_size < (self.chunks - 1) * 128:
                    raise Exception(f"File read failed due to invalid chunk or file size. chunks:{self.chunks} file size:{self.file_size}")
    
                # Create progress window
                self.progress_window = ProgressWindow(self.app.master, self.chunks)
                
                # Use `after` to prevent blocking and update progress asynchronously
                self.app.master.after(100, self._copy_from_pico, self.progress_window, file_name, local_file_path)
    
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
                chunk_frame = f'S:C0,FILE read,0x0D,{i+1},{self.chunks},{self.file_size},"{file_name}"#'
                response = self.app.serial_comm.send_buffered_log(chunk_frame)
    
                if (not response 
                    or response.split(',')[0] != 'A:C0'
                    or response.split(',')[1] != 'FILE read'
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
            self._update_local_files()
    
        except Exception as e:
            messagebox.showerror("Error", f"Failed to copy file from Pico: {e}")
            self.app._log_sent(f"Failed to copy file from Pico: {e}")
            if hasattr(self, 'progress_window') and self.progress_window.is_open:
                self.progress_window.close_window()
            self.app._check_log_buffer()



    def _delete_file_from_pico(self):
        """delte a file from the Pico's SD card."""
        selected_item = self.sd_files_tree.focus()
        if not selected_item:
            messagebox.showerror("Error", "No file selected to delete on Pico.")
            return
    
        file_name = self.sd_files_tree.item(selected_item, "values")[0]
        if file_name.endswith("/"):
            messagebox.showerror("Error", f"Selected item '{file_name}' is a directory, not a file.")
            return
    
        try:
            init_frame = f'S:C0,FILE delete,0,0,0,0,"{file_name}"#'
            response = self.app.serial_comm.send(init_frame)
    
            if not response or "OK-" not in response:
                raise Exception("Failed to delete file.")
            
            self._complete_update_sd_tree_view()
    
        except Exception as e:
            messagebox.showerror("Error", f"Failed to delete file from Pico: {e}")
            self.app._sent(f"Failed to delete file from Pico: {e}")
            self._complete_update_sd_tree_view()

    def _delete_dir_from_pico(self):
        """delte a directory (with complete content) from the Pico's SD card."""
        selected_item = self.sd_files_tree.focus()
        if not selected_item:
            messagebox.showerror("Error", "No directory selected to delete on Pico.")
            return
    
        path = self.sd_files_tree.item(selected_item, "values")[0]
        if not path.endswith("/"):
            messagebox.showerror("Error", f"Selected item '{path}' is not a directory.")
            return
    
        try:
            init_frame = f'S:C0,FILE rmdir,0,0,0,0,"{path}"#'
            response = self.app.serial_comm.send(init_frame)
    
            if not response or "OK-" not in response:
                raise Exception("Failed to delete directory.")
            
            self._complete_update_sd_tree_view()
    
        except Exception as e:
            messagebox.showerror("Error", f"Failed to delete directory from Pico: {e}")
            self.app._sent(f"Failed to delete directory from Pico: {e}")
            self._complete_update_sd_tree_view()


    def _create_new_directory(self):
        """Prompt the user for a new directory name and create it on the SD card."""
        # Ask for the new directory name
        new_dir_name = tk.simpledialog.askstring("New Directory", "Enter the name of the new directory:")
        new_dir_name = new_dir_name.strip()

        if not new_dir_name:
            messagebox.showwarning("Invalid Input", "No directory name entered.")
            return
        
        # Get the current selected directory from the combobox
        current_directory = self.sd_current_dir
        
        # Combine current directory with the new directory name
        new_dir_path = os.path.join(current_directory, new_dir_name)
        
        # Send the command to create the new directory on the SD card
        try:
            command = f'S:C0,FILE mkdir,0,0,0,0,"{new_dir_path}"#'
            response = self.serial_comm.send(command)
            
            if response and "#OK-" in response:
                messagebox.showinfo("Success", f"Directory '{new_dir_name}' created successfully.")
            else:
                messagebox.showerror("Error", f"Failed to create directory '{new_dir_name}'.")
            self._complete_update_sd_tree_view()

        except Exception as e:
            messagebox.showerror("Error", f"Failed to send the command: {e}")
            self._complete_update_sd_tree_view()
