import os
import sys

# Get the command line arguments
args = sys.argv
# Texture Folder
texture_folder = args[1]
# Lossy Compression Settings
lossy_speed = "4"             # speed/quality trade-off. 1=slow, 4=default, 11=fast & rough
lossy_quality = "65-80"       # don't save below min, use fewer colors below max (0-100)
lossy_force_overwrite = False # True or False
lossy_create_BMP = False      # Converts Back to BMP: True or False
# Lossless Compression Settings
lossless_level = "2"             # [0-7] 0 very fast, 2 default, 5 slow, 7 very slow
lossless_force_overwrite = False # True or False

# Since pngquant only supports PNG files and we've been using some BMP
# First I'll be converting BMP files to PNG before the Lossy Compression
#   __|                           |      _ )   \  |  _ \    |            _ \  \ |   __| 
#  (      _ \    \ \ \ /  -_)   _| _|    _ \  |\/ |  __/     _|   _ \    __/ .  |  (_ | 
# \___| \___/ _| _| \_/ \___| _| \__|   ___/ _|  _| _|     \__| \___/   _|  _|\_| \___| 
#                                                                                       
# Looks up for BMP files and convert them to PNG
for bmp_file in os.listdir(texture_folder):
    # Checks if it is a BMP file
    if bmp_file.endswith(".bmp"):
        # Grabs file name without the BMP extension
        filename = os.path.splitext(bmp_file)[0]
        # Concatenate the folder path and PNG extension
        # And checks if the converted file already exist
        png_file = os.path.join(texture_folder, filename + ".png")
        # If not convert it using Image Magick
        if not os.path.isfile(png_file):
            os.system(f"magick convert {os.path.join(texture_folder, bmp_file)} {png_file}")

#  |                              __|                                      _)             
#  |      _ \ (_-< (_-<  |  |    (      _ \   ` \   _ \   _| -_) (_-< (_-<  |   _ \    \  
# ____| \___/ ___/ ___/ \_, |   \___| \___/ _|_|_| .__/ _| \___| ___/ ___/ _| \___/ _| _| 
#                       ___/                      _|                                      
# Looks up for PNG files and apply Lossy Compression using pngquant
for png_file in os.listdir(texture_folder):
    # Checks if it is a PNG file
    if png_file.endswith(".png"):
        # Grabs file name without the PNG extension
        filename = os.path.splitext(png_file)[0]
        # Concatenate the folder path and PNG extension
        # And checks if the converted file already exist in /lossy folder
        lossy_png_file = os.path.join(texture_folder, "lossy", f"{filename}.png")
        # If not run pngquant to convert the PNG file to lossy PNG
        # Exception is if you choose to Overwrite so it will run on every PNG regardless
        if not os.path.isfile(lossy_png_file) or lossy_force_overwrite:
            # Prepares the pngquant command
            command = f"pngquant.exe --speed {lossy_speed} --quality={lossy_quality} {os.path.join(texture_folder, png_file)} --output {lossy_png_file}"
            # Adds the --force flag if lossy_force_overwrite is True
            if lossy_force_overwrite:
                command += " --force"
            # Converts PNG to BMP if needed
            if lossy_create_BMP:
                os.system(f'magick convert {lossy_png_file} {os.path.join(texture_folder, "lossy", f"{filename}.bmp")}')
            # Run the pngquant command
            os.system(command)
            
#  |                     |                     __|                                      _)             
#  |      _ \ (_-< (_-<  |   -_) (_-< (_-<    (      _ \   ` \   _ \   _| -_) (_-< (_-<  |   _ \    \  
# ____| \___/ ___/ ___/ _| \___| ___/ ___/   \___| \___/ _|_|_| .__/ _| \___| ___/ ___/ _| \___/ _| _| 
#                                                              _|                                      
# Looks up for texture files and apply Lossless Compression using optipng
for texture_file in os.listdir(texture_folder):
    # Checks if it is a PNG file
    if texture_file.endswith(".png"):
        # Prepares the check if lossless file already exist
        lossless_file = os.path.join(texture_folder, "lossless", texture_file)
        if not os.path.isfile(lossless_file) or lossless_force_overwrite:
            # Prepares the optipng command
            command = f"optipng.exe -o {lossless_level} {os.path.join(texture_folder, texture_file)} -out {lossless_file}"
            # Adds the -force flag if lossless_force_overwrite is True
            if lossless_force_overwrite:
                command += " -force"
            # Run the optipng command
            os.system(command)
        
#  _ \  _ \    __|     __|                                 _)             
#  |  | |  | \__ \    (      _ \    \ \ \ /  -_)   _| (_-<  |   _ \    \  
# ___/ ___/  ____/   \___| \___/ _| _| \_/ \___| _|   ___/ _| \___/ _| _| 
#                                                                         
# Looks up for PNG files and apply DDS Conversion using PVRTexToolCLI
for png_file in os.listdir(texture_folder):
    # Checks if it is a PNG file
    if png_file.endswith(".png"):
        # Grabs file name without the PNG extension
        filename = os.path.splitext(png_file)[0]
        # Concatenate the folder path and DDS extension
        # And checks if the converted file already exist in the same folder
        dds_file = os.path.join(texture_folder, f"{filename}.dds")
        if not os.path.isfile(dds_file):
            os.system(f"PVRTexToolCLI -i {os.path.join(texture_folder, png_file)} -o {dds_file} -f BC3")
