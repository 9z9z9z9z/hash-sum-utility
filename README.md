# README.md

## Описание

Этот проект представляет собой простую утилиту для вычисления хеш-суммы файла. Хеш-сумма файла представляет собой уникальный идентификатор его содержимого, вычисляемый с использованием криптографических алгоритмов. Эта утилита может быть полезна для проверки целостности файлов или сравнения их содержимого.

## Поддерживаемый метод вычисления хеш-суммы

Для вычисления хеш-суммы используется алгоритм **CRC32-IEEE**.

## Тестирование

Для запуска теста прошу написать:

```.\{имя исполняемого файла} -test ```

Для тестирования работы утилиты был взят файл **table.json**, который содержит информацию о файлах: 
1) elden-ring-nawpic-11.jpg
2) config.json
3) main_contorls.blk
4) Несуществующий файл test.txt (в случае, если фала нет, он просто игнорируется)

## Билд утилиты:

```mkdir build```
```cd build```
```cmake ..```
```make```
