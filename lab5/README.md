# CS3241 Lab 5 

Zhang Yijiang (A0135817B)

## Notes 

- When tested on VS2013, the program will only display the image when built using 'Release' configuration. When built with 'Debug' it always shows a black screen. Not entirely sure why. 

- The code contains a copy of a Perlin Noise generator used to create the height map used for bump mapping. I've checked with Prof Alan who said it is okay to use this. 

## Additional features 

- Bump mapping - implemented via:
  - The heightmaps is stored in `double bumps[][][]` and is generated in `fillBumpsMatrix()`, which generates four different heightmaps. The edges of the heightmap is mirrored 
  - The `Sphere` class has been augmented with two properties, a `map` variable that indicates which height map to use and a `bumpiness` multiplier for the bump heights. The function `intersectWithRay()` now calculates converts cartesian coordinates to spherical coordinates, which it then uses to look up the height map to calculate `du` and `dv`. It also calculates the longitudinal and latitudinal vectors to perturb the normal vector by the correct amount.
  
- Soft shadows created using area lighting (press 'h' - takes a few seconds). This is implemented in `getPixelLight()`, which calculates the amount of light falling on a particular pixel, `isLightBlocked()` which fires a shadow ray from a point to a point light source. By sampling over an area light source, we can 

- 2x2 supersampling (press 'a' - take a few seconds). This is implemented in `renderScene()` by simply firing off four rays per pixel instead of one. 

- Depth of field (press 'd' - takes up to a few **minutes**). This is implemented in `renderScene()`. Instead of using one ray per pixel, it fires off a circular pattern of rays converging on the position in the focal plane, simulating the circular aperture of a camera. 

All subsampling techniques above uses uniform sampling. I've experimented with stochastic sampling, but the results were not as good in this case. The algorithm used is pretty naive, and can definitely be improved to be more intelligent about when it needs to subsample. 

Turning on supersampling and depth of field together is **not advised** as the result could take a really long time to render. I've included an image showing the results of each feature being turned on. The last image took about 20 minutes to render.  
