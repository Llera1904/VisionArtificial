import numpy as np
import matplotlib.pyplot as plt
from matplotlib import pyplot
from scipy.spatial import distance

def euclideanDistance(v1, v2): # Calcula la distincia euclidiana 
    # return np.sqrt(np.sum((v1 - v2)**2))
    return distance.euclidean(v1, v2)

def crossProduct(v1, v2): # Producto cruz
    return np.cross(v1, v2)

def dotProduct(v1, v2): # Producto punto 
    return np.dot(v1, v2)

def f(l, x):
    y = -(l[0]*x + 1) / l[1]
    return y 

# Menu
while 1:
    print("Ingresa una opcion")
    print("1. Calcular producto punto entre dos lineas")
    print("2. Calcular punto de interseccion entre dos lineas")
    print("3. Calcular linea entre dos puntos")
    print("4. Calcular distancia entre dos puntos dadas 3 lineas")
    print("5. Salir")
    op = input("")

    if op == '5':
        break
    elif op != '3' and op != '2' and op != '1' and op != '4':
        print("\nOpcion invalida\n")
    else:
        if op == '1':    
            print("Ingresa las componentes (x, y) de los vectores")
            x1 = int(input("X1: "))
            y1 = int(input("y1: "))
            x2 = int(input("X2: "))
            y2 = int(input("y2: "))

            l1 = np.array([x1, y1, 1])
            l2 = np.array([x2, y2, 1])
            result = dotProduct(l1, l2)
            print("\nResultado producto punto:", result, "\n")

        elif op == '2':
            print("Ingresa las componentes (x, y) de los vectores")
            x1 = int(input("X1: "))
            y1 = int(input("y1: "))
            x2 = int(input("X2: "))
            y2 = int(input("y2: "))

            l1 = np.array([x1, y1, 1])
            l2 = np.array([x2, y2, 1])
            result = crossProduct(l1, l2)
            result = result / result[2]
            print("\nPunto de interseccion:", result, "\n")

            x = np.arange(-10, 10, 0.1)
            y1 = f(l1, x)
            y2 = f(l2, x)
            pyplot.plot(x, y1, color="green")
            pyplot.plot(x, y2, color="pink")

            pyplot.axhline(0, color="black")
            pyplot.axvline(0, color="black")

            # pyplot.xlim(-11, 11)
            # pyplot.ylim(-11, 11)

            plt.plot(result[0], result[1], color="purple", marker="o")

            pyplot.show()
            
        elif op == '3':
            print("Ingresa las componentes (x, y) de los vectores")
            x1 = int(input("X1: "))
            y1 = int(input("y1: "))
            x2 = int(input("X2: "))
            y2 = int(input("y2: "))

            m1 = np.array([x1, y1, 1])
            m2 = np.array([x2, y2, 1])
            result = crossProduct(m1, m2)
            result = result / result[2]
            print("\nLinea entre los puntos:", result, "\n")

            if x1 < x2:
                x = np.arange(x1, x2, 0.1)
            elif x2 < x1:
                x = np.arange(x2, x1, 0.1)

            y = f(result, x)
            pyplot.plot(x, y, color="orange")

            pyplot.axhline(0, color="black")
            pyplot.axvline(0, color="black")

            plt.plot(m1[0], m1[1], color="purple", marker="o")
            plt.plot(m2[0], m2[1], color="pink", marker="o")

            pyplot.show()
        elif op == '4':
            print("Ingresa las componentes (x, y) de los vectores")
            x1 = int(input("X1: "))
            y1 = int(input("y1: "))
            x2 = int(input("X2: "))
            y2 = int(input("y2: "))
            x3 = int(input("X3: "))
            y3 = int(input("y3: "))

            l1 = np.array([x1, y1, 1])
            l2 = np.array([x2, y2, 1])
            l3 = np.array([x3, y3, 1])

            intersection1 = crossProduct(l1, l3)
            intersection1 = intersection1 / intersection1[2]
            intersection2 = crossProduct(l2, l3)
            intersection2 = intersection2 / intersection2[2]

            distancePoints = euclideanDistance(intersection1, intersection2)
            print("\nDistancia entre intersecciones:", distancePoints, "\n")

            x = np.arange(-10, 10, 0.1)
            y1 = f(l1, x)
            y2 = f(l2, x)
            y3 = f(l3, x)
            pyplot.plot(x, y1, color="green")
            pyplot.plot(x, y2, color="pink")
            pyplot.plot(x, y3, color="orange")


            pyplot.axhline(0, color="black")
            pyplot.axvline(0, color="black")

            # pyplot.xlim(-11, 11)
            # pyplot.ylim(-11, 11)

            plt.plot(intersection1[0], intersection1[1], color="purple", marker="o")
            plt.plot(intersection2[0], intersection2[1], color="purple", marker="o")

            pyplot.show()





            

            









