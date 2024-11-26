import tkinter as tk

def show_bit_flags():
    flags = [bit_var.get() for bit_var in bit_vars]
    result = 0
    for i, val in enumerate(flags):
        if val:
            result |= (1 << i)
    result_label.config(text=f"Flags as Integer: {result} (0b{bin(result)})")

root = tk.Tk()
root.title("Bit/Flag Input")

bit_vars = [tk.IntVar() for _ in range(8)]  # For 8 bits

tk.Label(root, text="Set Flags (Bits)").grid(row=0, column=0, columnspan=8)

for i, bit_var in enumerate(bit_vars):
    tk.Checkbutton(root, text=f"Bit {i}", variable=bit_var).grid(row=1, column=i)

tk.Button(root, text="Show Flags", command=show_bit_flags).grid(row=2, column=0, columnspan=8)
result_label = tk.Label(root, text="Flags as Integer: ")
result_label.grid(row=3, column=0, columnspan=8)

root.mainloop()
