from PIL import Image
import struct
import os

OUTPUT_DIR = "Assets/Track"

os.makedirs(OUTPUT_DIR, exist_ok=True)

img = Image.open("Assets/Track/FullTrack.png").convert("RGB")
width, height = img.size
pixels = img.load()

def color_to_tile(r, g, b):

	if (r, g, b) == (41, 255, 0):
		return 1 # grass
	
	if (r, g, b) == (96, 96, 96):
		return 2 # road
	
	if (r, g, b) == (255, 0, 0):
		return 3 # hairpin
	
	if (r, g, b) == (255, 255, 255):
		return 3 # hairpin
	
	if (r, g, b) == (0, 0, 0):
		return 4 # finishLine
	
	return 0 # default

with open("Assets/Track/encodedTrack.ck", "wb") as f:
	for y in range(height):
		for x in range(width):
			r, g, b = pixels[x, y]
			tile = color_to_tile(r, g, b)
			f.write(struct.pack("B", tile))
