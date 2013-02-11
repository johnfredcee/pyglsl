import math
from euclid import Vector3
from euclid import Matrix4
from euclid import Matrix3


class Camera:
	through  = 1.2
	across   = 0
	up       = 1
	forward  = 2
	pitch    = 1
	yaw      = 2
	roll     = 3
	axes = [ Vector3(1.0, 0.0, 0.0),
			 Vector3(0.0, 1.0, 0.0),
			 Vector3(0.0, 0.0, 1.0) ]
	currentCamera = None

	def __init__(self, position = Vector3(0.0, 0.0, -10.0), lookat = Vector3(0.0, 0.0, 0.0), up = Vector3(0.0, 1.0, 0.0)):
		forward = (lookat - position)
		forward.normalize()
		up = up - up.dot(forward) * forward
		up.normalize()
		across = forward.cross(up)
		self.cameraMatrix = Matrix4.new_rotate_triple_axis(across, up, -forward)
		self.viewMatrix   = cameraMatrix.transpose()
		positionInv = -(self.viewMatrix * position)
		self.viewMatrix[12:15] = positionInv
		self.cameraMatrix[12:15] = position
		self.cameraMatrix[15]    = 1.0


	def makeCurrentCamera(self):
		Camera.currentCamera = self
		return

	def up(self):
		return Vector3(self.cameraMatrix[4], self.cameraMatrix[5], self.cameraMatrix[6])

	def forward(self):
		return Vector3(self.cameraMatrix[8], self.cameraMatrix[9], self.cameraMatrix[10])

	def across(self):
		return Vector3(self.cameraMatrix[0], self.cameraMatrix[1], self.cameraMatrix[2])

	def position(self):
		return Vector3(self.cameraMatrix[12], self.cameraMatrix[13], self.cameraMatrix[14])

	def move(self, distance):
		forward = self.forward()
		self.cameraMatrix[12] += forward[0] * distance
		self.cameraMatrix[13] += forward[1] * distance
		self.cameraMatrix[14] += forward[2] * distance

	#
	# I don't think this is a good way to control it
	#
	def rotate(self, axis=1, angle=math.pi / 180.0):
		rotmat = Matrix3.new_rotate_axis(angle, Camera.axes[axis])
		forward = self.forward()
		forward = rotmat * forward
		forward.normalize()
		up      = self.up()
		up      = rotmat * up
		up.normalize()
		across  = forward.cross(up)
		across.normalize()
		self.cameraMatrix[0:3]   = across
		self.cameraMatrix[4:7]   = up
		self.cameraMatrix[8:11]  = forward

	def project(self, fov, aspect, near, far):
		self.projectionMatrix = Matrix4.new_perspective(fov, aspect, near, far)
		pass
