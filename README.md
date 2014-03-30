
At the moment, this is just a little proof of context, derived from Tristam MacDonald's
code http://swiftcoder.wordpress.com/2008/12/19/simple-glsl-wrapper-for-pyglet/

This requires the source version of pyglet, found at..

hg clone https://code.google.com/p/pyglet/

..It also requires pyeuclid for maths, to be found here..

https://github.com/ezag/pyeuclid

..happy hacking!

Pyglet at present doesn't have a way to select a compatibility profile, only
a core profile. I was using an OpenGL 3.3 compatibility profile, so I imagine
you need at least this for it to work.
