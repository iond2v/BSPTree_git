Uses GLSDK and Freetype libraries. These are required to build.
http://glsdk.sourceforge.net/docs/html/index.html ,namely components GL Load,
GL Utility, OpenGL Mathematics and FreeGLUT.
FreeType http://www.freetype.org.

--translation pending

Pokyny ke spuštění a ovládání aplikace
   Lze spustit buď z projektu nebo samostatně. 
   Po spuštění si lze zobrazit nápovědu pomocí klávesy "h".
   Lze si zobrazit aktuální informace o běhu pomocí klávesy "i".

   Pohyb pomocí kláves WSAD
   Držení mezery umístí kameru do výšky, takže si lze lépe prohlédnout okolí
   P zapne zobrazení portálů mezi listy stromu
   1 zapne kreslení BSP
   2 BSP s occlusion query
   3 PVS
   4 PVS s occluion query
   5 zapne kreslení BSP po úrovních
      + jde o úroveň níže
      - jde o úroveň výše
   6 kreslení BSP odzadu bez depth testu
   7 kreslení PVS odzadu bez depth testu
   R restartuje časovač pro nahrávání pohybu kamery
   E přidá pozici kamery s časem do souboru s pohybem kamery bludiště
   G odstartuje nahraný pohyb kamery 


Poznámka
   Při nahrávání kamery lze jen přidávat a ne přepisovat, aby nedošlo k nechtěné ztrátě
   záznamů pro měření.
   Pro přehrání nahraného pohybu je nejlepší aplikaci spustit znova. Pozice kamery se
   počítá z čísla snímku a ty se nenulují.


Parametry aplikace

   Vždy jen v daném formátu bez pomlček. Všechny položky jsou povinné!
   Poslední číslo určuje úvodní způsob kreslení.
   Způsobu kreslení odpovídá stejné číslo jako uvedeno výše v pokynech k ovládání.
   
   Indexy bludišť
   1 - 17x14
   2 - 20x15
   3 - 30x30
   4 - 60x60
   5 - 50x50
   6 - 31x31 - nepoužívané při měření
   7 - 180x180
	
   appname index [1-7] [1-7]
   appname width depth [1-7]
     --otevřou bludiště s daným indexem nebo dimenzemi(pokud existuje takové už vygenerované)

   appname generate width depth [1-7]
   appname generate index [1-6] [1-7]
      --vygeneruje bludiště s dimenzemi. Při vygenerování bludiště s dimenzemi jako už
        existující, dojde k přepsání. Pozor na přepsání bludišť s indexem. Bludiště s indexem
        1-6 lze znovu vygenerovat. 7 ne.

   appname go width depth [1-7]
   appname go index [1-7] [1-7]
      --spustí nahraný pohyb kamery v bludišti s indexem nebo dimenzemi.