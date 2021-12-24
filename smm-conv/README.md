# Konwerter SFX Music Maker

Program, który konwertuje pliki muzyczne utworzone przez **SFX-Tracker** do pliku źródłowego w asemblerze - pozwala to na połączenie danych muzycznych z programem.

## Kompilacja

Wymagane jest środowisko **Free Pascal Compiler**.

Aby skompilować źródła programu do pliku wykonywalnego, użyj:

~~~bash
fpc -Mdelphi -v -O3s -Xs -FE./bin/ ./src/smm-conv.pas
~~~

Skompilowany program będzie znajdował się w katalogu `bin`.

## Linia poleceń

~~~bash
./smm-conv nazwa_pliku_źródłowego nazwa_pliku_wyjściowego [parametry]
~~~

**Istotne jest to, by w linii komend podać najpierw nazwy plików, a dopiero później parametry**

## Optymalizacja

Należy określić sposób optymalizacji, gdyż domyślnie konwerter jej niedokonuje.

### `-reduce:[SFXonly|TABonly|All]`
#### `-R:[SFXonly|TABonly|All]`

wyłącza z konwersji nieużywane definicje SFXów i/lub TABów, dzięki czemu, rozmiar użtej pamięci może być mniejszy.

### `-reindex:[SFXonly|TABonly|All]`
#### `-I:[SFXonly|TABonly|All]`

Powoduje poukładanie indeksów SFXów i/lub TABów kolejno po sobie, dzięki czemu, tablica wskaźników są bardziej zwięzłe i mogą zabierać mniej pamięci.

## Generowanie plików dla `SFX_API`

Konwerter może wygenerować potrzebne pliki dla biblioteki `SFX_API` przeznaczonej dla MAD Pascala.

### `-makeconfing`
#### `-MC`

Generuje plik konfiguracyjny dla silnika SFX do użycia z biblioteką **SFX-API dla MAD Pascala**.

Nazwa generowanego pliku: `sfx_engine.conf.inc`

Nazwa ta nie może by zmieniana, ze względu na ścisłe powiązanie z biblioteką `SFX_API`.

Jeżeli plik wynikowy `nazwa_pliku_wyjściowego`, będzie zawierał ścieżkę, plik konfiguracyjny, zostanie umieszczony w tej ścieżce!

### `-makeresource[:nazwa_pliku]`
#### `-MR[:nazwa_pliku]`

Generuje plik resource (.rc) dla MAD Pascala.

Domyślna nazwa generowanego pliku: `resource.rc`. Można ją zmienić, ustalają ją po znaku dwukropka `:` w przełączniku, np.

~~~bash
./smm-conv music.smm music.asm -MR:music.rc
~~~

Jeżeli plik wynikowy `nazwa_pliku_wyjściowego`, będzie zawierał ścieżkę, plik zasobów, zostanie umieszczony w tej ścieżce!

## Alokacja danych w pamięci ATARI

### `-org:address`
#### `-Ao:address`

Określa globalny adres konwersji danych. Domyślnie jest to $A000.

**Możesz użyć notacji szesnastkowej.** Aby to zrobić, należy poprzedzić właściwą wartość znakiem dolara `$...` lub przedrostkiem `0x...`', np. `$9000 0xC000, xC000`.

**UWAGA** W niektórych systemach znak `$` może być traktowany jako oznaczenie zmiennej lub parametru. W takiej sytuacji, należy cały przełącznik umieścić w apostrofach, np.

~~~bash
./smm-conv musci.smm music.asm -MC -MR '-org:$7000' '-Ad:$C000'
~~~

## Rejestry i bufory **SFX-Engine**

### `-audiobuffer:address`
#### `-Aa:address`

Określa adres początkowy dla bufora audio.

### `-regs:address`
#### `-Ar:address`

Określa adres początkowy dla głównych rejestrów silnika SFX. **Adres MUSI BYĆ w obszarze strony zerowej.**

### `-chnregs:address`
#### `-Ac:address`

Określa adres początkowy dla rejestrów roboczych kanałów silnika SFX.

## Dane SFXów i TABów

Ustawienie jednego z poniższych parametrów, nie zmienia globalnego adresu ustalonego za pomocą przełącznika `-org` lub `-Ao`. Adres ten, będzie kontynuowany, jeżeli któryś parametrów niezostanie ustalony.

**Parametry alokacji danych, po ich ustaleniu, będą generowały osobne pliki!**

### `-notetable:address`
#### `-An:address`

Określa adres początkowy dla danych definicji tablicy nut.

**Adres jest wyrównany do strony pamięci.**

### `-sfxmodetable:address`
#### `-Am:address`

Określa adres początkowy dla informacji o trybach modulaji SFXów.

### `-sfxtable:address`
#### `-As:address`

Określa adres początkowy dla tabeli adresów danych definicji SFXów.

### `-tabtable:address`
#### `-At:address`

Jak wyżej, ale dla danych definicji TABów.

### `-song:address`
#### `-Ag:address`

Określa adres początkowy dla danych definicji utworu.

### `-data:address`
#### `-Ad:address`

Określa adres początkowy dla danych definicji SFXów oraz TABów.

---

## The MIT License (MIT)

Copyright © 2021 GSD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
