import speech_recognition as sr     # Speech recognition library
from bleak import BleakClient       # BLE communication library
import asyncio                      # Asynchronous programming library

ADDRESS = "D4:D4:DA:4F:34:E6"                                   # Arduino's BLE address
CHARACTERISTIC_UUID = "88888888-4444-4444-4444-121212121212"    # Arduino's BLE characteristic

recogniser = sr.Recognizer()
mic = sr.Microphone(device_index=2)
recogniser.energy_threshold = 100

# Function to send command via Bluetooth from RPi to Arduino
async def sendVoiceCommand(command):
    async with BleakClient(ADDRESS) as client:                                  # Establish BLE connection with Arduino
        await client.write_gatt_char(CHARACTERISTIC_UUID, command.encode())     # Send command to BLE characteristic only once connection is established

        print("Command sent:", command)

# Function to capture audio input through microphone and translate to text
def listenForVoice():
    with mic as source:
        print("\nListening...")

        audio = recogniser.listen(source)               # Record spoken audio from microphone

        command = recogniser.recognize_google(audio)    # Convert recorded audio into text using Google speech recognition

        return command.lower()                          # Convert text to lowercase for parsing

while True:
    try:
        spokenPhrase = listenForVoice()
        print("User said:", spokenPhrase)

        if "bathroom" in spokenPhrase:                      # If spoken command "bathroom" found when parsing text, send BATHROOM_ON command to Arduino
            asyncio.run(sendVoiceCommand("BATHROOM_ON"))
        elif "hallway" in spokenPhrase:                     # If spoken command "hallway" found when parsing text, send HALLWAY_ON command to Arduino
            asyncio.run(sendVoiceCommand("HALLWAY_ON"))
        elif "exhaust" in spokenPhrase:                     # If spoken command "exhaust" found when parsing text, send FAN_ON command to Arduino
            asyncio.run(sendVoiceCommand("FAN_ON"))
        else:                                               # If spoken phrase does not match any valid command
            print("No valid command detected")           
        
    except Exception as e:
        print("Error:")
        print(e)