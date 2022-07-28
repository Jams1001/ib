run1:
	gcc main.c -o salida
	./salida
	rm salida
	echo "Waiting for 10 seconds..."
	sleep 10
	eog ./output/bintensity.bmp
	echo "Task Completed, Good bye"