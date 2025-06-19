import firebase_admin
from firebase_admin import credentials, db
import json

cred = credentials.Certificate("firebase-key.json")
firebase_admin.initialize_app(cred, {
    'databaseURL': 'https://your-project.firebaseio.com/'
})

def push_log_to_firebase(log_file='reinforce_log.json'):
    with open(log_file) as f:
        lines = f.readlines()
    ref = db.reference("/fusionpp/logs")
    for l in lines:
        entry = json.loads(l)
        ref.push(entry)
    print("[📲] Logs pushed to Firebase!")

if __name__ == "__main__":
    push_log_to_firebase()
