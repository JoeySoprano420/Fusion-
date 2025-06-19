import shutil
from datetime import datetime

def sync_to_mobile(local_path="reinforce_log.json", mobile_folder="C:/FusionPP/mobile_sync/"):
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    dest = f"{mobile_folder}log_{timestamp}.json"
    shutil.copy(local_path, dest)
    print(f"[📲] Synced to: {dest}")

if __name__ == "__main__":
    sync_to_mobile()
