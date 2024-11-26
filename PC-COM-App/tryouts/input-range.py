import tkinter as tk

def validate_range():
    try:
        num = int(entry_var.get())
        if min_val <= num <= max_val:
            result_label.config(text=f"Valid Number: {num}")
        else:
            result_label.config(text=f"Out of Range! Allowed: {min_val} - {max_val}")
    except ValueError:
        result_label.config(text="Invalid input! Enter an integer.")

root = tk.Tk()
root.title("Number Input with Range")

min_val, max_val = 10, 100

tk.Label(root, text=f"Enter Number ({min_val}-{max_val}):").grid(row=0, column=0)
entry_var = tk.StringVar()
tk.Entry(root, textvariable=entry_var).grid(row=0, column=1)
tk.Button(root, text="Validate", command=validate_range).grid(row=1, column=0, columnspan=2)
result_label = tk.Label(root, text="")
result_label.grid(row=2, column=0, columnspan=2)

root.mainloop()
