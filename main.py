#
# Copyright Tristam Macdonald 2008.
#
# Distributed under the Boost Software License, Version 1.0
# (see http://www.boost.org/LICENSE_1_0.txt)
#

import pyglet
import camera
import sphere
import util
from pyglet.gl import *
from euclid import *
from simplui import *

from shadersystem import ShaderSystem

proj_matrix = Matrix4()
view_matrix = Matrix4()
tex_matrix = Matrix4()
gui_matrix = Matrix4()
frame = None
dialogue = None

# create the window, but keep it offscreen until we are done with setup
window = pyglet.window.Window(800, 600,
                              vsync=False,
                              caption="GLSL Test")


# # centre the window on whichever screen it is currently on (in case of
# # multiple monitors)
# window.set_location(window.screen.width/2 - window.width/2,
#                     window.screen.height/2 - window.height/2)


@window.event
def on_resize(width, height):
    # Override the default on_resize handler to create a 3D projection
    global proj_matrix, gui_matrix
    glViewport(0, 0, width, height)
    aspect_ratio = float(width) / float(height)
    proj_matrix = Matrix4.new_perspective(math.pi / 3,
                                          aspect_ratio, 0.1, 100.0)
    gui_matrix = Matrix4.new_identity()
    #util.make_ortho(gui_matrix, 0, float(width), 0, float(height), 0.0, 1.0)
    return pyglet.event.EVENT_HANDLED


def update(dt):
    pass

def update_gui(dt):
    if dialogue.parent is not None:
        global cam
        fps = pyglet.clock.get_fps()
        pos = cam.position
        element = frame.get_element_by_name('fps_label')
        # and change the text, to display the current fps
        element. text = '%.1f fps' % (fps)
        element = frame.get_element_by_name('pos_label')
        element. text = '%6.2f,%6.2f,%6.2f pos' % (pos.x, pos.y, pos.z)

@window.event
def on_draw():
    global cam, proj_matrix, gui_matrix, tex_matrix, grid_texture, frame
    window.clear()
    #glClear(GL_COLOR_BUFFER_BIT)
    # mainShader = shaderSystem["main"]
    # mainShader.bind()
    # mainShader.uniform_matrixf("projMatrix", proj_matrix)
    # cam.apply(mainShader)
    # mainShader.uniform_matrixf("texMatrix",  tex_matrix)
    # glBindTexture(grid_texture.target, grid_texture.id)
    # mainShader.uniformi('tex0', 0)
    # planet.draw(mainShader)
    # glBindTexture(grid_texture.target, 0)
    # mainShader.unbind()
    guiShader = shaderSystem["gui"]
    guiShader.bind()
    glActiveTexture(GL_TEXTURE0)
    guiShader.uniform_matrixf("guiMatrix", gui_matrix)
    glBindTexture(grid_texture.target, grid_texture.id)    
    guiShader.uniformi('tex0', 0)
    positions   = guiShader.attributes["position"]["location"]
    uvs         = guiShader.attributes["uv"]["location"]
    vertattribs = "%dg2f/static" %  positions
    uvattribs   = "%dg2f/static" %  uvs    
    pyglet.graphics.draw_indexed(4, pyglet.gl.GL_TRIANGLES,
    [0, 1, 2, 0, 2, 3],
    (vertattribs, (100.0, 100.0,
             150.0, 100.0,
             150.0, 150.0,
             100.0, 150.0)),
    (uvattribs, (0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0)))    
    #frame.draw()  
    guiShader.unbind()


@window.event
def on_key_press(symbol, modifiers):
    global cam, dialogue
    if symbol == pyglet.window.key.I and modifiers & pyglet.window.key.MOD_ACCEL:
        if dialogue.parent is not None:
            frame.remove(dialogue)
        else:
            frame.add(dialogue)
    if symbol == pyglet.window.key.W:
        cam.move(0.01, camera.Z_AXIS)
    if symbol == pyglet.window.key.S:
        cam.move(-0.01, camera.Z_AXIS)
    if symbol == pyglet.window.key.D:
        cam.move(0.01, camera.X_AXIS)
    if symbol == pyglet.window.key.A:
        cam.move(-0.01, camera.X_AXIS)
    pass



@window.event
def on_key_release(symbol, modifiers):
    global cam    
    pass

def setup_gui():
    global frame, dialogue
    # load some gui themes
    themes = [Theme('themes/macos'), Theme('themes/pywidget')]
    theme = 0

    # create a frame to contain our gui, the full size of our window
    frame = Frame(themes[theme], w=800, h=600)
    window.push_handlers(frame)

    # create dialogue - note that we create the entire gui in a single call
    dialogue = Dialogue('Inspector', x=20, y=600-20,
                        # add a vertical layout to hold the window contents
                        content=VLayout(autosizex=True, children=[
                            # add a text label, note that this element is named...
                            Label('0.0 fps', name='fps_label'),
                            Label('000.00 000.00 000.00 pos', name='pos_label')]))
    frame.add( dialogue )
    

    
def setup():
    global shaderSystem, grid_image, grid_texture, planet, cam
    # One-time GL setup
    glClearColor(1, 1, 0, 1)
#     glColor3f(1, 0, 0)
#     glEnable(GL_DEPTH_TEST)
#    glEnable(GL_CULL_FACE)
    shaderSystem = ShaderSystem()
    mainShader = shaderSystem.createShader("main")
    guiShader  = shaderSystem.createShader("gui")
    grid_image = pyglet.image.load('images/grid.png')
    grid_texture = grid_image.get_texture()
    planet = sphere.Sphere(1.0)
    cam = camera.Camera(position=Vector3(0.0, 0.0, -10.0))

# schedule an empty update function, at 60 frames/second
pyglet.clock.schedule_interval(lambda dt: None, 1.0/60.0)

# schedule a gui update every half second
pyglet.clock.schedule_interval(update_gui, 0.5)

# make the window visible
window.set_visible(True)

# setup shaders etc
setup()
setup_gui()

window.push_handlers(frame)
window.push_handlers(on_key_press)


# finally, run the application
pyglet.app.run()
