from PIL import Image

# 1. Open the background and foreground images
# Ensure both are in "RGBA" mode to support transparency
background = Image.open("background.png").convert("RGBA")
bigknob = Image.open("bigknob.png").convert("RGBA")
smallknob1 = Image.open("smallknob1.png").convert("RGBA")
smallknob2 = Image.open("smallknob2.png").convert("RGBA")
button = Image.open("button.png").convert("RGBA")

# 2. Define coordinates for the top-left corner position (X, Y)
# Setting (0, 0) places it right in the top-left corner
position1 = (0, 0) 
#position1= (74,32)
position2 = (38,250)
position3 = (212,250)
position4 = (150,361)  

# 3. Paste the foreground using itself as the mask
background.paste(bigknob, position1, bigknob)
background.paste(smallknob1, position2, smallknob1)
background.paste(smallknob2, position3, smallknob2)
background.paste(button, position4, button)

# 4. Save the combined image
background.save("result.png")