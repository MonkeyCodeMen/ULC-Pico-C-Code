import tkinter as tk




def update_number():
    try:
        num = int(entry_var.get(), 0)  # Accepts both HEX (0x) and decimal
        result_label.config(text=f"Decimal: {num}, Hex: {hex(num).upper()}")
    except ValueError:
        result_label.config(text="Invalid input! Use 0x for HEX.")

root = tk.Tk()
root.title("HEX and Decimal Input")

tk.Label(root, text="Enter Number (Decimal or HEX with 0x):").grid(row=0, column=0)
entry_var = tk.StringVar()
tk.Entry(root, textvariable=entry_var).grid(row=0, column=1)
tk.Button(root, text="Convert", command=update_number).grid(row=1, column=0, columnspan=2)
result_label = tk.Label(root, text="Decimal: , Hex: ")
result_label.grid(row=2, column=0, columnspan=2)

root.mainloop()

