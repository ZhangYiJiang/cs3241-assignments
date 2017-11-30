# Lab 3

Zhang Yijiang (A0135817B)

## Objects rendered

1. Sphere
2. Cylinder
3. The rocket that carried Sputnik into orbit, also named [Sputnik][1]
4. The world's first artificial satellite - [Sputnik 1][2]

## Primitives

- Sphere (Sputnik's body)
- Cylinder (everything else, in various configurations, including with a top radius of 0 to act as cones)

## Materials

In addition to the original blue material provided, there are two additional materials

- Metal - a shiny, off-white material, used on the satellite and rocket body
- Black - a matte, black material, used for the rocket cone and boosters

## Additional features

- Small camera animation by simulating camera shake to make rocket launch seem more dramatic
- Object animation in the form of spinning them when 'R' is pressed (speed may depend on machine)
- Particle emitter for 2D shapes to fake fire from the rocket engine, with dynamic opacity and color

## Notes

- The original `drawSphere()` function was refactored extensively. The normal vector calculation and explanation are in the function called `drawSpherePoint()`. There is a similar function for `drawCylinder()` called `drawCylinderPoint`
- The `gluPerspective()` call has been moved into `display()` to allow zFar, zNear and fovy to be adjusted

## Coolest thing

- The rocket is made entirely of cylinders (except for the fins on the engines). It's pretty interesting how much you can do with a single primitive


[1]: https://en.wikipedia.org/wiki/Sputnik_(rocket)
[2]: https://en.wikipedia.org/wiki/Sputnik_1



