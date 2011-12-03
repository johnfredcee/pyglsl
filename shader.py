#
# Copyright Tristam Macdonald 2008.
#
# Distributed under the Boost Software License, Version 1.0
# (see http://www.boost.org/LICENSE_1_0.txt)
#

from pyglet.gl import *
from ctypes    import *

class Shader:
	# vert, frag and geom take arrays of source strings
	# the arrays will be concattenated into one string by OpenGL
	def __init__(self, vert = [], frag = [], geom = []):
		# create the program handle
		self.handle = glCreateProgram()
		# we are not linked yet
		self.linked = False

		# create the vertex shader
		self.createShader(vert, GL_VERTEX_SHADER)
		# create the fragment shader
		self.createShader(frag, GL_FRAGMENT_SHADER)
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
		shader = glCreateShader(type)

		# convert the source strings into a ctypes pointer-to-char array, and upload them
		# this is deep, dark, dangerous black magick - don't try stuff like this at home!
		src = (c_char_p * count)(*strings)
		glShaderSource(shader, count, cast(pointer(src), POINTER(POINTER(c_char))), None)

		# compile the shader
		glCompileShader(shader)

		temp = c_int(0)
		# retrieve the compile status
		glGetShaderiv(shader, GL_COMPILE_STATUS, byref(temp))

		# if compilation failed, print the log
		if not temp:
			# retrieve the log length
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, byref(temp))
			# create a buffer for the log
			buffer = create_string_buffer(temp.value)
			# retrieve the log text
			glGetShaderInfoLog(shader, temp, None, buffer)
			# print the log to the console
			print buffer.value
		else:
			# all is well, so attach the shader to the program
			glAttachShader(self.handle, shader);

	def link(self):
		glBindFragDataLocation(self.handle, 0, "fcolor")
		# link the program
		glLinkProgram(self.handle)
		temp = c_int(0)
		# retrieve the link status
		glGetProgramiv(self.handle, GL_LINK_STATUS, byref(temp))
		# if linking failed, print the log
		if not temp:
			#	retrieve the log length
			glGetProgramiv(self.handle, GL_INFO_LOG_LENGTH, byref(temp))
			# create a buffer for the log
			buffer = create_string_buffer(temp.value)
			# retrieve the log text
			glGetProgramInfoLog(self.handle, temp, None, buffer)
			# print the log to the console
			print buffer.value
		else:
			# all is well, so we are linked
			self.linked = True
			self.bind()
			self.enumerate_attributes()
			self.enumerate_uniforms()
			
			

	def enumerate_attributes(self):
		param = c_long()
		glGetProgramiv(self.handle, GL_ACTIVE_ATTRIBUTES, byref(param))
		self.attributeCount = param.value
		print "%d attributes " % self.attributeCount
		glGetProgramiv(self.handle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH,
					   byref(param))
		cnamelen = param.value
		cname = create_string_buffer(cnamelen + 1)
		for i in range(0, self.attributeCount):
			out_len = c_long()
			attrib_size = c_long()
			attrib_type = c_ulong()
			glGetActiveAttrib(self.handle, i, cnamelen, byref(out_len),
							  byref(attrib_size), byref(attrib_type), cname)
			print "Attrib size %d, type %04x, name %s " %	\
					(attrib_size.value, attrib_type.value, cname.value)
			location = glGetAttribLocation(self.handle, cname)
			self.attributes[cname.value] = \
			{ "location" : location, "info" : ( attrib_type.value, attrib_size.value ) }
		return

	def enumerate_uniforms(self):
		param = c_long()
		glGetProgramiv(self.handle, GL_ACTIVE_UNIFORMS, byref(param))
		self.uniformCount = param.value
		print "%d uniforms " % self.uniformCount
		glGetProgramiv(self.handle, GL_ACTIVE_UNIFORM_MAX_LENGTH,
					   byref(param))
		cnamelen = param.value
		cname = create_string_buffer(cnamelen + 1)
		for i in range(0, self.uniformCount):
			out_len = c_long()
			uniform_size = c_long()
			uniform_type = c_ulong()
			glGetActiveUniform(self.handle, i, cnamelen, byref(out_len),
							   byref(uniform_size), byref(uniform_type), cname)
			location = glGetUniformLocation(self.handle, cname)
			print "Uniform  location %d size %d, type %04x, name %s " %	\
					(location, uniform_size.value, uniform_type.value,
					 cname.value)
			self.uniforms[cname.value] = \
			{ "location" : location, "info" : ( uniform_type.value, uniform_size.value ) }
		return

	def bind(self):
		# bind the program
		glUseProgram(self.handle)

	def unbind(self):
		# unbind whatever program is currently bound - not necessarily this
		# program, so this should probably be a class method instead
		glUseProgram(0)

	# upload a floating point uniform
	# this program must be currently bound
	def uniformf(self, name, *vals):
		# check there are 1-4 values
		if len(vals) in range(1, 5):
			# select the correct function
			{ 1 : glUniform1f,
				2 : glUniform2f,
				3 : glUniform3f,
				4 : glUniform4f
				# retrieve the uniform location, and set
			}[len(vals)](self.uniforms[name]["location"], *vals)

	# upload an integer uniform
	# this program must be currently bound
	def uniformi(self, name, *vals):
		# check there are 1-4 values
		if len(vals) in range(1, 5):
			# select the correct function
			{ 1 : glUniform1i,
				2 : glUniform2i,
				3 : glUniform3i,
				4 : glUniform4i
				# retrieve the uniform location, and set
			}[len(vals)](self.uniforms[name]["location"], *vals)

	# upload a uniform matrix
	# works with matrices stored as lists,
	# as well as euclid matrices
	def uniform_matrixf(self, name, mat):
		# obtian the uniform location
		loc = self.uniforms[name]["location"]
		# uplaod the 4x4 floating point matrix
		glUniformMatrix4fv(loc, 1, False, (c_float * 16)(*mat))

	def attribute(self, name):
		return self.attributes[name]["location"]