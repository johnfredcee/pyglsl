
This repository has had a bad case of bitrot, mainly because Autodesk has spend a long time coming up with a solution for using fbx with a modern Python.

At last it has been done and I have updated the ExploreFbx notebook. 

The rest is still being updated. When it is finished, it should be self - contained with it's own versions of pyeuclid and simplui for Python3.

For now it's only useful if you want to look at a Jupyter notebook that explores how to work with the fbx SDK.

This was originally derived from [Tristam MacDonald's
code]{http://swiftcoder.wordpress.com/2008/12/19/simple-glsl-wrapper-for-pyglet/} but is rapidly becoming a very disorganised test bed for glsl and procedural geometry generation ideas. Fbx output is supported.

This requires the current hg version of pyglet found at:

hg clone https://code.google.com/p/pyglet/

The latest *binary* release will not work, it must be built from source.

It also requires pyeuclid for maths, to be found here..

https://github.com/ezag/pyeuclid

..and simpului, to be found here..

https://github.com/boskee/simplui

..happy hacking!

This uses a compatibility profile, because some old calls are hard-coded into Pyglet.

This makes me sad :-(
