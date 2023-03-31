# mini-minecraft CIS4600/5600
mini-minecraft-moo-moo-minecrafters created by GitHub Classroom

Game Engine Tick Function and Player Physics - Abhinav Srivastava

For player movement and physics: Added pressed/released events in mygl.cpp to keep track of which keys are preessed at any tick. Then in processInput, I calculated the total accereleration. Then in computePhysics, I calculated velocity using newtons equations of motion. 1 gameplay tweek I made was to only add gravity acc if the player was in air. If the player is in fly mode, both gravity and collisions are not calculated.

For collision detection: Used gridMarch algorithm to check and set the direction for the player to move in, did this every tick, basically checked the player's bounds aganist any block the ray hit, and then set the player's direction to oppose that.

For onGround detection: Used the bottom of the player and essentially cast a pseudo ray downward to check if are getting a block that is not empty at that location, using this information, determined status.

For block placement/removal: Removal was pretty straight forward, because we don't need to worry about block deletion from memory just yet, I just set the block I am clicking on to empty. to get a block, I use gridMarch to see which block is 3 units in front of me.

Simillarly for placement, I used gridMarch to get the block 3 units away from me, then checked if the closest axis adjacent block to that block is empty, then placed a block there.
