from fastapi import FastAPI
import uvicorn
import os
import subprocess

"""
Script to launch media files using FastAPI and VLC.
Be sure to have VLC in your path/environment variables to use it.

This script provides an API to control media playback using VLC media player. 
It allows playing specific files, stopping playback, and handling various 
remote control commands such as volume control, track navigation, and more.

The script maps media files in a specified directory to sequential numbers 
and allows playing them via API endpoints. It also integrates with an ESP32 
microcontroller to display the currently playing track on an OLED screen.

Endpoints:
- /play/{track_number}: Play a specific track by its number.
- /power: Stop playback (power off).
- /menu: Handle menu button press.
- /test: Handle test button press.
- /volume/up: Increase volume.
- /volume/down: Decrease volume.
- /back: Handle back button press.
- /previous: Play the previous track.
- /next: Play the next track.
- /c: Handle C button press.

Global Variables:
- MEDIA_DIR: Directory containing media files.
- vlc_process: Stores the current VLC process.

Dependencies:
- FastAPI: For creating the API.
- uvicorn: For running the API server.
- subprocess: For launching VLC.
- os: For file and directory operations.
"""

app = FastAPI()
MEDIA_DIR = "Your_Track_Directory_Path"   # Replace with the path of your directory containing media files
vlc_process = None

def get_media_files():
    """Retrieve all media and video files in the directory and map them to sequential numbers."""
    media_extensions = ('.mp3', '.wav', '.flac', '.ogg', '.aac', '.m4a', 
                        '.mp4', '.avi', '.mkv', '.mov', '.webm')
    media_files = [f for f in os.listdir(MEDIA_DIR) if f.lower().endswith(media_extensions)]
    media_files.sort()
    return {i + 1: os.path.join(MEDIA_DIR, file) for i, file in enumerate(media_files)}

def stop_vlc():
    """Stop the current VLC process."""
    global vlc_process
    if vlc_process:
        try:
            vlc_process.terminate()
            vlc_process.wait(timeout=5)
        except subprocess.TimeoutExpired:
            vlc_process.kill()
        vlc_process = None

@app.post("/play/{track_number}")
async def play_track(track_number: int):
    """Play a specific media file by its number."""
    global vlc_process
    print(f"Playing media {track_number}")
    media_files = get_media_files()
    if track_number not in media_files:
        return {"message": "File not found", "file_name": None}
    file_path = media_files[track_number]
    stop_vlc()
    vlc_process = subprocess.Popen(["vlc", file_path])
    return {"message": f"Playing media {track_number}", "file_name": os.path.basename(file_path)}

@app.post("/power")
async def power():
    """Log a power button press."""
    global vlc_process
    print("Power button pressed")
    stop_vlc()
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
    return {"message": "Volume increased"}

@app.post("/volume/down")
async def volume_down():
    """Log a volume down button press."""
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