# terrain-gen
Procedurally generates a terrain and displays it with OpenGL.

<img width="900" alt="terrain-gen" src="https://cloud.githubusercontent.com/assets/12981474/21300633/757424b2-c55a-11e6-98d1-ab0ce533653e.png">


### Features
* Texture mixing
* Terrain generated with Perlin Noise
* Lighting (Ambient, Diffuse and Specular)
* Water - using Frame Buffer Objects to render reflection and refraction to textures and distorted through a dudv map and a normal map.


### Libraries Used
* OpenGL 3.3
* GLFW 3
* GLEW
* SOIL
* GLM

### Credits for some code I used
Thank you to http://www.learnopengl.com/ for the great tutorials and the Shader & Camera classes which I am using in this project.
Thank you to https://github.com/sol-prog/Perlin_Noise for the PerlinNoise class.
