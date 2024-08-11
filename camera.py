
import math
from pyglet.math import Vec3
from pyglet.math import Mat4
from pyglet.graphics.shader import ShaderProgram


X_AXIS = 0
Y_AXIS = 1
Z_AXIS = 2

class Camera:
    axes = [Vec3(1.0, 0.0, 0.0),
            Vec3(0.0, 1.0, 0.0),
            Vec3(0.0, 0.0, 1.0)]
    world_up = Vec3(0.0, 1.0, 0.0)
    velocity = [ 0.0, 0.0, 0.0 ]
    angular_velocity = [ 0.0, 0.0, 0.0 ]
    def __init__(self, 
                 position=Vec3(0.0, 0.0, 0.0),
                 forward=Vec3(0.0, 0.0, 1.0)):
        self.position = position
        self.forward = forward.normalize()
        self.cam_matrix = Mat4()
        self.view_matrix = Mat4()
        self.yaw = 0.0
        self.pitch = 0.0
        self.calc_view()

    def calc_view(self):
        self.forward.x = math.cos(math.radians(self.pitch)) * math.cos(math.radians(self.yaw))
        self.forward.y = math.sin(math.radians(self.pitch))
        self.forward.z = math.cos(math.radians(self.pitch)) * math.sin(math.radians(self.yaw))
        self.forward = self.forward.normalize()
        self.across = Camera.world_up.cross(self.forward).normalize()
        self.up = self.forward.cross(self.across).normalize()
        self.cam_matrix = Mat4(values = (self.across[0], self.up[0], self.forward[0], 0.0,  
                                self.across[1], self.up[1], self.forward[1], 0.0,
                                self.across[2], self.up[2], self.forward[2], 0.0,
                                self.position.x, self.position.y, self.position.z, 1.0))                               
        self.target = self.position + self.forward;
        self.view_matrix = Mat4.look_at(self.position, self.target, Camera.world_up)
     
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

    def apply(self, shader : ShaderProgram) -> None:
        self.calc_view()
        uniforms = shader.uniforms
        if ("cameraMatrix" in uniforms):
            shader["cameraMatrix"] = self.cam_matrix
        if ("viewMatrix" in uniforms):
            shader["viewMatrix"] = self.view_matrix 
        return

    def update(self, dt):
        self.rotate(self.angular_velocity[Y_AXIS] * dt, Y_AXIS)
        self.move(self.velocity[X_AXIS] * dt, X_AXIS)
        self.move(self.velocity[Z_AXIS] * dt, Z_AXIS)
        #self.calc_view()
        return

if __name__ == "__main__":
    cam = Camera(position=Vec3(0.0, 0.0, -10.0))
    cam.pitch = 90
    print(cam.view_matrix)
    print(cam.cam_matrix)
    cam.calc_view()
    print(cam.view_matrix)
    print(cam.cam_matrix)
