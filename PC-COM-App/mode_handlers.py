# -*- coding: utf-8 -*-
"""
Created on Fri Nov 29 21:56:58 2024

@author: MonekyCodeMen
"""

class ModeHandler:
    def __init__(self, app):
        self.app = app

    def switch_mode(self, mode):
        """Switch between different modes."""
        mode_data = f"Mode {mode+1} selected"
        self.update_mode_ui(mode_data)

    def update_mode_ui(self, mode_data):
        """Update the UI based on the selected mode."""
        self.app.dynamic_area_frame.config(text=mode_data)
