import os

def push_to_git():
    os.system("git add reinforce_log.json")
    os.system("git commit -m \"🧠 Updated Fusion++ logs\"")
    os.system("git push")

if __name__ == "__main__":
    push_to_git()
