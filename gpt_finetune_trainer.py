import json

def convert_to_training_data():
    output = []
    with open("reinforce_log.json") as f:
        for line in f:
            entry = json.loads(line)
            prompt = f"Write a .fpp function similar to one that succeeded with metrics: {entry['metrics']}"
            completion = entry['code']
            output.append({"messages": [
                {"role": "user", "content": prompt},
                {"role": "assistant", "content": completion}
            ]})

    with open("gpt4_finetune_data.json", "w") as out:
        json.dump(output, out, indent=2)

if __name__ == "__main__":
    convert_to_training_data()

import json

def convert_to_training_data(input_path='reinforce_log.json', output_path='gpt4_finetune_data.json'):
    output = []
    with open(input_path, 'r') as f:
        for line in f:
            entry = json.loads(line)
            prompt = (f"Function metrics: {entry['metrics']}. "
                      f"Write a similar .fpp function that replicates the success.")
            completion = entry['code']
            output.append({"messages": [
                {"role": "user", "content": prompt},
                {"role": "assistant", "content": completion}
            ]})
    with open(output_path, 'w') as out:
        json.dump(output, out, indent=2)
    print(f"[🎯] Training data saved to {output_path}")

if __name__ == "__main__":
    convert_to_training_data()
