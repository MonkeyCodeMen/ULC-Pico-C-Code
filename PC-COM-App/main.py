import os
current_directory = os.getcwd()
print("Aktuelles Arbeitsverzeichnis:", current_directory)
if (not current_directory.endswith('PC-COM-App')):
    os.chdir('PC-COM-App')
    current_directory = os.getcwd()
    print("Aktuelles Arbeitsverzeichnis:", current_directory)


import tkinter as tk
from gui import SerialCommandApp



if __name__ == "__main__":
    root = tk.Tk()
    app = SerialCommandApp(root)
    root.mainloop()
