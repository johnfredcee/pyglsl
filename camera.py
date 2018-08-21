import math
from euclid import Vector3
from euclid import Matrix4
from euclid import Matrix3
from euclid import Quaternion


X_AXIS = 0
Y_AXIS = 1
Z_AXIS = 2

class Camera:
    axes = [Vector3(1.0, 0.0, 0.0),
            Vector3(0.0, 1.0, 0.0),
            Vector3(0.0, 0.0, 1.0)]
    velocity = [ 0.0, 0.0, 0.0 ]
    angular_velocity = [ 0.0, 0.0, 0.0 ]

    def __init__(self, 
                 position=Vector3(0.0, 0.0, 0.0),
                 forward=Vector3(0.0, 0.0, 1.0),
                 up=Vector3(0.0, 1.0, 0.0)):
        self.calc_matrix(position, forward, up)
        self.calc_view()

    def calc_matrix(self,
                    position=Vector3(0.0, 0.0, 0.0),
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
        return

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
        #print angle, self.axis(axis)
        #print forward
        #forward = forward.rotate_around(self.axis(axis), angle)
        #print forward
        return
        
    def apply(self, shader):
        if (shader.has_uniform("cameraMatrix")):
            shader.uniform_matrixf("cameraMatrix", self.camMatrix)
        if (shader.has_uniform("viewMatrix")):
            shader.uniform_matrixf("viewMatrix", self.viewMatrix)
        return

    def update(self, dt):
        self.rotate(self.angular_velocity[Y_AXIS] * dt, Y_AXIS)
        self.move(self.velocity[X_AXIS] * dt, X_AXIS)
        self.move(self.velocity[Z_AXIS] * dt, Z_AXIS)
        forward = self.axis(Z_AXIS)
        self.calc_matrix(self.position, forward)
        self.calc_view()
        return
