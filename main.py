#
# Distributed under the Boost Software License, Version 1.0
# (see http://www.boost.org/LICENSE_1_0.txt)
#


import sys
import os
sys.path = sys.path + [os.curdir + os.sep + "fbx2020_3"]   
sys.path = sys.path + [os.curdir + os.sep + "pyeuclid"]

import math
import camera
import pyglet.gl
import pyglet.window.key
import pyglet.app

from pyglet.math import Mat4
from pyglet.window import Window, FPSDisplay
from pyglet.event import EVENT_HANDLED, EVENT_UNHANDLED
# from pyglet.clock import get_fps

from sphere import Sphere

from shadersystem import ShaderSystem

proj_matrix = Mat4()
view_matrix = Mat4()
tex_matrix = Mat4()
frame = None
dialogue = None
window_width = 800
window_height = 600

# create the window, but keep it offscreen until we are done with setup
window = pyglet.window.Window(window_width, window_height,
                                 vsync=False,
                                  caption="GLSL Test")
fpsdisplay = pyglet.window.FPSDisplay(window)

@window.event
def on_close():
    window.close()


@window.event
def on_resize(width, height):
    # Override the default on_resize handler to create a 3D projection
    global proj_matrix, window_width, window_height
    window_width = width
    window_height = height
    aspect_ratio = float(width) / float(height)
    proj_matrix = Mat4.perspective_projection(aspect_ratio,  0.1, 100.0, math.pi / 3.0)
    return EVENT_HANDLED


def update(dt):
    global cam
    cam.update(dt)
    return
    
def update_gui(dt):
    if dialogue.parent is not None:
        global cam
        fps = 30.0 # to do
        pos = cam.position
        element = frame.get_element_by_name('fps_label')
        # and change the text, to display the current fps
        element. text = '%.1f fps' % (fps)
        element = frame.get_element_by_name('pos_label')
        element.text = '%6.2f,%6.2f,%6.2f pos' % (pos.x, pos.y, pos.z)


@window.event
def on_draw():
    global cam, proj_matrix, tex_matrix, grid_texture, frame, fpsdisplay, poslabel
    window.clear()
    # pyglet.gl.glEnable(pyglet.gl.GL_DEPTH_TEST)
    # pyglet.gl.glEnable(pyglet.gl.GL_CULL_FACE)
    # mainShader = shaderSystem["main"]
    # gradientShader = shaderSystem["gradient"]
    # mainShader.bind()
    # mainShader["projMatrix"] = proj_matrix
    # cam.apply(mainShader)
    # mainShader["texMatrix"] = tex_matrix
    # pyglet.gl.glActiveTexture(pyglet.gl.GL_TEXTURE0)
    # pyglet.gl.glBindTexture(grid_texture.target, grid_texture.id)
    # mainShader['tex0'] = 0
    # planet.draw(mainShader)
    # pyglet.gl.glBindTexture(grid_texture.target, 0)
    # mainShader.unbind()
    fpsdisplay.draw()
    poslabel.draw()
  

@window.event
def on_key_press(symbol, modifiers):
    global cam, dialogue
    if symbol == pyglet.window.key.I and modifiers & pyglet.window.key.MOD_ACCEL:
        if dialogue.parent is not None:
            frame.remove(dialogue)
        else:
            frame.add(dialogue)
    if symbol == pyglet.window.key.W:
        cam.velocity[camera.Z_AXIS] = 0.3
    if symbol == pyglet.window.key.S:
        cam.velocity[camera.Z_AXIS] = -0.3
    if symbol == pyglet.window.key.D:
        cam.velocity[camera.X_AXIS] = 0.3
    if symbol == pyglet.window.key.A:
        cam.velocity[camera.X_AXIS] = -0.3
    return pyglet.event.EVENT_HANDLED


@window.event
def on_key_release(symbol, modifiers):
    global cam
    if symbol == pyglet.window.key.W:
        cam.velocity[camera.X_AXIS] = 0.0
    if symbol == pyglet.window.key.S:
        cam.velocity[camera.Z_AXIS] = 0.0
    if symbol == pyglet.window.key.D:
        cam.velocity[camera.X_AXIS] = 0.0
    if symbol == pyglet.window.key.A:
        cam.velocity[camera.X_AXIS] = 0.0
    return pyglet.event.EVENT_HANDLED


@window.event
def on_mouse_drag(x, y, dx, dy, buttons, modifiers):
    global cam
    cam.yaw += dx;
    cam.pitch += dy
    if cam.pitch > 89.0:
        cam.pitch =  89.0
    if cam.pitch < -89.0:
        cam.pitch = -89.0
    return pyglet.event.EVENT_HANDLED

#     angle = float(dx) * math.pi / 180.0
#     cam.angular_velocity[camera.Y_AXIS] = angle
#     cam.rotate(angle, camera.Y_AXIS)
#     return pyglet.event.EVENT_HANDLED

def setup_gui():
    global fpslabel, poslabel
    fpslabel = pyglet.text.Label('0.0 fps', font_name='Lucida Sans', x = 20, y = 100)
    poslabel = pyglet.text.Label('000.00 000.00 000.00 pos', font_name='Lucida Sans', x = 20, y = 120) 
  

def setup():
    global shaderSystem, grid_image, grid_texture, planet, cam
    # One-time GL setup
    pyglet.gl.glClearColor(0.0, 0.0, 0.8, 1)
    shaderSystem = ShaderSystem()
    mainShader = shaderSystem.createShader("main")
    mainShader.bind()
    grid_texture = pyglet.resource.texture('images/grid.png')
    planet = Sphere(radius = 1.0)
    cam = camera.Camera(position=pyglet.math.Vec3(0.0, 0.0, -10.0))
    mainShader.unbind()
    gradientShader = shaderSystem.createShader("gradient")

# schedule an empty update function, at 60 frames/second
pyglet.clock.schedule_interval(update, 1.0/60.0)

# schedule a gui update every half second
#pyglet.clock.schedule_interval(update_gui, 0.5)

# make the window visible
window.set_visible(True)

# setup shaders etc
setup()
setup_gui()

window.push_handlers(on_key_press)


sys.path = sys.path + [os.curdir + os.sep + "fbx2020_3"]
import FbxCommon
try:
    from fbx import *
    # Prepare the FBX SDK.
    (lSdkManager, lScene) = FbxCommon.InitializeSdkObjects()
    try:
        print("Ipython import")
        from IPython.lib.inputhook import enable_gui
        enable_gui('pyglet')
    except ImportError:
         print("Ipython import failed")
    pyglet.app.run()
    lSdkManager.Destroy()
except ImportError:
    print("You need to build the python bindings for fbx 2020.1")
