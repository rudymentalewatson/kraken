# Kraken

Sintetizzatore VST3 ibrido: 16 oscillatori per voce (analogico, FM a 4 operatori, wavetable, supersaw),
doppio filtro, ensemble, delay e riverbero. Pensato per pad cinematografici e texture FM.

## Come ottenere il plugin per FL Studio (Windows)

1. Crea un account gratuito su https://github.com
2. Crea un nuovo repository (pulsante "New"), chiamalo `kraken`.
3. Carica tutti i file di questa cartella (compresa la cartella nascosta `.github`).
   Dal sito: "Add file" > "Upload files", trascina il contenuto della cartella.
4. Vai nella scheda **Actions**: parte da sola la compilazione (circa 10-15 minuti).
5. Quando e' verde, apri la compilazione e in fondo, sotto **Artifacts**, scarica `Kraken-Windows-VST3`.
6. Estrai lo zip e copia la cartella `Kraken.vst3` in:
   `C:\Program Files\Common Files\VST3`
7. In FL Studio: Options > Manage plugins > "Find installed plugins". Kraken compare tra gli strumenti.

Nello zip trovi anche `Kraken-Standalone.exe`: si apre da solo, senza FL Studio, con la tastiera sullo schermo.

## Struttura

- `Source/Dsp/`      motore audio (nessuna dipendenza dal plugin, testabile da solo)
- `Source/Plugin*`   collegamento a JUCE e interfaccia grafica
- `Tests/`           test che genera i suoni in file WAV e controlla errori
