from PIL import Image
import os
def process_image_sequence(directory_path):
    image_files = []
    for filename in os.listdir(directory_path):
        # Filter for common image file extensions (you can extend this list)
        if filename.lower().endswith(('.png', '.jpg', '.jpeg', '.gif', '.bmp', '.tiff')):
            image_files.append(filename)

    # Sort the files to ensure correct sequence order (e.g., frame001.png, frame002.png)
    image_files.sort()
    for image_filename in image_files:
        full_path = os.path.join(directory_path, image_filename)
        print(f"Processing image: {full_path}")
    return image_files
image_paths = process_image_sequence("./")
print(image_paths)
images = [Image.open(path) for path in image_paths]
max_width = max(img.width for img in images)
resized_images = []
for img in images:
    if img.width != max_width:
        # Maintain aspect ratio while resizing to max_width
        new_height = int(img.height * (max_width / img.width))
        resized_images.append(img.resize((max_width, new_height)))
    else:
        resized_images.append(img)

total_height = sum(img.height for img in resized_images)
# Use the mode of the first image (e.g., 'RGB', 'RGBA')
strip_image = Image.new(resized_images[0].mode, (max_width, total_height))
y_offset = 0
for img in resized_images:
    strip_image.paste(img, (0, y_offset))
    y_offset += img.height

strip_image.save("imagestrip.png")