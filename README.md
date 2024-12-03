# Парсер книжного магазина лабиринт для получения данных о книгах

Данные со страниц собраны в таблицу вида:

| id  | typeObject | groupOfType  | underGroup | genres         | bookName               | imgUrl                | age  | authors            | publisher           | datePublisher | series         | bookGenres      | allPrice | myPrice | sale | isbn          | pages | pageType | weight | da  | db  | dc  | box  | covers | decoration | illustrations | rate | rateSize | annotation                          |
|-----|------------|--------------|------------|----------------|------------------------|-----------------------|------|--------------------|---------------------|---------------|----------------|----------------|----------|---------|------|---------------|-------|----------|--------|-----|-----|-----|------|--------|------------|----------------|------|----------|-------------------------------------|
| 1   | Book       | Fiction      | Novel      | Adventure       | The Great Adventure     | http://example.com/img1.jpg | 12+  | John Doe           | Example Publisher    | 2023-01-01   | Adventure Series | Fiction         | 500      | 400     | 20%  | 978-3-16-148410-0 | 350   | Hardcover  | 0.5kg  | 12  | 4   | 3   | Yes  | Yes    | Yes        | Yes            | 4.5  | 100      | This is a thrilling adventure story. |
| 2   | Book       | Non-fiction  | Biography  | History         | The Life of Greatness   | http://example.com/img2.jpg | 15+  | Jane Smith         | Another Publisher    | 2023-02-01   | N/A            | Biography       | 600      | 480     | 20%  | 978-1-23-456789-7 | 300   | Paperback  | 0.4kg  | 15  | 5   | 4   | No   | No     | No         | Yes            | 4.2  | 150      | An inspiring biography of a great leader. |
| 3   | Book       | Children     | Storybook  | Fantasy         | The Magical Forest      | http://example.com/img3.jpg | 6+   | Alice Wonderland    | Kids Publisher       | 2023-03-01   | Fairy Tales    | Children        | 300      | 250     | 17%  | 978-0-12-345678-9 | 50    | Hardcover  | 0.3kg  | 5   | 3   | 2   | No   | Yes    | Yes        | No             | 4.8  | 200      | A charming story for young readers.    |

~~[Ссылка на таблицу.]() пока еще не готова~~ Для github слишком большая.

Таблица с необработанными данными, в чистом виде.

## Комментарий

Ориентировочное время выполнения на `AMD Ryzen 9 5950X` составил ~18 ч. Запросы создают большие задержки.

## Сборка & запуск

**У вас должен быть установленн [libcurl](https://curl.se/libcurl/c/libcurl-tutorial.html) для С++**

```cmd
cmake .
```

```cmd
make
```

```cmd
./main.out
```