# Does the same for Lossy Folder
lossy_folder = os.path.join(texture_folder, f"lossy")
for png_file in os.listdir(lossy_folder):
    # Checks if it is a PNG file
    if png_file.endswith(".png"):
        # Grabs file name without the PNG extension
        filename = os.path.splitext(png_file)[0]
        # Concatenate the folder path and DDS extension
        # And checks if the converted file already exist in the same folder
        dds_file = os.path.join(lossy_folder, f"{filename}.dds")
        if not os.path.isfile(dds_file):
            os.system(f"PVRTexToolCLI -i {os.path.join(lossy_folder, png_file)} -o {dds_file} -f BC3")
# Does the same for Lossless Folder
lossless_folder = os.path.join(texture_folder, f"lossless")
for png_file in os.listdir(lossless_folder):
    # Checks if it is a PNG file
    if png_file.endswith(".png"):
        # Grabs file name without the PNG extension
        filename = os.path.splitext(png_file)[0]
        # Concatenate the folder path and DDS extension
        # And checks if the converted file already exist in the same folder
        dds_file = os.path.join(lossless_folder, f"{filename}.dds")
        if not os.path.isfile(dds_file):
            os.system(f"PVRTexToolCLI -i {os.path.join(lossless_folder, png_file)} -o {dds_file} -f BC3")
            
#  _ \ \ \   / _ \     __|                                 _)             
#  __/  \ \ /    /    (      _ \    \ \ \ /  -_)   _| (_-<  |   _ \    \  
# _|     \_/  _|_\   \___| \___/ _| _| \_/ \___| _|   ___/ _| \___/ _| _| 
#                                                                         
# Looks up for PNG files and apply PVR Conversion using PVRTexToolCLI
for png_file in os.listdir(texture_folder):
    # Checks if it is a PNG file
    if png_file.endswith(".png"):
        # Grabs file name without the PNG extension
        filename = os.path.splitext(png_file)[0]
        # Concatenate the folder path and PVR extension
        # And checks if the converted file already exist in the same folder
        pvr_file = os.path.join(texture_folder, f"{filename}.pvr")
        if not os.path.isfile(pvr_file):
            os.system(f"PVRTexToolCLI -i {os.path.join(texture_folder, png_file)} -o {pvr_file} -q pvrtcfast")
# Does the same for Lossy Folder
lossy_folder = os.path.join(texture_folder, f"lossy")
for png_file in os.listdir(lossy_folder):
    # Checks if it is a PNG file
    if png_file.endswith(".png"):
        # Grabs file name without the PNG extension
        filename = os.path.splitext(png_file)[0]
        # Concatenate the folder path and PVR extension
        # And checks if the converted file already exist in the same folder
        pvr_file = os.path.join(lossy_folder, f"{filename}.pvr")
        if not os.path.isfile(pvr_file):
            os.system(f"PVRTexToolCLI -i {os.path.join(lossy_folder, png_file)} -o {pvr_file} -q pvrtcfast")
# Does the same for Lossless Folder
lossless_folder = os.path.join(texture_folder, f"lossless")
for png_file in os.listdir(lossless_folder):
    # Checks if it is a PNG file
    if png_file.endswith(".png"):
        # Grabs file name without the PNG extension
        filename = os.path.splitext(png_file)[0]
        # Concatenate the folder path and PVR extension
        # And checks if the converted file already exist in the same folder
        pvr_file = os.path.join(lossless_folder, f"{filename}.pvr")
        if not os.path.isfile(pvr_file):
            os.system(f"PVRTexToolCLI -i {os.path.join(lossless_folder, png_file)} -o {pvr_file} -q pvrtcfast")
            
#  |  / __ __| \ \  /     __|                                 _)             
#  . <     |    >  <     (      _ \    \ \ \ /  -_)   _| (_-<  |   _ \    \  
# _|\_\   _|    _/\_\   \___| \___/ _| _| \_/ \___| _|   ___/ _| \___/ _| _| 
#                                                                            
# Looks up for PNG files and apply KTX Conversion using PVRTexToolCLI
for png_file in os.listdir(texture_folder):
    # Checks if it is a PNG file
    if png_file.endswith(".png"):
        # Grabs file name without the PNG extension
        filename = os.path.splitext(png_file)[0]
        # Concatenate the folder path and KTX extension
        # And checks if the converted file already exist in the same folder
        ktx_file = os.path.join(texture_folder, f"{filename}.ktx")
        if not os.path.isfile(ktx_file):
            os.system(f"PVRTexToolCLI -i {os.path.join(texture_folder, png_file)} -o {ktx_file} -f ETC2_rgb -q etcfast")
# Does the same for Lossy Folder
lossy_folder = os.path.join(texture_folder, f"lossy")
for png_file in os.listdir(lossy_folder):
    # Checks if it is a PNG file
    if png_file.endswith(".png"):
        # Grabs file name without the PNG extension
        filename = os.path.splitext(png_file)[0]
        # Concatenate the folder path and KTX extension
        # And checks if the converted file already exist in the same folder
        ktx_file = os.path.join(lossy_folder, f"{filename}.ktx")
        if not os.path.isfile(ktx_file):
            os.system(f"PVRTexToolCLI -i {os.path.join(lossy_folder, png_file)} -o {ktx_file} -f ETC2_rgb -q etcfast")
# Does the same for Lossless Folder
lossless_folder = os.path.join(texture_folder, f"lossless")
for png_file in os.listdir(lossless_folder):
    # Checks if it is a PNG file
    if png_file.endswith(".png"):
        # Grabs file name without the PNG extension
        filename = os.path.splitext(png_file)[0]
        # Concatenate the folder path and KTX extension
        # And checks if the converted file already exist in the same folder
        ktx_file = os.path.join(lossless_folder, f"{filename}.ktx")
        if not os.path.isfile(ktx_file):
            os.system(f"PVRTexToolCLI -i {os.path.join(lossless_folder, png_file)} -o {ktx_file} -f ETC2_rgb -q etcfast")