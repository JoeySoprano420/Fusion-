import json
import matplotlib.pyplot as plt
from collections import Counter

def visualize_log(file="reinforce_log.json"):
    with open(file) as f:
        results = [json.loads(line)['result'] for line in f.readlines()]
    count = Counter(results)

    plt.bar(count.keys(), count.values(), color=["green", "red"])
    plt.title("Fusion++ .fpp Execution Results")
    plt.xlabel("Result")
    plt.ylabel("Count")
    plt.grid(True)
    plt.tight_layout()
    plt.savefig("fpp_execution_stats.png")
    plt.show()

if __name__ == "__main__":
    visualize_log()
