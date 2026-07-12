import os
from PIL import Image, ImageDraw

# 1. Define your crop area: (left, upper, right, lower) in pixels
crop_area = (213, 249, 303, 339)  # Adjust these values as needed

# 2. Define your directories
input_folder = "/Users/bernardahn/Desktop/code/thilter/blender files/thilter_ui/animation_frames"
output_folder = "/Users/bernardahn/Desktop/code/thilter/blender files/thilter_ui/image_processing/light_mode/smallknob2/raw_cuts"

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
                width, height = cropped_img.size
                mask = Image.new("L", (width, height), 0)
                draw = ImageDraw.Draw(mask)
                draw.ellipse((0, 0, width, height), fill=255)
                circular_img = Image.new("RGBA", (width, height))
                circular_img.paste(cropped_img, mask=mask)
                # Save to the output folder
                output_path = os.path.join(output_folder, "cropped_" + filename)
                circular_img.save(output_path)
                print(f"Successfully cropped and saved: {filename}")
                
        except Exception as e:
            print(f"Error processing {filename}: {e}")