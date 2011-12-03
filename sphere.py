
import math
import pyglet
from pyglet.gl import *
from euclid import *

class Sphere(object):
	def __init__(self, radius = 1.0, numRings = 16, numSegments = 16):
		def pointOnSphere(radius, phi, theta):
			u = radius * math.cos(phi)
			x = math.sqrt( radius * radius - u * u ) * math.cos(theta)
			y = math.sqrt( radius * radius - u * u ) * math.sin(theta)
			z = u
			return (x,y,z)
		self.vertices = []
		self.normals  = []
		self.uvs	  = []
		self.indexes  = []
		deltaRingAngle = (math.pi / numRings)
		deltaSegAngle  = (2.0 * math.pi / numSegments)
		offset = 0
		
		for ring in range(0, numRings+1):
			r0 = radius * math.sin(ring * deltaRingAngle)
			y0 = radius * math.cos(ring * deltaRingAngle)
			for seg in range(0, numSegments+1):
				x0 = r0 * math.sin(seg * deltaSegAngle)
				z0 = r0 * math.cos(seg * deltaSegAngle)
				self.vertices.extend([x0, y0, z0])
				normal = Vector3(x0, y0, z0)
				normal.normalize()
				self.normals.extend(normal)
				self.uvs.extend([ seg / float(numSegments), ring / float(numRings)])
				if (ring != numRings):
					self.indexes.append(offset +  numSegments + 1)
					self.indexes.append(offset)
					self.indexes.append(offset + numSegments)
					self.indexes.append(offset + numSegments + 1)
					self.indexes.append(offset + 1)
					self.indexes.append(offset)
					offset = offset  + 1
					
		self.radius = radius
		self.batch = None
		self.vertices = tuple(self.vertices)
		self.uvs      = tuple(self.uvs)
		self.normals  = tuple(self.normals)
		self.indexes  = tuple(self.indexes)
		return

	def draw(self, shader):
		positions   = shader.attributes["position"]["location"]
		uvs         = shader.attributes["uv"]["location"]
		vertattribs = "%dg3f/static" %  positions
		uvattribs   = "%dg2f/static" %  uvs
		pyglet.graphics.draw_indexed(len(self.vertices) / 3, GL_TRIANGLES, \
									 self.indexes, \
									(vertattribs, self.vertices), (uvattribs, self.uvs))
		
				  
		
