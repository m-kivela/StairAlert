import asyncio
import websockets
import winsound

print("PROGRAM START")

listen_address = "192.168.1.110"    # This computer's local IP
port = 80
alert_tone = "sounds/alert_tone.wav"

# Gets called on a WebSocket event
async def handle_connection(websocket, path):
    async for message in websocket:
        # This runs when motion is detected
        if message == "!":
            print("MOTION DETECTED")
            winsound.PlaySound(alert_tone, winsound.SND_FILENAME)
        
        # Other messages
        else:
            print(f"{message}")

start_server = websockets.serve(handle_connection, listen_address, port)

asyncio.get_event_loop().run_until_complete(start_server)
asyncio.get_event_loop().run_forever()