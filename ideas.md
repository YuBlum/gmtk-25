__The theme is: _loop___

What loop might suggest:
- Repeating the same thing over and over again
  - Rogue-like
  - Same room, different outcomes (?)
  - Reverse back in time
  - Need to break the loop
- A bended curve (like a circle or something)
  - I really have no idea


Maybe an infinite side-scroller or something like that, that you need to complete a set of specific things to break out from the loop.
A space-shooter would fit well if this.
Maybe not even a side-scroller, just something that needs to be repeated over and over again.
What code this be?
- You enter a room, it is the same as the room before it. In there you clear waves of enemies and proceeds to the next room.
- After a two rooms or something some new shit apear out of no where. What this could be? idk
- You then have to follow that strange thing or something

For now do the gameplay loop of entering a room, enemy waves, kill the enemies and thats it.

After that think of some random thing to put after a few rooms.

scrap most of the previous ideas.

__Recursive Room__ (name of the game probably)

Is going to be an escape room.

One room with a bunch of items, you can place some items into somewhere in the room, when you exit the room you end up in the same room, but things change depending on the items put in the input, some new items pop up and so on. The objective will be to find the correct order of items to exit the room.

Maybe add some simple story-line about a shut-in person overcomming their stuff, maybe the items cold relate to that it would be cool.

The room is going to have two boxes, you can put one item in each.

You have four options:
1. Put one item in the left box and nothing in the right box
2. Put one item in the right box and nothing in the left box
3. Put one item in each box
4. Don't put anything in neither boxes

After that you can cross through the door.
- The item on the left box determines the items of the room
- The item on the right box determines the layout of the room

No items on the left box: default items

No items on the right box: default layout

You can cross through the door holding an item

The room will have two items

You can also use the item instead of putting it on the box to interact with the layout

Maybe 5 or 6 items in the game

The title is going to be: _Shut in Recursion_

- __Layout box:__ This is the box on the left. It sets the layout of the next room state.
- __Content box:__ This is the box on the right. It sets the contents of the next room state.

Items:
- Rock
- Glass
- Broom
- Knife
- Key
- Mirror
- Box
- Trash
- Rope
- Lock

Rock breaks mirror get glass
Glass open box get broom
Broom cleans trash get knife
Knife cuts rope get key
Key opens locked door get out

