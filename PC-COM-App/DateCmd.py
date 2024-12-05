# -*- coding: utf-8 -*-
"""
Created on Fri Nov 29 21:56:58 2024

@author: MonekyCodeMen
"""

import time

class DateCmd:
    def __init__(self, app):
        """
        Initialize the DateMode.

        app: The main application instance to interact with the UI.
        """
        self.app = app

    def read_date_time(self):
        """Send a command to the Pico to read the current date and time."""
        self.app.serial_comm.send("S:C0,CLOCK#")
        # answer will be shown in recv data frame

    def update_date_time(self):
        """Send a command to the Pico to read the current date and time."""
        current_time = time.strftime("%Y-%m-%dT%H:%M:%S", time.localtime())
        command_frame = f'S:C0,CLOCK set,0,0,0,0,"{current_time}"#'
        self.app.serial_comm.send(command_frame)
        # answer will be shown in recv data frame
