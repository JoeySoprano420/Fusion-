import json

def build_prompt_training(logfile="reinforce_log.json", outfile="prompt_fusion_training.json"):
    data = []
    with open(logfile) as f:
        for line in f:
            entry = json.loads(line)
            if entry["result"] == "success":
                prompt = f"Write a .fpp function with metrics like: {entry['metrics']}"
                data.append({
                    "messages": [
                        {"role": "user", "content": prompt},
                        {"role": "assistant", "content": entry["code"]}
                    ]
                })
    with open(outfile, "w") as out:
        json.dump(data, out, indent=2)
    print(f"[📚] Training file ready: {outfile}")

if __name__ == "__main__":
    build_prompt_training()
