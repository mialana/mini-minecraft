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

Fly mode: pressing the F key allows the player to enable fly mode, in which the player can move freely throughout the world without the restrictions of collisions and gravity.


### Procedural Terrain - Rachel Lin

The procedural terrain generation is based off of several noise functions. 

In the terrain class, I added base noise functions to compute fractal brownian motion and perlin noise. These functions take in a vec2 representing an xz coordinate and output a float. 

I also added four functions that each used the base noise functions to generate terrains for different biomes: hills, forest, mountains, and islands. Each of these functions also takes in a vec2 representing an xz coordinate and outputs a float representing the height of the terrain at that coordinate.

All of the biome height fields are based off of a sum of perlin noise generated at different frequencies and amplitudes. However, the general frequency and amplitude varies between biomes: the forest has a lower frequency, and the mountains and islands have a greater amplitude. For the hills, I also flattened the tops of the hills and deepened the troughs to get the desired look terraced look.

I also wrote a blendTerrain() function that takes in two heights corresponding to different biomes and interpolates between the two heights based on perlin noise. This helps to create a smooth transition between biomes. 

In CreateTestScene(), I used the biome functions to generate four different terrain heights for each xz coordinate. I then used my blendTerrain() function to blend between two of them (right now I'm only using the hill and mountain
biomes, but I plan on implementing a new blendTerrain() function to allow for more than two biomes). Depending on which terrain was prioritized, I used setBlockAt() to populate each xyz coordinate with a specific block to match that terrain's desired appearance.


### Texturing - Rachel Lin

I created a texture class for creating, loading, and binding the texture atlas to a texture handle given the texture atlas's filepath so that the texture image could be accessed by the shaders. 

In the chunk class, I added each BlockType to the BlockType enum definition. In order to organize the program into more readable pieces, I defined seven different block structures: 

- hPlane: a horizontal plane like a lily pad with face directions YPOS and YNEG

- cross2: 2 diagonally crossed planes, each with a front and back face, for flowers and tall grass blocks with face directions XPOS_ZPOS, XNEG_ZNEG, XPOS_ZNEG, and XNEG_ZPOS

- cross4: 4 planes (each with a front and back face) crossed in a tic-tac-toe pattern when looking down from above, for wheat and sea grass blocks (face directions are XPOS, XNEG, ZPOS, and ZNEG)

- partialX: a block that has been scaled inward in the x-direction (face directions are XPOS, XNEG, YPOS, YNEG, ZPOS, and ZNEG)

- partialZ: a block that has been scaled inward in the z-direction (face directions are XPOS, XNEG, YPOS, YNEG, ZPOS, and ZNEG)

- partialY: a block whose YPOS and/or YNEG faces have been brought inward (face directions are XPOS, XNEG, YPOS, YNEG, ZPOS, and ZNEG)

- fullCube: a block that fills the full dimensions allocated for it in world space (face directions are XPOS, XNEG, YPOS, YNEG, ZPOS, and ZNEG)

Each BlockType can have one or more (think bamboo with a base stem and leaves) of these structures. When creating vbo data for a chunk, my program loops through each block and checks its BlockType. Depending on which of the structures that block's BlockType contains, the program loops through the corresponding faces. For each face, I call a function isVisible() to check if that face is visible to the player and needs to be drawn or not. If the face needs to be drawn, then I use the current block's position, the BlockType's uv coordinates in the texture atlas, and the face's direction to push back the necessary vbo data for each vertex on that face. 

The program pushes the vbo data for each block into either a vbo for transparent blocks or a vbo for opaque blocks. Opaque blocks are drawn first, and then the transparent blocks. The reason for this separation is to avoid clipping of transparent block faces depending on the order that faces are drawn. These interleaved vbos contain information about each vertex's position, normal, color(outdated), uv coordinates, biome weights, and how the texture sample at the given uv coordinates should be used. 

In the drawable class, I created new functions for generating vbos and binding the data for each characteristic to its corresponding in variable in the vertex shader. In the shaderprogram class, I used these functions to transfer the vbo data over to GPU shaders. 

For drawing each chunk, I used the lambert shader, which uses the texture() function to map the given sampler2D texture onto each face based on its uv coordinates at each vertex. For water and lava blocks, I offset the uv coordinates as a function of time to create an animated effect.


### Custom Texture Pack - Rachel Lin

Some block textures (dirt, tilled dirt, stone, cobblestone, moss stone, ice, bedrock, sand, wood, leaves, water, lava, lily pads, wheat, coral, kelp, bamboo, and seagrass) were taken from the original Minecraft texture pack and recolored. 

The paintings were taken from a texture pack called "Better Paintings - Japan Edition" (https://mcpedl.com/better-paintings-japan-edition/). 

The rice crops, ikebana flowers, lanterns, tatami mats, straw, roof tiles, cloth, painted wood, and plaster blocks were completely original, as well as the plank and window blocks for all six tree types.

As mentioned in the texturing section, since some of these blocks were more complex, this required the chunking to be reorganized based on the block structure.


### Additional Biomes - Rachel Lin

There are four biomes total (excluding caves): mountains, hills, forest, and islands, each with a different height generation function. See the procedural terrain section for more details.

I wrote the function blendMultipleBiomes() to take in the heights of each biome and use bilinear interpolation to output a blended height. The function also stores the weight of each biome at each xz coordinate in a set for later use when creating procedural grass and water color. These weights are determined by two different perlin noise functions that each take in an xz coordinate and output a float representing the elevation and temperature. Depending on the values of these two floats, a different biome emerges. 

In the mountains biome, the snow height is determined by a TV static noise function. In the islands biome, sand is used instead of dirt as the base block. In the hills biome, the topmost block of the terrain at each xz coordinate may be replaced by tilled dirt and wheat or irrigated soil and rice depending on a TV static noise function.


### Procedural Grass (and Water) Color - Rachel Lin

As mentioned previously, each time blendMultipleBiomes() is called for an xz coordinate, it stores the biome weightings at that xz coordinate in a set as the current chunk's member variable. These biome weightings are passed to the lambert shader as part of the vbo data for this chunk. In the lambert shader, there are preset color values for each biome. Based on the biome weights at that block, we perform a weighted average on the preset color values to find a final tint color, which is applied on the grayscale grass texture.


### Procedurally Placed Assets - Rachel Lin

A TV static noise function based on the xz coordinate is used to scatter assets across the terrain. 

Tall grass is scattered at varying frequencies across all biomes. In the mountains biome, lanterns are placed at varying heights above bodies of water, and cedar trees, teak trees, and cottages are placed occasionally on land. In the hills biome, rice and wheat is placed at different heights to create the appearance of terrace farms, and simple huts are placed at low frequencies. In the forest biome, tea houses and cherry trees are placed at low frequencies, and bamboo is placed at higher frequencies. In the islands biome, maple and pine trees are placed at low frequencies on land, and a fbm noise function is used to create patches of aquatic plant life. In the caves biome, wisteria trees and blue flowers are placed regularly. For kelp and bamboo, a second TV static noise function that takes in an additional y coordinate is used to make the plants grow at different heights. I also wrote functions to build trees and houses given a set of xyz coordinates, which were called based on Voronoi points to partition them and keep them from intersecting each other.

