#
# Copyright Tristam Macdonald 2008.
#
# Distributed under the Boost Software License, Version 1.0
# (see http://www.boost.org/LICENSE_1_0.txt)
#

import pyglet
import camera
from pyglet.gl import *
from euclid import *

from shadersystem import ShaderSystem
from sphere import Sphere

proj_matrix = Matrix4()
view_matrix = Matrix4()
tex_matrix = Matrix4()

# create the window, but keep it offscreen until we are done with setup
window = pyglet.window.Window(640, 480,
                              resizable=True, visible=False,
                              caption="GLSL Test")

# centre the window on whichever screen it is currently on (in case of
# multiple monitors)
window.set_location(window.screen.width/2 - window.width/2,
                    window.screen.height/2 - window.height/2)


@window.event
def on_resize(width, height):
    # Override the default on_resize handler to create a 3D projection
    global proj_matrix
    glViewport(0, 0, width, height)
    aspect_ratio = float(width) / float(height)
    proj_matrix = Matrix4.new_perspective(math.pi / 3,
                                          aspect_ratio, 1.0, 1000.0)
    return pyglet.event.EVENT_HANDLED


@window.event
def on_draw():
    global cam, proj_matrix, tex_matrix, grid_texture
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    mainShader = shaderSystem["main"]
    mainShader.bind()
    mainShader.uniform_matrixf("projMatrix", proj_matrix)
    cam.apply(mainShader)
    mainShader.uniform_matrixf("texMatrix",  tex_matrix)
    glActiveTexture(GL_TEXTURE0)
    glBindTexture(grid_texture.target, grid_texture.id)
    mainShader.uniformi('tex0', 0)
    planet.draw(mainShader)
    glBindTexture(grid_texture.target, 0)
    mainShader.unbind()


def setup():
    global shaderSystem, mainShader, grid_image, grid_texture, planet, cam
    # One-time GL setup
    glClearColor(1, 1, 1, 1)
    glColor3f(1, 0, 0)
    glEnable(GL_DEPTH_TEST)
    glEnable(GL_CULL_FACE)
    shaderSystem = ShaderSystem()
    mainShader = shaderSystem.createShader("main")
    grid_image = pyglet.image.load('images/grid.png')
    grid_texture = grid_image.get_texture()
    planet = Sphere(1.0)
    cam = camera.Camera(position=Vector3(0.0, 0.0, -10.0))

# schedule an empty update function, at 60 frames/second
pyglet.clock.schedule_interval(lambda dt: None, 1.0/60.0)

# make the window visible
window.set_visible(True)

# setup shaders etc
setup()

# finally, run the application
pyglet.app.run()
