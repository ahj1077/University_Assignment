import numpy as np
import imageio
save_array = []

for i in range(0, 10):
    filename = str(i) + '.png'
    img = 255.0 - (imageio.imread(filename, as_gray=True))
    reimg = np.append(np.array(i), np.array(img).reshape(1, 784))
    reimg.reshape(1, 785)
    save_array.append(reimg)

np.savetxt("MyDataSet.csv", save_array, fmt='%d', delimiter=',')


