import cv2
import matplotlib.pyplot as plt
import numpy as np
import math

def coordinatesImage(event, x, y, flags, param):
    if event == cv2.EVENT_LBUTTONDOWN:
        coordinatesX.append(x)
        coordinatesY.append(y)

def rotateImage(image, rows, cols, matrixEuclidianaInv):
    rotatedImage = np.zeros((rows, cols, 3), dtype=np.float32) # Imagen rotada
    for i in range(rows):
        for j in range(cols):
            matrixXY = np.array([i, j, 1], dtype=np.int32) # [x, y, 1]
            matMul = np.round(np.matmul(matrixEuclidianaInv, matrixXY)) # Multiplica las matrices
            matMul = matMul / matMul[2]
            x = np.int32(matMul[0])
            y = np.int32(matMul[1])
            if (x >= 0 and x < rows) and (y >= 0 and y < cols):
                rotatedImage[i, j] = image[x, y]

    return rotatedImage

image = cv2.imread("control.jpg") # Lee imagen
rows = image.shape[0]
cols = image.shape[1]
pixels = rows * cols # Pixeles totales
print("Tamaño imagen: " + str(rows) + " x " + str(cols))
print("Total pixeles:", pixels, "\n")

# Pide pixeles directo en la imagen
cv2.namedWindow("Imagen")
cv2.setMouseCallback("Imagen", coordinatesImage)
coordinatesX = []
coordinatesY = []
while True:
    cv2.imshow("Imagen",image)
    cv2.waitKey(1)

    if len(coordinatesX) == 4:
        break
cv2.destroyAllWindows()
# print(coordinatesX, coordinatesY)

xMin = min(coordinatesX)
xMax = max(coordinatesX)
yMin = min(coordinatesY)
yMax = max(coordinatesY)
# print("xMin:", xMin)
# print("xMax:", xMax)
# print("yMin:", yMin)
# print("yMax:", yMax)

croppedImage = image[yMin:yMax, xMin:xMax]
rows = croppedImage.shape[0]
cols = croppedImage.shape[1]

# Parametros
angle = 45
angleRadians = (angle * math.pi) / 180 # Angulo en radianes
t = np.array([50, 20, 1], dtype=np.float32) # Traslacion

# Matriz para trasladar 
T = np.array([
    [1, 0, int(rows / 2)],
    [0, 1, int(cols / 2)],
    [0, 0, 1]
])

# Crea matriz R (Rotacion)
matrixR = np.array([
    [math.cos(angleRadians), -1 * math.sin(angleRadians)],
    [math.sin(angleRadians), math.cos(angleRadians)],
    [0, 0]
])
# print("\nMatriz R:\n", matrixR)

# Transformacion euclidiana
matrixEuclidiana = T @ np.append(matrixR, [[t[0]], [t[1]], [t[2]]], axis=1) @ np.linalg.inv(T)
matrixEuclidianaInv = np.linalg.inv(matrixEuclidiana)
print("Matriz transformacion euclidiana:\n", matrixEuclidiana)
print("\nMatriz transformacion euclidiana inversa:\n", matrixEuclidianaInv)

# rotatedImage = rotateImage(image, rows, cols, matrixEuclidianaInv)
rotatedImage = rotateImage(croppedImage, rows, cols, matrixEuclidianaInv)
rotatedImage = np.uint8(rotatedImage)

cv2.imshow("Imagen original", image)
cv2.imshow("Imagen recortada", croppedImage)
cv2.imshow("Imagen rotada", rotatedImage)
cv2.waitKey(0)