from fastapi import FastAPI
import uvicorn

"""
Basic FastAPI Server for ESP32 Remote Control

This script provides a simple API to log button presses from an ESP32 remote control.
Each button press sends a specific API call (e.g., /play/1, /power, /volume/up), and the server logs the event.

Endpoints:
- /play/{track_number}: Log a play request for a specific track.
- /power: Log a power button press.
- /menu: Log a menu button press.
- /test: Log a test button press.
- /volume/up: Log a volume up button press.
- /volume/down: Log a volume down button press.
- /back: Log a back button press.
- /previous: Log a previous button press.
- /next: Log a next button press.
- /c: Log a C button press.

This is a template script and does not perform any actions other than logging.
"""

app = FastAPI()

@app.post("/play/{track_number}")
async def play_track(track_number: int):
    """Log a play request for a specific track."""
    print(f"Play button pressed for track {track_number}")
    return {"message": f"Playing track {track_number}"}

@app.post("/play")
async def play():
    """Log a play button press."""
    print("Play button pressed")
    return {"message": "Play button pressed"}

@app.post("/power")
async def power():
    """Log a power button press."""
    print("Power button pressed")
    return {"message": "Power button pressed"}

@app.post("/menu")
async def menu():
    """Log a menu button press."""
    print("Menu button pressed")
    return {"message": "Menu button pressed"}

@app.post("/test")
async def test():
    """Log a test button press."""
    print("Test button pressed")
    return {"message": "Test button pressed"}

@app.post("/volume/up")
async def volume_up():
    """Log a volume up button press."""
    print("Volume up button pressed")
    return {"message": "Volume increased"}

@app.post("/volume/down")
async def volume_down():
    """Log a volume down button press."""
    print("Volume down button pressed")
    return {"message": "Volume decreased"}

@app.post("/back")
async def back():
    """Log a back button press."""
    print("Back button pressed")
    return {"message": "Back button pressed"}

@app.post("/previous")
async def previous():
    """Log a previous button press."""
    print("Previous button pressed")
    return {"message": "Previous button pressed"}

@app.post("/next")
async def next():
    """Log a next button press."""
    print("Next button pressed")
    return {"message": "Next button pressed"}

@app.post("/c")
async def c_button():
    """Log a C button press."""
    print("C button pressed")
    return {"message": "C button pressed"}

if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=54321)