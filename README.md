# Pokémon Generazioni — Demo GBA

Prototipo homebrew per Game Boy Advance: esplorazione in una piccola mappa, personaggio controllato dal D-pad, incontro casuale e lotta a turni semplificata. È una base originale da espandere progressivamente.

## Come ottenere la ROM
Il progetto include una GitHub Actions workflow che prova a compilare `pokemon-generazioni.gba` con devkitPro/libgba.
1. Crea un repository GitHub vuoto.
2. Carica tutti i file e le cartelle di questo ZIP nella radice del repository.
3. Apri **Actions** e abilita i workflow se richiesto.
4. Apri il workflow **Build GBA ROM**, seleziona l'ultima esecuzione riuscita e scarica l'artifact `pokemon-generazioni-gba`.
5. Estrai lo ZIP dell'artifact e apri `pokemon-generazioni.gba` con un emulatore GBA.

## Nota tecnica
Questo è uno scheletro di progetto, non una ROM già compilata. La compilazione avviene nel runner GitHub Actions. Se il workflow segnala errori, apri il log e correggi il problema prima di usare la ROM.

## Controlli
- D-pad: movimento
- A: interagisci / conferma
- B: annulla / attacca in battaglia
- START: apri la schermata informativa

## Prossimi passi
1. Mappa e collisioni più ricche.
2. Starter selezionabile e squadra Pokémon.
3. Inventario, Pokédex e salvataggi.
4. Espansione progressiva delle regioni.
5. Mega Evoluzioni e forme Gigamax con regole originali implementate nel motore.

Distribuisci solo codice e risorse che hai il diritto di usare. Il progetto non include ROM Pokémon originali né sprite, musica o mappe ufficiali protetti.
