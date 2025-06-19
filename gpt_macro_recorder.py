import json
import time
from datetime import datetime
import openai

openai.api_key = "sk-..."

def generate_macro(prompt):
    response = openai.ChatCompletion.create(
        model="gpt-4o",
        messages=[{"role": "user", "content": prompt}]
    )
    script = response.choices[0].message.content.strip()
    entry = {
        "prompt": prompt,
        "response": script,
        "timestamp": datetime.now().isoformat()
    }
    with open("gpt_macros.json", "a") as f:
        f.write(json.dumps(entry) + "\n")
    return script

def replay_macros():
    with open("gpt_macros.json", "r") as f:
        for line in f:
            entry = json.loads(line)
            print(f"🕰️ [{entry['timestamp']}]\nPrompt: {entry['prompt']}\nResult:\n{entry['response']}\n---\n")
