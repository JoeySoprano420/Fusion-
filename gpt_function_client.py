# gpt_function_client.py
import openai
import sys

openai.api_key = "sk-..."  # Use env var in real code

def generate_function(prompt):
    response = openai.ChatCompletion.create(
        model="gpt-4o",
        messages=[
            {"role": "system", "content": "You are a compiler-integrated assistant that writes .fpp scripting functions."},
            {"role": "user", "content": prompt}
        ],
        temperature=0.4
    )
    code = response.choices[0].message.content.strip()
    return code

if __name__ == "__main__":
    user_prompt = sys.argv[1]
    print(generate_function(user_prompt))
