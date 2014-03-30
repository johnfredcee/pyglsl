#
# Copyright Tristam Macdonald 2008.
#
# Distributed under the Boost Software License, Version 1.0
# (see http://www.boost.org/LICENSE_1_0.txt)
#

import pyglet.gl as pygl
import ctypes as cffi

class Shader:
    # vert, frag and geom take arrays of source strings
    # the arrays will be concattenated into one string by OpenGL
    def __init__(self, vert = [], frag = [], geom = []):
        # create the program handle
        self.handle = pygl.glCreateProgram()
        # we are not linked yet
        self.linked = False

        # create the vertex shader
        self.createShader(vert, pygl.GL_VERTEX_SHADER)
        # create the fragment shader
        self.createShader(frag, pygl.GL_FRAGMENT_SHADER)
        # the geometry shader will be the same, once pyglet supports the extension
        # self.createShader(frag, GL_GEOMETRY_SHADER_EXT)
        self.attributes = {}
        self.uniforms   = {}
        # attempt to link the program
        self.link()

    def createShader(self, strings, type):
        count = len(strings)
        # if we have no source code, ignore this shader
        if count < 1:
            return

        # create the shader handle
        shader = pygl.glCreateShader(type)

        # convert the source strings into a ctypes pointer-to-char array, and upload them
        # this is deep, dark, dangerous black magick - don't try stuff like this at home!
        src = (cffi.c_char_p * count)(*strings)
        pygl.glShaderSource(shader, count, cffi.cast(cffi.pointer(src), cffi.POINTER(cffi.POINTER(cffi.c_char))), None)

        # compile the shader
        pygl.glCompileShader(shader)

        temp = cffi.c_int(0)
        # retrieve the compile status
        pygl.glGetShaderiv(shader, pygl.GL_COMPILE_STATUS, cffi.byref(temp))

        # if compilation failed, print the log
        if not temp:
            # retrieve the log length
            pygl.glGetShaderiv(shader, pygl.GL_INFO_LOG_LENGTH, cffi.byref(temp))
            # create a buffer for the log
            buffer = cffi.create_string_buffer(temp.value)
            # retrieve the log text
            cffi.glGetShaderInfoLog(shader, temp, None, buffer)
            # print the log to the console
            print buffer.value
        else:
            # all is well, so attach the shader to the program
            pygl.glAttachShader(self.handle, shader);

    def link(self):
        pygl.glBindFragDataLocation(self.handle, 0, "fcolor")
        # link the program
        pygl.glLinkProgram(self.handle)
        temp = cffi.c_int(0)
        # retrieve the link status
        pygl.glGetProgramiv(self.handle, pygl.GL_LINK_STATUS, cffi.byref(temp))
        # if linking failed, print the log
        if not temp:
            #   retrieve the log length
            pygl.glGetProgramiv(self.handle, pygl.GL_INFO_LOG_LENGTH, cffi.byref(temp))
            # create a buffer for the log
            buffer = pygl.create_string_buffer(temp.value)
            # retrieve the log text
            cffi.glGetProgramInfoLog(self.handle, temp, None, buffer)
            # print the log to the console
            print buffer.value
        else:
            # all is well, so we are linked
            self.linked = True
            self.bind()
            self.enumerate_attributes()
            self.enumerate_uniforms()


    def enumerate_attributes(self):
        param = cffi.c_long()
        pygl.glGetProgramiv(self.handle, pygl.GL_ACTIVE_ATTRIBUTES, cffi.byref(param))
        self.attributeCount = param.value
        print "%d attributes " % self.attributeCount
        pygl.glGetProgramiv(self.handle, pygl.GL_ACTIVE_ATTRIBUTE_MAX_LENGTH,
                       cffi.byref(param))
        cnamelen = param.value
        cname = cffi.create_string_buffer(cnamelen + 1)
        for i in range(0, self.attributeCount):
            out_len = cffi.c_long()
            attrib_size = cffi.c_long()
            attrib_type = cffi.c_ulong()
            pygl.glGetActiveAttrib(self.handle, i, cnamelen, cffi.byref(out_len),
                              cffi.byref(attrib_size), cffi.byref(attrib_type), cname)
            print "Attrib size %d, type %04x, name %s " %   \
                    (attrib_size.value, attrib_type.value, cname.value)
            location = pygl.glGetAttribLocation(self.handle, cname)
            self.attributes[cname.value] = \
            { "location" : location, "info" : ( attrib_type.value, attrib_size.value ) }
        return

    def enumerate_uniforms(self):
        param = cffi.c_long()
        pygl.glGetProgramiv(self.handle,
                            pygl.GL_ACTIVE_UNIFORMS, cffi.byref(param))
        self.uniformCount = param.value
        print "%d uniforms " % self.uniformCount
        pygl.glGetProgramiv(self.handle, pygl.GL_ACTIVE_UNIFORM_MAX_LENGTH,
                            cffi.byref(param))
        cnamelen = param.value
        cname = cffi.create_string_buffer(cnamelen + 1)
        for i in range(0, self.uniformCount):
            out_len = cffi.c_long()
            uniform_size = cffi.c_long()
            uniform_type = cffi.c_ulong()
            pygl.glGetActiveUniform(self.handle, i,
                                    cnamelen, cffi.byref(out_len),
                                    cffi.byref(uniform_size),
                                    cffi.byref(uniform_type), cname)
            location = pygl.glGetUniformLocation(self.handle, cname)
            print "Uniform  location %d size %d, type %04x, name %s " % \
                    (location, uniform_size.value, uniform_type.value,
                     cname.value)
            self.uniforms[cname.value] = {"location": location,
                                          "info": (uniform_type.value,
                                                   uniform_size.value)}
        return

    def has_uniform(self, name):
        return name in self.uniforms

    def bind(self):
        # bind the program
        pygl.glUseProgram(self.handle)

    def unbind(self):
        # unbind whatever program is currently bound - not necessarily this
        # program, so this should probably be a class method instead
        pygl.glUseProgram(0)

    # upload a floating point uniform
    # this program must be currently bound
    def uniformf(self, name, *vals):
        # check there are 1-4 values
        if len(vals) in range(1, 5):
            # select the correct function
            { 1 : pygl.glUniform1f,
                2 : pygl.glUniform2f,
                3 : pygl.glUniform3f,
                4 : pygl.glUniform4f
                # retrieve the uniform location, and set
            }[len(vals)](self.uniforms[name]["location"], *vals)

    # upload an integer uniform
    # this program must be currently bound
    def uniformi(self, name, *vals):
        # check there are 1-4 values
        if len(vals) in range(1, 5):
            # select the correct function
            {1: pygl.glUniform1i,
             2: pygl.glUniform2i,
             3: pygl.glUniform3i,
             4: pygl.glUniform4i}[len(vals)](self.uniforms[name]["location"], *
                                             vals)

    # upload a uniform matrix
    # works with matrices stored as lists,
    # as well as euclid matrices
    def uniform_matrixf(self, name, mat):
        # obtian the uniform location
        loc = self.uniforms[name]["location"]
        # uplaod the 4x4 floating point matrix
        pygl.glUniformMatrix4fv(loc, 1, False, (cffi.c_float * 16)(*mat))

    def attribute(self, name):
        return self.attributes[name]["location"]

    def has_attribute(self, name):
        return name in self.attributes
