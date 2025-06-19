import tkinter as tk
from hybrid_ai import hybrid_suggest

class AIEditorApp:
    def __init__(self, master):
        self.master = master
        master.title("Fusion++ AI Intent Manager")

        self.input = tk.Text(master, height=5, width=60)
        self.input.pack()

        self.generate_btn = tk.Button(master, text="🧠 Generate", command=self.generate)
        self.generate_btn.pack()

        self.output = tk.Text(master, height=20, width=80)
        self.output.pack()

    def generate(self):
        intent = self.input.get("1.0", tk.END).strip()
        suggestions = []
        for line in intent.split("\n"):
            if not line.strip(): continue
            print(f"[Intent]: {line}")
            code = hybrid_suggest(line.strip())
            suggestions.append(f"// Intent: {line}\n{code}\n")
        self.output.delete("1.0", tk.END)
        self.output.insert(tk.END, "\n---\n".join(suggestions))

root = tk.Tk()
app = AIEditorApp(root)
root.mainloop()
