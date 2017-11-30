# Lab 1 - Doodles

Zhang Yijiang (A0135817B) 

## Description 

A drawing of cutie marks from ponies in the My Little Pony: Friendship is Magic series. Clockwise from top left: 

- Twilight Sparkle 
- Rarity 
- Cheerilee
- Pinkie Pie 

## Primitives used 

All circles are drawn using GL_TRIANGLE_FAN primitives. To turn them into ovals, scale transform is used. A lot of marks consist of repeated elements, so glScale, glTranslate and glRotate are used extensively. 

### Twilight

- The six sided star is drawn using six GL_TRIANGLE and the center is filled with a GL_POLYGON 
- The shape of the star and their positions are asymmetric, which unfortunately means the position of the points need to be hard coded

### Rarity 

- Diamond background created with GL_POLYGON, the lines using GL_LINE and circular outline in the middle using two concentric circles as described above 


### Cheerilee 

- Everything is made of circles. The petals are made of overlapping circles, then the center is drawn over it. The eyes and mouths are made with two half-ovals drawn over each other to leave only the outline 

### Pinkie Pie

- The balloons are made of an oval, with the tie at the bottom created using a GL_POLYGON. 
- The string is made of a GL_LINE_STRIP positioned with a sine wave and a quarter of a circle 

## Modified methods 

- The header block was modified to load FreeGLUT on Linux
- display() to show the figures 
- init() to change the background color and enable polygon smoothing 

## Most interesting feature 

I think Cheerilee's mark, because it is composed entirely of circles, even though visually it looks like it would require more than just that to render.

