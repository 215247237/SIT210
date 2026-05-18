import speech_recognition as sr
from bleak import BleakClient
import asyncio

ADDRESS = "D4:D4:DA:4F:34:E6"
CHARACTERISTIC_UUID = "88888888-4444-4444-4444-121212121212"

recogniser = sr.Recognizer()
mic = sr.Microphone(device_index=2)
recogniser.energy_threshold = 100

async def sendVoiceCommand(command):
    async with BleakClient(ADDRESS) as client:
        await client.write_gatt_char(CHARACTERISTIC_UUID, command.encode())

        print("Command sent:", command)

def listenForVoice():
        with mic as source:
            print("\nListening...")

            audio = recogniser.listen(source)

            command = recogniser.recognize_google(audio)

            return command.lower()

while True:
    try:
        spokenPhrase = listenForVoice()
        print("User said:", spokenPhrase)

        if "bathroom" in spokenPhrase:
            asyncio.run(sendVoiceCommand("BATHROOM_ON"))
        elif "hallway" in spokenPhrase:
            asyncio.run(sendVoiceCommand("HALLWAY_ON"))
        elif "exhaust" in spokenPhrase:
            asyncio.run(sendVoiceCommand("FAN_ON"))
        else:
            print("No valid command detected")
        
    except Exception as e:
        print("Error:")
        print(e)