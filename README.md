# ScratchGraphics1

This project started as an attempt to take the code from my Graphics / Game Engine Programming 1 class, recreate it from scratch (hence the name), fix some broken project files, move it to more popular libraries, and rewrite it in an object-oriented and easier to reuse style. After a few false starts and plenty of banging my head against the wall, it has expanded quite a bit, incorporating more elements of game architecture, and has turned into a sort of proto-engine. Long-term, I plan to turn it into a simple and easy-to-work-with game engine, and fork it to form the basis of games and other graphics-related projects. Towards that end, I consider this commit to be version **Alpha 0.5**.

Currently the most recent bit is the audio, especially the synthesizer system.

Requires OpenGL, GLEW, freeGLUT, glm, and OpenAL.

Disclaimer: I'm not sure if this will work on anyone else's computer, partly because I'm not sure I've included the right files in the repository, and partly because graphics programming is a fickle beast. It works on my machine, running on an NVIDIA GPU, with OpenGL 4.5. Your mileage may vary. If you have a minute, try it out and let me know.
