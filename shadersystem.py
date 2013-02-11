import os

from shader import Shader

class ShaderSystem:
	vertShaders   = None
	fragShaders   = None
	geomShaders   = None
	shaders       = {}
	instance      = None
	currentShader = None


	def __init__(self, shaderdir = os.getcwd()):
		assert ShaderSystem.instance = None
		self.shaderdir = shaderdir
		def listShaders(d):
			f = [ shfile for shfile in os.listdir(os.path.join(shaderdir, d)) ]
			l = [ os.path.join(os.path.join(shaderdir,d), fl) for fl in f ]
			s = [ s for s in l if os.path.isfile(s) ]
			return s
		def shaderName(s):
			return os.path.splitext(os.path.basename(s))[0]
		ShaderSystem.vertShaders = { shaderName(shfile) : shfile for shfile in listShaders(r"glsl/vert") }
		ShaderSystem.fragShaders = { shaderName(shfile) : shfile for shfile in listShaders(r"glsl/frag") }
		ShaderSystem.geomShaders = { shaderName(shfile) : shfile for shfile in listShaders(r"glsl/geom") }
		instance = self
		print "Shaders available ", ShaderSystem.vertShaders.keys()

	def getShaderFileContents(self, name):
		vertFile  = None
		if ShaderSystem.vertShaders.has_key(name):
			vertFile = ShaderSystem.vertShaders[name]
			f = open(vertFile)
			vertFile = f.readlines()
			f.close()
		fragFile  = None
		if ShaderSystem.fragShaders.has_key(name):
			fragFile = ShaderSystem.fragShaders[name]
			f = open(fragFile)
			fragFile = f.readlines()
			f.close()
		geomFile  = None
		if ShaderSystem.geomShaders.has_key(name):
			geomFile = ShaderSystem.geomShaders[name]
			f = open(geomFile)
			geomFile = f.readlines()
			f.close()
		if (vertFile != None) or (fragFile != None) or (geomFile != None):
			return (vertFile, fragFile, geomFile)

	def createShader(self, name):
		shaderTexts = self.getShaderFileContents(name)
		print "Creating shader " + name
		ShaderSystem.shaders[name] = Shader(vert = shaderTexts[0], frag = shaderTexts[1], geom=shaderTexts[2])
		return ShaderSystem.shaders[name]

	def __getitem__(self, name):
		if (self.shaders.has_key(name)):
			return self.shaders[name]
		else:
			raise KeyError, name  # <<< DON'T FORGET THIS LINE !!
