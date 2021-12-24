# Jak skorzystać z SFX-Engine w MAD Pascalu

* Wejdź na stronę repozytorium [SFX-Engine](https://github.com/GSoftwareDevelopment/SFX-Engine) i ściągnik plik ZIP, klikając w przycisk **CODE** (jak na poniższym obrazku)

  ![](imgs/git_download.png)

- Paczkę rozpakuj gdzieś na swoim dysku

- Katalog `sfx_engine` należy skopiować do katalogu swojego projektu.

  Przykładowa struktura katalogu projektu, może wyglądać tak:

~~~txt
+ /Projekt
|
+- /sfx_engine
|  |  sfx_engine.conf.inc
|
+- /music
|  | {pliki wygenerowane przez smm-conv}
|
+-- main.pas
|   
~~~

- w głównym pliku swojego programu `main.pas` zadeklaruj ścieżkę dostępu do biblioteki `sfx_engine` i `music` oraz utwórz deklarację biblioteki w sekcji `uses`, np.

~~~pascal
{$librarypath './sfx_engine/'}
{$librarypath './music/'}

uses SFX_API, atari;
~~~

- plik `sfx_engine.conf.inc` należy dostosować do własnych potrzeb lub skasować :smile:



## Wykorzystanie programu `smm-conv`

Przed użyciem, należy skompilować program, ale z racji świąt, mam dla Was mały prezent. 

[Skompilowana wersja 1.0.1 programu]

* Skopiuj ściągnięty plik do katalogu `/music` . (Linuxiaże) nadaj mu prawa do uruchomienia.

Załóżmy, że nasz plik z muzyką nazywa się `music.smm` i jest on umieszczony w katalogu `/music` projektu.

W linii komend wpisz:

~~~bash
./smm-conv music.smm music.asm -reduce:all -reindex:all -MC -MR -Ao:0x7000 -Aa:0
~~~

Znaczenie parametrów:

- `music.smm` - nazwa pliku źródłowego (nasza muzyczka)
- `music.asm` - nazwa pliku wyjściowego - dane w assemblerze
- `-reduce:all` - wyłącza z pliku wyjściowego nieużywane definicje SFXów oraz TABów (opcja `all`)
- `-reindex:all` - układa kolejno indeksy definicji SFXów oraz TABów

Powyższe dwie opcje redukują rozmiar pliku wynikowego oraz zapotrzebowanie na pamięć w **ATARI**.

- `-MC` - generuje plik konfiguracyjny dla **SFX_API** `sfx_engine.conf.inc`
- `-MR` - generuje plik z definicją zasobów dla **MAD Pascala** `resource.rc`
- `-Ao:0x7000` - określa adres (tzw. globalny) dla generowanyh danych w assemblerze
- `-Aa:0` - powoduje wyłączenie buforowania audio (rejestrów **POKEY**) w pliku konfiguracyjnym `sfx_engine.conf.inc`

Po więcej szczegółów nt. konwertera odsyłam do pliku README.md w programie **SMM-CONV**.



Uruchomienie powyższej komendy, spowoduje wygenerowanie następujących plików w katalogu `/music`

~~~ascii
+- /music
   | music.asm
   | resource.rc
   } sfx_engine.conf.inc
~~~



Ważną rzeczą, jaką należy dokonać to ustawienie ścieżki w wygenerowanym pliku `music/resource.rc`, gdyż **kompilator MAD Pascal odwołuje się względem kompilowanego pliku głównego**, nie zaś pliku zasobu który jest dodany do programu.

~~~pascal
SFX_ORG rcasm 'music/music.asm';
~~~

W pliku głównym projektu dodać należy jeszcze wczytanie pliku zasobu muzyki. 

~~~pascal
{$librarypath './sfx_engine/'}
{$librarypath './music/'}

uses SFX_API, atari;

{$r "music/resource.rc"}
~~~

**UWAGA!** W przypadku rozdzielenia danych za pomocą przełącznika `-Ad:` (w konwerterze `smm-conv`) należy, wczytanie pliku zasobu `{$r "music/resource.rc"}`  umieścić <u>na samym początku programu</u>, przed wywołaniem innych zasobów.



I to tyle - można się cieszyć muzyką z programu **SFX Music Maker** (aka **SFX-Tracker**) w swoim projekcie :)
