import asyncio
import websockets
import json

clients = set()

async def handler(websocket, path):
    clients.add(websocket)
    try:
        async for message in websocket:
            for client in clients:
                if client != websocket:
                    await client.send(message)
    finally:
        clients.remove(websocket)

start_server = websockets.serve(handler, "0.0.0.0", 9876)
asyncio.get_event_loop().run_until_complete(start_server)
print("Fusion++ Collaboration Server Online 🚀")
asyncio.get_event_loop().run_forever()
