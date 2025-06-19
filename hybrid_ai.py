import openai
import json

def gpt_fallback(prompt):
    try:
        openai.api_key = "sk-..."
        response = openai.ChatCompletion.create(
            model="gpt-4",
            messages=[{"role": "user", "content": prompt}]
        )
        return response.choices[0].message.content
    except:
        return None

def offline_fallback(intent):
    with open("offline_ai_memory.json") as f:
        data = json.load(f)
        for key in data:
            if key in intent:
                return data[key]
    return "// No match found offline."

def hybrid_suggest(intent):
    prompt = f"Write a .fpp function to {intent}."
    gpt_result = gpt_fallback(prompt)
    if gpt_result:
        print("[🤖 GPT-4] Response:")
        return gpt_result
    print("[📡 Offline AI] Response:")
    return offline_fallback(intent)
