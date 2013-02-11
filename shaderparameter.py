
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

	def setUniform(self, currentShader, value):
		pass

	def bindAttribute(self, currentShader, value):
		pass
