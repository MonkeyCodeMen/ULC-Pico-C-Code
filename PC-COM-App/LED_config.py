import tkinter as tk
from tkinter import ttk, messagebox, StringVar
import threading 
import time


class LedConfig:
    def __init__(self, app):
        self.app = app
        self.serial_comm = app.serial_comm
        self.thread_run     = False 
        self.thread = threading.Thread(target=self._frame_preview_thread)


    def open_led_config(self,frame):
        """setup work frame for LED animation control"""
        self.frame = frame
        self.frame.configure(text="LED Animation Settings")
        
        self.send_frame = 'S:X0,none,0x0,0x0,0x0,0x0,"empty"#'
        self.send_frame_last = ""

        # upper frame for module selection and send button
        self.upper_frame = tk.Frame(self.frame)
        self.upper_frame.pack(side=tk.TOP, fill=tk.X, padx=5, pady=5)

        # mid frame for p1..p4 config
        self.mid_frame = tk.Frame(self.frame)
        self.mid_frame.pack(side=tk.TOP, fill=tk.BOTH, padx=5, pady=5)

        # lower frame for config str
        self.lower_frame = tk.Frame(self.frame)
        self.lower_frame.pack(side=tk.TOP, fill=tk.BOTH, padx=5, pady=5)

        # TODO: read programs from ULC-Pico (prepare PC tool compatible to next versions of ULC-Pico)
        # TODO: when programs are read form ULC,  configure progarms in correlation to Moule / index
        # TODO: read current program list from ULC - for each module here

        self._setup_selection_frame(self.upper_frame)
        self._setup_parameter_blocks(self.mid_frame)
        self._setup_string_frame(self.lower_frame)

        # start background thread to calculate and show frame
        self.thread_run = True 
        #self.thread.daemon = True  # Set thread as daemon, so it exits when the main program exits
        self.thread.start()


    def stop(self):
        """Stop the background thread by setting the stop flag"""
        self.thread_run = False
        
        if self.thread.is_alive():
            # wait until thread has been finsihed
            self.thread.join()

    def _frame_preview_thread(self):
        while self.thread_run == True:  # Check stop_flag to stop the loop
            self._calc_send_frame()
            time.sleep(0.1)  
            



    def _calc_send_frame(self):
        """Out of inputs, calculate the frame and show the frame."""
        # Convert the return values from get() to int
        led_module = self.led_module_var.get()
        index = self.index_var.get()  
        program = self.program_var.get()

        dim_cfg = 0
        color_cfg = 0
        flash_cfg = 0
        breath_cfg = 0

        str_cfg = self.cfg_str_var.get()  # This remains a string as it's not calculated

        # dimCfg
        dim_cfg |= int(self.dim_value.get()) & 0xFF              
        dim_cfg |= (int(self.dim_step.get()) & 0xFF) << 8         
        dim_cfg |= (int(self.dim_speed.get()) & 0xFFF) << 16                  
        if self.dim_wr_flag.get() == True:
            dim_cfg |= 0x80000000

        # colorCfg
        color_cfg |= int(self.color_start.get()) & 0xFF          
        color_cfg |= (int(self.color_step.get()) & 0xFF) << 8    
        color_cfg |= (int(self.color_t2.get()) & 0xFF) << 16     
        color_cfg |= (int(self.color_event.get()) & 0xFF) << 24  
        if self.color_wr_flag.get() == True:
            dim_cfg |= 0x40000000  

        # flashCfg
        flash_cfg |= int(self.flash_count.get()) & 0xFF            
        flash_cfg |= (int(self.flash_duration.get()) & 0xFF) << 8  
        flash_cfg |= (int(self.flash_pause.get()) & 0xFF) << 16    
        flash_cfg |= (int(self.flash_group_pause.get()) & 0xFF) << 24
        if self.flash_wr_flag.get() == True:
            dim_cfg |= 0x20000000

        # breathCfg
        breath_cfg |= int(self.breath_step.get()) & 0xFF           
        breath_cfg |= (int(self.breath_up_time.get()) & 0xFF) << 8 
        breath_cfg |= (int(self.breath_down_time.get()) & 0xFF) << 8 
        if self.breath_wr_flag.get() == True:
            dim_cfg |= 0x10000000

        # Format frame
        self.send_frame = f'S:{led_module}{index},{program},0x{dim_cfg:X},0x{color_cfg:X},0x{flash_cfg:X},0x{breath_cfg:X},"{str_cfg}"#'

        # Show frame
        if self.send_frame != self.send_frame_last:
            self.command_frame_out.config(text=self.send_frame)
            self.frame.update()
            self.send_frame_last = self.send_frame


    def _setup_selection_frame(self, frame):
        # selection of LED-Module (C, L, R, S, M)
        tk.Label(frame, text="Select LED Module:").pack(side=tk.LEFT, pady=5)
        led_modules = ["C", "L", "R", "S", "M"]
        self.led_module_var = StringVar(value=led_modules[0])
        led_module_menu = ttk.Combobox(frame, textvariable=self.led_module_var, values=led_modules, state="readonly" , width=3)
        led_module_menu.pack(side=tk.LEFT, pady=5)

        # Index selection (0-9)
        tk.Label(frame, text="Index (0-9):").pack(side=tk.LEFT, pady=5, padx=20)
        self.index_var = StringVar(value="0")
        index_menu = ttk.Combobox(frame, textvariable=self.index_var, values=[str(i) for i in range(10)], state="readonly" , width=3)
        index_menu.pack(side=tk.LEFT, pady=5)

        # program selection
        tk.Label(frame, text="Select Program:").pack(side=tk.LEFT, pady=5, padx=20)
        programs = [
            "off", "on", "breath", "blink", "multi flash", "rainbow",
            "off","on","breath","blink","multi flash","rainbow",
            "Off","Static","Blink","Breath","Color Wipe","Color Wipe Inverse","Color Wipe Reverse","Color Wipe Reverse Inverse","Color Wipe Random","Random Color","Single Dynamic",
            "Multi Dynamic","Rainbow","Rainbow Cycle","Scan","Dual Scan","Fade","Theater Chase","Theater Chase Rainbow","Running Lights","Twinkle","Twinkle Random","Twinkle Fade",
            "Twinkle Fade Random","Sparkle","Flash Sparkle","Hyper Sparkle","Strobe","Strobe Rainbow","Multi Strobe","Blink Rainbow","Chase White","Chase Color","Chase Random",
            "Chase Rainbow","Chase Flash","Chase Flash Random","Chase Rainbow White","Chase Blackout","Chase Blackout Rainbow","Color Sweep Random","Running Color","Running Red Blue",
            "Running Random","Larson Scanner","Comet","Fireworks","Fireworks Random","Merry Christmas","Fire Flicker","Fire Flicker (soft)","Fire Flicker (intense)","Circus Combustus",
            "Halloween","Bicolor Chase","Tricolor Chase","TwinkleFOX","Rain",
            "Custom 0","Custom 1","Custom 2","Custom 3","Custom 4","Custom 5","Custom 6","Custom 7"
        ]
        self.program_var = StringVar(value=programs[0])
        program_menu = ttk.Combobox(frame, textvariable=self.program_var, values=programs, state="readonly", width=50)
        program_menu.pack(side=tk.LEFT, pady=5)


        # Button zum Senden der Konfiguration
        tk.Button(frame, text="Send to ULC-Pico", command=self._send_led_config_to_pico).pack(side=tk.RIGHT, pady=5, padx=20)



    def _setup_parameter_blocks(self, frame):
        """setup configs for dimCfg, colorCfg, flashCfg, breathCfg"""
        ####################
        # DimCfg Block
        dim_block_frame = tk.LabelFrame(frame, text="dimCfg", padx=5, pady=5)
        dim_block_frame.pack(side=tk.LEFT, fill=tk.Y, padx=5, pady=5)

        self.dim_wr_flag = tk.BooleanVar(value=True)
        tk.Checkbutton(dim_block_frame, text="update", variable=self.dim_wr_flag).pack(side=tk.TOP, pady=5, padx=5, anchor="w")

        tk.Label(dim_block_frame, text="").pack(side=tk.TOP) 

        tk.Label(dim_block_frame, text="Dim level (0:255):").pack(side=tk.TOP, pady=5, padx=5, anchor="w") 
        self.dim_value = StringVar(value="128")
        tk.Entry(dim_block_frame, textvariable=self.dim_value).pack(side=tk.TOP, pady=5, padx=10, anchor="w")
        
        tk.Label(dim_block_frame, text="").pack(side=tk.TOP) 

        tk.Label(dim_block_frame, text="dec/off/inc (-127:0:128)").pack(side=tk.TOP, pady=5, padx=5, anchor="w")
        self.dim_step = StringVar(value="0")
        tk.Entry(dim_block_frame, textvariable=self.dim_step).pack(side=tk.TOP, pady=5, padx=10, anchor="w")

        tk.Label(dim_block_frame, text="").pack(side=tk.TOP) 

        tk.Label(dim_block_frame, text="speed NEO stripe (0:4095):").pack(side=tk.TOP, pady=5, padx=5, anchor="w")
        self.dim_speed = StringVar(value="0")
        tk.Entry(dim_block_frame, textvariable=self.dim_speed).pack(side=tk.TOP, pady=5, padx=10, anchor="w")


        ####################
        # ColorCfg Block
        color_block_frame = tk.LabelFrame(frame, text="colorCfg", padx=5, pady=5)
        color_block_frame.pack(side=tk.LEFT, fill=tk.Y, padx=5, pady=5)

        self.color_wr_flag = tk.BooleanVar(value=True)
        tk.Checkbutton(color_block_frame, text="update", variable=self.color_wr_flag).pack(side=tk.TOP, pady=5, padx=5, anchor="w") 

        tk.Label(dim_block_frame, text="").pack(side=tk.TOP) 

        tk.Label(color_block_frame, text="Start index (0:255)):").pack(side=tk.TOP, pady=5, padx=5, anchor="w") 
        self.color_start = StringVar(value="0")
        tk.Entry(color_block_frame, textvariable=self.color_start).pack(side=tk.TOP, pady=5, padx=10, anchor="w") 

        tk.Label(dim_block_frame, text="").pack(side=tk.TOP) 

        tk.Label(color_block_frame, text="dec/off/inc (-127:0:128)").pack(side=tk.TOP, pady=5, padx=5, anchor="w") 
        self.color_step = StringVar(value="0")
        tk.Entry(color_block_frame, textvariable=self.color_step).pack(side=tk.TOP, pady=5, padx=10, anchor="w") 

        tk.Label(dim_block_frame, text="").pack(side=tk.TOP) 

        tk.Label(color_block_frame, text="step time (0:255) ms:").pack(side=tk.TOP, pady=5, padx=5, anchor="w") 
        self.color_t2 = StringVar(value="0")
        tk.Entry(color_block_frame, textvariable=self.color_t2).pack(side=tk.TOP, pady=5, padx=10, anchor="w") 

        tk.Label(dim_block_frame, text="").pack(side=tk.TOP) 

        tk.Label(color_block_frame, text="event divider (1:256):").pack(side=tk.TOP, pady=5, padx=5, anchor="w") 
        self.color_event = StringVar(value="1")
        tk.Entry(color_block_frame, textvariable=self.color_event).pack(side=tk.TOP, pady=5, padx=10, anchor="w") 


        ####################
        # FlashCfg Block
        flash_block_frame = tk.LabelFrame(frame, text="flashCfg", padx=5, pady=5)
        flash_block_frame.pack(side=tk.LEFT, fill=tk.Y, padx=5, pady=5)

        self.flash_wr_flag = tk.BooleanVar(value=True)
        tk.Checkbutton(flash_block_frame, text="update", variable=self.flash_wr_flag).pack(side=tk.TOP, pady=5, padx=5, anchor="w") 

        tk.Label(flash_block_frame, text="").pack(side=tk.TOP) 

        tk.Label(flash_block_frame, text="flashes per group (0:255):").pack(side=tk.TOP, pady=5, padx=5, anchor="w") 
        self.flash_count = StringVar(value="5")
        tk.Entry(flash_block_frame, textvariable=self.flash_count).pack(side=tk.TOP, pady=5, padx=10, anchor="w") 

        tk.Label(flash_block_frame, text="").pack(side=tk.TOP) 

        tk.Label(flash_block_frame, text="t1: flash duration (0:255) 10 ms:").pack(side=tk.TOP, pady=5, padx=5, anchor="w") 
        self.flash_duration = StringVar(value="3")
        tk.Entry(flash_block_frame, textvariable=self.flash_duration).pack(side=tk.TOP, pady=5, padx=10, anchor="w") 

        tk.Label(flash_block_frame, text="").pack(side=tk.TOP) 

        tk.Label(flash_block_frame, text="t2: inter flash time (0:255) 10 ms:").pack(side=tk.TOP, pady=5, padx=5, anchor="w") 
        self.flash_pause = StringVar(value="10")
        tk.Entry(flash_block_frame, textvariable=self.flash_pause).pack(side=tk.TOP, pady=5, padx=10, anchor="w") 

        tk.Label(flash_block_frame, text="").pack(side=tk.TOP) 

        tk.Label(flash_block_frame, text="t3: inter group time (0:255) 100 ms:").pack(side=tk.TOP, pady=5, padx=5, anchor="w") 
        self.flash_group_pause = StringVar(value="30")
        tk.Entry(flash_block_frame, textvariable=self.flash_group_pause).pack(side=tk.TOP, pady=5, padx=10, anchor="w") 

        ####################
        # BreathCfg Block
        breath_block_frame = tk.LabelFrame(frame, text="breathCfg", padx=5, pady=5)
        breath_block_frame.pack(side=tk.LEFT, fill=tk.Y, padx=5, pady=5)

        self.breath_wr_flag = tk.BooleanVar(value=True)
        tk.Checkbutton(breath_block_frame, text="update", variable=self.breath_wr_flag).pack(side=tk.TOP, pady=5, padx=5, anchor="w") 

        tk.Label(breath_block_frame, text="").pack(side=tk.TOP) 

        tk.Label(breath_block_frame, text="setp (0:255)").pack(side=tk.TOP, pady=5, padx=5, anchor="w") 
        self.breath_step = StringVar(value="10")
        tk.Entry(breath_block_frame, textvariable=self.breath_step).pack(side=tk.TOP, pady=5, padx=10, anchor="w") 

        tk.Label(breath_block_frame, text="").pack(side=tk.TOP) 

        tk.Label(breath_block_frame, text="t1 up time (0:255) 100ms:").pack(side=tk.TOP, pady=5, padx=5, anchor="w") 
        self.breath_up_time = StringVar(value="30")
        tk.Entry(breath_block_frame, textvariable=self.breath_up_time).pack(side=tk.TOP, pady=5, padx=10, anchor="w") 

        tk.Label(breath_block_frame, text="").pack(side=tk.TOP) 

        tk.Label(breath_block_frame, text="t2 down time (0:255) 100ms:").pack(side=tk.TOP, pady=5, padx=5, anchor="w") 
        self.breath_down_time = StringVar(value="30")
        tk.Entry(breath_block_frame, textvariable=self.breath_down_time).pack(side=tk.TOP, pady=5, padx=10, anchor="w") 

    def _setup_string_frame(self,frame):
        # textfield for  `cfg.str`
        tk.Label(frame, text="Enter cfg.str (optional):").pack(side=tk.TOP, pady=5, padx=5, anchor="w")
        self.cfg_str_var = StringVar()
        cfg_str_entry = tk.Entry(frame, textvariable=self.cfg_str_var, width=125)
        cfg_str_entry.pack(side=tk.TOP, pady=5, padx=10, anchor="w")

        self.command_frame_out = tk.Label(frame,text=self.send_frame)
        self.command_frame_out.pack(side=tk.TOP, pady=5, padx=5, anchor="w")

    def _send_led_config_to_pico(self):
        self._calc_send_frame()
        # send to pico
        try:
            response = self.serial_comm.send(self.send_frame)
            if response and "OK-" in response:
                messagebox.showinfo("Success", "LED Animation Configuration sent to Pico.")
            else:
                messagebox.showerror("Error", "Failed to send LED configuration.")
        except Exception as e:
            messagebox.showerror("Error", f"Error sending configuration: {e}")
