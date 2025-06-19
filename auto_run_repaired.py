import json
import time
import subprocess
from datetime import datetime
from pathlib import Path
from typing import Dict

LOG_FILE = Path("reinforce_log.json")
REPAIRED_FILE = Path("repaired_fpp.json")

def log_result(code: str, success: bool, output: str, exec_time_ms: float) -> None:
    log_entry: Dict = {
        "timestamp": datetime.now().isoformat(),
        "code": code,
        "result": "success" if success else "fail",
        "output": output.strip(),
        "metrics": {
            "execution_time_ms": exec_time_ms
        }
    }
    LOG_FILE.parent.mkdir(parents=True, exist_ok=True)
    with LOG_FILE.open("a", encoding="utf-8") as f:
        f.write(json.dumps(log_entry) + "\n")
    print(f"[✅] Re-logged repaired script as {'success' if success else 'fail'}")

def execute_code(code: str) -> None:
    start_time = time.perf_counter()
    try:
        process = subprocess.run(
            ["python3", "-c", code],
            capture_output=True,
            text=True,
            timeout=10
        )
        end_time = time.perf_counter()
        exec_time_ms = (end_time - start_time) * 1000
        success = process.returncode == 0
        output = process.stdout if success else process.stderr
        log_result(code, success, output, exec_time_ms)
    except subprocess.SubprocessError as e:
        exec_time_ms = (time.perf_counter() - start_time) * 1000
        log_result(code, False, str(e), exec_time_ms)

def auto_run_repaired() -> None:
    if not REPAIRED_FILE.exists():
        print(f"[❌] Missing input file: {REPAIRED_FILE}")
        return

    with REPAIRED_FILE.open("r", encoding="utf-8") as f:
        repaired = json.load(f)

    for entry in repaired:
        code = entry.get("fixed", "")
        print(f"\n[🚀] Executing repaired script:\n{code}\n")
        if code.strip():
            execute_code(code)
        else:
            print("[⚠️] Skipped empty code entry.")
        time.sleep(1)

if __name__ == "__main__":
    auto_run_repaired()
