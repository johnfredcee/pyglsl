

Much of this was originally derived from [Tristam MacDonald's
code]{http://swiftcoder.wordpress.com/2008/12/19/simple-glsl-wrapper-for-pyglet/} but is rapidly becoming a very disorganised test bed for glsl and procedural geometry generation ideas.

For now it's only useful if you want to look at a Jupyter notebook that explores how to work with the fbx SDK.

This repository has had a bad case of bitrot, mainly because Autodesk has spend a long time coming up with a solution for using fbx with a modern Python.

At last it has been done and I have updated the ExploreFbx notebook. The other fbx related scripts still need updating.

The main renderer now runs (but does not yet draw anything with Pyglet 3)

This uses a compatibility profile, because some old calls were hard-coded into Pyglet.

It's possible it can now use a core profile, we shall see.

pyeuclid and simplui are no longder dependencies but have their own versions in the repo.

Useful scripts:

*track_gen.py* - generates a race track in an fbx file.

*fbx_dump.py* - generates a detailed info dump of the contents of an fbx file

*fbx_parse.py* - parses the node hierarchy of an fbx file

*geomgen.py* - generate various shapes procedurally

*main.py* - pyglet renderer

*ExploreFbx.ipynp* Notebook documenenting working with the fbx sdk api

