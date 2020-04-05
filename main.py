#
# Distributed under the Boost Software License, Version 1.0
# (see http://www.boost.org/LICENSE_1_0.txt)
#

import os
import sys
import camera
import sphere
import util
import pyglet
from euclid import *
from simplui import *

from shadersystem import ShaderSystem



proj_matrix = Matrix4()
view_matrix = Matrix4()
tex_matrix = Matrix4()
frame = None
dialogue = None
window_width = 800
window_height = 600

# create the window, but keep it offscreen until we are done with setup
window = pyglet.window.Window(window_width, window_height,
                              vsync=False,
                              caption="GLSL Test")


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
    proj_matrix = Matrix4.new_perspective(math.pi / 3,
                                          aspect_ratio, 0.1, 100.0)
    return pyglet.event.EVENT_HANDLED


def update(dt):
    global cam
    cam.update(dt)
    return
    
def update_gui(dt):
    if dialogue.parent is not None:
        global cam
        fps = pyglet.clock.get_fps()
        pos = cam.position
        element = frame.get_element_by_name('fps_label')
        # and change the text, to display the current fps
        element. text = '%.1f fps' % (fps)
        element = frame.get_element_by_name('pos_label')
        element.text = '%6.2f,%6.2f,%6.2f pos' % (pos.x, pos.y, pos.z)


@window.event
def on_draw():
    global cam, proj_matrix, tex_matrix, grid_texture, frame

    window.clear()
    pyglet.gl.glEnable(pyglet.gl.GL_DEPTH_TEST)
    pyglet.gl.glEnable(pyglet.gl.GL_CULL_FACE)
    mainShader = shaderSystem["main"]
    gradientShader = shaderSystem["gradient"]
    mainShader.bind()
    mainShader.uniform_matrixf("projMatrix", proj_matrix)
    cam.apply(mainShader)
    mainShader.uniform_matrixf("texMatrix",  tex_matrix)
    pyglet.gl.glActiveTexture(pyglet.gl.GL_TEXTURE0)
    pyglet.gl.glBindTexture(grid_texture.target, grid_texture.id)
    mainShader.uniformi('tex0', 0)
    planet.draw(mainShader)
    pyglet.gl.glBindTexture(grid_texture.target, 0)
    mainShader.unbind()
    # back to "old opengl" for the gui
    pyglet.gl.glViewport(0, 0, window_width, window_height)
    pyglet.gl.glMatrixMode(pyglet.gl.GL_PROJECTION)
    pyglet.gl.glLoadIdentity()
    pyglet.gl.glOrtho(0, window_width, 0, window_height, -1.0, 1.0)
    pyglet.gl.glMatrixMode(pyglet.gl.GL_MODELVIEW)
    pyglet.gl.glDisable(pyglet.gl.GL_DEPTH_TEST)
    pyglet.gl.glDisable(pyglet.gl.GL_CULL_FACE)
    frame.draw()


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
    global frame, dialogue
    # load some gui themesn
    theme = Theme('themes/macos')
    # create a frame to contain our gui, the full size of our window
    frame = Frame(theme, w=800, h=600)
    window.push_handlers(frame)
    # create dialogue - note that we create the entire gui in a single call
    dialogue = Dialogue('Inspector', x=20, y=600-20,
                        # add a vertical layout to hold the window contents
                        content=VLayout(autosizex=True, children=[
                            # add a text label, note that this element is named...
                            Label('0.0 fps', name='fps_label'),
                            Label('000.00 000.00 000.00 pos', name='pos_label')]))
    frame.add(dialogue)


def setup():
    global shaderSystem, grid_image, grid_texture, planet, cam
    # One-time GL setup
    pyglet.gl.glClearColor(1, 1, 1, 1)
    shaderSystem = ShaderSystem()
    mainShader = shaderSystem.createShader("main")
    mainShader.bind()
    grid_image = pyglet.image.load('images/grid.png')
    grid_texture = grid_image.get_texture()
    planet = sphere.Sphere(1.0)
    cam = camera.Camera(position=Vector3(0.0, 0.0, -10.0))
    mainShader.unbind()
    gradientShader = shaderSystem.createShader("gradient")

# schedule an empty update function, at 60 frames/second
pyglet.clock.schedule_interval(update, 1.0/60.0)

# schedule a gui update every half second
pyglet.clock.schedule_interval(update_gui, 0.5)

# make the window visible
window.set_visible(True)

# setup shaders etc
setup()
setup_gui()

window.push_handlers(frame)
window.push_handlers(on_key_press)


sys.path = sys.path + [os.curdir + os.sep + "fbx2018_1"]
import FbxCommon
try:
    from fbx import *
    # Prepare the FBX SDK.
    (lSdkManager, lScene) = FbxCommon.InitializeSdkObjects()
    try:
        print "Ipython import"
        from IPython.lib.inputhook import enable_gui
        enable_gui('pyglet')
    except ImportError:
         print "Ipython import failed"
    pyglet.app.run()
    lSdkManager.Destroy()
except ImportError:
    print 'You need to copy the content in compatible subfolder under ./fbx20151 into your pyglsl install folder.'
