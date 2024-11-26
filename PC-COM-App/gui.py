import tkinter as tk
from tkinter import ttk, messagebox
import threading
from serial_comm import SerialCommunicator
import os

class SerialCommandApp:
    def __init__(self, master):
        self.master = master
        self.master.title("Serial Command Interface")
        self.master.geometry("1200x600")
        self.serial_comm = SerialCommunicator()
        self.local_directory = os.getcwd()
        self._create_ui()

    def _create_ui(self):
        # Frame für die Buttons
        command_frame = tk.Frame(self.master)
        command_frame.pack(side=tk.TOP, fill=tk.X, padx=10, pady=5)
        tk.Button(command_frame, text="Serial Port", command=self.open_serial_config, width=12).pack(side=tk.LEFT, padx=5)
        tk.Button(command_frame, text="Clear Logs", command=self.clear_logs, width=12).pack(side=tk.LEFT, padx=5)

        # Frame für Logs
        log_frame = tk.Frame(self.master)
        log_frame.pack(side=tk.TOP, fill=tk.BOTH, expand=True, padx=10, pady=5)

        # Logs für gesendete Nachrichten
        sent_frame = tk.LabelFrame(log_frame, text="Sent Messages", width=50, height=10)
        sent_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=5, pady=5)
        self.sent_text = tk.Text(sent_frame, wrap=tk.WORD, state=tk.DISABLED, bg="lightgrey", height=15)
        self.sent_text.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        sent_scroll = tk.Scrollbar(sent_frame, command=self.sent_text.yview)
        sent_scroll.pack(side=tk.RIGHT, fill=tk.Y)
        self.sent_text.configure(yscrollcommand=sent_scroll.set)

        # Logs für empfangene Nachrichten
        received_frame = tk.LabelFrame(log_frame, text="Received Messages", width=50, height=10)
        received_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=5, pady=5)
        self.received_text = tk.Text(received_frame, wrap=tk.WORD, state=tk.DISABLED, bg="lightgrey", height=15)
        self.received_text.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        received_scroll = tk.Scrollbar(received_frame, command=self.received_text.yview)
        received_scroll.pack(side=tk.RIGHT, fill=tk.Y)
        self.received_text.configure(yscrollcommand=received_scroll.set)

        # Frame für den Datei-Manager
        file_manager_frame = tk.Frame(self.master)
        file_manager_frame.pack(side=tk.TOP, fill=tk.X, padx=10, pady=10)

        tk.Button(file_manager_frame, text="Copy to Pico", command=self.copy_to_pico, width=15).pack(side=tk.LEFT, padx=5)
        tk.Button(file_manager_frame, text="Copy from Pico", command=self.copy_from_pico, width=15).pack(side=tk.LEFT, padx=5)

    def open_serial_config(self):
        """Automatisch den ersten verfügbaren Port mit 115200 Baudrate verbinden."""
        success, message = self.serial_comm.auto_connect(baudrate=115200)
        if success:
            messagebox.showinfo("Success", message)
        else:
            messagebox.showerror("Error", message)

    def send_command(self, command_frame):
        """Sendet einen Befehl und zeigt die Antwort im GUI an."""
        try:
            response = self.serial_comm.send_command(command_frame)
            if response:
                self._log_message(self.received_text, response, is_sent=False)
        except Exception as e:
            messagebox.showerror("Error", f"Failed to send command: {e}")

    def _log_message(self, text_widget, message, is_sent=True):
        """Protokolliert eine Nachricht im GUI."""
        timestamp = time.strftime("%H:%M:%S", time.localtime())
        prefix = "[SENT] " if is_sent else "[RECV] "
        text_widget.configure(state=tk.NORMAL)
        text_widget.insert(tk.END, f"{prefix}{timestamp}: {message}\n")
        text_widget.see(tk.END)
        text_widget.configure(state=tk.DISABLED)

    def clear_logs(self):
        """Löscht die Protokolle für gesendete und empfangene Nachrichten."""
        self.sent_text.delete(1.0, tk.END)
        self.received_text.delete(1.0, tk.END)

    def copy_to_pico(self):
        """Kopiert eine Datei vom lokalen Dateisystem auf die Pico SD-Karte."""
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

            # Fortschrittsbalken einrichten
            progress_window = tk.Toplevel(self.master)
            progress_window.title("Copy Progress")
            progress = ttk.Progressbar(progress_window, orient="horizontal", length=300, mode="determinate", maximum=chunks)
            progress.pack(padx=10, pady=10)

            # INIT-Frame senden
            init_frame = f'S:C0,FILEWRITE,0,0,0,0,"INIT~&~{target_path}~&~{file_size}~&~{chunks}"#'
            response = self.serial_comm.send_command(init_frame)
            if not response or "OK" not in response:
                raise Exception("Failed to initialize file write.")

            # Chunks senden mit Fortschrittsaktualisierung
            for i in range(chunks):
                chunk_data = data[i * chunk_size:(i + 1) * chunk_size]
                encoded_data = base64.b64encode(chunk_data).decode("utf-8")
                chunk_frame = f'S:C0,FILEWRITE,0,0,0,0,"DATA~&~{target_path}~&~{i}~&~{encoded_data}"#'
                response = self.serial_comm.send_command(chunk_frame)
                if not response or "OK" not in response:
                    raise Exception(f"Failed to write chunk {i}.")
                progress["value"] = i + 1  # Fortschrittsbalken aktualisieren
                progress_window.update_idletasks()

            messagebox.showinfo("Copy to Pico", f"File '{filename}' successfully copied to Pico.")
            progress_window.destroy()
        except Exception as e:
            messagebox.showerror("Error", f"Failed to copy file to Pico: {e}")

    def copy_from_pico(self):
        """Kopiert eine Datei von der Pico SD-Karte auf das lokale Dateisystem."""
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
            # Anfrage zur Dateiinformation senden
            init_frame = f'S:C0,FILEREAD,0,0,0,0,"INIT~&~{filename}"#'
            response = self.serial_comm.send_command(init_frame)
            if not response or "OK-" not in response:
                raise Exception("Failed to initialize file read.")

            # Daten empfangen und lokal speichern
            data = bytearray()
            chunks = int(response.split("~&~")[3].strip("#"))
            for i in range(chunks):
                chunk_frame = f'S:C0,FILEREAD,0,0,0,0,"DATA~&~{filename}~&~{i}"#'
                response = self.serial_comm.send_command(chunk_frame)
                if not response or "OK" not in response:
                    raise Exception(f"Failed to read chunk {i}.")
                encoded_data = response.split("~&~")[2]
                data.extend(base64.b64decode(encoded_data))

            # Lokale Datei schreiben
            with open(local_file_path, "wb") as f:
                f.write(data)

            messagebox.showinfo("Copy from Pico", f"File '{filename}' successfully copied to {local_file_path}.")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to copy file from Pico: {e}")
