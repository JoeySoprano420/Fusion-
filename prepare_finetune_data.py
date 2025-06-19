import json

def convert_for_openai(log_file='reinforce_log.json', out_file='fine_tune.jsonl'):
    with open(log_file) as f:
        lines = f.readlines()
    
    with open(out_file, 'w') as out:
        for line in lines:
            entry = json.loads(line)
            if entry['result'] == 'success':
                prompt = f"Write a .fpp function that runs like:\n{entry['code']}"
                completion = entry['code'].strip() + "\n"
                out.write(json.dumps({
                    "messages": [
                        {"role": "user", "content": prompt},
                        {"role": "assistant", "content": completion}
                    ]
                }) + '\n')
    print(f"[✅] Created fine-tune file: {out_file}")

if __name__ == "__main__":
    convert_for_openai()
