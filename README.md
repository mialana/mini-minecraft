# mini-minecraft CIS4600/5600
mini-minecraft-moo-moo-minecrafters created by GitHub Classroom

### Terrain Rendering and Chunking - Amy Liu

#### For drawing terrain on per-chunk basis: 
To implement this feature, I created a function that iterates through the faces of a block and draws the vertices based on whether the adjacent block in that direction is empty. This involved checking whether that block is in the same chunk as the current block, and accessing another chunk's data if not.

To buffer interleaved data into one VBO, I created a Vertex structure that allowed me to store data in an organized fashion, and pushed the Vertex vector to the VBO all at once. I then created a new `draw()` function for the `ShaderProgram` class that handles the interleaved data and renders the `Drawable` correctly.

For the `draw()` function in `Terrain`, I simply iterated through the chunks in the terrain, created vbo data for them, and set the model matrix to the correct chunk origin.

#### For loading chunks based on player proximity:
To implement this feature, I looped through the 4 directions and checked whether the player was currently less than 16 blocks from an unloaded chunk. If they are, I calculated the correct chunk origin and instantiated a new chunk in that direction.


### Game Engine Tick Function and Player Physics - Abhinav Srivastava

For player movement and physics: Added pressed/released events in mygl.cpp to keep track of which keys are preessed at any tick. Then in processInput, I calculated the total accereleration. Then in computePhysics, I calculated velocity using newtons equations of motion. 1 gameplay tweek I made was to only add gravity acc if the player was in air. If the player is in fly mode, both gravity and collisions are not calculated.

For collision detection: Used gridMarch algorithm to check and set the direction for the player to move in, did this every tick, basically checked the player's bounds aganist any block the ray hit, and then set the player's direction to oppose that.

For onGround detection: Used the bottom of the player and essentially cast a pseudo ray downward to check if are getting a block that is not empty at that location, using this information, determined status.

For block placement/removal: Removal was pretty straight forward, because we don't need to worry about block deletion from memory just yet, I just set the block I am clicking on to empty. to get a block, I use gridMarch to see which block is 3 units in front of me.

Simillarly for placement, I used gridMarch to get the block 3 units away from me, then checked if the closest axis adjacent block to that block is empty, then placed a block there.

### Procedural Terrain - Rachel Lin

The procedural terrain generation is based off of several noise functions. 

In the terrain class, I added base noise functions to compute fractal brownian motion and perlin noise. These functions take in a vec2 representing an xz coordinate and output a float. 

I also added four functions that each used the base noise functions to generate terrains for different biomes: hills, forest, mountains, and islands. Each of these functions also takes in a vec2 representing an xz coordinate and outputs a float representing the height of the terrain at that coordinate.

All of the biome height fields are based off of a sum of perlin noise generated at different frequencies and amplitudes. However, the general frequency and amplitude varies between biomes: the forest has a lower frequency, and the mountains and islands have a greater amplitude. For the hills, I also flattened the tops of the hills and deepened the troughs to get the desired look terraced look.

I also wrote a blendTerrain() function that takes in two heights corresponding to different biomes and interpolates between the two heights based on perlin noise. This helps to create a smooth transition between biomes. 

In CreateTestScene(), I used the biome functions to generate four different terrain heights for each xz coordinate. I then used my blendTerrain() function to blend between two of them (right now I'm only using the hill and mountain
biomes, but I plan on implementing a new blendTerrain() function to allow for more than two biomes). Depending on which terrain was prioritized, I used setBlockAt() to populate each xyz coordinate with a specific block to match that terrain's desired appearance.