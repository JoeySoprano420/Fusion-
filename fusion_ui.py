import tkinter as tk
from tkinter import messagebox
import subprocess

def generate():
    intent = entry.get()
    if not intent:
        messagebox.showerror("Error", "Enter an intent.")
        return
    result = subprocess.check_output(["python3", "gpt_function_client.py", intent])
    with open("autogen.fpp", "a") as f:
        f.write("\n" + result.decode())
    output.delete("1.0", tk.END)
    output.insert(tk.END, result)
    messagebox.showinfo("Done", "Function added to autogen.fpp")

root = tk.Tk()
root.title("Fusion++ Autogen Console")

tk.Label(root, text="🧠 Describe the function you need:").pack()
entry = tk.Entry(root, width=80)
entry.pack()

tk.Button(root, text="✨ Generate & Save", command=generate).pack(pady=10)

output = tk.Text(root, height=12, width=80)
output.pack()

root.mainloop()
