
from pyglet.math import Mat4, Vec3
import pyglet.window
import camera
import sphere
import gui
from math import pi

from shadersystem import ShaderSystem

proj_matrix = Mat4()
view_matrix = Mat4()
tex_matrix = Mat4()
frame = None
pos_label = None
fps_label = None
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
    proj_matrix = Mat4.new_perspective(pi / 3.0,
                                       aspect_ratio, 0.1, 100.0)
    return pyglet.event.EVENT_HANDLED


def update(dt):
    pass


def update_gui(dt):
    global cam, fps_label, pos_label
    fps = pyglet.clock.get_fps()
    pos = cam.position
    # and change the text, to display the current fps
    fps_label.set_text(f'{fps:.1f} fps')
    pos_label.set_text(f'{pos.x:6.2f}, {pos.y:6.2f}, {pos.z:6.2f} pos')


@window.event
def on_draw():
    global cam, proj_matrix, tex_matrix, grid_texture, frame

    window.clear()
    pyglet.gl.glEnable(pyglet.gl.GL_DEPTH_TEST)
    pyglet.gl.glEnable(pyglet.gl.GL_CULL_FACE)
    mainShader = shaderSystem["main"]
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
    global cam
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
    global frame, fps_label, pos_label
    batch = pyglet.graphics.Batch()
    frame = gui.Frame(batch)
    fps_label = gui.SimpleLabel('0.0 fps', 20, 580, batch, name='fps_label')
    pos_label = gui.SimpleLabel(
        '000.00 000.00 000.00 pos', 20, 560, batch, name='pos_label')
    frame.add(fps_label)
    frame.add(pos_label)


def setup():
    global shaderSystem, grid_image, grid_texture, planet, cam
    # One-time GL setup
    pyglet.gl.glClearColor(1, 1, 1, 1)
    shaderSystem = ShaderSystem()
    mainShader = shaderSystem.createShader("main")
    grid_image = pyglet.image.load('images/grid.png')
    grid_texture = grid_image.get_texture()
    planet = sphere.Sphere(shader = mainShader, texture = grid_texture, radius = 1.0)
    cam = camera.Camera(position=Vec3(0.0, 0.0, -10.0))
    mainShader.unbind()


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

from IPython import get_ipython
get_ipython().run_line_magic('gui', 'pyglet')