import euclid

# rule based gen
# Each shape is a 2-tuple: (verts, faces)
shapes = { 'box' : ([ \
    # Box Verts
    (0, 0, 0),
    (0, 0, 1),
    (0, 1, 0),
    (0, 1, 1),
    (1, 0, 0),
    (1, 0, 1),
    (1, 1, 0),
    (1, 1, 1) ], [ \
    # Box Faces
    (0, 1, 4), (1, 5, 4), # Front
    (2, 3, 6), (3, 7, 6), # Back
    (0, 2, 6), (6, 4, 0), # Top
    (1, 3, 7), (7, 5, 1), # Bottom
    ]) }

# Build a tube shape programmatically
if True:
    slices = 16
    circle = []
    for slice in xrange(slices):
        theta = 2.0 * pi * slice / slices
        circle.append((cos(theta), sin(theta)))
    faces = []
    for s in xrange(slices):
        s0 = s*2
        s1 = (s0+1) % (2*slices)
        s2 = (s0+2) % (2*slices)
        s3 = (s0+3) % (2*slices)
        faces.append((s2, s1, s0))
        faces.append((s2, s3, s1))
    verts = []
    for p in circle:
        verts.append((0.0, p[0], p[1]))
        verts.append((1.0, p[0], p[1]))
    shapes['tubex'] = (verts, faces)
    verts = []
    for p in circle:
        verts.append((p[0], 0.0, p[1]))
        verts.append((p[0], 1.0, p[1]))
    shapes['tubey'] = (verts, faces)
    verts = []
    for p in circle:
        verts.append((p[0], p[1], 0.0))
        verts.append((p[0], p[1], 1.0))
    shapes['tubez'] = (verts, faces)
    
def pick_rule(rule_names, rules):
    n = len(rules)
    sum = 0
    for r in rule_names:
        rule = rules[r]
        weight = int(rule["weight"] if "weight" in rule else 1.0)
        sum = sum + weight
        tuples.append((r, weight))
    n = random.randint(0, sum - 1)
    for (r, weight) in tuples:
        if (n < weight):
            break
        n = n - weight
    return rules[r]
        
def parse_xform(xform_string):
    matrix = Matrix4.new_identity()
    tokens = xform_string.split(' ')
    t = 0
    while t < len(tokens) - 1:
        command, t = tokens[t], t + 1        
        # Translation
        if command == 'tx':
            x, t = float(tokens[t]), t + 1
            matrix *= Matrix4.new_translate(x, 0, 0)
        elif command == 'ty':
            y, t = float(tokens[t]), t + 1
            matrix *= Matrix4.new_translate(0, y, 0)
        elif command == 'tz':
            z, t = float(tokens[t]), t + 1
            matrix *= Matrix4.new_translate(0, 0, z)
        elif command == 't':
            x, t = float(tokens[t]), t + 1
            y, t = float(tokens[t]), t + 1
            z, t = float(tokens[t]), t + 1
            matrix *= Matrix4.new_translate(x, y, z)
        # Rotation
        elif command == 'rx':
            theta, t = radians(float(tokens[t])), t + 1
            matrix *= Matrix4.new_rotatex(theta)
        elif command == 'ry':
            theta, t = radians(float(tokens[t])), t + 1
            matrix *= Matrix4.new_rotatey(theta)
        elif command == 'rz':
            theta, t = radians(float(tokens[t])), t + 1
            matrix *= Matrix4.new_rotatez(theta)
        # Scale
        elif command == 'sx':
            x, t = float(tokens[t]), t + 1
            matrix *= Matrix4.new_scale(x, 1, 1)
        elif command == 'sy':
            y, t = float(tokens[t]), t + 1
            matrix *= Matrix4.new_scale(1, y, 1)
        elif command == 'sz':
            z, t = float(tokens[t]), t + 1
            matrix *= Matrix4.new_scale(1, 1, z)
        elif command == 'sa':
            v, t = float(tokens[t]), t + 1
            x, y, z = v, v, v
            matrix *= Matrix4.new_scale(x, y, z)
        elif command == 's':
            x, t = float(tokens[t]), t + 1
            y, t = float(tokens[t]), t + 1
            z, t = float(tokens[t]), t + 1
            matrix *= Matrix4.new_scale(x, y, z)
        else:
            print "unrecognized transformation: '%s' at position %d in '%s'" % (command, t, xform_string)
    return matrix

def process_rule(rule, rules, depth, verts, faces, matrix = euclid.Matrix4.new_identity()):
    if (depth < 1):
        return
    for statement in rule:
        xform = parse_xform(statement["transforms"])
        count = statement["count"]
        for n in xrange(count):
            matrix *= xform
            if "call" in statement:
                next_rule = pick_rule(statment["call"]["rules"], rules)
                cloned_matrix = matrix.copy()
                process_rule(rules[next_rule], rules, depth - 1, verts, faces, cloned_matrix)
            elif "instance" in statment:
                shape_name = statement["instance"]["shape"]
                shape_verts, shape_faces = shapes[shape_name]
                n  = len(verts)
                for v in shape_verts:
                    transformed_vert = matrix * euclid.Point3(*v)
                    verts.append(transformed_vert[:])
                for i, j, k in shape_faces:
                    faces.append((i+n, j+n, k+n))
                else:
                    print "Malformed statement"
                    return
                
def surface(rules, slices = 32, stacks = 32):
    verts, faces = [], []
    max_depth = rules["max_depth"]
    r = rules["entry"]
    process_rule(r, rules, max_depth, verts, faces)
    return { "vertices" : verts, "faces" : faces }

if __name__ == "__main__":        
    rules = { "max_depth" : 20,
              "entry" : [ { "call" : { "rules" : [ "arm1", "arm0" ] }, "transforms" : "ry 3.6" } ],

              "arm0" : [ { "call" : { "rules": [ "arm1", "arm0" ] }, "transforms" : "sa 0.9 rz 6 tx 1" },
                         { "instance" : { "shape" : "box" }, "transforms" : "s 1 0.2 0.5" } ],
              
              "arm1" : [ { "call" : { "rules" : [ "arm1", "arm0" ] }, "transforms" : "sa 0.9 rz -6 tx 1" },
                         { "instance" : { "shape" : "box" }, "transforms" : "s 1 0.2 0.5" } ] }
    surface[rules]
              
