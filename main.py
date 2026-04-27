import speech_recognition as sr
import serial
import os

try:
    from serial.tools import list_ports
except ImportError:
    list_ports = None

SERIAL_PORT = os.getenv("SERIAL_PORT", "COM3")
BAUD_RATE = 9600


def open_serial_connection():
    if not hasattr(serial, "Serial") or not hasattr(serial, "SerialException"):
        print("Invalid 'serial' package detected. Install pyserial with: pip install pyserial")
        return None

    available_ports = []
    if list_ports is not None:
        available_ports = sorted(port.device for port in list_ports.comports())

    if available_ports:
        print(f"Detected serial ports: {', '.join(available_ports)}")
    else:
        print("No serial ports detected.")

    try:
        print(f"Attempting serial connection on {SERIAL_PORT} @ {BAUD_RATE} baud...")
        arduino = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        print(f"Connected to Arduino on {SERIAL_PORT} @ {BAUD_RATE} baud")
        return arduino
    except serial.SerialException as e:
        print(f"Could not open serial port {SERIAL_PORT}: {e}")
        if "Access is denied" in str(e):
            print("The port is currently in use by another app.")
            print("Close Arduino Serial Monitor/Plotter, VS Code serial terminals, and any Python process using this port.")
        if available_ports and SERIAL_PORT not in available_ports:
            print(f"'{SERIAL_PORT}' is not currently available.")
            print("Set the correct port in PowerShell, then run again:")
            print("$env:SERIAL_PORT = 'COMx'")
        elif not available_ports:
            print("Reconnect Arduino and verify the USB cable/driver, then retry.")
        return None

def listen_for_commands():
    # Initialize the recognizer
    recognizer = sr.Recognizer()
    arduino = open_serial_connection()

    if arduino is None:
        return
    
    # Use the default laptop microphone as the audio source
    try:
        with sr.Microphone() as source:
            print("Adjusting for background noise... Please wait.")
            # This helps the mic ignore background hums like fans or AC
            recognizer.adjust_for_ambient_noise(source, duration=1)

            print("\nReady! Listening for commands...")
            print("Say 'open' or 'close' (Press Ctrl+C to stop)")
            print("-" * 40)

            # Start an infinite loop to continuously listen
            while True:
                try:
                    # Listen for audio.
                    # timeout: wait 10 seconds for someone to speak.
                    # phrase_time_limit: stop recording after 3 seconds of speaking.
                    audio = recognizer.listen(source, timeout=10, phrase_time_limit=3)

                    # Use Google's free speech recognition to convert audio to text
                    text = recognizer.recognize_google(audio).lower()

                    # Send commands to Arduino over serial
                    if "close" in text:
                        arduino.write(b"CLOSE\n")
                        print(f"I heard: '{text}' -> Output: CLOSE")
                    elif "open" in text:
                        arduino.write(b"OPEN\n")
                        print(f"I heard: '{text}' -> Output: OPEN")
                    else:
                        # Optional: Print what it heard even if it wasn't a command
                        print(f"I heard: '{text}' (No command detected)")

                # Error handling so the script doesn't crash
                except sr.WaitTimeoutError:
                    # It was quiet for 10 seconds, just loop back and listen again
                    pass
                except sr.UnknownValueError:
                    # It heard noise, but couldn't understand the words
                    pass
                except sr.RequestError as e:
                    print(f"Internet connection error: {e}")
                except serial.SerialException as e:
                    print(f"Serial write error: {e}")
                    break
    except KeyboardInterrupt:
        print("\nStopped by user.")
    finally:
        arduino.close()
        print("Serial connection closed.")

if __name__ == "__main__":
    listen_for_commands()