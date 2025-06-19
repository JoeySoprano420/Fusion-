from flask import Flask, request
import subprocess

app = Flask(__name__)

@app.route("/gpt")
def gpt_gen():
    prompt = request.args.get("prompt")
    output = subprocess.check_output(["python3", "gpt_function_client.py", prompt])
    return output

@app.route("/push", methods=["POST"])
def push_script():
    code = request.data.decode()
    with open("autogen.fpp", "w") as f:
        f.write(code)
    return "Script received"

app.run(host="0.0.0.0", port=8000)
