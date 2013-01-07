import math
from euclid import Vector3
from euclid import Matrix4
from euclid import Matrix3


class Camera:
	through  = 1.2
	across   = 1
	up       = 2
	forward  = 3
	pitch    = 1
	yaw      = 2
	roll     = 3
	axes = [ Vector3(1.0, 0.0, 0.0),
			 Vector3(0.0, 1.0, 0.0),
			 Vector3(0.0, 0.0, 1.0) ]
	
	def __init__(self, position = Vector3(0.0, 0.0, 0.0), forward = Vector3(0.0, 0.0, 1.0), up = Vector3(0.0, 1.0, 0.0)):
		across = forward.cross(up)
		self.matrix = Matrix4()
		print across
		self.matrix[0:3]   = across
		self.matrix[4:7]   = up
		self.matrix[8:11]  = forward
		self.matrix[12:15] = position
		self.matrix[15]    = 1.0

	def up(self):
		return Vector3(self.matrix[4], self.matrix[5], self.matrix[6])

	def forward(self):
		return Vector3(self.matrix[8], self.matrix[9], self.matrix[10])
	
	def across(self):
		return Vector3(self.matrix[0], self.matrix[1], self.matrix[2])

	def position(self):
		return Vector3(self.matrix[12], self.matrix[13], self.matrix[14])
		
	def move(self, distance):
		forward = self.forward()
		self.matrix[12] += forward[0] * distance
		self.matrix[13] += forward[1] * distance
		self.matrix[14] += forward[2] * distance
	
	def rotate(self, axis=1, angle=math.pi / 180.0):
		rotmat = Matrix3.new_rotate_axis(angle, Camera.axes[axis])
		forward = self.forward()
		forward = rotmat * forward
		up      = self.up()
		up      = rotmat * up
		across  = forward.cross(up)
		self.matrix[0:2]   = across
		self.matrix[4:6]   = up
		self.matrix[8:10]  = forward

		
		
