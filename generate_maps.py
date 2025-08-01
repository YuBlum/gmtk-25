#!/usr/bin/env python3

import os
import json

maps_path = "./assets/maps/"
file_names = [f for f in os.listdir(maps_path) if os.path.isfile(os.path.join(maps_path, f)) and f.endswith("json") ]
file_paths = [os.path.join(maps_path, n) for n in file_names]

names = [n.removesuffix(".json") for n in file_names]
maps_json = []

for path in file_paths:
    data = {}
    try:
        with open(path, "r") as f:
            maps_json.append(json.load(f))
    except:
        print("couldn't open '", path, "'", sep="")
        exit(1)

maps = []

all_entity_types = {};

tilemap_width = 20
tilemap_height = 20

for name, map in zip(names, maps_json):
    if len(map["tilesets"]) != 1:
        print("the number of tilesets permitted is exactly 1")
    tileset_path = maps_path + map["tilesets"][0]["source"]
    tileset = {}
    try:
        with open(tileset_path, "r") as f:
            tileset = json.load(f)
    except:
        print("couldn't open '", tileset_path, "'", sep="");
        exit(1)
    m = {
        "name"         : name,
        "tileset"      : "SPR_" + tileset["name"].upper(),
        "tiles"        : [],
        "entity_types" : []
    }
    for layer in map["layers"]:
        if layer["type"] == "tilelayer":
            if m["tiles"] != []:
                print("can't have more than one tileset layer")
                exit(1)
            if layer["width"] != tilemap_width or layer["height"] != tilemap_height:
                print("tileset layer has to be %d by %d tiles" % (tilemap_width, tilemap_height))
                exit(1)
            tile_ids = [t - map["tilesets"][0]["firstgid"] for t in layer["data"]]
            m["tiles"] = [
                {
                    "x": t % tileset["columns"] * tileset["tilewidth"],
                    "y": t // tileset["columns"] * tileset["tileheight"],
                    "w": tileset["tilewidth"],
                    "h": tileset["tileheight"]
                }
                for t in tile_ids
            ]
        elif layer["type"] == "objectgroup":
            if not layer["name"] in all_entity_types:
                all_entity_types[layer["name"]] = layer["name"]
            manual_center = False
            if "properties" in layer:
                for p in layer["properties"]:
                    if p["name"] == "manual-center":
                        manual_center = True
            m["entity_types"].append({
                "type": layer["name"],
                "entities": [
                    {
                        "x": o["x"] + (o["width"]  * 0.5 if manual_center else 0),
                        "y": o["y"] + (o["height"] * 0.5 if manual_center else 0),
                        "w": o["width"],
                        "h": o["height"]
                    }
                    for o in layer["objects"]
                ]
            })
        else:
            print("layer type '" + layer["type"] + "', isn't supported ")
            exit(1)
    maps.append(m)

maps_h  = "#ifndef __MAPS_H__\n"
maps_h += "#define __MAPS_H__\n\n"
maps_h += "enum map {\n"
for i, m in enumerate(maps):
    maps_h += "  MAP_" + m["name"].upper()
    if i == 0:
        maps_h += " = 0"
    maps_h += ",\n"
maps_h += "  MAPS_AMOUNT\n"
maps_h += "};\n\n"
maps_h += "#define MAP_WIDTH %d\n\n" % tilemap_width
maps_h += "#define MAP_HEIGHT %d\n\n" % tilemap_height
maps_h += "#define MAP_TILES_AMOUNT (MAP_WIDTH*MAP_HEIGHT)\n\n"
maps_h += "#endif/*__MAPS_H__*/\n"

maps_data_h  = "#ifndef __MAPS_DATA_H__\n"
maps_data_h += "#define __MAPS_DATA_H__\n\n"
maps_data_h += "#include \"engine/math.h\"\n"
maps_data_h += "#include \"engine/maps.h\"\n"
maps_data_h += "#include \"engine/sprites.h\"\n"
maps_data_h += "#include \"engine/core.h\"\n\n"
for m in maps:
    for t in m["entity_types"]:
            maps_data_h += "static struct v2 g_map_" + m["name"] + "_" + t["type"] + ("_position[%d] = {\n" % len(t["entities"]))
            for e in t["entities"]:
                x = "%g" % e["x"]
                if "." not in x:
                    x += ".0"
                y = "%g" % e["y"]
                if "." not in y:
                    y += ".0"
                maps_data_h += "  { " + x + "/UNIT_PER_PIXEL - GAME_W*0.5f, GAME_H*0.5f - (" + y + "/UNIT_PER_PIXEL) },\n"
            maps_data_h += "};\n\n"
            maps_data_h += "static struct v2 g_map_" + m["name"] + "_" + t["type"] + ("_size[%d] = {\n" % len(t["entities"]))
            for e in t["entities"]:
                w = "%g" % e["w"]
                if "." not in w:
                    w += ".0"
                h = "%g" % e["h"]
                if "." not in h:
                    h += ".0"
                maps_data_h += "  { " + w + "/UNIT_PER_PIXEL, " + h + "/UNIT_PER_PIXEL },\n"
            maps_data_h += "};\n\n"
maps_data_h += "static const struct {\n"
maps_data_h += "  struct v2u tiles_position[MAP_TILES_AMOUNT];\n"
maps_data_h += "  struct v2u tiles_size[MAP_TILES_AMOUNT];\n"
for t in all_entity_types:
    maps_data_h += "  struct v2 *" + t + "_position;\n"
    maps_data_h += "  struct v2 *" + t + "_size;\n"
for t in all_entity_types:
    maps_data_h += "  uint32_t " + t + "_amount;\n"
maps_data_h += "  enum sprite tileset;\n"
maps_data_h += "} g_maps_data[MAPS_AMOUNT] = {\n"
for m in maps:
    maps_data_h += "  {\n"
    maps_data_h += "    .tiles_position = {\n"
    for tile in m["tiles"]:
        maps_data_h += "      { %d, %d },\n" % (tile["x"], tile["y"])
    maps_data_h += "    },\n"
    maps_data_h += "    .tiles_size = {\n"
    for tile in m["tiles"]:
        maps_data_h += "      { %d, %d },\n" % (tile["w"], tile["h"])
    maps_data_h += "    },\n"
    for t in all_entity_types:
        type_index = -1
        for i, map_t in enumerate(m["entity_types"]):
            if map_t["type"] == t:
                type_index = i
                break
        if type_index != -1:
            maps_data_h += "    ." + t + "_position = g_map_" + m["name"] + "_" + t + "_position,\n"
            maps_data_h += "    ." + t + "_size     = g_map_" + m["name"] + "_" + t + "_size,\n"
            maps_data_h += "    ." + t + "_amount   = %d,\n" % len(m["entity_types"][type_index]["entities"])
        else:
            maps_data_h += "    ." + t + "_position = 0,\n"
            maps_data_h += "    ." + t + "_size     = 0,\n"
            maps_data_h += "    ." + t + "_amount   = 0,\n"
    maps_data_h += "    .tileset = " + m["tileset"] + ",\n"
    maps_data_h += "  },\n"
maps_data_h += "};\n\n"
maps_data_h += "#endif/*__MAPS_DATA_H__*/\n"

path = "./include/engine/maps_data.h"
try:
    with open(path, "w") as f:
        f.write(maps_data_h)
        pass
except:
    print("couldn't open '", path, "'", sep="");
    exit(1)

path = "./include/engine/maps.h"
try:
    with open(path, "w") as f:
        f.write(maps_h)
        pass
except:
    print("couldn't open '", path, "'", sep="");
    exit(1)

print("maps updated")
