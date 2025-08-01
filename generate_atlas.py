#!/usr/bin/env python3

import os

from PIL import Image

imgs_path = "./assets/imgs/"
file_names = [f for f in os.listdir(imgs_path) if os.path.isfile(os.path.join(imgs_path, f)) and f.endswith("png") ]
file_paths = [os.path.join(imgs_path, n) for n in file_names]

imgs = [Image.open(file).convert("RGBA") for file in file_paths]

names = [n.removesuffix(".png").upper() for n in file_names]

def next_pow2(x):
    return 1 << (x - 1).bit_length()

atlas_height = 0

x, y = 0, 0
row_height = 0

sprite_imgs  = []
sprite_infos = []

atlas_width = 0
exp = 0
for img in imgs:
    w, _ = img.size
    while w > atlas_width:
        exp += 1
        atlas_width = 1<<exp

for name, img in zip(names, imgs):
    w, h = img.size
    if x + w > atlas_width:
        x = 0
        y += row_height
        row_height = 0
    sprite_imgs.append((img, x, y))
    sprite_infos.append({ "name": name, "x": x, "y": y, "w": w, "h": h, });
    x += w
    row_height = max(row_height, h)
    atlas_height = max(atlas_height, y + h)

atlas_height = next_pow2(atlas_height)

atlas_img = Image.new("RGBA", (atlas_width, atlas_height), (0, 0, 0, 0))
for img, x, y in sprite_imgs:
    atlas_img.paste(img, (x, y))

atlas_data = atlas_img.tobytes()

sprites_h  = "#ifndef __SPRITES_H__\n"
sprites_h += "#define __SPRITES_H__\n\n"
sprites_h += "enum sprite {\n"
for i, info in enumerate(sprite_infos):
    sprites_h += "  SPR_%s" % info["name"]
    if i == 0:
        sprites_h += " = 0"
    sprites_h += ",\n"
sprites_h += "  SPRITES_AMOUNT\n};\n\n"
sprites_h += "#endif/*__SPRITES_H__*/\n"

atlas_h  = "#ifndef __ATLAS_H__\n"
atlas_h += "#define __ATLAS_H__\n\n"
atlas_h += "#include \"engine/math.h\"\n"
atlas_h += "#include \"engine/sprites.h\"\n\n"
atlas_h += "#define ATLAS_WIDTH %d\n" % atlas_width
atlas_h += "#define ATLAS_HEIGHT %d\n" % atlas_height
atlas_h += "#define ATLAS_PIXEL_W (1.0f/%d)\n" % atlas_width
atlas_h += "#define ATLAS_PIXEL_H (1.0f/%d)\n" % atlas_height
atlas_h += "static const uint32_t g_atlas_data[ATLAS_WIDTH*ATLAS_HEIGHT] = {\n"
for i in range(0, len(atlas_data), 4):
    atlas_h += "0x%02x%02x%02x%02x," % (atlas_data[i + 3], atlas_data[i + 2], atlas_data[i + 1], atlas_data[i + 0])
atlas_h += "\n};\n\n"
atlas_h += "static const struct v2 g_atlas_sprite_positions[SPRITES_AMOUNT] = {\n"
for info in sprite_infos:
    atlas_h += "  { %d.0f * ATLAS_PIXEL_W, %d.0f * ATLAS_PIXEL_H },\n" % (info["x"], info["y"])
atlas_h += "};\n\n"
atlas_h += "static const struct v2 g_atlas_sprite_sizes[SPRITES_AMOUNT] = {\n"
for info in sprite_infos:
    atlas_h += "  { %d.0f * ATLAS_PIXEL_W, %d.0f * ATLAS_PIXEL_H },\n" % (info["w"], info["h"])
atlas_h += "};\n\n"
atlas_h += "static const struct v2 g_atlas_sprite_half_sizes[SPRITES_AMOUNT] = {\n"
for info in sprite_infos:
    w = "%g" % (info["w"] * 0.5)
    if "." not in w:
        w += ".0"
    h = "%g" % (info["h"] * 0.5)
    if "." not in h:
        h += ".0"
    atlas_h += "  { %sf * UNIT_ONE_PIXEL, %sf * UNIT_ONE_PIXEL },\n" % (w, h)
atlas_h += "};\n\n"
atlas_h += "#endif/*__ATLAS_H__*/\n"

atlas_img.save("./atlas_debug.png");


path = "./include/engine/atlas.h"
try:
    with open(path, "w") as f:
        f.write(atlas_h)
        pass
except:
    print("couldn't open '", path, "'", sep="")

path = "./include/engine/sprites.h"
try:
    with open(path, "w") as f:
        f.write(sprites_h)
        pass
except:
    print("couldn't open '", path, "'", sep="")

print("atlas updated")
