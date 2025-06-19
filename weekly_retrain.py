import json
from datetime import datetime

def rebuild_finetune_jsonl(log_file='reinforce_log.json', out_file='fine_tune.jsonl'):
    examples = []
    with open(log_file) as f:
        for line in f:
            entry = json.loads(line)
            if entry["result"] == "success":
                prompt = f"Write a .fpp function that:\n{entry['code'][:80]}..."
                completion = entry["code"].strip()
                examples.append({
                    "messages": [
                        {"role": "user", "content": prompt},
                        {"role": "assistant", "content": completion}
                    ]
                })
    with open(out_file, "w") as out:
        for item in examples:
            out.write(json.dumps(item) + "\n")
    print(f"[🧠] Fine-tune dataset written to {out_file} ({len(examples)} entries)")

if __name__ == "__main__":
    print(f"[⏱] Weekly Retrain @ {datetime.now().isoformat()}")
    rebuild_finetune_jsonl()
