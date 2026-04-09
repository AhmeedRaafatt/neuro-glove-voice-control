import speech_recognition as sr

def listen_for_commands():
    # Initialize the recognizer
    recognizer = sr.Recognizer()
    
    # Use the default laptop microphone as the audio source
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
                # timeout: wait 5 seconds for someone to speak. 
                # phrase_time_limit: stop recording after 3 seconds of speaking.
                audio = recognizer.listen(source, timeout=5, phrase_time_limit=3)
                
                # Use Google's free speech recognition to convert audio to text
                text = recognizer.recognize_google(audio).lower()
                
                # Check if our specific keywords are in the transcribed text
                if "close" in text:
                    print(f"I heard: '{text}' -> Output: CLOSE ✊")
                elif "open" in text:
                    print(f"I heard: '{text}' -> Output: OPEN 🖐️")
                else:
                    # Optional: Print what it heard even if it wasn't a command
                    print(f"I heard: '{text}' (No command detected)")
                    
            # Error handling so the script doesn't crash
            except sr.WaitTimeoutError:
                # It was quiet for 5 seconds, just loop back and listen again
                pass 
            except sr.UnknownValueError:
                # It heard noise, but couldn't understand the words
                pass
            except sr.RequestError as e:
                print(f"Internet connection error: {e}")

if __name__ == "__main__":
    listen_for_commands()