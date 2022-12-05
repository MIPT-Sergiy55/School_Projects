from pygame import sprite, image, Rect


class Camera(object):
    def __init__(self, function, width, height):
        self.camera_function = function
        self.state = Rect(0, 0, width, height)

    def apply(self, target):
        return target.rect.move(self.state.topleft)

    def update(self, target, level_width, level_height):
        self.state = self.camera_function(self.state, target.rect, level_width, level_height)


def camera_configure(camera, target_rect, level_width, level_height):
    left, top = target_rect.topleft
    x, y, w, h = camera
    left -= w / 2
    top -= h / 2

    left = max(0, left)
    top = max(0, top)
    left = min(level_width - w, left)
    top = min(level_height - h, top)

    return Rect(-left, -top, w, h)
