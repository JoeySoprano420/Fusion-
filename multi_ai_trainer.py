import json

def export_for_gpt(log_file='reinforce_log.json', out='fine_tune_gpt.jsonl'):
    with open(log_file) as f, open(out, 'w') as out_file:
        for line in f:
            e = json.loads(line)
            if e['result'] == 'success':
                j = {
                    "messages": [
                        {"role": "user", "content": f"Build a .fpp like this:\n{e['code'][:80]}"},
                        {"role": "assistant", "content": e["code"]}
                    ]
                }
                out_file.write(json.dumps(j) + "\n")

def export_for_lora(log_file='reinforce_log.json', out='lora_dataset.txt'):
    with open(log_file) as f, open(out, 'w') as out_file:
        for line in f:
            e = json.loads(line)
            if e['result'] == 'success':
                out_file.write(e['code'].strip() + "\n\n")

def export_for_tinyai(log_file='reinforce_log.json', out='tinyai_intents.json'):
    from collections import defaultdict
    db = defaultdict(str)
    with open(log_file) as f:
        for line in f:
            e = json.loads(line)
            if e['result'] == 'success':
                key = e['code'].split('\n')[0].strip()
                db[key] = e['code']
    with open(out, 'w') as f:
        json.dump(db, f, indent=2)

if __name__ == "__main__":
    export_for_gpt()
    export_for_lora()
    export_for_tinyai()
    print("[🧬] Datasets ready for GPT, LoRA, and TinyAI.")
