import tkinter as tk, asyncio, websockets, threading
from subprocess import check_output

uri = "ws://localhost:9876"

def start_client(text_widget):
    async def run():
        async with websockets.connect(uri) as websocket:
            async def recv():
                while True:
                    msg = await websocket.recv()
                    text_widget.insert(tk.END, f"\n🔁 Update from teammate:\n{msg}\n")

            recv_task = asyncio.create_task(recv())

            def send_edit():
                content = text_widget.get("1.0", tk.END).strip()
                asyncio.create_task(websocket.send(content))

            send_btn.config(command=send_edit)
            await recv_task

    threading.Thread(target=lambda: asyncio.run(run())).start()

def gpt_generate():
    intent = entry.get()
    result = check_output(["python3", "gpt_function_client.py", intent])
    text.insert(tk.END, "\n" + result.decode())

root = tk.Tk()
root.title("Fusion++ Multiplayer Co-Edit")

entry = tk.Entry(root, width=60)
entry.pack()

tk.Button(root, text="🧠 Generate with GPT", command=gpt_generate).pack()
send_btn = tk.Button(root, text="📡 Sync to Others")
send_btn.pack()

text = tk.Text(root, height=30, width=100)
text.pack()

start_client(text)
root.mainloop()
