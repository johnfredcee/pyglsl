import pyglet

# Create a context matching the above configuration.  Will fail if
# OpenGL 3 is not supported by the driver.
window = pyglet.window.Window(visible=False)

# Print the version of the context created.
#print 'OpenGL version:', window.context.get_info().get_version()
print(window.context._info.version)

window.close()

