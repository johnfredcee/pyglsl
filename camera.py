import math
from euclid import Vector3
from euclid import Matrix4
from euclid import Matrix3
from euclid import Quaternion


X_AXIS = 0
Y_AXIS = 1
Z_AXIS = 2

class Camera:
    through = 1.2
    across = 1
    up = 2
    forward = 3
    pitch = 1
    yaw = 2
    roll = 3
    axes = [Vector3(1.0, 0.0, 0.0),
            Vector3(0.0, 1.0, 0.0),
            Vector3(0.0, 0.0, 1.0)]

    def __init__(self, position=Vector3(0.0, 0.0, 0.0),
                 forward=Vector3(0.0, 0.0, 1.0),
                 up=Vector3(0.0, 1.0, 0.0)):
        assert(up.dot(forward)*forward != up)
        self.position = position
        realforward = forward.normalized()
        realup = up - (up.dot(realforward)) * realforward
        realup.normalize()
        across = forward.cross(realup)
        self.camMatrix = Matrix4.new_rotate_triple_axis(across,
                                                        realup,
                                                        -realforward)
        self.calc_view()

    def calc_view(self):
        self.viewMatrix = self.camMatrix.transposed()
        self.eyeInv = -(self.viewMatrix * self.position)
        self.viewMatrix[12:15] = self.eyeInv
        self.camMatrix[12:15] = self.position

    def up(self):
        return Vector3(self.camMatrix[4], self.camMatrix[5], self.camMatrix[6])

    def forward(self):
        return Vector3(self.camMatrix[8], self.camMatrix[9], self.camMatrix[10])

    def across(self):
        return Vector3(self.camMatrix[0], self.camMatrix[1], self.camMatrix[2])

    def axis(self, axis):
        if (axis == 0):
            return self.across()
        if (axis == 1):
            return self.up()
        if (axis == 2):
            return self.forward()

    def position(self):
        return self.positionm

    def move(self, distance, axis=2):
        forward = self.axis(axis)
        self.position.x += forward.x * distance
        self.position.y += forward.y * distance
        self.position.z += forward.z * distance
        self.calc_view()

    def rotate(self, angle=math.pi / 180.0, axis=2):
        rot = Quaternion(angle, self.axis(axis))
        self.orientaion = self.orientation * rot

    def apply(self, shader):
        if (shader.has_uniform("cameraMatrix")):
            shader.uniform_matrixf("cameraMatrix", self.camMatrix)
        if (shader.has_uniform("viewMatrix")):
            shader.uniform_matrixf("viewMatrix", self.viewMatrix)
        return
