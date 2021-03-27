CC=gcc
TARGET=class_parser.a
 
all:
	$(CC) main.c class_reader.c class_reader.h pretty_printer.c pretty_printer.h -o $(TARGET)
 
clean:
	rm $(TARGET)