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
	current_camera = None

	def __init__(self, position = Vector3(0.0, 0.0, -10.0), lookat = Vector3(0.0, 0.0, 0.0), up = Vector3(0.0, 1.0, 0.0)):
		forward = (lookat - position)
		forward.normalize()
		up = up - up.dot(forward) * forward
		up.normalize()
		across = forward.cross(up)
		self.camera_matrix = Matrix4.new_rotate_triple_axis(across, up, -forward)
		self.view_matrix   = self.camera_matrix.transposed()
		positionInv = -(self.view_matrix * position)
		self.view_matrix[12:15] = positionInv
		self.camera_matrix[12:15] = position
		self.camera_matrix[15]    = 1.0
		self.projection_matrix = Matrix4.new_perspective(math.pi / 2.0, 4.0 / 3.0, 1.0, 1000.0)
		return
	
	def makeCurrentCamera(self):
		global currentCamera
		current_camera = self
		return

	def up(self):
		return Vector3(self.camera_matrix[4], self.camera_matrix[5], self.camera_matrix[6])

	def forward(self):
		return Vector3(self.camera_matrix[8], self.camera_matrix[9], self.camera_matrix[10])

	def across(self):
		return Vector3(self.camera_matrix[0], self.camera_matrix[1], self.camera_matrix[2])

	def position(self):
		return Vector3(self.camera_matrix[12], self.camera_matrix[13], self.camera_matrix[14])

	def move(self, distance):
		forward = self.forward()
		self.camera_matrix[12] += forward[0] * distance
		self.camera_matrix[13] += forward[1] * distance
		self.camera_matrix[14] += forward[2] * distance

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
		self.camera_matrix[0:3]   = across
		self.camera_matrix[4:7]   = up
		self.camera_matrix[8:11]  = forward

	def project(self, fov, aspect, near, far):
		self.projection_matrix = Matrix4.new_perspective(fov, aspect, near, far)
		pass
