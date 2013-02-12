
import pyglet.gl as gl
from shadersystem import ShaderSystem

class ShaderParameter(object):
	FLOAT_PARAMETER = 0,
	INT_PARAMETER  = 1,
	BOOL_PARAMETER = 2,
	FLOAT_VECTOR2_PARAMETER = 3,
	FLOAT_VECTOR3_PARAMETER = 4,
	FLOAT_VECTOR4_PARAMETER = 5,
	FLOAT_MATRIX2_PARAMETER = 6,
	FLOAT_MATRIX3_PARAMETER = 7,
	FLOAT_MATRIX4_PARAMETER = 8,
	BOOL_VECTOR2_PARAMETER = 9,
	BOOL_VECTOR3_PARAMETER = 10,
	BOOL_VECTOR4_PARAMETER = 11,
	INT_VECTOR2_PARAMETER = 12,
	INT_VECTOR3_PARAMETER = 13,
	INT_VECTOR4_PARAMETER = 14,
	UNSUPPORTED_PARAMETER = 15

	ATTRIBUTE_PARAMETER = 1
	UNIFORM_PARAMETER   = 2

	def __init__(self, name, **kwargs):
		self.name = name
		self.handle = if (kwargs.has_key["handle"]) kwargs["handle"] else -1
		self.kind   = if (kwargs.has_key["kind"]) kwargs["kind"] else -1
		self.datatype = if (kwargs.has_key["datatype"]) kwargs["datatype"] else -1

	def updateParameter(self, updater):
		value = updater.getValue()
		self.setParameter(value)
		pass

	def setParameter(self, value):
		currentShader = ShaderSystem.currentShader
		assert currentShader.parameters.has_key[self.name]
		if (self.kind == ShaderParameter.ATTRIBUTE_PARAMETER):
			setUniform(self, currentShader, value)
		else:
			bindAttribute(self, currentShader, value)
		pass

	# To do .. still to account for these
	# gl.GL_BOOL
	# gl.GL_BOOL_VEC2
	# gl.GL_BOOL_VEC3
	# gl.GL_BOOL_VEC4
	# gl.GL_SAMPLER_1D
	# gl.GL_SAMPLER_2D
	# gl.GL_SAMPLER_3D
	# gl.GL_SAMPLER_CUBE
	# gl.GL_SAMPLER_1D_SHADOW
	# gl.GL_SAMPLER_2D_SHADOW

	def setUniform(self, currentShader, value):
		handlers = { gl.GL_FLOAT : [ Shader.uniformf, [ currentShader, self.name, value ] ],
					 gl.GL_FLOAT_VEC2 :  [ Shader.uniformf, [ currentShader, self.name, value ] ],
					 gl.GL_FLOAT_VEC3 : [ Shader.uniformf, [ currentShader, self.name, value ] ],
					 gl.GL_FLOAT_VEC4 : [ Shader.uniformf, [ currentShader, self.name, value ] ],
					 gl.GL_INT : [ Shader.uniformi, [ currentShader, self.name, value ] ],
					 gl.GL_INT_VEC2 : [ Shader.uniformi, [ currentShader, self.name, value ] ],
					 gl.GL_INT_VEC3 : [ Shader.uniformi, [ currentShader, self.name, value ] ],
					 gl.GL_INT_VEC4 : [ Shader.uniformi, [ currentShader, self.name, value ] ],
					 gl.GL_FLOAT_MAT2 : [ Shader.uniform_matrixf, [ currentShader, self.name, value ] ],
					 gl.GL_FLOAT_MAT3 : [ Shader.uniform_matrixf, [ currentShader, self.name, value ] ],
					 gl.GL_FLOAT_MAT4 : [ Shader.uniform_matrixf, [ currentShader, self.name, value ] ] }
		if handlers.has_key[self.datatype]:
			handler = handlers[self.datatype][0]
			handler(*handlers[self.datatype][1])


	def bindAttribute(self, currentShader, value):
		pass