| Layout box | Content box | Output room                      |
|------------|-------------|----------------------------------|
| Empty      | Empty       | Default room with lock item      |
| Empty      | Lock        | Default room with trash item     |
| Empty      | Trash       | Default room with box item       |
| Empty      | Box         | Default room with rope item      |
| Empty      | Rope        | Default room with mirror item    |
| Empty      | Mirror      | Default room with no items       |
| Empty      | Rock        | Default room with mirror item    |
| Empty      | Glass       | Default room with box item       |
| Empty      | Broom       | Default room with trash item     |
| Empty      | Knife       | Default room with rope item      |
| Empty      | Key         | Default room with lock item      |
| Lock       | Empty       | Lock room with rock item         |
| Lock       | Lock        | Lock room with no items          |
| Lock       | Trash       | Lock room with no items          |
| Lock       | Box         | Lock room with no items          |
| Lock       | Rope        | Lock room with no items          |
| Lock       | Mirror      | Lock room with lock item         |
| Lock       | Rock        | Lock room with no items          |
| Lock       | Glass       | Lock room with no items          |
| Lock       | Broom       | Lock room with no items          |
| Lock       | Knife       | Lock room with no items          |
| Lock       | Key         | Lock room with opened door       |
| Trash      | Empty       | Trash room with trash items      |
| Trash      | Lock        | Trash room with trash items      |
| Trash      | Trash       | Trash room with more trash items |
| Trash      | Box         | Trash room with trash items      |
| Trash      | Rope        | Trash room with trash items      |
| Trash      | Mirror      | Trash room with more trash items |
| Trash      | Rock        | Trash room with trash items      |
| Trash      | Glass       | Trash room with trash items      |
| Trash      | Broom       | Trash room with knife item       |
| Trash      | Knife       | Trash room with trash items      |
| Trash      | Key         | Trash room with trash items      |
| Box        | Empty       | Box room with no items           |
| Box        | Lock        | Box room with no items           |
| Box        | Trash       | Box room with no items           |
| Box        | Box         | Box room with no items           |
| Box        | Rope        | Box room with no items           |
| Box        | Mirror      | Box room with box item           |
| Box        | Rock        | Box room with no items           |
| Box        | Glass       | Box room with brom item          |
| Box        | Broom       | Box room with no items           |
| Box        | Knife       | Box room with brom item          |
| Box        | Key         | Box room with no items           |
| Rope       | Empty       | Rope room with key               |
| Rope       | Lock        | Rope room with key               |
| Rope       | Trash       | Rope room with key               |
| Rope       | Box         | Rope room with key               |
| Rope       | Rope        | Rope room with key               |
| Rope       | Mirror      | Rope room with rope              |
| Rope       | Rock        | Rope room with key               |
| Rope       | Glass       | Rope room with key               |
| Rope       | Broom       | Rope room with key               |
| Rope       | Knife       | Rope room with key               |
| Rope       | Key         | Rope room with key               |
| Mirror     | Empty       | Mirror room with no items        |
| Mirror     | Lock        | Mirror room with no items        |
| Mirror     | Trash       | Mirror room with no items        |
| Mirror     | Box         | Mirror room with no items        |
| Mirror     | Rope        | Mirror room with no items        |
| Mirror     | Mirror      | Mirror room with mirror          |
| Mirror     | Rock        | Mirror room with glass item      |
| Mirror     | Glass       | Mirror room with no items        |
| Mirror     | Broom       | Mirror room with no items        |
| Mirror     | Knife       | Mirror room with no items        |
| Mirror     | Key         | Mirror room with no items        |
| Rock       | Empty       | Default room with no items       |
| Rock       | Lock        | Default room with no items       |
| Rock       | Trash       | Default room with no items       |
| Rock       | Box         | Default room with no items       |
| Rock       | Rope        | Default room with no items       |
| Rock       | Mirror      | Default room with rock item      |
| Rock       | Rock        | Default room with no items       |
| Rock       | Glass       | Default room with no items       |
| Rock       | Broom       | Default room with no items       |
| Rock       | Knife       | Default room with no items       |
| Rock       | Key         | Default room with no items       |
| Glass      | Empty       | Default room with no items       |
| Glass      | Lock        | Default room with no items       |
| Glass      | Trash       | Default room with no items       |
| Glass      | Box         | Default room with no items       |
| Glass      | Rope        | Default room with no items       |
| Glass      | Mirror      | Default room with glass item     |
| Glass      | Rock        | Default room with no items       |
| Glass      | Glass       | Default room with no items       |
| Glass      | Broom       | Default room with no items       |
| Glass      | Knife       | Default room with no items       |
| Glass      | Key         | Default room with no items       |
| Broom      | Empty       | Default room with no items       |
| Broom      | Lock        | Default room with no items       |
| Broom      | Trash       | Default room with no items       |
| Broom      | Box         | Default room with no items       |
| Broom      | Rope        | Default room with no items       |
| Broom      | Mirror      | Default room with broom item     |
| Broom      | Rock        | Default room with no items       |
| Broom      | Glass       | Default room with no items       |
| Broom      | Broom       | Default room with no items       |
| Broom      | Knife       | Default room with no items       |
| Broom      | Key         | Default room with no items       |
| Knife      | Empty       | Default room with no items       |
| Knife      | Lock        | Default room with no items       |
| Knife      | Trash       | Default room with no items       |
| Knife      | Box         | Default room with no items       |
| Knife      | Rope        | Default room with no items       |
| Knife      | Mirror      | Default room with knife item     |
| Knife      | Rock        | Default room with no items       |
| Knife      | Glass       | Default room with no items       |
| Knife      | Broom       | Default room with no items       |
| Knife      | Knife       | Default room with no items       |
| Knife      | Key         | Default room with no items       |
| Key        | Empty       | Default room with no items       |
| Key        | Lock        | Default room with no items       |
| Key        | Trash       | Default room with no items       |
| Key        | Box         | Default room with no items       |
| Key        | Rope        | Default room with no items       |
| Key        | Mirror      | Default room with key item       |
| Key        | Rock        | Default room with no items       |
| Key        | Glass       | Default room with no items       |
| Key        | Broom       | Default room with no items       |
| Key        | Knife       | Default room with no items       |
| Key        | Key         | Default room with no items       |

# Rooms

## Default room
Simple room with just one item in it.

## Locker room
A room with a locked door on its center. The door opens if the key was passed in the content box.

## Trash room
A room full of trash. The trash is cleaned if the broom was passed in the content box, then the knife item that was burried in trash will be revealed.

## Box room
A room with a closed box with tape on its center. The box will open and its item will be revealed (a broom) if the knife or the glass was passed in the content box.

## Rope room
A room with a thick rope separating the entrance and the exit sections, the key item is in the exit section. The rope will be cut if the knife was passed in the content box.

## Mirror room
A room with a big mirror in its center. The mirror will be breaked and a glass will be obtained if the rock was passed in the content box.

