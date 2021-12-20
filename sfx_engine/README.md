#

- Katalog `sfx_engine` należy skopiować do katalogu swojego projektu, gdzie trzymasz biblioteki.
- plik konfiguracyny `sfx_engine.conf.inc` należy przenieść do katalogu, gdzie mieści się główny program.
- przykładowa struktura katalogu projektu, wyglądać tak:

~~~
+ Projekt
|   |  /sfx_engine
|   |  /data
|   |   start.pas
|   |   sfx_engine.conf.inc
~~~

- w głównym pliku swojego programu zadeklaruj ścieżkę dostępu do biblioteki `sfx_engine` oraz utwórz deklarację biblioteki w sekcji `uses`, np.

start.pas
~~~pascal
{$librarypath './sfx_engine/'}

uses SFX_API, atari;
~~~

- plik `sfx_engine.conf.inc` należy dostosować do własnych potrzeb lub, korzystając z dołączonego programu użytkowego `smm-conv`, utworzyć i zastąpić.
- dane stworzone programem `smm-conv` należy umieścić w przykładowym katalogu `/data`