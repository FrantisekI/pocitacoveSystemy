reálně nejužitečnější je se podívat na konec [poslední přednášky](https://cunicz.sharepoint.com/sites/NSWI170PCSystems/_layouts/15/stream.aspx?id=%2Fsites%2FNSWI170PCSystems%2FShared%20Documents%2Fvideo%2D2025%2F2025%2D05%2D22%2Dpara%2Den%2Emp4&ga=1&referrer=StreamWebApp%2EWeb&referrerScenario=AddressBarCopied%2Eview%2E7c8b9453%2Dd21b%2D490b%2Daba0%2D7b632ba82cc8) od kruliše

## Data Alignment 

[17@1](https://cunicz.sharepoint.com/:p:/s/NSWI170PCSystems/EfriQYchYz5IoaT6MDFGQvcBa-A-p0U_ZQAtCMvktLzO3g?e=ayym1t&nav=eyJzSWQiOjI5NiwiY0lkIjozMjU0MTgxMDI4fQ), 
[10@3](https://cunicz.sharepoint.com/:p:/s/NSWI170PCSystems/EXTMToPy4y1Pg1qjcAxtyxwBglrMgcGdxXJXiAyeyheg9A?e=iMkP6f&nav=eyJzSWQiOjI2MywiY0lkIjoxMzA2NDU3OTV9), [zapisky](250326.md#data-alignment)


## Alokace fragmentované paměti

[14@3](https://cunicz.sharepoint.com/:p:/s/NSWI170PCSystems/EXTMToPy4y1Pg1qjcAxtyxwBglrMgcGdxXJXiAyeyheg9A?e=XGlcPy&nav=eyJzSWQiOjI2NiwiY0lkIjozNjk5MzI4ODN9), [zapisky1](250326.md#Alokační-algoritmy), [zapisky2](250402.md#pameti,-furt)


## FAT

[48@5](https://cunicz.sharepoint.com/:p:/s/NSWI170PCSystems/EcegExT3UclFiszQ0PLqWM4B26DDW76aqwy4UKHSlVGNkw?e=7Dc6Ea&nav=eyJzSWQiOjMwNSwiY0lkIjozNDM3MzMzMjQ4fQ), [zapisky](250507.md#FAT-bude-v-testu)

## Stránkování

[67@5](https://cunicz.sharepoint.com/:p:/s/NSWI170PCSystems/EcegExT3UclFiszQ0PLqWM4B26DDW76aqwy4UKHSlVGNkw?e=xv9CDI&nav=eyJzSWQiOjMxMSwiY0lkIjo2MjAwMTQ0NjR9), [zapisky](250514.md#stránkování)

bude se ptát, do kolika stránek se vejdou nějaká data, to je prostě velikost dat děleno velikostí stránky (4kB) a chce odpověď v nejhorším případě - pro 8kB to bude 3 stránky

počet page faults - budu otevírat 3 datové stránky + až 2 stránkovací tabulky (stránkovací tabulka 2. úrovně, 1. musí být naalokovaná)

vole a ještě do toho máš započítat i to, že musíš načíst kód, který to bude zpracovávat, ten se obvykle vejde so 1 stránky, takže může být až přes 2 datové stránky a vy můžou být až přes 2 stránkovací tabulky

takže (3 datové stránky + 2 tabulky) *pro data* + (2 datové stránky +  2 tabulky) *pro kód* = 9 page faults

---
- struktura (struct) – vnější a vnitřní zarovnání, počítání offsetu
- FAT (odpověď: 10, 15)
- vybrat kód
- Jedná se o alokaci souvislých bloků paměti. Máte zadaný typ algoritmu pro alokaci (např. first fit) a sekvenci alokací a dealokací bloků různé velikosti. A otázka pak zní: Na jaké adrese leží nějaký blok na heapu, pokud byl použitý daný algoritmus?
- page faults
	- pozor na rozdíl mezi čtením a kopírováním
	- při kopírování jich vypadne 8 (úloha viz prezentace)

---

citace: [zapisky vitkolos](https://github.com/vitkolos/notes-ipp/blob/main/semestr2/pocitacove-systemy/systemy-prednaska.md),
[prezentace](https://www.ksi.mff.cuni.cz/teaching/nswi170-web/pages/lectures)

