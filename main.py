import pygame
import sys

from camera import camera_configure, Camera
from player import Player
from level import Level

pygame.init()

display_flags = pygame.RESIZABLE

w_width, w_height = 1920, 1080

window = pygame.display.set_mode((w_width, w_height), display_flags)

pygame.display.set_caption("Deometry Gash")

framerate = 120

player = Player(400, 800, 0, 300, 30, 5000)

level = Level("./levels/first_test")

timer = pygame.time.Clock()

jump_button = False

camera = Camera(camera_configure, w_width, w_height)

if __name__ == '__main__':
    while True:

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                sys.exit()
            if event.type == pygame.KEYDOWN:
                if event.unicode == ' ' or event.unicode == 'w' or event.key == pygame.K_UP:
                    jump_button = True
            if event.type == pygame.KEYUP:
                if event.unicode == ' ' or event.unicode == 'w' or event.key == pygame.K_UP:
                    jump_button = False

        window.fill((0, 0, 0))
        player.update(jump_button, 1 / framerate, level)
        camera.update(player, level.width, level.height)
        level.draw(window, camera)
        player.draw(window, camera)
        pygame.display.update()
        timer.tick(framerate)
