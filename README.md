
 This is a very disorganised test bed for glsl and procedural geometry generation ideas.

For now it's only useful if you want to look at code that explores how to work with the fbx SDK.

This repository has had a bad case of bitrot, mainly because Autodesk has spend a long time coming up with a solution for using fbx with a modern Python.

At last it has been done and I have updated the ExploreFbx notebook. The other fbx related scripts still need updating.

The main renderer now runs (but does not yet draw anything with Pyglet and Python 3)

This uses a compatibility profile, because some old calls were hard-coded into Pyglet.

It's possible it can now use a core profile, we shall see.

The dependencies pyeuclid and simplui are now submodules pointing to my forks of these packages.

Useful scripts:

*pyglet_version.py* - prints out the version of pyglet that is being used

*track_gen.py* - generates a race track in an fbx file.

*fbx_dump.py* - generates a detailed info dump of the contents of an fbx file

*fbx_parse.py* - parses the node hierarchy of an fbx file

*geomgen.py* - generate various shapes procedurally

*main.py* - pyglet renderer

*ExploreFbx.ipynp* Notebook documenenting working with the fbx sdk api


Much of this was originally derived from [Tristam MacDonald's
code]{http://swiftcoder.wordpress.com/2008/12/19/simple-glsl-wrapper-for-pyglet/}

Simplui Lives [here]{https://github.com/swiftcoder/simplui}
Pyeuclid lives {here}[https://github.com/ezag/pyeuclid]
