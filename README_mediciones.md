Mediciones
============

En general para correr las mediciones estan los 3 ficheros .sh, dos corren mediciones relativas
a los threads por ficheros, y una corre mediciones para encontrar el maximo.

Los archivos .sh son simplemente un call a los scripts meditions.py y meditions_files.py , ambos aceptan
el parametro --help y permiten testear cualquier cosa que se quiera.

Se modifico el programa de C++ para que el output tenga el siguiente formato separado por comas:
resultado_maximo,tiempo_maximo,tiempo_files

Los casos de tests estan en las carpetas test_cases y son generados con el script .php en cada uno.

Los graficos se realizaron copiando los .csv correspondientes a las mediciones que nos interesan a
la carpeta jupyter/ y el Jupyter Notebook que hay en esa carpeta.


