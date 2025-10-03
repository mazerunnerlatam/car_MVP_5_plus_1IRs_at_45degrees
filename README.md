# car_MVP_5_plus_1IRs_at_45degrees
Los scripts de este repositorio estan considerados para una placa de Arduino Mega
## Testeo de 5 + 1 sensores
Este repositorio contiene el código para verificar el funcionamiento de los sensores donde se incluye al sexto sensor en negación para que coincida.

## Código para maze runner
El código para la resolución del laberinto utiliza funciones básicas (avanzar, deteccion bifurcacion, deteccion dead end, etc.) para que sea facil de implementar un algoritmo de resolución propio. Actualmente resuelve el laberinto con fuerza bruta, girando a la derecha en cada bifurcación.

## Video y comportamiento del auto
En el video se puede ver el comportamiento del auto. En las bifurcaciones se puede dar cuenta que el auto lo esta detectando correctamente dado que la rutina para doblar a la derecha o izquierda es ininterrumpido, no es afectado por las pausas que hace el auto en los otros movimientos. En estas bifurcaciones se pueden observar movimientos continuos hasta que el sensor central sea el unico que detecte la linea.
