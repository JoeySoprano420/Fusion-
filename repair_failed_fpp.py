import openai, json

openai.api_key = "sk-..."  # Replace with your key

def repair_failed(log_file='reinforce_log.json', output_file='repaired_fpp.json'):
    repairs = []
    with open(log_file) as f:
        for line in f:
            entry = json.loads(line)
            if entry['result'] == 'fail':
                prompt = f"The following .fpp function failed. Please correct it:\n{entry['code']}"
                try:
                    response = openai.ChatCompletion.create(
                        model="gpt-4",
                        messages=[{"role": "user", "content": prompt}]
                    )
                    fixed_code = response['choices'][0]['message']['content']
                    repairs.append({"original": entry['code'], "fixed": fixed_code})
                except Exception as e:
                    print("Error:", e)

    with open(output_file, 'w') as out:
        json.dump(repairs, out, indent=2)
    print(f"[🛠] Repaired code saved to: {output_file}")

if __name__ == "__main__":
    repair_failed()
