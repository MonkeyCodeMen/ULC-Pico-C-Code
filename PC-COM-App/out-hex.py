import tkinter as tk

def display_hex():
    try:
        num = int(entry_var.get())
        hex_value = hex(num).upper()
        result_label.config(text=f"Hexadecimal: {hex_value}")
    except ValueError:
        result_label.config(text="Invalid input! Enter a number.")

root = tk.Tk()
root.title("Hexadecimal Output")

tk.Label(root, text="Enter a Number:").grid(row=0, column=0)
entry_var = tk.StringVar()
tk.Entry(root, textvariable=entry_var).grid(row=0, column=1)
tk.Button(root, text="Convert to Hex", command=display_hex).grid(row=1, column=0, columnspan=2)
result_label = tk.Label(root, text="")
result_label.grid(row=2, column=0, columnspan=2)

root.mainloop()
