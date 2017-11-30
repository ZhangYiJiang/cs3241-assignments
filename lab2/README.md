# Lab 2

Zhang Yijiang A0135817B

This is a colorful drawing of a solar system. The drawing is interactive - use the mouse to launch a rocket from the innermost purple rocket. Hold the mouse to activate the rocket's engines.

## Additional features

- Rocket launched towards mouse from purple planet. Click to launch, hold to aim and activate engines.
- Particle emitter at the tail of the rocket to represent the rocket engine firing.
- Flickering stars in the background.
- Planets have a glow around them.

## Methods you have modified

- `main` was modified to listen to mouse event callbacks

## Any other things the TA should know?

The speed at which the simulation runs is framerate dependent, so on a faster machine the simulation runs more quickly. Try tweaking the `TIME_MULTIPLIER` , `DELTA_V` and `G` constants if the simulation is running too slowly or quickly. The submitted constants were tested on the lab computers.

## What is the coolest thing(s) in your drawing

The rocket is the most interesting thing. It is interactive, features semi-realistic physics, collision detection, and includes a particle emitter.
