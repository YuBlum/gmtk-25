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
tile_width = 16
tile_height = 16

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
        "entity-types" : []
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
            if tileset["tilewidth"] != tile_width or tileset["tileheight"] != tile_height:
                print("tile size has to be %d by %d tiles" % (tile_width, tile_height))
                exit(1)
            m["tiles"] = [
                {
                    "x": t % tileset["columns"] * tile_width,
                    "y": t // tileset["columns"] * tile_height,
                }
                for t in tile_ids
            ]
        elif layer["type"] == "objectgroup":
            has_size = False
            extra_buffers = {}
            if "properties" in layer:
                for p in layer["properties"]:
                    if p["name"] == "has-size":
                        if p["type"] != "bool":
                            print("'has-size' property has to be a boolean, but in layer '", layer["name"], "' it's a '", p["type"], "'", sep="")
                            exit(1)
                        has_size = p["value"]
                        continue
                    if p["type"] != "string":
                        print("layer properties represent variable definitions, their value needs to be a string representing a type. property '", p["name"], "' of layer '", layer["name"], "' is a '", p["type"], "', not a string", sep="")
                        exit(1)
                    extra_buffers[p["name"]] = p["value"]
                    pass
            if not layer["name"] in all_entity_types:
                all_entity_types[layer["name"]] = {
                    "has-size": has_size,
                    "extra-buffers": extra_buffers
                }
            elif all_entity_types[layer["name"]]["has-size"] != has_size:
                print("some maps have 'has-size' on '", layer["name"], "', but map '", name, "' don't", sep="")
                exit(1)
            else:
                for pname0, typ0 in all_entity_types[layer["name"]]["extra-buffers"].items():
                    found = False
                    for pname1, typ1 in extra_buffers.items():
                        if pname0 == pname1:
                            found = True
                            if typ0 != typ1:
                                print("the type of '", pname0,"' of layer '", layer["name"], "' is '" ,typ0, "' in other maps, but on the map '", name, "' is '", typ1, "'", sep="")
                                exit(1)
                            break
                    if not found:
                        print("the property '", pname0, "' of the layer '", layer["name"], "' don't exists on the map '", name, "'", sep="")
                        exit(1)
                for pname0 in extra_buffers.keys():
                    found = False
                    for pname1 in all_entity_types[layer["name"]]["extra-buffers"].keys():
                        if pname0 == pname1:
                            found = True
                            break
                    if not found:
                        print("the property '", pname0, "' of the layer '", layer["name"], "' is set on the map '", name,"', but in some of the other maps it isn't", sep="")
                        exit(1)
            entities = []
            for o in layer["objects"]:
                e = {}
                e["x"] = o["x"]
                e["y"] = o["y"]
                if has_size:
                    e["x"] += o["width"] * 0.5
                    e["y"] += o["height"] * 0.5
                    e["w"]  = o["width"]
                    e["h"]  = o["height"]
                for pname in extra_buffers.keys():
                    if not "properties" in o:
                        print("a object of the layer '", layer["name"], "', on map '", name, "', doesn't implement the property '", pname, "'",sep="")
                        exit(1)
                    found = False
                    for prop in o["properties"]:
                        if pname == prop["name"]:
                            found = True
                            break
                    if not found:
                        print("a object of the layer '", layer["name"], "', on map '", name, "', doesn't implement the property '", pname, "'",sep="")
                        exit(1)
                if "properties" in o:
                    for prop in o["properties"]:
                        for pname in extra_buffers.keys():
                            if prop["name"] != pname:
                                print("a object of the layer '", layer["name"], "', on map '", name, "', has invalid property '", pname, "'",sep="")
                                exit(1)
                            if prop["type"] != "string":
                                print("object properties represent variable assignments, their value needs to be a string representing a value. property '", prop["name"], "' of an object in the layer '", layer["name"], "' is a '", p["type"], "', not a string", sep="")
                                exit(1)
                        e[prop["name"]] = prop["value"]
                entities.append(e)
            m["entity-types"].append({
                "has-size": has_size,
                "type": layer["name"],
                "extra-buffers": extra_buffers,
                "entities": entities
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
maps_h += "#define MAP_WIDTH %d\n" % tilemap_width
maps_h += "#define MAP_HEIGHT %d\n" % tilemap_height
maps_h += "#define MAP_TILES_AMOUNT (MAP_WIDTH*MAP_HEIGHT)\n\n"
maps_h += "#define TILE_WIDTH %d\n" % tile_width
maps_h += "#define TILE_HEIGHT %d\n\n" % tile_height
maps_h += "#endif/*__MAPS_H__*/\n"

maps_data_h  = "#ifndef __MAPS_DATA_H__\n"
maps_data_h += "#define __MAPS_DATA_H__\n\n"
maps_data_h += "#include \"engine/math.h\"\n"
maps_data_h += "#include \"engine/maps.h\"\n"
maps_data_h += "#include \"engine/sprites.h\"\n"
maps_data_h += "#include \"game/core.h\"\n"
maps_data_h += "#include \"engine/core.h\"\n\n"
for m in maps:
    for t in m["entity-types"]:
            maps_data_h += "static const struct v2 g_map_" + m["name"] + "_" + t["type"] + ("_position[%d] = {\n" % len(t["entities"]))
            for e in t["entities"]:
                x = "%g" % e["x"]
                if "." not in x:
                    x += ".0"
                y = "%g" % e["y"]
                if "." not in y:
                    y += ".0"
                maps_data_h += "  { " + x + "/UNIT_PER_PIXEL - GAME_W*0.5f, GAME_H*0.5f - (" + y + "/UNIT_PER_PIXEL) },\n"
            maps_data_h += "};\n\n"
            if t["has-size"]:
                maps_data_h += "static const struct v2 g_map_" + m["name"] + "_" + t["type"] + ("_size[%d] = {\n" % len(t["entities"]))
                for e in t["entities"]:
                    w = "%g" % e["w"]
                    if "." not in w:
                        w += ".0"
                    h = "%g" % e["h"]
                    if "." not in h:
                        h += ".0"
                    maps_data_h += "  { " + w + "/UNIT_PER_PIXEL, " + h + "/UNIT_PER_PIXEL },\n"
                maps_data_h += "};\n\n"
            for bname, btyp in t["extra-buffers"].items():
                maps_data_h += "static const " + btyp + " g_map_" + m["name"] + "_" + t["type"] + ("_" + bname + "[%d] = {\n" % len(t["entities"]))
                for e in t["entities"]:
                    maps_data_h += "  " + e[bname] + ",\n"
                maps_data_h += "};\n\n"
maps_data_h += "static const struct {\n"
maps_data_h += "  struct v2 tiles_position[MAP_TILES_AMOUNT];\n"
maps_data_h += "  struct v2u tiles_sprite_position[MAP_TILES_AMOUNT];\n"
for name, props in all_entity_types.items():
    maps_data_h += "  const struct v2 *" + name + "_position;\n"
    if props["has-size"]:
        maps_data_h += "  const struct v2 *" + name + "_size;\n"
    for bname, btyp in props["extra-buffers"].items():
        maps_data_h += "  const " + btyp + " *" + name + "_" + bname + ";\n"
for name in all_entity_types.keys():
    maps_data_h += "  uint32_t " + name + "_amount;\n"
maps_data_h += "  enum sprite tileset;\n"
maps_data_h += "} g_maps_data[MAPS_AMOUNT] = {\n"
for m in maps:
    maps_data_h += "  {\n"
    maps_data_h += "    .tiles_position = {\n"
    for y in range(0, tilemap_height):
        for x in range(0, tilemap_width):
            maps_data_h += "      { %d - GAME_W * 0.5f + 0.5f, GAME_H * 0.5f - %d - 0.5f },\n" % (x, y)
    maps_data_h += "    },\n"
    maps_data_h += "    .tiles_sprite_position = {\n"
    for tile in m["tiles"]:
        maps_data_h += "      { %d, %d },\n" % (tile["x"], tile["y"])
    maps_data_h += "    },\n"
    for name, props in all_entity_types.items():
        type_index = -1
        for i, map_t in enumerate(m["entity-types"]):
            if map_t["type"] == name:
                type_index = i
                break
        if type_index != -1:
            maps_data_h += "    ." + name + "_position = g_map_" + m["name"] + "_" + name + "_position,\n"
            if props["has-size"]:
                maps_data_h += "    ." + name + "_size     = g_map_" + m["name"] + "_" + name + "_size,\n"
            for bname in props["extra-buffers"].keys():
                maps_data_h += "    ." + name + "_" + bname + "     = g_map_" + m["name"] + "_" + name + "_" + bname + ",\n"
            maps_data_h += "    ." + name + "_amount   = %d,\n" % len(m["entity-types"][type_index]["entities"])
        else:
            maps_data_h += "    ." + name + "_position = 0,\n"
            if props["has-size"]:
                maps_data_h += "    ." + name + "_size     = 0,\n"
            for bname in props["extra-buffers"].keys():
                maps_data_h += "    ." + name + "_" + bname + "     = 0,\n"
            maps_data_h += "    ." + name + "_amount   = 0,\n"
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
