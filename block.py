from pygame import sprite, image, Rect


class Block(sprite.Sprite):
    def __init__(self, x, y, width, height):
        sprite.Sprite.__init__(self)
        self.image = image.load("levels/block.png").convert_alpha()
        self.rect = Rect(x, y, width, height)

    def draw(self, window, camera):
        window.blit(self.image, camera.apply(self))

class Triangle(Block):
    def __init__(self, x, y, width, height):
        sprite.Sprite.__init__(self)
        self.image = image.load("levels/triangle.png").convert_alpha()
        self.rect = Rect(x, y, width, height)
