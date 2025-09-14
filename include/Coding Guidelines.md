# 📑 Coding Guidelines

## 1) Konvence názvů

* **Struktury:** nové typy se jmenují s prefixem `s_` (např. `s_uart_handle_t`).
* **Enumy:** nové typy se jmenují s prefixem `e_` a **položky enumu** také začínají `e_` (např. `e_uart_result_t`, `e_UART_RESULT_OK`).
* **Funkce:** malými písmeny, slova oddělená podtržítkem (`uart_open`, `fifo_create`).

### Prefixy proměnných (kombinační)

| Prefix | Co znamená                      | Příklad         |
| ------ | ------------------------------- | --------------- |
| `a_`   | argument – hodnota              | `a_length`      |
| `p_`   | lokální ukazatel                | `p_uart_handle` |
| `pp_`  | lokální ukazatel na ukazatel    | `pp_list`       |
| `ap_`  | argument – ukazatel             | `ap_buffer`     |
| `app_` | argument – ukazatel na ukazatel | `app_handle`    |
| `fp_`  | funkční ukazatel                | `fp_callback`   |

## 2) Datové typy

* Preferuj **pevné šířky** (`uint16_t`, `uint32_t`, `int32_t`, …).
* Vyhýbej se nejednoznačným typům (`int`, `long`) kvůli portabilitě.

## 3) Defenzivní programování

* Vstupy vždy ověřuj: `if (NULL == ap_ptr)`.
* Porovnání: **konstanta vlevo** – `if (0 == a_length)`, `if (e_UART_RESULT_OK != result)`.
* Nikdy nedereferencuj ukazatel bez testu na `NULL`.
* Před prací se stavem ověř stav (např. `e_UART_STATUS_OPEN` apod.).

## 4) Jednotný návratový mechanismus

* Funkce má lokální `result`.
* `result` se nastavuje průběžně a **na konci** se jedním `return` vrací.
* Žádné „early return“ (výjimky si vyhrazujeme jen pro výjimečné low‑level utility).

## 5) Alokace a cleanup

1. Zkontroluj **všechny** vstupy.
2. Proveď **všechny alokace**.
3. Až pak volej závislé inicializace.
4. Na chybě prováděj **reverse‑cleanup** (opačným pořadím).

## 6) Logování

* Makro `LOG(module, level, "msg")`; úrovně globální i per‑modul.
* Výstup nastav přes `log_set_output_function()`. Default je `printf`.

---

## 7) Formátování a styl kódu

* **Závorky** na stejném řádku: `if (...) {`.
* **Odsazení**: 4 mezery, ne taby.
* **Mezery**: `if (cond)`, `a + b`, `ptr != NULL`.
* **Řádky**: drž krátké (cca do 100–120 znaků), dlouhé výrazy zalamuj.

## 8) Komentáře a dokumentace

* Každá **veřejná** funkce: krátký hlavičkový komentář (co dělá, vstupy, návratové kódy, vedlejší efekty).
* Používej `// TODO:`, `// FIXME:` s krátkým popisem, ať se dají grepovat.
* Komentuj **záměr**, ne samozřejmý kód.

## 9) Konstanty a „magic numbers“

* Nepiš „magická“ čísla do kódu. Vytáhni je do `#define` / `enum` / `const`.
* Příklad: `#define UART_TIMEOUT_MS 100` místo `if (time > 100)`.

## 10) Kritické sekce & atomická práce

* `disable_interrupts()` / `enable_interrupts()` používej **co nejkratší** možný čas.
* V kritické sekci nikdy nealokuj, nespouštěj blokující operace, nevolej logování (pokud by mohlo blokovat).

## 11) Jazyk a čitelnost

* **Kód (identifikátory, komentáře)** piš anglicky pro budoucí portabilitu.
* Češtinu nech pro README / interní dokumenty mimo zdroják.
* Preferuj jasná jména: `rx_dma_length` je lepší než `len1`.

