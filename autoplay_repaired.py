import json
import time
from fpp_executor import run_fpp

def autoplay_repaired(file="repaired_fpp.json"):
    with open(file) as f:
        scripts = json.load(f)
    for entry in scripts:
        print(f"\n[🎮] Playing repaired .fpp:\n{entry['fixed']}\n")
        result = run_fpp(entry["fixed"])
        print("[🎯] Result:", "Success" if result == 0 else "Failed")
        time.sleep(1)

if __name__ == "__main__":
    autoplay_repaired()
