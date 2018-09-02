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
    world_up = Vector3(0.0, 1.0, 0.0)
    velocity = [ 0.0, 0.0, 0.0 ]
    angular_velocity = [ 0.0, 0.0, 0.0 ]
    def __init__(self, 
                 position=Vector3(0.0, 0.0, 0.0),
                 forward=Vector3(0.0, 0.0, 1.0)):
        self.position = position
        self.forward = forward.normalized()
        self.cam_matrix = Matrix4()
        self.view_matrix = Matrix4()
        self.yaw = 0.0
        self.pitch = 0.0
        self.calc_view()
     
    def calc_view(self):
        self.forward.x = math.cos(math.radians(self.pitch)) * math.cos(math.radians(self.yaw))
        self.forward.y = math.sin(math.radians(self.pitch))
        self.forward.z = math.cos(math.radians(self.pitch)) * math.sin(math.radians(self.yaw))
        self.forward = self.forward.normalized()
        self.across = Camera.world_up.cross(self.forward).normalized()
        self.up = self.forward.cross(self.across).normalized()
        self.cam_matrix = Matrix4.new_rotate_triple_axis(self.across, self.up, self.forward)
        self.cam_matrix.d = self.position.x
        self.cam_matrix.h = self.position.y
        self.cam_matrix.l = self.position.z
        self.target = self.position + self.forward;
        self.view_matrix = Matrix4.new_look_at(self.position, self.target, Camera.world_up)
     
    def axis(self, axis):
        if (axis == 0):
            return self.across
        if (axis == 1):
            return self.up
        if (axis == 2):
            return self.forward

    def move(self, distance, axis=2):
        forward = self.axis(axis)
        self.position.x += forward.x * distance
        self.position.y += forward.y * distance
        self.position.z += forward.z * distance
        #self.calc_view()

    def rotate(self, angle=math.pi / 180.0, axis=2):
        #print angle, self.axis(axis)
        #print forward
        #forward = forward.rotate_around(self.axis(axis), angle)
        #print forward
        return

    def apply(self, shader):
        self.calc_view()
        if (shader.has_uniform("cameraMatrix")):
            shader.uniform_matrixf("cameraMatrix", self.cam_matrix)
        if (shader.has_uniform("viewMatrix")):
            shader.uniform_matrixf("viewMatrix", self.view_matrix)
        return

    def update(self, dt):
        self.rotate(self.angular_velocity[Y_AXIS] * dt, Y_AXIS)
        self.move(self.velocity[X_AXIS] * dt, X_AXIS)
        self.move(self.velocity[Z_AXIS] * dt, Z_AXIS)
        #self.calc_view()
        return

if __name__ == "__main__":
    cam = Camera(position=Vector3(0.0, 0.0, -10.0))
    cam.pitch = 90
    print cam.view_matrix
    print cam.cam_matrix
    cam.calc_view()
    print cam.view_matrix
    print cam.cam_matrix
