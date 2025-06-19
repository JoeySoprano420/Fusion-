import json
import openai
import time
from fpp_executor import run_fpp

openai.api_key = "sk-..."  # Secure this in prod

def fix_code(code):
    prompt = f"This .fpp code failed. Fix it:\n\n{code}"
    res = openai.ChatCompletion.create(
        model="gpt-4",
        messages=[{"role": "user", "content": prompt}]
    )
    return res['choices'][0]['message']['content']

def loop_until_success(code, max_tries=5):
    for attempt in range(max_tries):
        print(f"\n[🔁] Attempt #{attempt + 1}...")
        result = run_fpp(code)
        if result == 0:
            print("[✅] Success!")
            return code
        code = fix_code(code)
        time.sleep(1)
    print("[❌] Max attempts reached. Still failed.")
    return None

def run_loop_on_failed_log(log_file='reinforce_log.json'):
    with open(log_file) as f:
        failed = [json.loads(l)['code'] for l in f if '"result": "fail"' in l]
    for fcode in failed:
        repaired = loop_until_success(fcode)
        if repaired:
            with open("self_healed.json", "a") as log:
                json.dump({"original": fcode, "fixed": repaired}, log)
                log.write("\n")

if __name__ == "__main__":
    run_loop_on_failed_log()
