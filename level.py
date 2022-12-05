from block import Block, Triangle
from camera import Camera

class Level:
    def __init__(self, filename, restart_height=5):
        file = open(filename, 'r')
        self.width = int(file.readline())
        self.height = int(file.readline())
        self.block_size = int(file.readline())
        self.blocks = [[None for i in range(self.width)]
                       for j in range(self.height)]

        for i in range(self.height):
            string = file.readline()
            for j in range(self.width):
                letter = string[j]
                if letter == 'b':
                    self.blocks[i][j] = Block(j * self.block_size, i * self.block_size,
                                              self.block_size, self.block_size)
                if letter == 't':
                    self.blocks[i][j] = Triangle(j * self.block_size, i * self.block_size,
                                                 self.block_size, self.block_size)

        self.blocks_width = self.width
        self.blocks_height = self.height
        self.width *= self.block_size
        self.height *= self.block_size
        self.restart_height = restart_height

    def draw(self, window, camera):
        for block_line in self.blocks:
            for block in block_line:
                if block is None:
                    continue
                block.draw(window, camera)
