import cv2
import matplotlib.pyplot as plt
import numpy as np 
import random
import math
from scipy.spatial import distance

# image = cv2.imread("Jit1.jpg")
# rows = image.shape[0]
# cols = image.shape[1]
# pixels = rows * cols # Pixeles totales
# print("Tamaño imagen: " + str(rows) + " x " + str(cols))
# print("Total pixeles:", pixels, "\n")

# valuePixels = np.float32(image.reshape((-1, 3)).copy())
# criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 20, 1.0)
# _, labels, centers = cv2.kmeans(valuePixels, 3, None, criteria, 10, cv2.KMEANS_RANDOM_CENTERS) 

# center = np.uint8(centers)
# imageClustering = center[labels.flatten()]
# imageClustering = imageClustering.reshape((image.shape))

# imageSeg = image.reshape((-1, 3)).copy()
# removeCluster = [0, 2, 3]
# for i in removeCluster:
#     imageSeg[labels.flatten() == i] = np.uint8(np.array([0, 0, 0]))
# imageSeg = imageSeg.reshape((image.shape))

# # cv2.imshow("imageClustering", imageClustering)
# # cv2.imshow("imageSeg", imageSeg)
# # cv2.waitKey(0)

# image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
# imageClustering = cv2.cvtColor(imageClustering, cv2.COLOR_BGR2RGB)
# imageSeg = cv2.cvtColor(imageSeg, cv2.COLOR_BGR2RGB)

# fig, axs = plt.subplots(1, 3, figsize=(20, 10))
# axs[0].imshow(image)
# axs[1].imshow(imageClustering)
# axs[2].imshow(imageSeg)

# for a in axs:
#     a.set_axis_off()

# plt.show()

for i in range(1, 6):
  print(i)
