
import euclid
import math

def project(forward, up):
    A = euclid.Vector3(*forward)
    A.normalize()
    B = up - up.dot(A) * A
    B.normalize()
    C = A.cross(B)
    return (A, B, C)

def smoothstep(x,a,b):
    if (x < a):
        return 0.0
    if (x > b):
        return 1.0
    t = (x-a)/(b-a)
    return 3.0*t*t - 2.0*t*t*t;

def spline(t, v0, v1, v2, v3, tension = 0.5):
    # Coefficients for Matrix M
    M12 =    1.0;
    M21 =   -tension;
    M23 =    tension;
    M31 =    2.0 * tension;
    M32 =    tension - 3.0;
    M33 =    3.0 - 2.0 * tension;
    M34 =   -tension;
    M41 =   -tension;
    M42 =    2.0 - tension;
    M43 =   tension - 2.0;
    M44 =   tension;
    
    c1 = M12*v1;
    c2 = M21*v0          + M23*v2;
    c3 = M31*v0 + M32*v1 + M33*v2 + M34*v3;
    c4 = M41*v0 + M42*v1 + M43*v2 + M44*v3;

    derivative  = c2 + (c3 * 2.0 * t) + (3.0 * c4 * t * t)
    interpolant = c1 + ( c2 * t ) + ( c3 * t * t ) + ( c4 * t * t * t )
    return (interpolant, derivative)


def make_spline(p0, p1, p2, p3, roll0, roll1, width, segments):
    verts = []
    colors = []
    uvs = []
    trackColor = (255, 255, 255)
    width = width * 0.5
    for i in range(0, segments):
        t = i / float(segments)
        color = (255 * t, 0, 255 - (255 * t))
        (x0, dx0) = spline(t, p0.x, p1.x, p2.x, p3.x)
        (y0, dy0) = spline(t, p0.y, p1.y, p2.y, p3.y)
        (z0, dz0) = spline(t, p0.z, p1.z, p2.z, p3.z)
        at0 = euclid.Point3(x0, y0, z0)
        d0 = euclid.Vector3(dx0, dy0, dz0)
        d0.normalize()
        up = euclid.Vector3(0.0, 0.0, 1.0)
        roll = roll0 + (roll1-roll0) * (3.0*t*t-2.0*t*t*t)
        (forward, normal, perp) = project(d0, up)

        # left back
        leftOffset = perp * -width
        rollTransform = euclid.Matrix4.new_rotate_axis(roll * math.pi / 180.0, forward)
        leftOffset = rollTransform * leftOffset
        pos = at0 + leftOffset
        verts.append(pos)
        colors.append(color)
        uvs.append((0.0, 0.0))

        # right back
        rightOffset = perp * width
        rollTransform = euclid.Matrix4.new_rotate_axis(roll * math.pi / 180.0, forward)
        rightOffset = rollTransform * rightOffset
        pos = at0 + rightOffset
        verts.append(pos)
        colors.append(color)
        uvs.append((1.0, 0.0))

        t = (i+1) / float(segments)
        color = (255 * t, 0, 255 - (255 * t))
        (x1, dx1) = spline(t, p0.x, p1.x, p2.x, p3.x)
        (y1, dy1) = spline(t, p0.y, p1.y, p2.y, p3.y)
        (z1, dz1) = spline(t, p0.z, p1.z, p2.z, p3.z)
        at1 = euclid.Point3(x1, y1, z1)
        d1 = euclid.Vector3(dx1, dy1, dz1)
        d1.normalize()
        roll = roll0 + (roll1-roll0) * (3.0*t*t-2.0*t*t*t)
        (forward, normal, perp) = project(d1, up)

        # left front
        leftOffset = perp * -width
        rollTransform = euclid.Matrix4.new_rotate_axis(roll * math.pi / 180.0, d1)
        leftOffset = rollTransform * leftOffset
        pos = at1 + leftOffset
        verts.append(pos)
        colors.append(color)
        uvs.append((0.0, 1.0))

        # right front        
        rightOffset = perp * width
        rollTransform = euclid.Matrix4.new_rotate_axis(roll * math.pi / 180.0, d1)
        rightOffset = rollTransform * rightOffset
        pos = at1 + rightOffset
        verts.append(pos)
        colors.append(color)
        uvs.append((1.0, 1.0))

    faces = []        
    for i in range(0, segments):
        faces.append(((i*4), (i*4+1), (i*4+2)))
        faces.append(((i*4+2), (i*4+1), (i*4+3)))
        
    return { "verts" : verts, "faces" : faces, "colors" : colors, "uvs" : uvs }

def make_vertex(point):
    return euclid.Point3(point["x"], point["y"], point["z"])

def make_track():
    track_points = [ { "x":-8000.0, "y":-7.0, "z":2000.0},
                     { "x":-5000.0, "y":-7.0, "z":0000.0},
                     { "x":5000.0,  "y":-7.0, "z":0000.0},
                     { "x":8000.0,  "y":-7.0, "z":2000.0},
                     { "x":8000.0,  "y":-7.0, "z":4000.0},
                     { "x":5000.0,  "y":-7.0, "z":6000.0},
                     { "x":-5000.0, "y":-7.0, "z":6000.0},
                     { "x":-8000.0, "y":-7.0, "z":4000.0} ]
    track_width = 800.0
    track = { "verts" : [], "faces" : [], "uvs" : [], "colors" : [] }
    for i in range(1, len(track_points) + 1):
        i0 = (i - 1) % len(track_points)
        i1 = (i) % len(track_points)
        i2 = (i+1) % len(track_points)
        i3 = (i+2) % len(track_points)
        segment = make_spline(make_vertex(track_points[i0]),
                              make_vertex(track_points[i1]),
                              make_vertex(track_points[i2]),
                              make_vertex(track_points[i3]),
                              track_points[i1]["roll"] if "roll" in track_points[i1] else 0.0,
                              track_points[i2]["roll"] if "roll" in track_points[i2] else 0.0,
                              track_width, 30)
        vi0 = len(track["verts"])
        track = { "verts" : track["verts"] + segment["verts"],
                  "uvs" : track["uvs"] + segment["uvs"],
                  "colors" : track["colors"] + segment["colors"],
                  "faces" : track["faces"] + map(lambda x: (x[0] + vi0, x[1] + vi0, x[2] + vi0), segment["faces"]) }

#    print track
    return track

