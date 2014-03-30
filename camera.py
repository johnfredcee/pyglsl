import math
from euclid import Vector3
from euclid import Matrix3
from euclid import Matrix4
from euclid import Quaternion


class Camera:
    X_AXIS = 0
    Y_AXIS = 1
    Z_AXIS = 2
    through = 1.2
    across = 1
    up = 2
    forward = 3
    pitch = 1
    yaw = 2
    roll = 3
    axes = [Vector3(1.0, 0.0, 0.0), Vector3(0.0, 1.0, 0.0),
            Vector3(0.0, 0.0, 1.0)]

    def __init__(self, position=Vector3(0.0, 0.0, 0.0),
                 forward=Vector3(0.0, 0.0, 1.0),
                 up=Vector3(0.0, 1.0, 0.0)):
        assert(up.dot(forward)*forward != up)
        realup = up - (up.dot(forward)) * forward
        across = forward.cross(realup)
        self.position = position
        matrix = Matrix4()
        matrix[0:3] = across
        matrix[4:7] = up
        matrix[8:11] = forward
        self.orientation = Quaternion.new_rotate_matrix(matrix)

    def up(self):
        matrix = Matrix3(self.orientation)
        return Vector3(matrix[3], matrix[4], matrix[5])

    def forward(self):
        matrix = Matrix3(self.orientation)
        return Vector3(matrix[6], matrix[7], matrix[8])

    def across(self):
        matrix = Matrix3(self.orientation)
        return Vector3(matrix[0], matrix[1], matrix[2])

    def axis(self, axis):
        if (axis == 0):
            return self.across()
        if (axis == 1):
            return self.up()
        if (axis == 2):
            return self.forward()

    def position(self):
        return self.position

    def move(self, distance, axis=2):
        forward = self.axis(axis)
        self.position[0] += forward[0] * distance
        self.position[1] += forward[1] * distance
        self.position[2] += forward[2] * distance

    def rotate(self, angle=math.pi / 180.0, axis=2):
        rot = Quaternion(angle, self.axis(axis))
        self.orientaion = self.orientation * rot

    def apply(self, shader):
        mat = self.orientation.get_matrix()
        if (shader.has_uniform("cameraMatrix")):
            cam_mat = mat
            cam_mat[12:15] = self.position
            shader.uniform_matrixf("cameraMatrix", cam_mat)
        if (shader.has_uniform("viewMatrix")):
            view_mat = mat.transposed()
            view_translate = view_mat.transform(self.position)
            view_mat[12:15] = view_translate
            shader.uniform_matrixf("viewMatrix", view_mat)
        return
