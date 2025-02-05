from pyglet.text import Label

# GUI Components
class Frame:
    def __init__(self, batch):
        self.batch = batch
        self.elements = []

    def add(self, element):
        self.elements.append(element)

    def draw(self):
        self.batch.draw()
        for element in self.elements:
            element.draw()

class SimpleLabel:
    def __init__(self, text, x, y, batch, name=None):
        self.label = Label(text, font_size=14, x=x, y=y, anchor_x='left', anchor_y='top', batch=batch)
        self.name = name

    def set_text(self, text):
        self.label.text = text

    def draw(self):
        self.label.draw()