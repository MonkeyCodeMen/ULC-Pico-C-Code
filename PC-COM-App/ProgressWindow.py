# -*- coding: utf-8 -*-
"""
Created on Fri Nov 29 21:56:58 2024

@author: MonekyCodeMen


def long_running_task(progress_window):
    #Simulate a long-running task and update the progress.
    for i in range(1, progress_window.end_value + 1):
        time.sleep(0.1)  # Simulate work
        progress_window.update_progress(i, text="Working...")
    
    # Close the window once the task is complete
    progress_window.close_window()


def start_task():
    #Start the progress window and the long-running task.
    end_value = 100  # End value for the progress bar
    progress_window = ProgressWindow(root, end_value)

    # Start the long-running task without blocking the main window
    root.after(100, long_running_task, progress_window)

# Create the main window
root = tk.Tk()
root.title("Main Window")
root.geometry("300x100")

# Button to start the progress task
start_button = tk.Button(root, text="Start", command=start_task)
start_button.pack(pady=30)

root.mainloop()


"""


import tkinter as tk
from tkinter import ttk
import time

class ProgressWindow:
    def __init__(self, master, end_value):
        # Create a new top-level window for progress
        self.progress_window = tk.Toplevel(master)
        self.progress_window.title("Progress")
        self.progress_window.geometry("400x150")
        self.is_open = True  # Flag to track if the window is open

        # End value and current progress
        self.end_value = end_value
        self.current_value = 0

        # Create progress bar
        self.progress_bar = ttk.Progressbar(self.progress_window, orient="horizontal", length=300, mode="determinate", maximum=self.end_value)
        self.progress_bar.pack(pady=20)

        # Label to show progress text
        self.text_label = tk.Label(self.progress_window, text="Starting...")
        self.text_label.pack()

        # Close button
        self.close_button = tk.Button(self.progress_window, text="Close", command=self.close_window)
        self.close_button.pack(pady=10)
        self.close_button.config(state=tk.DISABLED)  # Initially disabled

    def update_progress(self, current_value, text=""):
        """Update progress bar and text."""
        if not self.is_open:  # If the window has been closed, skip the update
            return
        self.current_value = current_value
        self.progress_bar["value"] = self.current_value
        self.text_label.config(text=f"{text} ({self.current_value}/{self.end_value})")
        self.progress_window.update_idletasks()

    def close_window(self):
        """Close the progress window."""
        self.is_open = False  # Set flag to indicate the window is closed
        self.progress_window.destroy()