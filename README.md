## Вывод данных пула констант
Программа для парсинга .class файлов и извлечения из них пула констант. Примером работы являлся инструмент `javap`.  

Сборка осуществляется посрдством утилиты `make`:
```
$ make
gcc main.c class_reader.c class_reader.h pretty_printer.c pretty_printer.h -o class_parser.a
```

Пример запуска:
```
$ ./class_parser.a examples/MyTcpForwardServer.class
minor version: 0
major version: 55

#1 = MethodRef          #39.#77         // java/lang/Object.<init>:()V
#2 = Class              #78             // org/apache/commons/cli/Options
#3 = MethodRef          #2.#77          // org/apache/commons/cli/Options.<init>:()V
#4 = Class              #79             // org/apache/commons/cli/Option
#5 = String             #80             // h
#6 = String             #81             // destination-host
#7 = String             #82             // host to forward
#8 = MethodRef          #4.#83          // org/apache/commons/cli/Option.<init>:(Ljava/lang/String;Ljava/lang/String;ZLjava/lang/String;)V
#9 = String             #84             // p
#10 = String            #59             // port
#11 = String            #85             // port to listen
#12 = String            #86             // hp
#13 = String            #87             // host-port
#14 = String            #88             // host port
#15 = MethodRef         #4.#89          // org/apache/commons/cli/Option.setRequired:(Z)V
#16 = MethodRef         #2.#90          // org/apache/commons/cli/Options.addOption:(Lorg/apache/commons/cli/Option;)Lorg/apache/commons/cli/Options;
#17 = Class             #91             // org/apache/commons/cli/DefaultParser
#18 = MethodRef         #17.#77         // org/apache/commons/cli/DefaultParser.<init>:()V
...
```

По примеру запуска видно, что мне удалось воссоздать точную копию вывода пула констант как из `javap`.

В папке `examples` можно найти парочку `.class` файлов которые я позаимствовал из лаб 5 семестра.
