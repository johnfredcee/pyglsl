
import simplui


class Node(simplui.Dialogue):
    nodeid = 42
    def __init__(self, title, func, ins, outs, **kwargs):
        self.info = {"title": title, "function": func, "inputs": ins, "outputs": outs}
        left_pins = []
        right_pins = []
        self.ins = self.info["inputs"]
        for i in ins:
            i["id"] = self.nodeid + 1
            i["gui"] = simplui.Label(i["field"] % i["value"], i["name"])
            left_pins.append(i["gui"])
            self.ins[i["name"]] = i
        self.outs = self.info["outputs"]
        for o in outs:
            o["id"] = self.nodeid + 1
            o["gui"] = simplui.Label(o["field"] % o["value"], o["name"])
            right_pins.append(o["gui"])
            self.outs[o["name"]] = i
        self.in_content = simplui.VLayout(autosizex=True, children=left_pins)
        self.out_content = simplui.VLayout(autosizex=True, children=right_pins)
        kwargs["content"] = simplui.HLayout(children=[self.in_content, self.out_content])
        super(Node, self).__init__(title, kwargs)

    def update(self):
        for i in self.in_content:
            input = self.info["inputs"][i.name]
            i.text = input["field"] % input["value"]
        for o in self.out_content:
            output = self.info["outputs"][o.name]
            o.text = output["field"] % output["value"]

    def evaluate(self):
        params = []
        for i in self.info["inputs"]:
            params.append(i["value"])
        result = self.info["function"](*params)
        for r in result:
            self.info["outputs"]["value"] = r

    def input(self, name):
        return self.ins[name]

    def output(self, name):
        return self.outs[name]

    def input_pos(self, name):
        i = self.info["inputs"]["gui"]
        return (i.x, i.y + (i.h / 2))

    def output_pos(self, name):
        o = self.info["outputs"]["gui"]
        return (o.x + o.w, o.y + (o.h / 2))
    
