import json, time
from datetime import datetime

def log_feedback(fpp_code, result, metrics):
    log = {
        "timestamp": datetime.now().isoformat(),
        "code": fpp_code,
        "result": result,
        "metrics": metrics
    }
    with open("reinforce_log.json", "a") as f:
        f.write(json.dumps(log) + "\n")

# EXAMPLE USAGE
if __name__ == "__main__":
    fpp_code = """
func attack_enemy
    let damage = 15
    print "Enemy hit!"
endfunc
"""
    result = "success"
    metrics = {"execution_time_ms": 52, "state_changes": 2}
    log_feedback(fpp_code, result, metrics)
