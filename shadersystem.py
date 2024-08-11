import os

from pyglet.graphics.shader import Shader, ShaderProgram

class ShaderSystem:
    vertShaders = None
    fragShaders = None
    geomShaders = None
    shaders = {}

    def __init__(self, shaderdir=os.getcwd()):
        self.shaderdir = shaderdir

        def listShaders(d):
            f = [shfile for shfile in os.listdir(os.path.join(shaderdir, d))]
            l = [os.path.join(os.path.join(shaderdir, d), fl) for fl in f]
            s = [s for s in l if os.path.isfile(s)]
            return s

        def shaderName(s):
            return os.path.splitext(os.path.basename(s))[0]

        # setup lists of available shader files
        ShaderSystem.vertShaders = { shaderName(shfile) : shfile for shfile in listShaders(r"glsl/vert") }
        ShaderSystem.fragShaders = { shaderName(shfile) : shfile for shfile in listShaders(r"glsl/frag") }
        ShaderSystem.geomShaders = { shaderName(shfile) : shfile for shfile in listShaders(r"glsl/geom") }
        print("Shaders available ", ShaderSystem.vertShaders.keys())

    def getShaderFileContents(self, name):
        vertFile = None
        if name in ShaderSystem.vertShaders:
            vertFile = ShaderSystem.vertShaders[name]
            f = open(vertFile)
            vertFile = f.read()
            f.close()
        fragFile = None
        if name in ShaderSystem.fragShaders:
            fragFile = ShaderSystem.fragShaders[name]
            f = open(fragFile)
            fragFile = f.read()
            f.close()
        geomFile = None
        if name in ShaderSystem.geomShaders:
            geomFile = ShaderSystem.geomShaders[name]
            f = open(geomFile)
            geomFile = f.read()
            f.close()
        if (vertFile != None) or (fragFile != None) or (geomFile != None):
            return (vertFile, fragFile, geomFile)

    def createShader(self, name):
        shaderTexts = self.getShaderFileContents(name)
        print("Creating shader " + name)
        vert = Shader(shaderTexts[0], 'vertex')
        frag = Shader(shaderTexts[1], 'fragment')
        geom = None
        if (shaderTexts[2]):
            geom = Shader(shaderTexts[2], 'geom')
            ShaderSystem.shaders[name] = ShaderProgram(vert, frag, geom)
        else:
            ShaderSystem.shaders[name] = ShaderProgram(vert, frag)
        return ShaderSystem.shaders[name]

    def __getitem__(self, name):
        if (name in self.shaders):
            return self.shaders[name]
        else:
            raise KeyError(name)  # <<< DON'T FORGET THIS LINE !!

