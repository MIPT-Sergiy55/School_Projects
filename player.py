from pygame import sprite, image, Rect, time, transform

from block import Block, Triangle

class Player(sprite.Sprite):
    def __init__(self, vel, jump_vel, x, y, size, gravity_acceleration,
                 restart_time_await=100, rotation_speed=550):
        sprite.Sprite.__init__(self)
        self.size = size
        self.x_vel = vel
        self.jump_vel = jump_vel
        self.y_vel = 0
        self.x = x
        self.y = y
        self.y_acceleration = gravity_acceleration
        self.on_platform = False
        self.image = image.load("levels/player.bmp").convert_alpha()
        self.rotation = 0
        self.rect = Rect(x, y, size, size)
        self.restart_time_await = restart_time_await
        self.rotation_speed = rotation_speed

    def update(self, jumping, delta_time, level):
        self.y_vel += self.y_acceleration * delta_time
        self.y_vel = min(self.y_vel, self.jump_vel)
        self.collision_check(level)
        if self.on_platform:
            if jumping:
                self.y_vel = -self.jump_vel
            else:
                self.y_vel = 0
            self.rotation = 0
        else:
            self.rotation += -self.rotation_speed * delta_time
        self.x += self.x_vel * delta_time
        self.y += self.y_vel * delta_time
        self.rect.x = self.x
        self.rect.y = self.y
        self.collision_check(level)
        if self.on_platform:
            self.rotation = 0


    def collision_check(self, level):
        blocks = self.find_closest_blocks(level)

        self.on_platform = False

        for block_line in blocks[:1]:
            for block in block_line:
                if block is None:
                    continue
                collision_coords = sprite.collide_mask(self, block)
                if collision_coords is None:
                    continue
                else:
                    self.restart(level.height - level.block_size * 5)

        for block_line in blocks[-2:]:
            for block in block_line:
                if block is None:
                    continue
                collision_coords = sprite.collide_mask(self, block)
                if collision_coords is None:
                    continue
                if isinstance(block, Triangle):
                    self.restart(level.height - level.block_size * 5)
                else:
                    self.on_platform = True
                    self.y = block.rect.top - self.size + 1

    def find_closest_blocks(self, level):
        x, y = int(self.x), int(self.y)
        if x > level.width or y > level.height:
            self.restart(level.height - level.restart_height * level.block_size)
            return

        blocks = [[None for i in range(3)]
                  for j in range(3)]

        x_block = (x + level.block_size - 1) // level.block_size - 1
        y_block = (y + level.block_size - 1) // level.block_size - 1

        x_block = max(x_block, 0)
        y_block = max(y_block, 0)

        for i in range(3):
            for j in range(3):
                if i + y_block >= level.blocks_height or j + x_block >= level.blocks_width:
                    continue
                blocks[i][j] = level.blocks[i + y_block][j + x_block]

        return blocks

    def restart(self, height):
        time.wait(self.restart_time_await)
        self.teleport(0, height)

    def teleport(self, x, y):
        self.x = x
        self.y = y
        self.y_vel = 0
        self.on_platform = False

    def draw(self, window, camera):
        window.blit(transform.rotozoom(self.image, self.rotation, 1), camera.apply(self))
