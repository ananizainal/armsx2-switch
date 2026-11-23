# ARMSX2 Nintendo Switch (Horizon OS) Native Port

Porting native ARM64 dari ARMSX2 (PlayStation 2 Emulator) untuk Nintendo Switch CFW (Atmosphere / Horizon OS) dalam format `.nro`.

## Fitur & Arsitektur
- **Bentuk Format:** `.nro` Homebrew Application
- **Graphics Synthesizer Backend:** Vulkan 1.1 via `switch-mesa` (libvulkan_nouveau)
- **JIT Execution:** Dual-Mapping JIT via libnx (`svcControlCodeMemory`)
- **Translasi Memori:** Software TLB (Fase 1)
- **Frontend & UI:** SDL3 + Dear ImGui Handheld Fullscreen UI
- **Input:** Native Joy-Con / Pro Controller via SDL2/SDL3
- **Audio:** SPU2 output via `switch-sdl2` (driver audio native `audren`)

---

## Struktur Folder di SD Card

Letakkan file di kartu microSD Nintendo Switch Anda dengan struktur berikut:

```text
sdmc:/
  ├── switch/
  │   └── armsx2/
  │       ├── armsx2.nro           # Binary aplikasi
  │       ├── inis/                # File konfigurasi PCSX2.ini
  │       ├── bios/                # Letakkan file BIOS PS2 di sini (contoh: scph39001.bin)
  │       ├── memcards/            # Virtual Memory Card (Mcd001.ps2)
  │       ├── cheats/              # PNACH patches
  │       └── logs/                # File log debugging (armsx2.log)
  └── roms/
      └── ps2/                     # Letakkan file game (.iso, .chd, .cso)
```

---

## Panduan Menjalankan di Nintendo Switch

> [!WARNING]
> **JANGAN PERNAH MENJALANKAN DARI ALBUM (APPLET MODE)!**
> Applet Mode hanya menyediakan RAM ~450MB dan memblokir alokasi memori executable (JIT). Aplikasi akan langsung crash.

### Cara 1: Menggunakan Title Override (Hold R)
1. Pada menu utama Horizon OS (Home Menu), pilih game Switch resmi apapun yang terpasang.
2. Tahan tombol **R** pada Joy-Con / Pro Controller Anda, lalu tekan **A** untuk membuka game.
3. Tetap tahan tombol **R** sampai **Homebrew Menu (hbmenu)** muncul.
4. Buka **ARMSX2**.

### Cara 2: Menggunakan Forwarder NSP (Sapphire / NRO2NSP)
1. Buat forwarder NSP yang mengarah ke `sdmc:/switch/armsx2/armsx2.nro` menggunakan Sapphire.
2. Pastikan permission JIT diaktifkan pada template NSP.
3. Install NSP ke Switch menggunakan DBI / Tinfoil / Goldleaf.
4. Buka ARMSX2 langsung dari Home Menu seperti game biasa.

---

## Rekomendasi Overclock (`sys-clk`)

Untuk performa emulasi PS2 yang playable, profil overclock berikut sangat disarankan:

- **Docked Mode:**
  - CPU: `1785 MHz` (atau `1963 MHz` jika didukung)
  - GPU: `768 MHz` (atau `921 MHz`)
  - MEM: `1600 MHz` (atau `1996 MHz`)
- **Handheld Mode:**
  - CPU: `1785 MHz`
  - GPU: `460 MHz`
  - MEM: `1600 MHz`

---

## Cara Build via GitHub Actions
Workflow CI telah dikonfigurasi di `.github/workflows/switch_build_nro.yml`.
1. Push commit ke repositori GitHub.
2. Buka tab **Actions** di repositori GitHub Anda.
3. Pilih workflow **Build Nintendo Switch NRO**.
4. Unduh artifact zip yang berisi `armsx2.nro`.
