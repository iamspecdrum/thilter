import os
from PIL import Image

# 1. Define your crop area: (left, upper, right, lower) in pixels
crop_area = (0, 0, 341, 420)  # Adjust these values as needed

# 2. Define your directories
input_folder = "/Users/bernardahn/Desktop/code/thilter/blender files/thilter_ui/animation_frames"
output_folder = "/Users/bernardahn/Desktop/code/thilter/blender files/thilter_ui/image_processing/light_mode/bigknob/raw_cuts"

# Ensure the output directory exists
os.makedirs(output_folder, exist_ok=True)

# 3. Iterate through all files in the folder
valid_extensions = ('.png', '.jpg', '.jpeg', '.bmp')

for filename in os.listdir(input_folder):
    if filename.lower().endswith(valid_extensions):
        input_path = os.path.join(input_folder, filename)
        
        # 4. Open, crop, and save
        try:
            with Image.open(input_path) as img:
                cropped_img = img.crop(crop_area)
                
                # Save to the output folder
                output_path = os.path.join(output_folder, "cropped_" + filename)
                cropped_img.save(output_path)
                print(f"Successfully cropped and saved: {filename}")
                
        except Exception as e:
            print(f"Error processing {filename}: {e}")